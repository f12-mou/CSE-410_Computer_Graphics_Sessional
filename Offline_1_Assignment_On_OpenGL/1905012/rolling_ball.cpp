#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#elif WIN32
#include <windows.h>
#include <glut.h>
#endif

using namespace std;
int drawaxes;
int c = 0;
double cameraHeight;
double cameraAngle;
double arrowX, arrowY, arrowZ;
double angleArrow = 0.0;
double translateX = 0.0;
double translateY = 0.0;
double translateX2 = 0.0;
double translateY2 = 0.0;
double draw_translateX2 = 0.0;
double draw_translateY2 = 0.0;
double lol = 0.0;
double arrowX2 = 0, arrowY2 = 0;
double rotateAngle = 0;
double ball_radius = 10;
int simulation_mode = 0;
double pos_x, pos_y, pos_z, l_x, l_y, l_z, r_x, r_y, r_z, u_x, u_y, u_z;

vector<pair<double, double>> vecDraw;
vector<pair<double, double>> vecTranslate;
vector<pair<double, double>> vecTranslate2;
vector<pair<double, double>> vecArrow;
int currIdx = 0;

#define pi (2 * acos(0.0))

class point
{
public:
	double x;
	double y;
	double z;
	point()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	point(double a, double b, double c)
	{
		x = a;
		y = b;
		z = c;
	}
	void setValues(double a, double b, double c)
	{
		x = a;
		y = b;
		x = c;
	}
};

point pos;
point u, r, l;

point addPoints(point a, point b)
{
	point t;
	t.x = a.x + b.x;
	t.y = a.y + b.y;
	t.z = a.z + b.z;
	return t;
}

point subtractPoints(point a, point b)
{
	point t;
	t.x = a.x - b.x;
	t.y = a.y - b.y;
	t.z = a.z - b.z;
	return t;
}
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

double degree_to_rad(double angle)
{
	return (pi / 180.0) * angle;
}

point multiplyPoint(point p, double c)
{
	point t;
	t.x = p.x * c;
	t.y = p.y * c;
	t.z = p.z * c;
	return t;
}

void init()
{
	printf("Do your initialization here\n");
	pos.x = 150.0;
	pos.y = 150.0;
	pos.z = 30.0;
	u.x = 0;
	u.y = 0;
	u.z = 1;
	r.x = -(1.0 / sqrt(2.0));
	r.y = 1.0 / sqrt(2.0);
	r.z = 0;
	l.x = -1.0 / sqrt(2);
	l.y = -1.0 / sqrt(2);
	l.z = 0;
	cameraHeight = 150.0;
	cameraAngle = 1.0;
	drawaxes = 1;
	arrowX = 40.0;
	arrowY = 10.0;
	translateX2 = 250.0;
	translateY2 = 250.0;
	translateX = 20.0;
	translateY = 20.0;
	arrowZ = 0.0;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
	setPoint3D(1, 5, 0, 2);
	setPoint3D(2, -1, 0, 0);
	setPoint3D(3, 0, 1, 0);
	setPoint3D(4, 0, 0, 1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(80, 1, 1, 1000.0);
}

void drawAxes()
{
	if (drawaxes == 1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);
		{
			glColor3f(1.0, 0, 0);
			glVertex3f(200, 0, 0);
			glVertex3f(-200, 0, 0);

			glColor3f(0, 1.0, 0);
			glVertex3f(0, -200, 0);
			glVertex3f(0, 200, 0);
			glColor3f(0, 0, 1.0);
			glVertex3f(0, 0, 200);
			glVertex3f(0, 0, -200);
		}
		glEnd();
	}
	glLineWidth(8);
	glBegin(GL_LINES);
	{
		glVertex3f(10, 10, 0);
		glVertex3f(10, 150, 0);

		glVertex3f(10, 150, 0);
		glVertex3f(150, 150, 0);

		glVertex3f(150, 150, 0);
		glVertex3f(150, 10, 0);

		glVertex3f(150, 10, 0);

		glVertex3f(10, 10, 0);
	}
	glEnd();
	glColor3f(1.0, 1.0, 1.0);
}
void drawArrow()
{
	glBegin(GL_LINES);
	{
		glVertex3f(0, 0, 0);
		glVertex3f(arrowX, arrowY, arrowZ);
	}
	glEnd();
}

