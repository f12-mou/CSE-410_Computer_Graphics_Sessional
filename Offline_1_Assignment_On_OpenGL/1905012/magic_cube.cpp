#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif
using namespace std;

#define pi (2 * acos(0.0))

struct Point
{
	Point() {}
	double x, y, z;

	Point(double x, double y, double z) : x(x), y(y), z(z) {}
	Point(const Point &p) : x(p.x), y(p.y), z(p.z) {}
	Point operator+(Point b) { return Point(x + b.x, y + b.y, z + b.z); }
	Point operator-(Point b) { return Point(x - b.x, y - b.y, z - b.z); }
	Point operator*(double b) { return Point(x * b, y * b, z * b); }
	Point operator/(double b) { return Point(x / b, y / b, z / b); }
	Point operator*(Point b) { return Point(y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x); }
};

double dot(Point a, Point b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
double length(Point a) { return sqrt(dot(a, a)); }
Point normalize(Point a) { return a / length(a); }
double pos_x, pos_y, pos_z, l_x, l_y, l_z, r_x, r_y, r_z, u_x, u_y, u_z;

// task 3
double angleRotationZ = 45.0;
double maxTriangleLength = 1.6;
double triangleLength = 1.6;

double maxSphereRadius = maxTriangleLength / sqrt(3.0);
double sphereRadius = 0;
double sphereStep = maxSphereRadius / 16.0;
int numSegments = 36;

void setPoint3D(int who, double x, double y, double z)
{
	if (who == 1)
	{
		pos_x = x;
		pos_y = y;
		pos_z = z;
	}
	else if (who == 2)
	{
		l_x = x;
		l_y = y;
		l_z = z;
	}
	else if (who == 3)
	{
		r_x = x;
		r_y = y;
		r_z = z;
	}
	else if (who == 4)
	{
		u_x = x;
		u_y = y;
		u_z = z;
	}
}
void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	setPoint3D(1, 5, 0, 2);
	setPoint3D(2, -1, 0, 0);
	setPoint3D(3, 0, 1, 0);
	setPoint3D(4, 0, 0, 1);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(80, 1, 1, 1000.0);
}
void drawAxes()
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	{
		glVertex3f(200, 0, 0);
		glVertex3f(-200, 0, 0);

		glVertex3f(0, -200, 0);
		glVertex3f(0, 200, 0);

		glVertex3f(0, 0, 200);
		glVertex3f(0, 0, -200);
	}
	glEnd();
}
void drawTriangle()
{
	glBegin(GL_TRIANGLES);
	{
		glVertex3f(1, 0, 0);
		glVertex3f(0, 1, 0);
		glVertex3f(0, 0, 1);
	}
	glEnd();
}

void drawCylinder_v2(double height, double radius, int segments)
{
	struct Point points[segments + 1];

	double offset = 70.5287794 * M_PI / 180.0;

	for (int i = 0; i < segments + 1; i++)
	{
		double theta = -offset / 2 + i * offset / segments;
		points[i].x = radius * cos(theta);
		points[i].y = radius * sin(theta);
	}

	glBegin(GL_QUADS);
	for (int i = 0; i < segments; i++)
	{
		glVertex3f(points[i].x, points[i].y, height / 2);
		glVertex3f(points[i].x, points[i].y, -height / 2);
		glVertex3f(points[i + 1].x, points[i + 1].y, -height / 2);
		glVertex3f(points[i + 1].x, points[i + 1].y, height / 2);
	}
	glEnd();
}

