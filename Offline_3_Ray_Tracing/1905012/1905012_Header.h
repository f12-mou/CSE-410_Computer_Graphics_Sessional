#include <bits/stdc++.h>
#include "bitmap_image.hpp"
#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif
using namespace std;
#define pi (2 * acos(0.0))

// Create a base class named Object in the header/src file mentioned in Step 2. You
// should define separate classes for each object (shape), and all of them should inherit the
// Object class.
class Object;
class Light;
class SpotLight;

extern vector<Light *> lights;
extern vector<SpotLight *> spotlights;
extern vector<Object *> objects;
extern int recursionLevel;

extern bitmap_image image;
class Point
{
public:
	double x, y, z, n;

	Point()
	{
		x = y = z = 0.0;
		n = 1.0;
	}

	Point(double x, double y, double z) : x(x), y(y), z(z), n(1.0) {}
	Point(double x, double y, double z, double n) : x(x), y(y), z(z), n(n) {}
	Point(const Point &p) : x(p.x), y(p.y), z(p.z), n(p.n) {}

	Point operator+(Point b) { return Point(x + b.x, y + b.y, z + b.z); }
	Point operator-(Point b) { return Point(x - b.x, y - b.y, z - b.z); }
	Point operator*(double b) { return Point(x * b, y * b, z * b); }
	Point operator/(double b) { return Point(x / b, y / b, z / b); }
	double operator*(Point b) { return x * b.x + y * b.y + z * b.z; }									// DOT PRODUCT
	Point operator^(Point b) { return Point(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); } // CROSS PRODUCT
	Point operator-() { return Point(-x, -y, -z); }
	double length() { return sqrt(x * x + y * y + z * z); }

	void normalize()
	{
		double len = length();
		x /= len;
		y /= len;
		z /= len;
	}
	friend istream &operator>>(istream &in, Point &p)
	{
		in >> p.x >> p.y >> p.z;
		return in;
	}
};
// to return Color object easily, vectors sometimes create problem in return
class Color
{
public:
	double r, g, b;
	Color()
	{
		r = g = b = 0.0;
	}
	Color(double r, double g, double b) : r(r), g(g), b(b) {}
};
// pointlight
class Light
{
public:
	// The PointLight class may contain the position of the point light source and its color.
	Point pos;
	Color color;
	vector<double> colors; // for calculating using loops
	void draw()
	{
		glPointSize(5);
		glBegin(GL_POINTS);
		glColor3f(color.r, color.g, color.b);
		glVertex3f(pos.x, pos.y, pos.z);
		glEnd();
	}
	// input of the PointLight
	friend istream &operator>>(istream &in, Light &l)
	{
		in >> l.pos.x >> l.pos.y >> l.pos.z;
		in >> l.color.r >> l.color.g >> l.color.b;
		l.colors.push_back(l.color.r);
		l.colors.push_back(l.color.g);
		l.colors.push_back(l.color.b);
		return in;
	}
	void print()
	{
		cout << endl;
		cout << "------------------ POINTLIGHT------------------" << endl;
		cout << "In the print function of Point Light Object" << endl;
		cout << "Here are the position :" << endl;
		cout << "x: " << pos.x << " y: " << pos.y << " z:" << pos.z << endl;
		cout << "The colors are: " << endl;
		for (int i = 0; i < 3; i++)
		{
			cout << colors[i] << endl;
		}
		cout << "------------------ POINTLIGHT------------------" << endl;
		cout << endl;
	}
};

// SpotLight
class SpotLight
{
public:
	// The SpotLight class, on the other hand, should contain direction and cutoff angle in
	// addition.So it can have a PointLight member variable and these two other properties.
	Light pointLight;	// main source
	Point dir;			// direction of the light
	double cutoffAngle; // after which light does not go beyond

	void draw()
	{
		Color color = pointLight.color;
		Point pos = pointLight.pos;

		glPointSize(15);
		glBegin(GL_POINTS);
		glColor3f(color.r, color.g, color.b);
		glVertex3f(pos.x, pos.y, pos.z);
		glEnd();
	}