// a openGL integer
GLint counter = 0;

double _rand()
{
	return (double)rand() / RAND_MAX;
}

pair<double, double> calc_draw_arrow(double x1, double y1, double x2, double y2)
{
	double now = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
	double now_x = x1 + ((x2 - x1) / now) * 30;
	double now_y = y1 + ((y2 - y1) / now) * 30;
	return {now_x, now_y};
}
float lineColor[4] = {1.0, 256, 400.0, 50.0};

std::vector<int> indices;
std::vector<float> vertices;
std::vector<int> lineIndices;
int k1, k2;

void prepVerticesFunc()
{
	float x, y, z, xy; // vertex position
	double radius = ball_radius;
	int sectorCount = 36;
	int stackCount = 18;
	float sectorStep = 2 * pi / sectorCount;
	float stackStep = pi / stackCount;
	float sectorAngle, stackAngle;
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
}

void display()
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0); // color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// upto
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(pos_x, pos_y, pos_z, pos_x + l_x, pos_y + l_y, pos_z + l_z, u_x, u_y, u_z);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	drawAxes();
	glPushMatrix();

	for (int i = -5; i < 200; i++)
	{
		for (int j = -5; j < 200; j++)
		{
			if ((i + j) % 2 == 0)
				glColor3f(1, 1, 1);
			else
				glColor3f(0, 0, 0);
			glPushMatrix();
			glTranslatef(i * 30, j * 30, 0);
			glBegin(GL_QUADS);
			{
				glVertex3f(0, 0, -100);
				glVertex3f(0, 30, -100);
				glVertex3f(30, 30, -100);
				glVertex3f(30, 0, -100);
			}
			glEnd();
			glPopMatrix();
		}
	}
	glPopMatrix();

	glLineWidth(5.0);
	glColor3f(1, 0, 0);
	glBegin(GL_LINES);
	{
		glVertex3f(translateX, translateY, 0);
		glVertex3f(draw_translateX2, draw_translateY2, 0);
	}
	glEnd();
	glLineWidth(1);

	glTranslatef(translateX, translateY, 0);
	glPushMatrix();
	glRotatef(rotateAngle, -(translateY2 - translateY), (translateX2 - translateX), 0);
	// sphere here
	int k1, k2;
	int sectorCount = 36;
	int stackCount = 18;
	int flag = 1;
	for (int i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1); // beginning of current stack
		k2 = k1 + sectorCount + 1;	// beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
		{

			if (flag == 1)
			{
				flag = 2;
				glColor3f(1, 0, 0);
			}
			else
			{
				flag = 1;
				glColor3f(0, 1, 0);
			}
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
		if (flag == 1)
		{
			flag = 2;
			glColor3f(1, 0, 0);
		}
		else
		{
			flag = 1;
			glColor3f(0, 1, 0);
		}
	}
	glColor3f(0, 0, 0);
	glPopMatrix();
	glPopMatrix();
	glColor3f(0, 0, 0);

	glutSwapBuffers();
}

int check_boundary()
{
	if (((10 <= translateX - ball_radius) && (translateX + ball_radius <= 150)) && (translateY - ball_radius <= 10))
	{
		cout << "returning 1" << endl;
		return 1;
	}
	else if (((10 <= translateX - ball_radius) && (translateX + ball_radius <= 150)) && (translateY + ball_radius >= 150))
	{
		cout << "returning 2" << endl;
		return 2;
	}
	else if (((10 <= translateY - ball_radius) && (translateY + ball_radius <= 150)) && (translateX - ball_radius <= 10))
	{
		cout << "returning 3" << endl;
		return 3;
	}
	else if (((10 <= translateY - ball_radius) && (translateY + ball_radius <= 150)) && (translateX + ball_radius >= 150))
	{
		cout << "returning 4" << endl;
		return 4;
	}
	else
	{
		return -1;
	}
}

double get_dot_product(double a1, double a2, double a3, double unit_1, double unit_2, double unit_3)
{
	return a1 * unit_1 + a2 * unit_2 + a3 * unit_3;
}

