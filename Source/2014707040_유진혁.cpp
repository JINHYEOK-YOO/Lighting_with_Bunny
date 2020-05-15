#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

class Vector;
class Position	// Coordinate of points
{ 
public:
	GLfloat x, y, z;
	const Vector CreateVector(const Position& p);	// Computing a vector between two points
	const Position operator+(const Vector& v);	// Move the point by the value of the vector
};
class Vector : public Position	// Coordinate of vectors
{
public:
	void Normalize();	// Normalize a vector
	const Vector operator*(const Vector & v);	// Cross product
	const Vector operator/(int n);	// Dividing the coordinate values of the vector into constants
	Vector& operator+=(const Vector& v);	// Adding the value of the vector
	Vector& operator/=(int n);	// Dividing the coordinate values of the vector into constants
};
const Vector Position::CreateVector(const Position& p)	// Computing a vector between two points
{
	Vector v;
	v.x = p.x - this->x;
	v.y = p.y - this->y;
	v.z = p.z - this->z;
	return v;
}
const Position Position::operator+(const Vector& v)	// Move the point by the value of the vector
{
	Position r;
	r.x = this->x + v.x;
	r.y = this->y + v.y;
	r.z = this->z + v.z;
	return r;
}
void Vector::Normalize()	// Normalize a vector
{
	GLfloat d = x * x + y * y + z * z;
	if (d > 0)
	{
		GLfloat len = (1.0f / sqrt(d));
		x *= len;
		y *= len;
		z *= len;
	}
}
const Vector Vector::operator*(const Vector & v)	// Cross product
{
	Vector r;
	r.x = this->y * v.z - this->z * v.y;
	r.y = this->z * v.x - this->x * v.z;
	r.z = this->x * v.y - this->y * v.x;
	r.Normalize();
	return r;
}
const Vector Vector::operator/(int n)	// Dividing the coordinate values of the vector into constants
{
	Vector r;
	r.x = this->x / n;
	r.y = this->y / n;
	r.z = this->z / n;
	return r;
}
Vector& Vector::operator+=(const Vector& v)	// Adding the value of the vector
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}
Vector& Vector::operator/=(int n)	// Dividing the coordinate values of the vector into constants
{
	this->x /= n;
	this->y /= n;
	this->z /= n;
	return *this;
}

vector<Position> p;	// Coordinates of points
vector<Vector> n;	// Normal vectors of points
vector<int> t;		//	Vertices of triangles
vector<int> cnt;	// The number of triangles connected to the vertex
static float thetaP = 0;	// Point light rotation angle
static float thetaD = 0;	// Directional light rotation angle
static float thetaS = 0;	// Spot light rotation angle
static GLfloat spotShine = 1.0f;	// The shininess of the spot light  (0 ~ 1)
static GLfloat spotRange = 5.0f;	// The cut-off angle of the spot light (5 ~ 25)
static bool toggleVertexNormal = false;
static bool toggleRotationPoint = false;
static bool toggleRotationDirectional = false;
static bool toggleRotationSpot = false;
static bool toggleSpotCutOff = false;
static bool toggleSpotShininess = false;
static int signCutOff = -1;
static int signShininess = 1;

// Draw axes
void DrawAxis()
{
	glBegin(GL_LINES);
	{
		glColor3f(1, 0, 0);		// red x-axis
		glVertex3f(0, 0, 0);
		glVertex3f(5, 0, 0);

		glColor3f(0, 1, 0);		// green y-axis
		glVertex3f(0, 0, 0);
		glVertex3f(0, 5, 0);

		glColor3f(0, 0, 1);		// blue z-axis
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 5);

	}
	glEnd();
}

// Draw normal vectors of points
void DrawVertexNormal()
{
	glColor3f(0, 0, 0);	// black
	glBegin(GL_LINES);
	{
		for (size_t i = 0; i < p.size(); i++)
		{
			glVertex3f(p[i].x, p[i].y, p[i].z);
			glVertex3f((p[i] + (n[i] / 10)).x, (p[i] + (n[i] / 10)).y, (p[i] + (n[i] / 10)).z);
		}
	}
	glEnd();
}

// Draw the Stanford Bunny
void DrawBunny()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBegin(GL_TRIANGLES);
	{
		for (size_t i = 0; i < t.size(); i++)
		{
			glNormal3f(n[t[i]].x, n[t[i]].y, n[t[i]].z);
			glVertex3f(p[t[i]].x, p[t[i]].y, p[t[i]].z);
		}
	}
	glEnd();
}