	// input of the SpotLight
	friend istream &operator>>(istream &in, SpotLight &l)
	{
		in >> l.pointLight.pos;
		in >> l.pointLight.color.r >> l.pointLight.color.g >> l.pointLight.color.b;
		in >> l.dir;
		in >> l.cutoffAngle;
		l.pointLight.colors.push_back(l.pointLight.color.r);
		l.pointLight.colors.push_back(l.pointLight.color.g);
		l.pointLight.colors.push_back(l.pointLight.color.b);
		return in;
	}
	void print()
	{
		cout << endl;
		cout << "------------------ SPOTLIGHT------------------" << endl;
		cout << "In the print function of SpotLight: " << endl;
		cout << "Here is the cutOff Angle: " << cutoffAngle << endl;
		cout << "Here is the direction vector " << endl;
		cout << "x: " << dir.x << " y: " << dir.y << " z: " << dir.z << endl;
		cout << "------------------ SPOTLIGHT------------------" << endl;
		cout << endl;
	}
};

class Ray
{
public:
	Point origin, dir;
	Ray(Point origin, Point dir)
	{
		this->origin = origin;
		dir.normalize();
		this->dir = dir;
	}
	void print()
	{
		cout << endl;
		cout << "------------------ RAY------------------" << endl;
		cout << "In the print function of Ray: " << endl;
		cout << "Here is the direction vector " << endl;
		cout << "x: " << dir.x << " y: " << dir.y << " z: " << dir.z << endl;
		cout << "Here is the origin " << endl;
		cout << "x: " << origin.x << " y: " << origin.y << " z: " << origin.z << endl;
		cout << "------------------ RAY------------------" << endl;
		cout << endl;
	}
};

class Object
{
public:
	Point reference_point;		  // Reference Point for the object's position - SPEC
	double height, width, length; // SPEC
	vector<double> colors;		  // RGB color codes of the object
	double coeff_ambient;
	double coeff_diffuse;
	double coeff_specular;
	double coeff_reflection;
	int shine; // The exponent term for the Specular Component of the Object

	Object()
	{
		coeff_ambient = coeff_diffuse = coeff_specular = coeff_reflection = 0.0;
		shine = 0;
		height = width = length = 0.0;
		reference_point.x = reference_point.y = reference_point.z = 0.0;
		colors = vector<double>(4, 0.0);
	}
	// Setting the Color of the Object
	void setColor(Color color)
	{
		colors.push_back(color.r);
		colors.push_back(color.g);
		colors.push_back(color.b);
	}
	void setCoefficients(vector<double> coefficients)
	{
		coeff_ambient = coefficients[0];
		coeff_diffuse = coefficients[1];
		coeff_specular = coefficients[2];
		coeff_reflection = coefficients[3];
	}
	// Setting the exponent term of the specular component
	void setShine(int shine_inp)
	{
		shine = shine_inp;
	}
	virtual Color getColorAt(Point point)
	{
		return Color(this->colors[0], this->colors[1], this->colors[2]);
	}
	virtual void draw() = 0;
	virtual double intersectHelper(Ray ray, vector<double> &colors, int level) = 0;
	virtual Ray getNormal(Point point, Ray incidentRay) = 0;