point get_reflection(double a1, double a2, double a3, double unit_1, double unit_2, double unit_3)
{
	double c = get_dot_product(a1, a2, a3, unit_1, unit_2, unit_3);
	c = c * 2;
	point p(unit_1, unit_2, unit_3);
	point a(a1, a2, a3);
	point now = multiplyPoint(p, -c);
	point ans = addPoints(now, a);
	return ans;
}

void calc_final_draw_points(int caseNo)
{
	double unit_1, unit_2, unit_3;
	if (caseNo == -1)
	{
		return;
	}
	else if (caseNo == 1)
	{
		unit_1 = 0;
		unit_2 = 1;
		unit_3 = 0;
	}
	else if (caseNo == 2)
	{
		unit_1 = 0;
		unit_2 = -1;
		unit_3 = 0;
	}
	else if (caseNo == 3)
	{
		unit_1 = 1;
		unit_2 = 0;
		unit_3 = 0;
	}
	else if (caseNo == 4)
	{
		unit_1 = -1;
		unit_2 = 0;
		unit_3 = 0;
	}
	point now = get_reflection((translateX2 - translateX), (translateY2 - translateY), 0, unit_1, unit_2, unit_3);
	double here = sqrt(now.x * now.x + now.y * now.y + now.z * now.z);
	translateX2 = translateX + now.x;
	translateY2 = translateY + now.y;
}

void eventSchedule()
{
	cout << "in event scheduler" << endl;
	vecArrow.clear();
	vecTranslate.clear();
	vecTranslate2.clear();
	vecDraw.clear();
	double now_arrowX2 = arrowX2;
	double now_arrowY2 = arrowY2;
	double now_translateX2 = translateX2;
	double now_translateY2 = translateY2;
	double now_translateX = translateX;
	double now_translateY = translateY;
	double now_draw_translateX2 = draw_translateX2;
	double now_draw_translateY2 = draw_translateY2;
	for (int i = 0; i < 400; i++)
	{
		double prevX = now_arrowX2;
		double prevY = now_arrowY2;
		double now;
		pair<double, double> p;
		int caseno;
		now_arrowX2 = now_translateX2;
		now_arrowY2 = now_translateY2;
		now = sqrt(((now_arrowX2 - now_translateX) * (now_arrowX2 - now_translateX)) + ((now_arrowY2 - now_translateY) * (now_arrowY2 - now_translateY)));
		now_translateX += (2.0 * ((now_arrowX2 - now_translateX) / now));
		now_translateY += (2.0 * ((now_arrowY2 - now_translateY) / now));
		now_translateX2 += (2.0 * ((now_arrowX2 - now_translateX) / now));
		now_translateY2 += (2.0 * ((now_arrowY2 - now_translateY) / now));

		p = calc_draw_arrow(now_translateX, now_translateY, now_translateX2, now_translateY2);
		now_draw_translateX2 = p.first;
		now_draw_translateY2 = p.second;
		// vector pushing
		vecDraw.push_back({now_draw_translateX2, now_draw_translateY2});
		vecTranslate.push_back({now_translateX, now_translateY});
		vecTranslate2.push_back({now_translateX2, now_translateY2});
		vecArrow.push_back({now_arrowX2, now_arrowY2});
	}
	currIdx = 0;
}
void eventSchedulerHelper(int hehe)
{
	if (simulation_mode == 1)
	{
		currIdx++;
		rotateAngle += 10;
		draw_translateX2 = vecDraw[currIdx].first;
		draw_translateY2 = vecDraw[currIdx].second;
		translateX2 = vecTranslate2[currIdx].first;
		translateY2 = vecTranslate2[currIdx].second;
		translateX = vecTranslate[currIdx].first;
		translateY = vecTranslate[currIdx].second;
		arrowX = vecArrow[currIdx].first;
		arrowY = vecArrow[currIdx].second;
		int caseNo = check_boundary();
		calc_final_draw_points(caseNo);
		if (caseNo != -1) // jhamela, bari khaise kothau
		{
			eventSchedule();
		}
		glutTimerFunc(50, eventSchedulerHelper, 0);
	}
}