// Set lighting and material, Render bunny
void RenderBunny()
{
	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(1.0, 1.0, 1.0, 0, 0, 0, 0.0, 1.0, 0.0);

	DrawAxis();

	// Turn on the lights
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	// Point light
	glEnable(GL_LIGHT1);	// Directional light
	glEnable(GL_LIGHT2);	// Spot light

	// Point light
	glPushMatrix();
	glRotatef(thetaP, 1, 1, 1);	// Rotate light
	GLfloat positionP[4] = { -0.224745f, 1.61237f, 1.61237f, 1 };
	GLfloat ambientP[4] = { 0, 0, 0, 1 };
	GLfloat diffuseP[4] = { 1, 0, 0, 1 };
	GLfloat specularP[4] = { 1, 0, 0, 1 };	// The red light is point light

	glLightfv(GL_LIGHT0, GL_POSITION, positionP);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientP);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseP);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularP);

	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.2);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05);
	glPopMatrix();
	//

	// Directional light
	glPushMatrix();
	glRotatef(thetaD, 1, 1, 1);	// Rotate light
	GLfloat positionD[4] = { -0.224745f, 1.61237f, 1.61237f, 0 };
	GLfloat ambientD[4] = { 0, 0, 0, 1 };
	GLfloat diffuseD[4] = { 0, 1, 0, 1 };
	GLfloat specularD[4] = { 0, 1, 0, 1 };	// The green light is directional light

	glLightfv(GL_LIGHT1, GL_POSITION, positionD);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientD);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseD);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularD);
	glPopMatrix();
	//

	// Spot light
	glPushMatrix();
	glRotatef(thetaS, 1, 1, 1);	// Rotate light
	GLfloat positionS[4] = { -0.224745f, 1.61237f, 1.61237f, 1 };
	GLfloat ambientS[4] = { 0, 0, 0, 1 };
	GLfloat diffuseS[4] = { 0, 0, spotShine, 1 };
	GLfloat specularS[4] = { 0, 0, spotShine, 1 };	// The blue light is spot light

	glLightfv(GL_LIGHT2, GL_POSITION, positionS);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientS);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseS);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularS);

	GLfloat spot_direction[3] = { 0.224745f, -1.61237f, -1.61237f };
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, spotRange);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 1.0);
	glPopMatrix();
	//

	// Specifying a material of the Stanford bunny
	GLfloat mat_ambient[4] = { 0, 0, 0, 1 };
	GLfloat mat_diffuse[4] = { 1, 1, 1, 1 };
	GLfloat mat_specular[4] = { 1, 1, 1, 1 };
	GLfloat mat_shininess = 20.0;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);
	//

	DrawBunny();

	// Turn off the lights
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);	// Point light
	glDisable(GL_LIGHT1);	// Directional light
	glDisable(GL_LIGHT2);	// Spot light

	// Draw the geometry of lights
	glPushMatrix();
	glColor3f(1, 0, 0);	// Red point light
	GLUquadric* sphere = gluNewQuadric(); // Create a sphere
	glRotatef(thetaP, 1, 1, 1);	// Rotate the sphere
	glTranslatef(-0.224745f, 1.61237f, 1.61237f);	// Translate the sphere
	gluSphere(sphere, 0.15f, 20, 20);  // Draw the sphere 
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 1, 0);	// Green Directional light
	GLUquadric* cylinder = gluNewQuadric();	// Create a cylinder
	glRotatef(thetaD, 1, 1, 1);	// Rotate the cylinder
	glTranslatef(-0.224745f, 1.61237f, 1.61237f);	// Translate the cylinder
	glRotatef(-45, 1, 0, 0);	//	Rotate to look at the bunny
	gluCylinder(cylinder, 0.2f, 0.2f, 0.4f, 20, 20);	// Draw the cylinder
	glPopMatrix();

	glPushMatrix();
	glColor3f(0, 0, 1);	// Blue spot light
	GLUquadric* cylinderZeroTop = gluNewQuadric();	// Create a cylinder with zero top
	glRotatef(thetaS, 1, 1, 1);	// Rotate the cylinder
	glTranslatef(-0.224745f, 1.61237f, 1.61237f);	// Translate the cylinder
	glRotatef(-45, 1, 0, 0);	// Rotate to look at the bunny
	gluCylinder(cylinderZeroTop, 0.1f, 0, 0.8f, 20, 20);	// Draw the cylinder
	glPopMatrix();
	//

	if (toggleVertexNormal)
		DrawVertexNormal();

	glutSwapBuffers();
}