	virtual double intersect(Ray ray, vector<double> &colors, int level)
	{
		double t = intersectHelper(ray, colors, level);

		if (t < 0)
			return -1;
		if (level == 0)
			return t;
		Point intersectionPoint = ray.origin + ray.dir * t;
		Color colorAtIntersection2 = getColorAt(intersectionPoint);

		vector<double> colorAtIntersection;
		colorAtIntersection.push_back(colorAtIntersection2.r);
		colorAtIntersection.push_back(colorAtIntersection2.g);
		colorAtIntersection.push_back(colorAtIntersection2.b);

		// --------------AMBIENT LIGHT EFFECT------------------------------

		for (int i = 0; i < 3; i++)
		{
			colors[i] = colorAtIntersection[i] * coeff_ambient; // Follows the formula I = Ka* Ia + ............
		}
		// --------------- Point Lights Effect---------------------------
		for (int i = 0; i < lights.size(); i++)
		{

			Point lightPosition = lights[i]->pos;
			Point lightDirection = intersectionPoint - lightPosition;
			lightDirection.normalize();
			Ray lightRay = Ray(lightPosition, lightDirection);
			Ray normal = getNormal(intersectionPoint, lightRay);
			double t2 = (intersectionPoint - lightPosition).length();
			if (t2 < 1e-5)
				continue;

			bool obscured = false;

			for (Object *obj : objects)
			{
				double t3 = obj->intersectHelper(lightRay, colors, 0);
				if (t3 > 0 && t3 + 1e-5 < t2)
				{
					obscured = true;
					break;
				}
			}

			if (!obscured)
			{
				double val = max(0.0, -lightRay.dir * normal.dir);
				Ray reflection = Ray(intersectionPoint, lightRay.dir - normal.dir * 2 * (lightRay.dir * normal.dir));
				double phong = max(0.0, -ray.dir * reflection.dir);

				for (int k = 0; k < 3; k++)
				{
					colors[k] += lights[i]->colors[k] * coeff_diffuse * val * colorAtIntersection[k];
					colors[k] += lights[i]->colors[k] * coeff_specular * pow(phong, shine) * colorAtIntersection[k];
				}
			}
		}
		// -----------------------SpotLights-------------------------------------
		for (int i = 0; i < spotlights.size(); i++)
		{

			Point lightPosition = spotlights[i]->pointLight.pos;
			Point lightDirection = intersectionPoint - lightPosition;
			lightDirection.normalize();

			double dot = lightDirection * spotlights[i]->dir;
			double angle = acos(dot / (lightDirection.length() * spotlights[i]->dir.length())) * (180.0 / pi);

			if (fabs(angle) < spotlights[i]->cutoffAngle)
			{

				Ray lightRay = Ray(lightPosition, lightDirection);
				Ray normal = getNormal(intersectionPoint, lightRay);

				Ray reflection = Ray(intersectionPoint, lightRay.dir - normal.dir * 2 * (lightRay.dir * normal.dir));

				double t2 = (intersectionPoint - lightPosition).length();
				if (t2 < 1e-5)
					continue;

				bool obscured = false;
				for (Object *obj : objects)
				{
					double t3 = obj->intersectHelper(lightRay, colors, 0);
					if (t3 > 0 && t3 + 1e-5 < t2)
					{
						obscured = true;
						break;
					}
				}
				if (!obscured)
				{
					double phong = max(0.0, -ray.dir * reflection.dir);
					double val = max(0.0, -lightRay.dir * normal.dir);
					for (int k = 0; k < 3; k++)
					{
						colors[k] += spotlights[i]->pointLight.colors[k] * coeff_diffuse * val * colorAtIntersection[k];
						colors[k] += spotlights[i]->pointLight.colors[k] * coeff_specular * pow(phong, shine) * colorAtIntersection[k];
					}
				}
			}
		}
		if (level < recursionLevel)
		{
			Ray normal = getNormal(intersectionPoint, ray);
			Ray reflectionRay = Ray(intersectionPoint, ray.dir - normal.dir * 2 * (ray.dir * normal.dir));
			reflectionRay.origin = reflectionRay.origin + reflectionRay.dir * 1e-5;
			int nearestObjectIndex = -1;
			double t = -1, tMin = 1e9;

			for (int k = 0; k < (int)objects.size(); k++)
			{
				// Color color_now(colors[0], colors[1], colors[2]);
				t = objects[k]->intersect(reflectionRay, colors, 0);
				if (t > 0 && t < tMin)
					tMin = t, nearestObjectIndex = k;
			}

			if (nearestObjectIndex != -1)
			{
				vector<double> colorTemp(3, 0.0); // reflection color
				double t = objects[nearestObjectIndex]->intersect(reflectionRay, colorTemp, level + 1);
				for (int k = 0; k < 3; k++)
				{
					colors[k] += colorTemp[k] * coeff_reflection;
				}
			}
		}
		return t;
	}

	// destructor
	virtual ~Object()
	{
		colors.clear();
		colors.shrink_to_fit();
	}
};

struct General : public Object
{
	double A, B, C, D, E, F, G, H, I, J;

	General()
	{
	}

	virtual void draw()
	{
		return;
	}

	virtual Ray getNormal(Point point, Ray incidentRay)
	{
		Point dir(2 * A * point.x + D * point.y + E * point.z + G,
				  2 * B * point.y + D * point.x + F * point.z + H,
				  2 * C * point.z + E * point.x + F * point.y + I);

		return Ray(point, dir);
	}