void keyboardListener(unsigned char key, int x, int y)
{
	double prevX = arrowX;
	double prevY = arrowY;
	double now;
	pair<double, double> p;
	int caseno;
	double rate = 0.1;
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
	case 'j':
		angleArrow = (10.0);
		rotateAngle = 0;
		arrowX2 = ((translateX2) * (cos(degree_to_rad(angleArrow))) - translateY2 * (sin(degree_to_rad(angleArrow))));
		arrowY2 = (translateX2) * (sin(degree_to_rad(angleArrow))) + translateY2 * cos(degree_to_rad(angleArrow));
		translateX2 = arrowX2;
		translateY2 = arrowY2;
		p = calc_draw_arrow(translateX, translateY, translateX2, translateY2);
		draw_translateX2 = p.first;
		draw_translateY2 = p.second;
		check_boundary();
		eventSchedule();
		break;
	case 'l':
		angleArrow = -(10.0);
		rotateAngle = 0;
		arrowX2 = ((translateX2) * (cos(degree_to_rad(angleArrow))) - translateY2 * (sin(degree_to_rad(angleArrow))));
		arrowY2 = (translateX2) * (sin(degree_to_rad(angleArrow))) + translateY2 * cos(degree_to_rad(angleArrow));
		translateX2 = arrowX2;
		translateY2 = arrowY2;
		p = calc_draw_arrow(translateX, translateY, translateX2, translateY2);
		draw_translateX2 = p.first;
		draw_translateY2 = p.second;
		check_boundary();
		eventSchedule();
		break;
	case 'i':
		arrowX2 = translateX2;
		arrowY2 = translateY2;
		now = sqrt(((arrowX2 - translateX) * (arrowX2 - translateX)) + ((arrowY2 - translateY) * (arrowY2 - translateY)));
		translateX += (1.0 * ((arrowX2 - translateX) / now));
		translateY += (1.0 * ((arrowY2 - translateY) / now));
		translateX2 += (1.0 * ((arrowX2 - translateX) / now));
		translateY2 += (1.0 * ((arrowY2 - translateY) / now));
		rotateAngle += 10;
		caseno = check_boundary();
		calc_final_draw_points(caseno); // translateX2 and translateY2 calculated
		p = calc_draw_arrow(translateX, translateY, translateX2, translateY2);
		draw_translateX2 = p.first;
		draw_translateY2 = p.second;
		break;
	case 'k':
		rotateAngle -= 10;
		arrowX2 = translateX2;
		arrowY2 = translateY2;
		now = sqrt(((arrowX2 - translateX) * (arrowX2 - translateX)) + ((arrowY2 - translateY) * (arrowY2 - translateY)));
		translateX -= (1.0 * ((arrowX2 - translateX) / now));
		translateY -= (1.0 * ((arrowY2 - translateY) / now));
		translateX2 -= (1.0 * ((arrowX2 - translateX) / now));
		translateY2 -= (1.0 * ((arrowY2 - translateY) / now));
		caseno = check_boundary();
		calc_final_draw_points(caseno); // translateX2 and translateY2 calculated
		p = calc_draw_arrow(translateX, translateY, translateX2, translateY2);
		draw_translateX2 = p.first;
		draw_translateY2 = p.second;
		break;
		break;
	case 32:
		if (simulation_mode == 0)
		{
			simulation_mode = 1;
			eventSchedule();
			glutTimerFunc(50, eventSchedulerHelper, 0);
		}
		else
		{
			simulation_mode = 0;
		}
		break;

	default:
		printf("We don't know what you pressed\n");
		break;
	}
}

void setPosition(double x, double y, double z)
{
	pos_x = pos_x + x;
	pos_y = pos_y + y;
	pos_z = pos_z + z;
}

void specialKeyListener(int key, int x, int y)
{
	double prevX = arrowX;
	double prevY = arrowY;
	double now;
	pair<double, double> p;
	int caseno;

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
	prepVerticesFunc();
	eventSchedule(); // 400 points done
	// cout << "total points are: " << indices.size() << endl;
	glutInit(&argc, argv);
	glutInitWindowSize(600, 400);	  // Set the window's initial width & height
	glutInitWindowPosition(750, 250); // Position the window's initial top-left corner
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Test");
	init();
	glEnable(GL_DEPTH_TEST); // enable Depth Testing
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutTimerFunc(50, eventSchedulerHelper, 0);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}