void drawSphereQuad(double radius, int segments)
{
	float x, y, z, xy; // vertex position
	int sectorCount = segments;
	int stackCount = segments / 2;
	float sectorStep = 2 * pi / sectorCount;
	float stackStep = pi / stackCount;
	float sectorAngle, stackAngle;
	vector<double> vertices;
	for (int i = 0; i <= stackCount; ++i)
	{
		stackAngle = pi / 2 - i * stackStep; // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);		 // r * cos(u)
		z = radius * sinf(stackAngle);		 // r * sin(u)
		for (int j = 0; j <= sectorCount; ++j)
		{
			sectorAngle = j * sectorStep; // starting from 0 to 2pi
			x = xy * cosf(sectorAngle);	  // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);	  // r * cos(u) * sin(v)
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
		}
	}

	for (int i = 0; i < stackCount; ++i)
	{
		int k1 = i * (sectorCount + 1); // beginning of current stack
		int k2 = k1 + sectorCount + 1;	// beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{
			if (i != 0)
			{
				glBegin(GL_TRIANGLES);
				{
					glVertex3f(vertices[3 * k1], vertices[3 * k1 + 1], vertices[3 * k1 + 2]);
					glVertex3f(vertices[3 * k2], vertices[3 * k2 + 1], vertices[3 * k2 + 2]);
					glVertex3f(vertices[3 * (k1 + 1)], vertices[3 * (k1 + 1) + 1], vertices[3 * (k1 + 1) + 2]);
				}
				glEnd();
			}
			if (i != (stackCount - 1))
			{
				glBegin(GL_TRIANGLES);
				{
					glVertex3f(vertices[3 * (k1 + 1)], vertices[3 * (k1 + 1) + 1], vertices[3 * (k1 + 1) + 2]);
					glVertex3f(vertices[3 * k2], vertices[3 * k2 + 1], vertices[3 * k2 + 2]);
					glVertex3f(vertices[3 * (k2 + 1)], vertices[3 * (k2 + 1) + 1], vertices[3 * (k2 + 1) + 2]);
				}
				glEnd();
			}
		}
	}
}

void drawCylinders()
{

	glColor3f(1.0f, 1.0f, 0.0f);

	for (int i = 0; i < 4; i++)
	{

		glPushMatrix();
		{

			glRotatef(45 + i * 90, 0, 1, 0);
			glTranslatef(triangleLength / sqrt(2), 0, 0);
			drawCylinder_v2(triangleLength * sqrt(2), sphereRadius, 100);
		}
		glPopMatrix();
	}
	for (int i = 0; i < 4; i++)
	{

		glPushMatrix();
		{
			glRotatef(90, 1, 0, 0);
			glRotatef(45 + i * 90, 0, 1, 0);
			glTranslatef(triangleLength / sqrt(2), 0, 0);
			drawCylinder_v2(triangleLength * sqrt(2), sphereRadius, 100);
		}
		glPopMatrix();
	}
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		{
			glRotatef(90, 0, 0, 1);
			glRotatef(45 + i * 90, 0, 1, 0);
			glTranslatef(triangleLength / sqrt(2), 0, 0);
			drawCylinder_v2(triangleLength * sqrt(2), sphereRadius, 100);
		}
		glPopMatrix();
	}
}

void drawSpheres()
{

	for (int i = 0; i < 4; i++)
	{

		glPushMatrix();
		{
			glColor3f(0, i % 2, (i + 1) % 2);
			glRotatef(90 * i, 0, 1, 0);
			glTranslatef(0, 0, triangleLength);
			drawSphereQuad(sphereRadius, 100);
		}
		glPopMatrix();
	}

	for (int i = 0; i < 2; i++)
	{

		glPushMatrix();
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glRotatef(90 + 180 * i, 1, 0, 0);
			glTranslatef(0, 0, triangleLength);
			drawSphereQuad(sphereRadius, 100);
		}
		glPopMatrix();
	}
}