	bool func(Point point)
	{
		if (fabs(length) > 1e-5)
		{
			if (point.x < reference_point.x)
				return false;
			if (point.x > reference_point.x + length)
				return false;
		}

		if (fabs(height) > 1e-5)
		{
			if (point.z < reference_point.z)
				return false;
			if (point.z > reference_point.z + height)
				return false;
		}

		if (fabs(width) > 1e-5)
		{
			if (point.y < reference_point.y)
				return false;
			if (point.y > reference_point.y + width)
				return false;
		}

		return true;
	}

	virtual double intersectHelper(Ray ray, vector<double> &colors, int level)
	{

		double X0 = ray.origin.x;
		double Y0 = ray.origin.y;
		double Z0 = ray.origin.z;

		double X1 = ray.dir.x;
		double Y1 = ray.dir.y;
		double Z1 = ray.dir.z;

		// Putting the x,y and z components of Ray in the equation of the Slide
		// a component
		double C0 = A * X1 * X1 + B * Y1 * Y1 + C * Z1 * Z1 + D * X1 * Y1 + E * X1 * Z1 + F * Y1 * Z1;
		// b component
		double C1 = 2 * A * X0 * X1 + 2 * B * Y0 * Y1 + 2 * C * Z0 * Z1 + D * (X0 * Y1 + X1 * Y0) + E * (X0 * Z1 + X1 * Z0) + F * (Y0 * Z1 + Y1 * Z0) + G * X1 + H * Y1 + I * Z1;
		// c component
		double C2 = A * X0 * X0 + B * Y0 * Y0 + C * Z0 * Z0 + D * X0 * Y0 + E * X0 * Z0 + F * Y0 * Z0 + G * X0 + H * Y0 + I * Z0 + J;

		double discriminant = C1 * C1 - 4 * C0 * C2;
		if (discriminant < 0)
			return -1;
		// KAAJ NA KORLE BAAD DIBO ETA
		if (fabs(C0) < 1e-5)
		{
			return -C2 / C1;
		}
		double t1 = (-C1 - sqrt(discriminant)) / (2 * C0);
		double t2 = (-C1 + sqrt(discriminant)) / (2 * C0);

		if (t1 < 0 && t2 < 0)
		{
			// cout<<" duitai < 0 "<<endl;
			return -1;
		}

		if (t2 < t1)
			swap(t1, t2);

		// lowest positive return dea lagbe (slide porer porerta)

		if (t1 > 0)
		{
			Point intersectionPoint = ray.origin + ray.dir * t1;
			if (func(intersectionPoint))
			{
				return t1;
			}
			// cout<<"vai return kore nai t1 -----------------------"<<endl;
		}
		if (t2 > 0)
		{
			Point intersectionPoint = ray.origin + ray.dir * t2;
			if (func(intersectionPoint))
			{
				return t2;
			}
			// cout<<"vai return kore nai t2 -----------------------"<<endl;
		}
		// cout<<endl;
		// cout<<"returning -1 from the last point of the function"<<endl;
		// cout<<endl;
		return -1;
	}

	// input stream
	friend istream &operator>>(istream &in, General &g)
	{
		in >> g.A >> g.B >> g.C >> g.D >> g.E >> g.F >> g.G >> g.H >> g.I >> g.J;
		in >> g.reference_point >> g.length >> g.width >> g.height;

		for (int i = 0; i < 3; i++)
			in >> g.colors[i];

		in >> g.coeff_ambient >> g.coeff_diffuse >> g.coeff_specular >> g.coeff_reflection;
		in >> g.shine;
		return in;
	}
	void print()
	{
		cout << endl;
		cout << "---------------------GENERAL OBJECT--------------------------" << endl;
		cout << "In the print function of general" << endl;
		cout << "A: " << A << " B: " << B << " C: " << C << " D: " << D << " E: " << E << endl;
		cout << "F: " << F << " G: " << G << " H: " << H << " I: " << I << " J: " << J << endl;
		cout << "---------------------GENERAL OBJECT--------------------------" << endl;
		cout << endl;
	}
};

double determinant(double arr[3][3])
{
	double v1 = arr[0][0] * (arr[1][1] * arr[2][2] - arr[1][2] * arr[2][1]);
	double v2 = arr[0][1] * (arr[1][0] * arr[2][2] - arr[1][2] * arr[2][0]);
	double v3 = arr[0][2] * (arr[1][0] * arr[2][1] - arr[1][1] * arr[2][0]);
	// cout<<"--------------IN THE DETERMINANT FUNCTION----------------"<<endl;
	// cout<<"v1: "<<v1<<" v2: "<<v2<<" v3: "<<v3<<endl;
	return v1 - v2 + v3;
}