// Setup the rendering state
void SetupRC(void)
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

// Called by GLUT library when the window has changed size
void ChangeSize(int w, int h)
{
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	// Set viewport to window dimension
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h)
		glOrtho(-2.0, 2.0, -2.0*(float)h / (float)w, 2.0*(float)h / (float)w, -10.0, 10.0);
	else
		glOrtho(-2.0*(float)w / (float)h, 2.0*(float)w / (float)h, -2.0, 2.0, -10.0, 10.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'v':
		if (toggleVertexNormal)
			toggleVertexNormal = false;
		else
			toggleVertexNormal = true;
		glutPostRedisplay();
		break;
	case 'p':
		if (toggleRotationPoint)
			toggleRotationPoint = false;
		else
			toggleRotationPoint = true;
		glutPostRedisplay();
		break;
	case 'd':
		if (toggleRotationDirectional)
			toggleRotationDirectional = false;
		else
			toggleRotationDirectional = true;
		glutPostRedisplay();
		break;
	case 's':
		if (toggleRotationSpot)
			toggleRotationSpot = false;
		else
			toggleRotationSpot = true;
		glutPostRedisplay();
		break;
	case 'c':
		if (toggleSpotCutOff)
			toggleSpotCutOff = false;
		else
			toggleSpotCutOff = true;
		glutPostRedisplay();
		break;
	case 'n':
		if (toggleSpotShininess)
			toggleSpotShininess = false;
		else
			toggleSpotShininess = true;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void timer(int value)
{
	if (toggleRotationPoint)
	{
		thetaP += 1.0;
		if (thetaP >= 360.0)
			thetaP -= 360.0;
	}

	if (toggleRotationDirectional)
	{
		thetaD += 1.0;
		if (thetaD >= 360.0)
			thetaD -= 360.0;
	}

	if (toggleRotationSpot)
	{
		thetaS += 1.0;
		if (thetaS >= 360.0)
			thetaS -= 360.0;
	}

	if (toggleSpotCutOff)
	{
		if (spotRange >= 25 || spotRange <= 5)
			signCutOff *= -1;
		spotRange += signCutOff * 0.111f;
	}

	if (toggleSpotShininess)
	{
		if (spotShine <= 0.0f || spotShine >= 1.0f)
			signShininess *= -1;
		spotShine += signShininess * 0.006f;
	}

	glutPostRedisplay();
	glutTimerFunc(1000 / 90, timer, 1);
}

// Main program entry point
void main(int argc, char* argv[])
{
	ifstream ifs("bunny_origin.txt", ifstream::binary);	// Read a file 'bunny_origin.txt'
	if (ifs.is_open())
	{
		int points, triangles;
		ifs >> points;	// The number of points
		ifs >> triangles;	// The number of triangles
		p.resize(points);
		n.resize(points);
		cnt.resize(points);
		t.resize(3 * triangles);
		// The coordinates of the points are input
		for (int i = 0; i < points; i++)
		{
			ifs >> p[i].x;
			ifs >> p[i].y;
			ifs >> p[i].z;
		}
		// The points of the triangle are input
		for (int i = 0; i < 3 * triangles; i++)
		{
			ifs >> t[i];
			t[i]--;
		}
		for (int i = 0; i < 3 * triangles; i += 3)
		{
			// Find and assign normal vectors of planes
			Vector v1, v2, r;
			v1 = p[t[i]].CreateVector(p[t[i + 1]]);
			v2 = p[t[i]].CreateVector(p[t[i + 2]]);
			r = v1 * v2;
			n[t[i]] += r;
			n[t[i + 1]] += r;
			n[t[i + 2]] += r;
			// Assign the number of triangles connected to the vertex
			cnt[t[i]]++;
			cnt[t[i + 1]]++;
			cnt[t[i + 2]]++;
		}
		// Computing normal vectors of points
		for (int i = 0; i < points; i++)
			n[i] /= cnt[i];
	}
	else
	{
		cout << "Error opening bunny_origin.txt" << endl;
		exit(0);
	}
	ifs.close();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL HW 2");
	glutDisplayFunc(RenderBunny);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(1000 / 90, timer, 1);

	SetupRC();

	glutMainLoop();
}