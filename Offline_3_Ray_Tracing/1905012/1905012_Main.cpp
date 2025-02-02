#include <bits/stdc++.h>
#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif
using namespace std;
#include "bitmap_image.hpp"
#include "1905012_Header.h"
#define pi (2.0 * acos(0.0))

int recursionLevel;
int imageHeight, imageWidth;
bitmap_image image;

double cameraHeight;
double cameraAngle;
double angle;

vector<Object *> objects;
vector<Light *> lights;
vector<SpotLight *> spotlights;
int imageCount = 1;

// Camera Position initially
Point pos(200, 0, 10);
Point up(0, 0, 1);
Point rightV(-1 / sqrt(2), 1 / sqrt(2), 0);
Point look(-1 / sqrt(2), -1 / sqrt(2), 0);

double ROT_ANG = pi / 180;
int numSegments;

double windowWidth = 500, windowHeight = 500;
double viewAngle = 80;

void rotate3D(Point &vec, Point &axis, double ang)
{
	vec = vec * cos(ang) + (axis ^ vec) * sin(ang);
}

// draw axes
void drawAxes()
{

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	{
		glVertex3f(100, 0, 0);
		glVertex3f(-100, 0, 0);

		glVertex3f(0, -100, 0);
		glVertex3f(0, 100, 0);

		glVertex3f(0, 0, 100);
		glVertex3f(0, 0, -100);
	}
	glEnd();
}

void capture()
{
	cout << "In the capture function" << endl;

	// initialize bitmap image and set background color to black
	for (int i = 0; i < imageWidth; i++)
		for (int j = 0; j < imageHeight; j++)
			image.set_pixel(i, j, 0, 0, 0);

	double planeDistance = (windowHeight / 2.0) / tan((pi * viewAngle) / (360.0));

	Point topLeft = pos + (look * planeDistance) + (up * (windowHeight / 2.0)) - (rightV * (windowWidth / 2.0));

	double du = windowWidth / (imageWidth * 1.0);
	double dv = windowHeight / (imageHeight * 1.0);

	// Choose middle of the grid cell
	topLeft = topLeft + (rightV * du / 2.0) - (up * dv / 2.0);

	int nearestObjectIndex = -1;
	double t, tMin;

	for (int i = 0; i < imageWidth; i++)
	{
		for (int j = 0; j < imageHeight; j++)
		{
			Point pixel = topLeft + (rightV * du * i) - (up * dv * j);
			Ray ray(pos, pixel - pos); // casting ray
			vector<double> color(3, 0);
			tMin = -1;
			nearestObjectIndex = -1;
			for (int k = 0; k < (int)objects.size(); k++)
			{
				t = objects[k]->intersect(ray, color, 0);
				if (t > 0 && (nearestObjectIndex == -1 || t < tMin))
					tMin = t, nearestObjectIndex = k;
			}
			if (nearestObjectIndex != -1)
			{
				vector<double> color(3, 0);
				double t = objects[nearestObjectIndex]->intersect(ray, color, 1);
				for (int y = 0; y < 3; y++)
				{
					if (color[y] > 1)
						color[y] = 1;
					if (color[y] < 0)
						color[y] = 0;
				}
				image.set_pixel(i, j, 255 * color[0], 255 * color[1], 255 * color[2]);
			}
		}
	}
	image.save_image("Output_now_" + to_string(imageCount) + ".bmp");
	cout << "Image is saved now!" << endl;
	imageCount++;
}

void keyboardListener(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '0':
		capture();
		break;
	case '1':
		// rotate LEFT
		rotate3D(rightV, up, ROT_ANG);
		rotate3D(look, up, ROT_ANG);
		break;
	case '2':
		// rotate right
		rotate3D(rightV, up, -ROT_ANG);
		rotate3D(look, up, -ROT_ANG);
		break;
	case '3':
		// rotate UP
		rotate3D(up, rightV, ROT_ANG);
		rotate3D(look, rightV, ROT_ANG);
		break;
	case '4':
		// rotate DOWN
		rotate3D(up, rightV, -ROT_ANG);
		rotate3D(look, rightV, -ROT_ANG);
		break;
	case '5':
		// tilt CLKWISE
		rotate3D(rightV, look, ROT_ANG);
		rotate3D(up, look, ROT_ANG);
		break;
	case '6':
		// tilt COUNTER CLKWISE
		rotate3D(rightV, look, -ROT_ANG);
		rotate3D(up, look, -ROT_ANG);
		break;
	default:
		break;
	}
}