struct Triangle : public Object
{
	Point a, b, c;
	double betaMat[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}};
	double gammaMat[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}};
	double tMat[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}};
	double AMat[3][3] = {
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}};

	Triangle()
	{
	}

	void set_beta_matrix()
	{
		betaMat[0][1] = a.x - c.x;
		betaMat[1][1] = a.y - c.y;
		betaMat[2][1] = a.z - c.z;
	}
	void set_gamma_matrix()
	{
		gammaMat[0][0] = a.x - b.x;
		gammaMat[1][0] = a.y - b.y;
		gammaMat[2][0] = a.z - b.z;
	}
	void set_t_matrix()
	{
		tMat[0][0] = a.x - b.x;
		tMat[1][0] = a.y - b.y;
		tMat[2][0] = a.z - b.z;
		// here
		tMat[0][1] = a.x - c.x;
		tMat[1][1] = a.y - c.y;
		tMat[2][1] = a.z - c.z;
	}
	void set_A_matrix()
	{
		AMat[0][0] = a.x - b.x;
		AMat[1][0] = a.y - b.y;
		AMat[2][0] = a.z - b.z;
		// here
		AMat[0][1] = a.x - c.x;
		AMat[1][1] = a.y - c.y;
		AMat[2][1] = a.z - c.z;
	}
	void adjust_beta_matrix(Ray &ray)
	{
		// 1st row adjusting the 1st and 3rd col
		betaMat[0][0] = a.x - ray.origin.x;
		betaMat[0][2] = ray.dir.x;
		// 2nd row adjusting the 1st and 3rd col
		betaMat[1][0] = a.y - ray.origin.y;
		betaMat[1][2] = ray.dir.y;
		// 3rd row adjusting the 1st and 3rd col
		betaMat[2][0] = a.z - ray.origin.z;
		betaMat[2][2] = ray.dir.z;
	}
	void adjust_gamma_matrix(Ray &ray)
	{
		// 1st row adjusting the 2nd and 3rd col
		gammaMat[0][1] = a.x - ray.origin.x;
		gammaMat[0][2] = ray.dir.x;
		// 2nd row adjusting the 2nd and 3rd col
		gammaMat[1][1] = a.y - ray.origin.y;
		gammaMat[1][2] = ray.dir.y;
		// 3rd row adjusting the 2nd and 3rd col
		gammaMat[2][1] = a.z - ray.origin.z;
		gammaMat[2][2] = ray.dir.z;
	}
	void adjust_t_matrix(Ray &ray)
	{
		tMat[0][2] = a.x - ray.origin.x;
		tMat[1][2] = a.y - ray.origin.y;
		tMat[2][2] = a.z - ray.origin.z;
	}
	void adjust_A_matrix(Ray &ray)
	{
		AMat[0][2] = ray.dir.x;
		AMat[1][2] = ray.dir.y;
		AMat[2][2] = ray.dir.z;
	}

	Triangle(Point a, Point b, Point c)
	{
		this->a = a;
		this->b = b;
		this->c = c;
		set_beta_matrix();
		set_gamma_matrix();
		set_t_matrix();
		set_A_matrix();
	}

	virtual Ray getNormal(Point point, Ray incidentRay)
	{
		Point normal = (b - a) ^ (c - a);
		normal.normalize();

		if (incidentRay.dir * normal < 0)
		{
			return Ray(point, -normal);
		}
		else
		{
			return Ray(point, normal);
		}
	}

	virtual void draw()
	{
		glColor3f(colors[0], colors[1], colors[2]);
		glBegin(GL_TRIANGLES);
		{
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(b.x, b.y, b.z);
			glVertex3f(c.x, c.y, c.z);
		}
		glEnd();
	}

	virtual double intersectHelper(Ray ray, vector<double> &colors, int level)
	{
		set_beta_matrix();
		adjust_beta_matrix(ray);
		set_gamma_matrix();
		adjust_gamma_matrix(ray);
		set_t_matrix();
		adjust_t_matrix(ray);
		set_A_matrix();
		adjust_A_matrix(ray);

		double Adet = determinant(AMat);
		double beta = determinant(betaMat) / Adet;
		double gamma = determinant(gammaMat) / Adet;
		double t = determinant(tMat) / Adet;

		if (beta + gamma < 1 && beta > 0 && gamma > 0 && t > 0)
		{
			return t;
		}
		else
		{
			return -1;
		}
	}

	// input stream
	friend istream &operator>>(istream &in, Triangle &t)
	{
		in >> t.a >> t.b >> t.c; // 3 vertices

		for (int i = 0; i < 3; i++)
			in >> t.colors[i];
		in >> t.coeff_ambient >> t.coeff_diffuse >> t.coeff_specular >> t.coeff_reflection; // color
		in >> t.shine;
		return in;
	}
	void print_matrix_beta()
	{
		cout << endl;
		cout << "------------------ Beta Matrix---------------" << endl;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				cout << betaMat[i][j] << " ";
			}
			cout << endl;
		}
		cout << "------------------ Beta Matrix---------------" << endl;
		cout << endl;
	}
	void print_matrix_alpha()
	{
		cout << endl;
		cout << "------------------ Alpha Matrix---------------" << endl;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				cout << AMat[i][j] << " ";
			}
			cout << endl;
		}
		cout << "------------------ Alpha Matrix---------------" << endl;
		cout << endl;
	}
	void print_matrix_gamma()
	{
		cout << endl;
		cout << "------------------ Gamma Matrix---------------" << endl;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				cout << gammaMat[i][j] << " ";
			}
			cout << endl;
		}
		cout << "------------------ Gamma Matrix---------------" << endl;
		cout << endl;
	}
	void print_matrix_t()
	{
		cout << endl;
		cout << "------------------t Matrix---------------" << endl;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				cout << tMat[i][j] << " ";
			}
			cout << endl;
		}
		cout << "------------------ t Matrix---------------" << endl;
		cout << endl;
	}
	void print()
	{
		cout << endl;
		cout << "--------------------TRIANGLE---------------------------" << endl;
		cout << "In the print function of Triangle" << endl;
		cout << "The a point of the triangle " << endl;
		cout << "a.x: " << a.x << " a.y: " << a.y << " a.z: " << a.z << endl;
		cout << "b.x: " << b.x << " b.y: " << b.y << " b.z: " << b.z << endl;
		cout << "c.x: " << c.x << " c.y: " << c.y << " c.z: " << c.z << endl;
		cout << "--------------------TRIANGLE---------------------------" << endl;
		cout << endl;
	}
};