void drawPyramids()
{
	double diff = maxTriangleLength - triangleLength;
	diff = diff / 3.0;
	for (int i = 0; i < 4; i++)
	{

		glPushMatrix();
		{
			glColor3f((i + 1) % 2, i % 2, 1.0f);
			glRotatef(90 * i, 0, 1, 0);
			glTranslatef(diff, diff, diff);
			glScaled(triangleLength, triangleLength, triangleLength);
			drawTriangle();
		}
		glPopMatrix();
	}

	for (int i = 0; i < 4; i++)
	{

		glPushMatrix();
		{
			glColor3f(i % 2, (i + 1) % 2, 1.0f);
			glRotatef(90 * i, 0, 1, 0);
			glRotatef(180, 1, 0, 1);
			glTranslatef(diff, diff, diff);
			glScaled(triangleLength, triangleLength, triangleLength);
			drawTriangle();
		}
		glPopMatrix();
	}
}
void keyboardListener(unsigned char key, int x, int y)
{
	double v = 0.1;
	double rate = 0.1;
	double s;
	switch (key)
	{
	case '1':
		r_x = r_x * cos(rate) + l_x * sin(rate);
		r_y = r_y * cos(rate) + l_y * sin(rate);
		r_z = r_z * cos(rate) + l_z * sin(rate);
		l_x = l_x * cos(rate) - r_x * sin(rate);
		l_y = l_y * cos(rate) - r_y * sin(rate);
		l_z = l_z * cos(rate) - r_z * sin(rate);
		break;

	case '2':
		r_x = r_x * cos(-rate) + l_x * sin(-rate);
		r_y = r_y * cos(-rate) + l_y * sin(-rate);
		r_z = r_z * cos(-rate) + l_z * sin(-rate);
		l_x = l_x * cos(-rate) - r_x * sin(-rate);
		l_y = l_y * cos(-rate) - r_y * sin(-rate);
		l_z = l_z * cos(-rate) - r_z * sin(-rate);
		break;

	case '3':
		l_x = l_x * cos(rate) + u_x * sin(rate);
		l_y = l_y * cos(rate) + u_y * sin(rate);
		l_z = l_z * cos(rate) + u_z * sin(rate);
		u_x = u_x * cos(rate) - l_x * sin(rate);
		u_y = u_y * cos(rate) - l_y * sin(rate);
		u_z = u_z * cos(rate) - l_z * sin(rate);
		break;

	case '4':
		l_x = l_x * cos(-rate) + u_x * sin(-rate);
		l_y = l_y * cos(-rate) + u_y * sin(-rate);
		l_z = l_z * cos(-rate) + u_z * sin(-rate);
		u_x = u_x * cos(-rate) - l_x * sin(-rate);
		u_y = u_y * cos(-rate) - l_y * sin(-rate);
		u_z = u_z * cos(-rate) - l_z * sin(-rate);
		break;

	case '5':
		u_x = u_x * cos(rate) + r_x * sin(rate);
		u_y = u_y * cos(rate) + r_y * sin(rate);
		u_z = u_z * cos(rate) + r_z * sin(rate);
		r_x = r_x * cos(rate) - u_x * sin(rate);
		r_y = r_y * cos(rate) - u_y * sin(rate);
		r_z = r_z * cos(rate) - u_z * sin(rate);
		break;

	case '6':
		u_x = u_x * cos(-rate) + r_x * sin(-rate);
		u_y = u_y * cos(-rate) + r_y * sin(-rate);
		u_z = u_z * cos(-rate) + r_z * sin(-rate);
		r_x = r_x * cos(-rate) - u_x * sin(-rate);
		r_y = r_y * cos(-rate) - u_y * sin(-rate);
		r_z = r_z * cos(-rate) - u_z * sin(-rate);

		break;

	case ',':
		triangleLength -= 0.1;
		sphereRadius += sphereStep;
		if (triangleLength < 0)
		{
			triangleLength = 0;
			sphereRadius = maxSphereRadius;
		}
		break;
	case '.':
		triangleLength += 0.1;
		sphereRadius -= sphereStep;
		if (triangleLength > maxTriangleLength)
		{
			triangleLength = maxTriangleLength;
			sphereRadius = 0;
		}
		break;
	case 'd':
		angleRotationZ -= 5.0f;
		break;
	case 'a':
		angleRotationZ += 5.0f;
		break;
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0); // color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos_x, pos_y, pos_z, pos_x + l_x, pos_y + l_y, pos_z + l_z, u_x, u_y, u_z);
	glMatrixMode(GL_MODELVIEW);
	glRotatef(angleRotationZ, 0, 0, 1);
	drawAxes();
	drawPyramids();
	drawSpheres();
	drawCylinders();
	glutSwapBuffers();
}

void setPosition(double x, double y, double z)
{
	pos_x = pos_x + x;
	pos_y = pos_y + y;
	pos_z = pos_z + z;
}

void specialKeyListener(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		setPosition(l_x, l_y, l_z);
		break;
	case GLUT_KEY_DOWN:
		setPosition(-l_x, -l_y, -l_z);
		break;

	case GLUT_KEY_RIGHT:
		setPosition(r_x, r_y, r_z);
		break;
	case GLUT_KEY_LEFT:
		setPosition(-r_x, -r_y, -r_z);
		break;

	case GLUT_KEY_PAGE_UP:
		setPosition(u_x, u_y, u_z);
		break;
	case GLUT_KEY_PAGE_DOWN:
		setPosition(-u_x, -u_y, -u_z);
		break;
	default:
		break;
	}
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitWindowSize(450, 450);
	glutInitWindowPosition(750, 250);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("MyTetra");
	init();
	glEnable(GL_DEPTH_TEST); // enable Depth Testing
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}