void specialKeyListener(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_DOWN: // down arrow key
		pos = pos - look * 3;
		break;
	case GLUT_KEY_UP: // up arrow key
		pos = pos + look * 3;
		break;
	case GLUT_KEY_RIGHT:
		pos = pos + rightV * 3;
		break;
	case GLUT_KEY_LEFT:
		pos = pos - rightV * 3;
		break;
	case GLUT_KEY_PAGE_UP:
		pos = pos + up * 3;
		break;
	case GLUT_KEY_PAGE_DOWN:
		pos = pos - up * 3;
		break;
	default:
		break;
	}
}

void display()
{
	// clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0); // color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// initialize the matrix
	glLoadIdentity();

	gluLookAt(pos.x, pos.y, pos.z,
			  pos.x + look.x, pos.y + look.y, pos.z + look.z,
			  up.x, up.y, up.z);

	// again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);
	drawAxes();

	// Draw Objects
	for (int i = 0; i < objects.size(); i++)
	{
		Object *object = objects[i];
		object->draw();
	}
	// Draw Point Lights
	for (int i = 0; i < lights.size(); i++)
	{
		lights[i]->draw();
	}
	// Draw Spot Lights
	for (int i = 0; i < spotlights.size(); i++)
	{
		SpotLight *l = spotlights[i];
		spotlights[i]->draw();
	}
	glutSwapBuffers();
}

void animate()
{
	glutPostRedisplay();
}

void loadData()
{
	ifstream in("scene.txt");
	in >> recursionLevel >> imageHeight;
	imageWidth = imageHeight;

	int objCount;
	in >> objCount;

	for (int i = 0; i < objCount; i++)
	{
		string objType;
		in >> objType;

		Object *obj;

		if (objType == "sphere")
		{
			obj = new Sphere();
			in >> *((Sphere *)obj);
		}
		else if (objType == "triangle")
		{
			obj = new Triangle();
			in >> *((Triangle *)obj);
		}
		else if (objType == "general")
		{
			obj = new General();
			in >> *((General *)obj);
		}
		else
		{
			cout << objType << " is not a valid object" << endl;
		}
		objects.push_back(obj);
		// objects[objects.size()-1]->print();
	}

	int lightCount;
	in >> lightCount;

	for (int i = 0; i < lightCount; i++)
	{
		Light *light = new Light();
		in >> *light;
		lights.push_back(light);
		// lights[lights.size()-1]->print();
	}

	int spotlightCount;
	in >> spotlightCount;

	for (int i = 0; i < spotlightCount; i++)
	{
		SpotLight *spotlight = new SpotLight();
		in >> *spotlight;
		spotlights.push_back(spotlight);
		// spotlights[spotlights.size()-1]->print();
	}

	Object *floor;
	floor = new Floor(400, 10);
	floor->setColor(Color(0.5, 0.5, 0.5));
	vector<double> coefficients = {0.4, 0.2, 0.2, 0.2};
	floor->setCoefficients(coefficients);
	objects.push_back(floor);
}

void init()
{
	// codes for initialization
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	angle = 0;
	numSegments = 36;

	loadData();
	image = bitmap_image(imageWidth, imageHeight);
	glClearColor(0, 0, 0, 0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80, 1, 1, 1000.0);
}

int main(int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB); // Depth, Double buffer, RGB color

	glutCreateWindow("Ray Tracing Amar");

	init();

	glEnable(GL_DEPTH_TEST); // enable Depth Testing

	glutDisplayFunc(display); // display callback function
	glutIdleFunc(animate);

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMainLoop(); // The main loop of OpenGL

	objects.clear();
	objects.shrink_to_fit();

	lights.clear();
	lights.shrink_to_fit();

	spotlights.clear();
	spotlights.shrink_to_fit();

	return 0;
}