struct Sphere : public Object
{

	Sphere()
	{
	}

	Sphere(Point center, double radius)
	{
		reference_point = center;
		length = radius;
	}

	virtual Ray getNormal(Point point, Ray incidentRay)
	{
		return Ray(point, point - reference_point);
	}

	virtual void draw()
	{
		// copied from template
		int stacks = 30;
		int slices = 20;

		Point points[100][100];
		int i, j;
		double h, r;
		// generate points
		for (i = 0; i <= stacks; i++)
		{
			h = length * sin(((double)i / (double)stacks) * (pi / 2));
			r = length * cos(((double)i / (double)stacks) * (pi / 2));
			for (j = 0; j <= slices; j++)
			{
				points[i][j].x = r * cos(((double)j / (double)slices) * 2 * pi);
				points[i][j].y = r * sin(((double)j / (double)slices) * 2 * pi);
				points[i][j].z = h;
			}
		}
		// draw quads using generated points
		for (i = 0; i < stacks; i++)
		{
			glPushMatrix();
			glTranslatef(reference_point.x, reference_point.y, reference_point.z);
			glColor3f(colors[0], colors[1], colors[2]);
			for (j = 0; j < slices; j++)
			{
				glBegin(GL_QUADS);
				{
					// upper hemisphere
					glVertex3f(points[i][j].x, points[i][j].y, points[i][j].z);
					glVertex3f(points[i][j + 1].x, points[i][j + 1].y, points[i][j + 1].z);
					glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, points[i + 1][j + 1].z);
					glVertex3f(points[i + 1][j].x, points[i + 1][j].y, points[i + 1][j].z);
					// lower hemisphere
					glVertex3f(points[i][j].x, points[i][j].y, -points[i][j].z);
					glVertex3f(points[i][j + 1].x, points[i][j + 1].y, -points[i][j + 1].z);
					glVertex3f(points[i + 1][j + 1].x, points[i + 1][j + 1].y, -points[i + 1][j + 1].z);
					glVertex3f(points[i + 1][j].x, points[i + 1][j].y, -points[i + 1][j].z);
				}
				glEnd();
			}
			glPopMatrix();
		}
	}

	virtual double intersectHelper(Ray ray, vector<double> &colors, int level)
	{

		ray.origin = ray.origin - reference_point; // adjust ray origin

		double a = 1;
		double b = 2 * (ray.dir * ray.origin);
		double c = (ray.origin * ray.origin) - (length * length);

		double discriminant = pow(b, 2) - 4 * a * c;
		double t = -1;
		if (discriminant < 0)
		{
			t = -1;
		}
		else
		{

			if (fabs(a) < 1e-5)
			{
				t = -c / b;
				return t;
			}

			double t1 = (-b - sqrt(discriminant)) / (2 * a);
			double t2 = (-b + sqrt(discriminant)) / (2 * a);

			if (t2 < t1)
				swap(t1, t2);

			if (t1 > 0)
			{
				t = t1;
			}
			else if (t2 > 0)
			{
				t = t2;
			}
			else
			{
				t = -1;
			}
		}

		return t;
	}

	// input stream
	friend std::istream &operator>>(std::istream &in, Sphere &s)
	{
		in >> s.reference_point >> s.length; // center and radius
		for (int i = 0; i < 3; i++)
			in >> s.colors[i];
		in >> s.coeff_ambient >> s.coeff_diffuse >> s.coeff_specular >> s.coeff_reflection; // color
		in >> s.shine;
		return in;
	}
	void print()
	{
		cout << endl;
		cout << "--------------------- SPHERE---------------------------" << endl;
		cout << "Reference Point " << endl;
		cout << reference_point.x << " " << reference_point.y << " " << reference_point.z << endl;
		cout << "radius : " << length << endl;
		cout << "--------------------- SPHERE---------------------------" << endl;
		cout << endl;
	}
};

struct Floor : public Object
{

	int tiles;

	Floor()
	{
		tiles = 1;
	}

	Floor(int floorWidth, int tileWidth)
	{
		tiles = floorWidth / tileWidth;
		reference_point = Point(-floorWidth / 2, -floorWidth / 2, 0);
		length = tileWidth;
	}

	virtual Color getColorAt(Point point)
	{

		int tileX = (point.x - reference_point.x) / length;
		int tileY = (point.y - reference_point.y) / length;

		if (tileX < 0 || tileX >= tiles || tileY < 0 || tileY >= tiles)
		{
			return Color(0, 0, 0);
		}

		if (((tileX + tileY) % 2) == 0)
		{
			return Color(1, 1, 1);
		}
		else
		{
			return Color(0, 0, 0);
		}
	}

	virtual Ray getNormal(Point point, Ray incidentRay)
	{
		if (incidentRay.dir.z > 0)
			return Ray(point, Point(0, 0, 1));
		else
			return Ray(point, Point(0, 0, -1));
	}

	virtual void draw()
	{
		for (int i = 0; i < tiles; i++)
		{
			for (int j = 0; j < tiles; j++)
			{
				if (((i + j) % 2) == 0)
					glColor3f(1, 1, 1);
				else
					glColor3f(0, 0, 0);

				glBegin(GL_QUADS);
				{
					glVertex3f(reference_point.x + i * length, reference_point.y + j * length, 0);
					glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + j * length, 0);
					glVertex3f(reference_point.x + (i + 1) * length, reference_point.y + (j + 1) * length, 0);
					glVertex3f(reference_point.x + i * length, reference_point.y + (j + 1) * length, 0);
				}
				glEnd();
			}
		}
	}

	virtual double intersectHelper(Ray ray, vector<double> &colors, int level)
	{
		Point normal = Point(0, 0, 1);
		double dotP = normal * ray.dir;

		if (round(dotP * 100) == 0)
			return -1;

		double t = -(normal * ray.origin) / dotP;

		Point p = ray.origin + ray.dir * t;

		if (p.x <= reference_point.x || p.x >= abs(reference_point.x) && p.y <= reference_point.y && p.y >= abs(reference_point.y))
		{
			return -1;
		}

		return t;
	}
};
