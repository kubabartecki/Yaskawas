// Gl_template.c
//Wy��czanie b��d�w przed "fopen"
#define  _CRT_SECURE_NO_WARNINGS

// �adowanie bibliotek:
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif

// Ustalanie trybu tekstowego:
#ifdef _MSC_VER        // Check if MS Visual C compiler
#   ifndef _MBCS
#      define _MBCS    // Uses Multi-byte character set
#   endif
#   ifdef _UNICODE     // Not using Unicode character set
#      undef _UNICODE 
#   endif
#   ifdef UNICODE
#      undef UNICODE 
#   endif
#endif


#include <windows.h>            // Window defines
#include <gl\gl.h>              // OpenGL
#include <gl\glu.h>             // GLU library
#include <math.h>				// Define for sqrt
#include <stdio.h>
#include "resource.h"           // About box resource identifiers.

#define glRGB(x, y, z)	glColor3ub((GLubyte)x, (GLubyte)y, (GLubyte)z)
#define BITMAP_ID 0x4D42		// identyfikator formatu BMP
#define GL_PI 3.14

// Color Palette handle
HPALETTE hPalette = NULL;

// Application name and instance storeage
static LPCTSTR lpszAppName = "Yaskawas";
static HINSTANCE hInstance;

// Rotation amounts
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


static GLsizei lastHeight;
static GLsizei lastWidth;

// Opis tekstury
BITMAPINFOHEADER	bitmapInfoHeader;	// nag��wek obrazu
unsigned char*		bitmapData;			// dane tekstury
unsigned int		texture[2];			// obiekt tekstury


										// Declaration for Window procedure
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam);

// Dialog procedure for about box
BOOL APIENTRY AboutDlgProc(HWND hDlg, UINT message, UINT wParam, LONG lParam);

void RenderScene(void);

// Set Pixel Format function - forward declaration
void SetDCPixelFormat(HDC hDC);



// Reduces a normal vector specified as a set of three coordinates,
// to a unit normal vector of length one.
void ReduceToUnit(float vector[3])
{
	float length;

	// Calculate the length of the vector		
	length = (float)sqrt((vector[0] * vector[0]) +
		(vector[1] * vector[1]) +
		(vector[2] * vector[2]));

	// Keep the program from blowing up by providing an exceptable
	// value for vectors that may calculated too close to zero.
	if (length == 0.0f)
		length = 1.0f;

	// Dividing each element by the length will result in a
	// unit normal vector.
	vector[0] /= length;
	vector[1] /= length;
	vector[2] /= length;
}


// Points p1, p2, & p3 specified in counter clock-wise order
void calcNormal(float v[3][3], float out[3])
{
	float v1[3], v2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;

	// Calculate two vectors from the three points
	v1[x] = v[0][x] - v[1][x];
	v1[y] = v[0][y] - v[1][y];
	v1[z] = v[0][z] - v[1][z];

	v2[x] = v[1][x] - v[2][x];
	v2[y] = v[1][y] - v[2][y];
	v2[z] = v[1][z] - v[2][z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out
	out[x] = v1[y] * v2[z] - v1[z] * v2[y];
	out[y] = v1[z] * v2[x] - v1[x] * v2[z];
	out[z] = v1[x] * v2[y] - v1[y] * v2[x];

	// Normalize the vector (shorten length to one)
	ReduceToUnit(out);
}



// Change viewing volume and viewport.  Called when window is resized
void ChangeSize(GLsizei w, GLsizei h)
{
	GLfloat nRange = 300.0f; // extendsssss area that u can observe!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	GLfloat fAspect;
	// Prevent a divide by zero
	if (h == 0)
		h = 1;

	lastWidth = w;
	lastHeight = h;

	fAspect = (GLfloat)w / (GLfloat)h;
	// Set Viewport to window dimensions
	glViewport(0, 0, w, h);

	// Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Establish clipping volume (left, right, bottom, top, near, far)
	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	// Establish perspective: 
	
	//gluPerspective(40.0f,fAspect, 50.0,50);
	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}



// This function does any needed initialization on the rendering
// context.  Here it sets up and initializes the lighting for
// the scene.
void SetupRC()
{
	// Light values and coordinates
	GLfloat  ambientLight[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat  diffuseLight[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat	 lightPos[] = { 0.0f, 150.0f, 150.0f, 1.0f };
	GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };


	glEnable(GL_DEPTH_TEST);	// Hidden surface removal
	glFrontFace(GL_CCW);		// Counter clock-wise polygons face out
								glEnable(GL_CULL_FACE);		// Do not calculate inside of jet

								// Enable lighting
								glEnable(GL_LIGHTING);

								// Setup and enable light 0
								glLightfv(GL_LIGHT0,GL_AMBIENT,ambientLight);
								glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuseLight);
								glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
								glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
								glEnable(GL_LIGHT0);

								// Enable color tracking
								glEnable(GL_COLOR_MATERIAL);

								// Set Material properties to follow glColor values
								glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

								// All materials hereafter have full specular reflectivity
								// with a high shine
								glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
								glMateriali(GL_FRONT,GL_SHININESS,128);


								// White background
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Black brush
	glColor3f(0.0, 0.0, 0.0);
}

void skrzynka(void)
{
	glColor3d(0.8, 0.7, 0.3);


	glEnable(GL_TEXTURE_2D); // W��cz teksturowanie

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glNormal3d(0, 0, 1);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(-25, 25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(-25, -25, 25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, -25, 25);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glNormal3d(1, 0, 0);
	glTexCoord2d(1.0, 1.0); glVertex3d(25, 25, 25);
	glTexCoord2d(0.0, 1.0); glVertex3d(25, -25, 25);
	glTexCoord2d(0.0, 0.0); glVertex3d(25, -25, -25);
	glTexCoord2d(1.0, 0.0); glVertex3d(25, 25, -25);
	glEnd();

	glDisable(GL_TEXTURE_2D); // Wy��cz teksturowanie



	glBegin(GL_QUADS);
	glNormal3d(0, 0, -1);
	glVertex3d(25, 25, -25);
	glVertex3d(25, -25, -25);
	glVertex3d(-25, -25, -25);
	glVertex3d(-25, 25, -25);

	glNormal3d(-1, 0, 0);
	glVertex3d(-25, 25, -25);
	glVertex3d(-25, -25, -25);
	glVertex3d(-25, -25, 25);
	glVertex3d(-25, 25, 25);

	glNormal3d(0, 1, 0);
	glVertex3d(25, 25, 25);
	glVertex3d(25, 25, -25);
	glVertex3d(-25, 25, -25);
	glVertex3d(-25, 25, 25);

	glNormal3d(0, -1, 0);
	glVertex3d(25, -25, 25);
	glVertex3d(-25, -25, 25);
	glVertex3d(-25, -25, -25);
	glVertex3d(25, -25, -25);
	glEnd();
}

void walec01(void)
{
	GLUquadricObj*obj;
	obj = gluNewQuadric();
	gluQuadricNormals(obj, GLU_SMOOTH);
	//glColor3d(1, 0, 0);
	glPushMatrix();
	gluCylinder(obj, 20, 20, 30, 15, 7);
	gluCylinder(obj, 0, 20, 0, 15, 7);
	glTranslated(0, 0, 60);
	glRotated(180.0, 0, 1, 0);
	gluCylinder(obj, 0, 20, 30, 15, 7);
	glPopMatrix();
}

void kula(void)
{
	GLUquadricObj*obj;
	obj = gluNewQuadric();
	gluQuadricTexture(obj, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor3d(1.0, 0.8, 0.8);
	glEnable(GL_TEXTURE_2D);
	gluSphere(obj, 40, 15, 7);
	glDisable(GL_TEXTURE_2D);
}

void walec(double h, double r)
{
	double angle, x, y;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= (2.0f * GL_PI); angle += (GL_PI / 16.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex2d(x, y);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, h);
	for (angle = 0.0f; angle >= -(2.0f * GL_PI); angle -= (GL_PI / 16.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, h);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle >= -(2.0f * GL_PI); angle -= (GL_PI / 16.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x, y, h);
		glVertex3d(x, y, 0);
	}
	glEnd();
}


void prostopadloscian(float x, float y, float z) {
	//toward Y axis
	glBegin(GL_QUADS);

	x /= 2;
	z /= 2;

	glNormal3d(0, 0, 1);
	glVertex3d(x, y, z);
	glVertex3d(-x, y, z);
	glVertex3d(-x, 0, z);
	glVertex3d(x, 0, z);

	glNormal3d(1, 0, 0);
	glVertex3d(x, y, z);
	glVertex3d(x, 0, z);
	glVertex3d(x, 0, -z);
	glVertex3d(x, y, -z);

	glNormal3d(0, 0, -1);
	glVertex3d(x, y, -z);
	glVertex3d(x, 0, -z);
	glVertex3d(-x, 0, -z);
	glVertex3d(-x, y, -z);

	glNormal3d(-1, 0, 0);
	glVertex3d(-x, y, -z);
	glVertex3d(-x, 0, -z);
	glVertex3d(-x, 0, z);
	glVertex3d(-x, y, z);

	glNormal3d(0, 1, 0);
	glVertex3d(x, y, z);
	glVertex3d(x, y, -z);
	glVertex3d(-x, y, -z);
	glVertex3d(-x, y, z);

	glNormal3d(0, -1, 0);
	glVertex3d(x, 0, z);
	glVertex3d(-x, 0, z);
	glVertex3d(-x, 0, -z);
	glVertex3d(x, 0, -z);
	glEnd();
}

void box(float x, float y, float z) {
	//toward Y axis
	float thickness = 1.0f;

	prostopadloscian(x, thickness, z);

	glPushMatrix();
		glTranslated(0, y / 2.0f, z / 2.0f);
		glRotated(-90, 1, 0, 0);
		prostopadloscian(x, thickness, y);

		glTranslated(0, z, 0);
		glRotated(180, 1, 0, 0);
		prostopadloscian(x, thickness, y);
	glPopMatrix();
	
	glPushMatrix();
		glTranslated(x / 2.0f, y / 2.0f, 0);
		glRotated(-90, 0, 0, 1);
		prostopadloscian(y, thickness, z);

		glTranslated(0, -x, 0);
		glRotated(180, 1, 0, 0);
		prostopadloscian(y, thickness, z);
	glPopMatrix();
}

void box_top(float x, float z) {
	prostopadloscian(x, 1.0f, z);
}

void semicircleZ(float r, float h) {
	//toward Z axis

	//edit
	double angle, x, y;
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = GL_PI / 2; angle <= (GL_PI * 1.50f); angle += (GL_PI / 16.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex2d(x, y);
	}
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, h);
	for (angle = GL_PI * 1.5f; angle >= (GL_PI / 2.00f - 0.01f); angle -= (GL_PI / 16.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glVertex3d(x, y, h);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = GL_PI * 1.5f; angle >= (GL_PI / 2.00f - 0.01f); angle -= (GL_PI / 16.0f))
	{
		x = r * sin(angle);
		y = r * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x, y, h);
		glVertex3d(x, y, 0);
	}
	glEnd();
}

void ramie(double r1, double r2, double h, double d) {
	double angle, x, y;

	// 1 polowa walca ----------------------------
	//--------------------------------------------
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(0.0f, 0.0f, 0.0f);
	for (angle = GL_PI; angle <= (2.0f * GL_PI); angle += (GL_PI / 16.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glVertex3d(x, y, 0.0);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = GL_PI; angle <= (2.0f * GL_PI); angle += (GL_PI / 16.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x, y, 0);
		glVertex3d(x, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0f, 0.0f, h);
	for (angle = 0.0f; angle >= (-1.0f * GL_PI); angle -= (GL_PI / 16.0f))
	{
		x = r1 * sin(angle);
		y = r1 * cos(angle);
		glVertex3d(x, y, h);
	}
	glEnd();

	// 2 mniejsza polowa walca ----------------------------
	//--------------------------------------------
	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(d, 0.0f, 0.0f);
	for (angle = 0.0f; angle <= (1.0f * GL_PI); angle += (GL_PI / 16.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glVertex3d(x + d, y, 0.0);
	}
	glEnd();
	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle <= GL_PI; angle += (GL_PI / 16.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glNormal3d(sin(angle), cos(angle), 0.0);
		glVertex3d(x + d, y, 0);
		glVertex3d(x + d, y, h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(d, 0.0f, h);
	for (angle = GL_PI; angle >= -0.01f; angle -= (GL_PI / 16.0f))
	{
		x = r2 * sin(angle);
		y = r2 * cos(angle);
		glVertex3d(x + d, y, h);
	}
	glEnd();

	//connection walls
	float norm[3];
	float v[3][3] = {
		{0, -r1, 0},
		{d, -r2, 0},
		{d, -r2, h}
	};
	
	calcNormal(v, norm);
	glBegin(GL_QUADS);


	glNormal3d(norm[0], norm[1], norm[2]);
	glVertex3d(0, -r1, 0);
	glVertex3d(d, -r2, 0);
	glVertex3d(d, -r2, h);
	glVertex3d(0, -r1, h);

	glNormal3d(0.0f, 0.0f, 1.0f);
	glVertex3d(0, -r1, h);
	glVertex3d(d, -r2, h);
	glVertex3d(d, r2, h);
	glVertex3d(0, r1, h);

	float v2[3][3] = {
		{0, r1, h},
		{d, r2, h},
		{d, r2, 0}
	};

	calcNormal(v2, norm);

	glNormal3d(norm[0], norm[1], norm[2]);
	glVertex3d(0, r1, h);
	glVertex3d(d, r2, h);
	glVertex3d(d, r2, 0);
	glVertex3d(0, r1, 0);

	glNormal3d(0.0f, 0.0f, -1.0f);
	glVertex3d(0, r1, 0);
	glVertex3d(d, r2, 0);
	glVertex3d(d, -r2, 0);
	glVertex3d(0, -r1, 0);
	glEnd();

}

int base(int x, int y, int z, int y2) {
	prostopadloscian(x, y, z);

	float x2 = 0.9f * x;
	float radius = x2 / 2.0f;
	float front = 0.05f * z + radius;
	float z2 = z - front;
	float move_z = (z - z2) / 2.0f;
	glTranslated(0, y, -move_z);
	prostopadloscian(x2, y2, z2); //TODO? moze jakies wciecie

	glTranslated(0, 0, z2 / 2);
	glRotated(-90, 1, 0, 0);
	semicircleZ(radius, y2);

	return radius;
}

float scara_rot_1 = -90.0f, scara_rot_2 = 0.0f, scara_picker = 0.0f;
int ScaraPicked = 1;
float SCARA_PICKER_DISTANCE = 12.0f;

void picker(float r, float h, float h2, float cube_side) {
	float init_h = h2 / 2.0f + h * 0.5f + scara_picker - 3.0f;
	glTranslated(0, r * 0.5f, 0);
	glRotated(180, 1, 0, 0);

	glPushMatrix();

	glTranslated(0, 0, -init_h);
	glColor3d(0.0, 0.0, 0.0); //black
	float radius_ring = r * 0.9f * 0.5f;
	float height_ring = h * 0.05f;
	float radius_picker = radius_ring * 0.5f;
	walec(height_ring, radius_ring);

	glTranslated(0, 0, height_ring);
	glColor3d(0.7, 0.7, 0.7); //grey
	walec(h - height_ring, radius_picker);
	if (ScaraPicked) {
		glColor3d(0.0, 0.0, 0.0); //black
		glPushMatrix();
			glTranslated(0, 0, h - height_ring);
			glRotated(90, 1, 0, 0);
			prostopadloscian(cube_side, cube_side, cube_side);
		glPopMatrix();
	}


	glPopMatrix();
	glTranslated(0, 0, h2);
	glColor3d(0.0, 0.0, 0.0); //black
	walec(height_ring, radius_ring);
	
}
void scara(int x_base, int z_base, float cube_side) {
	glColor3d(0.2, 0.6, 0.9); // light blue
	glPushMatrix();
	//base
	float y2 = 30.0f;
	float radius1, radius2;
	radius1 = base(x_base, 8, z_base, y2);

	glTranslated(0, 0, y2);
	glColor3d(0.0, 0.0, 0.0); //black
	float h_walca = 2.0f;
	walec(h_walca, radius1);
	//radius1 *= 0.95f;
	
	// 1 arm
	glColor3d(0.2, 0.6, 0.9); // light blue
	glTranslated(0, 0, h_walca);
	glRotated(scara_rot_1, 0, 0, 1);
	float y3 = 7.0f, d1 = 0.71f * z_base;
	radius2 = radius1 * 0.7f;
	ramie(radius1, radius2, y3, d1);

	glTranslated(d1, 0, y3);
	glRotated(scara_rot_2, 0, 0, 1);

	// 2 arm
	glColor3d(0.0, 0.0, 0.0); //black
	float d2 = d1 * 0.8f;
	ramie(radius1, radius2, y3, d2);
	glTranslated(0, 0, y3);

	// top on 2 arm
	glColor3d(0.2, 0.6, 0.9); // light blue
	float y4 = y3 * 1.5f;
	ramie(radius1, radius2, y4, d2);
	glTranslated(0, 0, y4);
	float y5 = y4 + y3;
	glRotated(-90, 0, 0, 1);
	semicircleZ(radius1, y5);

	//top1 slope
	float middle_top = (radius1 + radius2 + d2) / 2.0f - radius1; //cord, not lenght
	float x_new = radius1 - (radius1 - radius2) / d2 * middle_top;
	float norm[3];
	float v[3][3] = {
		{radius1, 0, y5},
		{x_new, middle_top, y5},
		{-x_new, middle_top, y5}
	};
	float v2[3][3] = {
		{x_new, middle_top, y5},
		{radius2, d2, 0},
		{-x_new, middle_top, y5}
	};
	calcNormal(v, norm);
	glNormal3d(norm[0], norm[1], norm[2]);
	glBegin(GL_QUADS);
		glVertex3d(radius1, 0, y5);
		glVertex3d(x_new, middle_top, y5);
		glVertex3d(-x_new, middle_top, y5);
		glVertex3d(-radius1, 0, y5);

		calcNormal(v2, norm);
		glNormal3d(norm[0], norm[1], norm[2]);
		glVertex3d(x_new, middle_top, y5);
		glVertex3d(radius2, d2, 0);
		glVertex3d(-radius2, d2, 0);
		glVertex3d(-x_new, middle_top, y5);

	//sides
	float v3[3][3] = {
		{-radius2, d2, 0},
		{-radius1, 0, 0},
		{-radius1, 0, y5}
	};
	float v4[3][3] = {
		{radius1, 0, y5},
		{radius1, 0, 0},
		{radius2, d2, 0}
	};
		calcNormal(v3, norm);
		glNormal3d(norm[0], norm[1], norm[2]);
		glVertex3d(-x_new, middle_top, y5);
		glVertex3d(-radius2, d2, 0);
		glVertex3d(-radius1, 0, 0);
		glVertex3d(-radius1, 0, y5);
		
		calcNormal(v4, norm);
		glNormal3d(norm[0], norm[1], norm[2]);

		glVertex3d(radius1, 0, y5);
		glVertex3d(radius1, 0, 0);
		glVertex3d(radius2, d2, 0);
		glVertex3d(x_new, middle_top, y5);
		
		// -- top2slope --
		float radius3 = radius2 * 0.65f;
		float y6 = y5 / 2.0f;
		float x_new2 = radius3 + (radius1 - radius2) / d2 * (middle_top - radius3);
		float v5[3][3] = {
		{-x_new2, middle_top, y5},
		{x_new2, middle_top, y5},
		{x_new2, d2, y6}
		};
		calcNormal(v5, norm);
		glNormal3d(norm[0], norm[1], norm[2]);
		glVertex3d(-x_new2, middle_top, y5);
		glVertex3d(x_new2, middle_top, y5);
		glVertex3d(radius3, d2, y6);
		glVertex3d(-radius3, d2, y6);
	glEnd();

	// sides of top2slope
	//edit
	float v6[3][3] = {
		{x_new2, middle_top, y5},
		{radius3, d2, 0},
		{radius3, d2, y6},
	};
	calcNormal(v6, norm);
	glNormal3d(norm[0], norm[1], norm[2]);
	glBegin(GL_TRIANGLES);
		glVertex3d(x_new2, middle_top, y5);
		glVertex3d(radius3, d2, 0);
		glVertex3d(radius3, d2, y6);
	glEnd();

	float v7[3][3] = {
		{-x_new2, middle_top, y5},
		{-radius3, d2, y6},
		{-radius3, d2, 0},
		
	};
	calcNormal(v7, norm);
	glNormal3d(norm[0], norm[1], norm[2]);
	glBegin(GL_TRIANGLES);
		glVertex3d(-x_new2, middle_top, y5);
		glVertex3d(-radius3, d2, y6);
		glVertex3d(-radius3, d2, 0);
	glEnd();

	glTranslated(0,d2,0);
	glPushMatrix();
		glRotated(180, 0, 0, 1);
		semicircleZ(radius3, y6);
	glPopMatrix();

	// picker
	float picker_h = 90.0f; //TODO overall height
	picker(radius3, picker_h, y5, cube_side);
	glPopMatrix();
}
float CONV_TIME = 5.0f; // okres ruchu klocka o 1 pozycje na tasmie
float scara_time = 0.0f; // to samo wsm tylko zmienne
float cube_move = 0.0f; // ruch klocka na tasmie
// zakladam ze sa 4 pozycje na tasmie i scara zabbiera klocek z 3 pozycji i odklada do pudla
float conveyor(float w, float h, float l) {

	glColor3d(0.7, 0.7, 0.7); //grey
	glPushMatrix();
		glRotated(90, 1, 0, 0);
		prostopadloscian(l, w, h);
	glPopMatrix();
	
	glPushMatrix();
		glTranslated(-l / 2.0f, 0, 0); // x axis was towards left
		glRotated(-90, 0, 0, 1);
		semicircleZ(h / 2.0f, w);
	glPopMatrix();

	glPushMatrix();
		glTranslated(l / 2.0f, 0, 0);
		glRotated(90, 0, 0, 1);
		semicircleZ(h / 2.0f, w);
	glPopMatrix();

	float cube_side = w / 5.0f;
	glTranslated(cube_move, h / 2.0f, w / 2.0f);
	glColor3d(0.0, 0.0, 0.0); //black
	prostopadloscian(cube_side, cube_side, cube_side);
	glTranslated(-l / 4.0f, 0, 0);
	prostopadloscian(cube_side, cube_side, cube_side);
	glTranslated(-l / 4.0f, 0, 0);
	prostopadloscian(cube_side, cube_side, cube_side);

	return cube_side;
}

float cobot_rot1 = 0.0f;
float cobot_rot2 = 0.0f;
float cobot_rot3 = 0.0f;
float cobot_rot4 = 0.0f;
float cobot_rot5 = 0.0f;
float cobot_rot6 = 0.0f;

float cobot_time = 0.0f;
float COBOT_TIME = 8.0f;
int PickedTop = 0;
int PickedBox = 0;

void cobot(float box_side_x, float box_side_y, float box_side_z) {
	glColor3d(1.0, 0.9, 0.9); //ala walter white
	float base_x = 50.0f;
	float base_z = 50.0f;
	float base_y = 5.0f;
	prostopadloscian(base_x, base_y, base_z);
	
	GLUquadricObj* obj;
	obj = gluNewQuadric();
	gluQuadricNormals(obj, GLU_SMOOTH);
	glPushMatrix();
		glTranslated(0, base_y, 0);
		glRotated(-90, 1, 0, 0);

		float base_x2 = base_x * 0.85f;
		float base_y2 = base_y * 5;
		gluCylinder(obj, base_x / 2.0f, base_x2 / 2.0f , base_y2, 15, 7);

		glTranslated(0, 0, base_y2);
		glRotated(cobot_rot1, 0, 0, 1); // 1 rot
		walec(base_y2, base_x2 / 2.0f);

		glTranslated(0, 0, base_y2);
		glColor3d(0.2, 0.6, 0.9); // light blue
		gluSphere(obj, base_x2 / 2.0f, 15, 7);

		
		glTranslated(0, 0, -base_y2 / 2.0f);
		glRotated(-90, 0, 1, 0);
		glRotated(-cobot_rot2, 0, 0, 1); // 2 rot
		float h3 = base_y2 * 2.5f;
		float r3 = base_x2 / 2.3f; 
		glColor3d(1.0, 0.9, 0.9); //ala walter white
		walec(h3, r3);
		glTranslated(0, 0, h3);
		glColor3d(0.2, 0.6, 0.9); // light blue
		gluSphere(obj, r3, 15, 7);

		glTranslated(0, 0, -h3 * 0.4f);
		glRotated(90, 0, 1, 0);
		float r4 = r3 * 0.8f;
		float h4 = base_y2 * 7.0f;
		glColor3d(1.0, 0.9, 0.9); //ala walter white
		gluCylinder(obj, r3, r4, h4, 15, 7);
		glTranslated(-r4 * 1.5f, 0, h4);
		glRotated(-90, 0, 1, 0);
		glColor3d(0.2, 0.6, 0.9); // light blue
		gluSphere(obj, r4, 15, 7);
		glRotated(180, 0, 1, 0);
		glColor3d(1.0, 0.9, 0.9); //ala walter white
		float h5 = h3;
		float r5 = r4;
		walec(h5, r5);

		glTranslated(0, 0, h5);
		glRotated(-cobot_rot3, 0, 0, 1); //3 rot todo spr
		float h6 = h5 / 2.0f; // h6 is just for rotation
		float r6 = r5;
		walec(h6, r6);
		glTranslated(0, 0, h6);
		glColor3d(0.2, 0.6, 0.9); // light blue
		gluSphere(obj, r6, 15, 7);

		float h7 = h4 * 0.7f;
		float r7 = r6;
		float r8 = 0.75f * r7;
		glTranslated(0, 0, -h6 / 2.0f);
		glRotated(90, 0, 1, 0);
		glRotated(-cobot_rot4, 0, 0, 1); // 4 rot todo spr
		glColor3d(1.0, 0.9, 0.9); // ala walter white
		gluCylinder(obj, r7, r8, h7, 15, 7);

		float h8 = h6 * 0.8f;
		glTranslated(0, 0, h7);
		glRotated(90, 0, 1, 0);
		glTranslated(0, 0, h8 / 2.0f);
		glColor3d(0.2, 0.6, 0.9); // light blue
		gluSphere(obj, r8, 15, 7);
		glRotated(180, 0, 1, 0);
		glColor3d(1.0, 0.9, 0.9); //ala walter white
		walec(h8, r8);

		glTranslated(0, 0, h8);
		glRotated(-cobot_rot5, 0, 0, 1); //5 rot spr
		float h9 = h8 / 2.0f;
		float r9 = r8;
		walec(h9, r9);
		float h10 = h8;
		float r10 = r8;
		glTranslated(-h10 / 2.0f, 0, h9 + r10 / 2.0f);
		glRotated(-90, 0, 1, 0);
		glColor3d(0.2, 0.6, 0.9); // light blue
		gluSphere(obj, r10, 15, 7);
		glRotated(180, 0, 1, 0);
		glColor3d(1.0, 0.9, 0.9); //ala walter white
		walec(h10, r10);
		glTranslated(0, 0, h10);
		
		float h11 = h10 / 2.0f;
		float r11 = r10 * 0.8f;
		gluCylinder(obj, r10, r11, h11, 15, 7);
		glTranslated(0, 0, h11);
		glRotated(cobot_rot6, 0, 0, 1); //6 rot spr

		float h12 = h11 * 0.5f;
		float r12 = r11;
		glColor3d(0.7, 0.7, 0.7); //grey
		walec(h12, r12);

		//przyssawka
		glTranslated(0, 0, h12);

		float h13 = h12 * 0.6f;
		float x13 = r12 * 2.0f;
		float z13 = r12 * 3.0f;
		glColor3d(0, 0, 0); // black
		glRotated(90, 1, 0, 0);
		prostopadloscian(x13, h13, z13);

		if (PickedTop) {
			glTranslated(0, h13, 0);
			glColor3d(0.6, 0.5, 0.3); //brown
			box_top(box_side_x, box_side_z);
		}

	glPopMatrix();

}
// LoadBitmapFile
// opis: �aduje map� bitow� z pliku i zwraca jej adres.
//       Wype�nia struktur� nag��wka.
//	 Nie obs�uguje map 8-bitowych.
unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							// wska�nik pozycji pliku
	BITMAPFILEHEADER	bitmapFileHeader;		// nag��wek pliku
	unsigned char		*bitmapImage;			// dane obrazu
	int					imageIdx = 0;		// licznik pikseli
	unsigned char		tempRGB;				// zmienna zamiany sk�adowych

												// otwiera plik w trybie "read binary"
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// wczytuje nag��wek pliku
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// sprawdza, czy jest to plik formatu BMP
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// wczytuje nag��wek obrazu
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// ustawia wska�nik pozycji pliku na pocz�tku danych obrazu
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// przydziela pami�� buforowi obrazu
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// sprawdza, czy uda�o si� przydzieli� pami��
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// wczytuje dane obrazu
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// sprawdza, czy dane zosta�y wczytane
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// zamienia miejscami sk�adowe R i B 
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// zamyka plik i zwraca wska�nik bufora zawieraj�cego wczytany obraz
	fclose(filePtr);
	return bitmapImage;
}

float conveyor_len = 200.0f;
int cube_in_box = 0;
int MAX_CUBES_IN_BOX = 1;
int ClosingBox = 0;
int ClosedBox = 0;
// Called to draw scene
void RenderScene(void)
{
	//float normal[3];	// Storeage for calculated surface normal

	// Clear the window with current clearing color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Save the matrix state and do the rotations
	glPushMatrix();
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);

	/////////////////////////////////////////////////////////////////
	// MIEJSCE NA KOD OPENGL DO TWORZENIA WLASNYCH SCEN:		   //
	/////////////////////////////////////////////////////////////////

	//Spos�b na odr�nienie "przedniej" i "tylniej" �ciany wielok�ta:
	glPolygonMode(GL_BACK, GL_LINE);

	//Uzyskanie siatki:
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//Wyrysowanie prostokata:
	//glRectd(-10.0, -10.0, 20.0, 20.0);
	
	float conveyor_wid = 50.0f;
	conveyor_len = 200.0f;
	float conveyor_h = 15.0f;
	int x_base = 50, z_base = 70;
	float scara_z = -(conveyor_wid + z_base * 0.5f);
	float cube_side;
	// init point
	glPushMatrix();
		glTranslated(conveyor_len / 2.0f, conveyor_h / 2.0f, 0);
		glRotated(180, 0, 1, 0);
		cube_side = conveyor(conveyor_wid, conveyor_h, conveyor_len);
	glPopMatrix();

	glPushMatrix();
		// wąwóz tybetyjski																								  |do tad 2 ramiona
		float picker_z = -conveyor_wid - 0.05f * (float)z_base - 0.9f * 0.5f * (float)x_base + 0.71 * (float)z_base * 1.8f + 0.5f * 0.65f * 0.7f * 0.5f * 0.9f * (float)x_base;
		glTranslated(conveyor_len / 4.0f, 0, scara_z - abs(picker_z + conveyor_wid * 0.5f));
		scara(x_base, z_base, cube_side);
	glPopMatrix();

	//box
	float box_side_x = 0.8f * x_base;
	float box_side_z = 0.8f * z_base;
	float box_side_y = 20.0f;
	glPushMatrix();
		glTranslated(-x_base / 3, 0, -(conveyor_wid + (float)z_base * 1.5f));
		glColor3d(0.6, 0.5, 0.3); //brown
		box(box_side_x, box_side_y, box_side_z);
		if (cube_in_box == 2) {
			glTranslated(0, 0, -cube_side / 2.0f);
		}
		else if (cube_in_box == 3) {
			glTranslated(0, 0, -cube_side);
		}
		for (int i = 0; i < cube_in_box; ++i) {
			glColor3d(0.0, 0.0, 0.0); //black
			prostopadloscian(cube_side, cube_side, cube_side);
			glTranslated(0, 0, cube_side + 2.0f);
		}
	glPopMatrix();

	//// --------- cobot
	// sheets
	glColor3d(0.6, 0.5, 0.3); //brown
	glPushMatrix();
		glTranslated(-100, 0, -box_side_x / 2.0f); //todo zanik arkuszy
		glRotated(90, 0, 1, 0);
		for (int i = 0; i < 10; ++i) {
			box_top(box_side_z, box_side_x);
			glTranslated(0, 1, 0);
		}
	glPopMatrix();

	//robot
	glTranslated(-150, 0, -200);
	cobot(box_side_x, box_side_y, box_side_z);

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Flush drawing commands
	glFlush();
}


// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
		1,                                                              // Version of this structure    
		PFD_DRAW_TO_WINDOW |                    // Draw to Window (not to bitmap)
		PFD_SUPPORT_OPENGL |					// Support OpenGL calls in window
		PFD_DOUBLEBUFFER,                       // Double buffered
		PFD_TYPE_RGBA,                          // RGBA Color mode
		24,                                     // Want 24bit color 
		0,0,0,0,0,0,                            // Not used to select mode
		0,0,                                    // Not used to select mode
		0,0,0,0,0,                              // Not used to select mode
		32,                                     // Size of depth buffer
		0,                                      // Not used to select mode
		0,                                      // Not used to select mode
		PFD_MAIN_PLANE,                         // Draw in main plane
		0,                                      // Not used to select mode
		0,0,0 };                                // Not used to select mode

												// Choose a pixel format that best matches that described in pfd
	nPixelFormat = ChoosePixelFormat(hDC, &pfd);

	// Set the pixel format for the device context
	SetPixelFormat(hDC, nPixelFormat, &pfd);
}



// If necessary, creates a 3-3-2 palette for the device context listed.
HPALETTE GetOpenGLPalette(HDC hDC)
{
	HPALETTE hRetPal = NULL;	// Handle to palette to be created
	PIXELFORMATDESCRIPTOR pfd;	// Pixel Format Descriptor
	LOGPALETTE *pPal;			// Pointer to memory for logical palette
	int nPixelFormat;			// Pixel format index
	int nColors;				// Number of entries in palette
	int i;						// Counting variable
	BYTE RedRange, GreenRange, BlueRange;
	// Range for each color entry (7,7,and 3)


	// Get the pixel format index and retrieve the pixel format description
	nPixelFormat = GetPixelFormat(hDC);
	DescribePixelFormat(hDC, nPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	// Does this pixel format require a palette?  If not, do not create a
	// palette and just return NULL
	if (!(pfd.dwFlags & PFD_NEED_PALETTE))
		return NULL;

	// Number of entries in palette.  8 bits yeilds 256 entries
	nColors = 1 << pfd.cColorBits;

	// Allocate space for a logical palette structure plus all the palette entries
	pPal = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + nColors * sizeof(PALETTEENTRY));

	// Fill in palette header 
	pPal->palVersion = 0x300;		// Windows 3.0
	pPal->palNumEntries = nColors; // table size

								   // Build mask of all 1's.  This creates a number represented by having
								   // the low order x bits set, where x = pfd.cRedBits, pfd.cGreenBits, and
								   // pfd.cBlueBits.  
	RedRange = (1 << pfd.cRedBits) - 1;
	GreenRange = (1 << pfd.cGreenBits) - 1;
	BlueRange = (1 << pfd.cBlueBits) - 1;

	// Loop through all the palette entries
	for (i = 0; i < nColors; i++)
	{
		// Fill in the 8-bit equivalents for each component
		pPal->palPalEntry[i].peRed = (i >> pfd.cRedShift) & RedRange;
		pPal->palPalEntry[i].peRed = (unsigned char)(
			(double)pPal->palPalEntry[i].peRed * 255.0 / RedRange);

		pPal->palPalEntry[i].peGreen = (i >> pfd.cGreenShift) & GreenRange;
		pPal->palPalEntry[i].peGreen = (unsigned char)(
			(double)pPal->palPalEntry[i].peGreen * 255.0 / GreenRange);

		pPal->palPalEntry[i].peBlue = (i >> pfd.cBlueShift) & BlueRange;
		pPal->palPalEntry[i].peBlue = (unsigned char)(
			(double)pPal->palPalEntry[i].peBlue * 255.0 / BlueRange);

		pPal->palPalEntry[i].peFlags = (unsigned char)NULL;
	}


	// Create the palette
	hRetPal = CreatePalette(pPal);

	// Go ahead and select and realize the palette for this device context
	SelectPalette(hDC, hRetPal, FALSE);
	RealizePalette(hDC);

	// Free the memory used for the logical palette structure
	free(pPal);

	// Return the handle to the new palette
	return hRetPal;
}


// Entry point of all Windows programs
int APIENTRY WinMain(HINSTANCE       hInst,
	HINSTANCE       hPrevInstance,
	LPSTR           lpCmdLine,
	int                     nCmdShow)
{
	MSG                     msg;            // Windows message structure
	WNDCLASS        wc;                     // Windows class structure
	HWND            hWnd;           // Storeage for window handle

	hInstance = hInst;

	// Register Window style
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	// No need for background brush for OpenGL window
	wc.hbrBackground = NULL;

	wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wc.lpszClassName = lpszAppName;

	// Register the window class
	if (RegisterClass(&wc) == 0)
		return FALSE;


	// Create the main application window
	hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,

		// OpenGL requires WS_CLIPCHILDREN and WS_CLIPSIBLINGS
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,

		// Window position and size
		50, 50,
		700, 700,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If window was not created, quit
	if (hWnd == NULL)
		return FALSE;


	// Display the window
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Process application messages until the application closes
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


//times ms
UINT SCARA_ELAPSE = 50;
int Cobot = 0;
int Scara = 1;
// Window procedure, handles all messages for this program
LRESULT CALLBACK WndProc(HWND    hWnd,
	UINT    message,
	WPARAM  wParam,
	LPARAM  lParam)
{
	static HGLRC hRC;               // Permenant Rendering context
	static HDC hDC;                 // Private GDI Device context

	////distances per 50ms
	//scara
	float CONV_TIMER_RATIO = SCARA_ELAPSE / (CONV_TIME * 1000.0f);
	float picker_dist = SCARA_PICKER_DISTANCE * (CONV_TIMER_RATIO / 0.2f);
	float scara_dist = 80.0f * (CONV_TIMER_RATIO / 0.5f);
	
	//cobot
	float COBOT_TIMER_RATIO = SCARA_ELAPSE / (COBOT_TIME * 1000.0f);
	// 1
	// w 75 e 50 g 25 
	float cobot_dist_1w = 48.0f * (COBOT_TIMER_RATIO * 6.0f); //		1/6 of peroid
	float cobot_dist_1e = 70.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_1t = 20.0f * (COBOT_TIMER_RATIO * 6.0f);

	float cobot_dist_2q = 50.0f * (COBOT_TIMER_RATIO * 6.0f); //		above scara
	float cobot_dist_2w = 50.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_2e = 30.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_2r = 50.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_2t = 50.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_2y = 50.0f * (COBOT_TIMER_RATIO * 6.0f);

	float cobot_dist_3q = 30.0f * (COBOT_TIMER_RATIO * 6.0f); //		to the box
	float cobot_dist_3w = 30.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_3e = 40.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_3r = 50.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_3t = 50.0f * (COBOT_TIMER_RATIO * 6.0f);
	float cobot_dist_3y = 50.0f * (COBOT_TIMER_RATIO * 6.0f);

	switch (message)
	{
	case WM_TIMER:
		if (wParam == 101)
		{	
			
			if (Scara) {
				if (scara_time >= (CONV_TIME / 2.0f) && ScaraPicked) {
					ScaraPicked = 0;
					cube_in_box += 1;
					if (cube_in_box % MAX_CUBES_IN_BOX == 0) {
						ClosingBox = 1;
					}
				}

				if (scara_time < CONV_TIME * 0.2f) {
					scara_picker += picker_dist; // up 1st time
				}
				else if (scara_time < CONV_TIME * 0.5f && scara_time > CONV_TIME * 0.3f) {
					scara_picker -= picker_dist;// down 1st
				}
				else if (scara_time < CONV_TIME * 0.7f && scara_time >= CONV_TIME * 0.5f) { // second condition to protect from move while gap
					scara_picker += picker_dist;// up 2nd
				}
				else if (scara_time < CONV_TIME && scara_time > CONV_TIME * 0.8f) {
					scara_picker -= picker_dist;// down 2nd
				}

				if (scara_time < CONV_TIME * 0.5f) {
					scara_rot_1 -= scara_dist;
					scara_rot_2 -= scara_dist;
				}
				else {
					scara_rot_1 += scara_dist;
					scara_rot_2 += scara_dist;
				}

				if (scara_time >= CONV_TIME) {
					cube_move = 0.0f;
					scara_time = 0.0f;
					scara_picker = 0.0f; // sometimes it needs adjustment
					ScaraPicked = 1; //cube attached to scara

					scara_rot_1 = -90.0f;
					scara_rot_2 = 0.0f;

					if (ClosingBox) {
						Scara = 0;
						Cobot = 1;
					}
				}
			}

			// cobot
			if (ClosedBox) {
				Scara = 1;
				ClosingBox = 0;
				ClosedBox = 0;
				cube_in_box = 0;
			}
			if (Cobot) {
				if (cobot_time >= COBOT_TIME) {
					cobot_time = 0.0f;
					Cobot = 0;
				}

				else if (cobot_time >= COBOT_TIME) {
				

				}
				else if (cobot_time >= COBOT_TIME / 2.0f) {		// picked box
					if (!ClosedBox) {
						ClosedBox = 1;
					}
					if (PickedTop) {
						PickedTop = 0;
					}
					if (!PickedBox) {
						PickedBox = 1;
					}
					
				}
				else if (cobot_time >= COBOT_TIME / 3.0f) {		// above scara
					cobot_rot1 += cobot_dist_3q;
					cobot_rot2 += cobot_dist_3w;
					cobot_rot3 -= cobot_dist_3e;
					cobot_rot4 += cobot_dist_3r;
					cobot_rot5 += cobot_dist_3t;
					cobot_rot6 += cobot_dist_3y;
				}
				else if (cobot_time >= COBOT_TIME / 6.0f) {		//picked top 
					if (!PickedTop) {
						PickedTop = 1;
					}
					cobot_rot1 += cobot_dist_2q;
					cobot_rot2 -= cobot_dist_2w;
					cobot_rot3 += cobot_dist_2e;
					cobot_rot4 += cobot_dist_2r;
					cobot_rot5 += cobot_dist_2t;
					cobot_rot6 += cobot_dist_2y;

				}
				else if (cobot_time >= 0.0f) {		// go to tops
					cobot_rot2 += cobot_dist_1w;
					cobot_rot3 += cobot_dist_1e;
					cobot_rot5 -= cobot_dist_1t;
				}
			}
			
			
			
			

			if (Scara) {
				cube_move += (conveyor_len / 4.0f * CONV_TIMER_RATIO); // * 50ms
				scara_time += (float)SCARA_ELAPSE / 1000.0f;
			}

			if (Cobot) {
				cobot_time += (float)SCARA_ELAPSE / 1000.0f;
			}

			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
		// Window creation, setup for OpenGL
	case WM_CREATE:
		//timer
		SetTimer(hWnd, 101, SCARA_ELAPSE, NULL);

		// Store the device context
		hDC = GetDC(hWnd);

		// Select the pixel format
		SetDCPixelFormat(hDC);

		// Create palette if needed
		hPalette = GetOpenGLPalette(hDC);

		// Create the rendering context and make it current
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		SetupRC();
		glGenTextures(2, &texture[0]);                  // tworzy obiekt tekstury			

														// �aduje pierwszy obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\checker.bmp", &bitmapInfoHeader);

		glBindTexture(GL_TEXTURE_2D, texture[0]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// �aduje drugi obraz tekstury:
		bitmapData = LoadBitmapFile("Bitmapy\\crate.bmp", &bitmapInfoHeader);
		glBindTexture(GL_TEXTURE_2D, texture[1]);       // aktywuje obiekt tekstury

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// tworzy obraz tekstury
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmapInfoHeader.biWidth,
			bitmapInfoHeader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmapData);

		if (bitmapData)
			free(bitmapData);

		// ustalenie sposobu mieszania tekstury z t�em
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		break;

		// Window is being destroyed, cleanup
	case WM_DESTROY:
		//timer
		KillTimer(hWnd, 101);

		// Deselect the current rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		// Delete the palette if it was created
		if (hPalette != NULL)
			DeleteObject(hPalette);

		// Tell the application to terminate after the window
		// is gone.
		PostQuitMessage(0);
		break;

		// Window is resized.
	case WM_SIZE:
		// Call our function which modifies the clipping
		// volume and viewport
		ChangeSize(LOWORD(lParam), HIWORD(lParam));
		break;


		// The painting function.  This message sent by Windows 
		// whenever the screen needs updating.
	case WM_PAINT:
	{
		// Call OpenGL drawing code
		RenderScene();

		SwapBuffers(hDC);

		// Validate the newly painted client area
		ValidateRect(hWnd, NULL);
	}
	break;

	// Windows is telling the application that it may modify
	// the system palette.  This message in essance asks the 
	// application for a new palette.
	case WM_QUERYNEWPALETTE:
		// If the palette was created.
		if (hPalette)
		{
			int nRet;

			// Selects the palette into the current device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries from the currently selected palette to
			// the system palette.  The return value is the number 
			// of palette entries modified.
			nRet = RealizePalette(hDC);

			// Repaint, forces remap of palette in current window
			InvalidateRect(hWnd, NULL, FALSE);

			return nRet;
		}
		break;


		// This window may set the palette, even though it is not the 
		// currently active window.
	case WM_PALETTECHANGED:
		// Don't do anything if the palette does not exist, or if
		// this is the window that changed the palette.
		if ((hPalette != NULL) && ((HWND)wParam != hWnd))
		{
			// Select the palette into the device context
			SelectPalette(hDC, hPalette, FALSE);

			// Map entries to system palette
			RealizePalette(hDC);

			// Remap the current colors to the newly realized palette
			UpdateColors(hDC);
			return 0;
		}
		break;

		// Key press, check for arrow keys to do cube rotation.
	case WM_KEYDOWN:
	{
		if (wParam == VK_UP)
			xRot -= 5.0f;

		if (wParam == VK_DOWN)
			xRot += 5.0f;

		if (wParam == VK_LEFT)
			yRot -= 5.0f;

		if (wParam == VK_RIGHT)
			yRot += 5.0f;

		xRot = (const int)xRot % 360;
		yRot = (const int)yRot % 360;

		if (wParam == '1')
			scara_rot_1 -= 5.0f;

		if (wParam == '2')
			scara_rot_1 += 5.0f;

		if (wParam == '3')
			scara_rot_2 -= 5.0f;

		if (wParam == '4')
			scara_rot_2 += 5.0f;

		if (wParam == '5')
			scara_picker -= 5.0f;

		if (wParam == '6')
			scara_picker += 5.0f;

		//cobot
		if (wParam == VkKeyScanA('q'))
			cobot_rot1 += 5.0f;

		if (wParam == VkKeyScanA('w'))
			cobot_rot2 += 5.0f;

		if (wParam == VkKeyScanA('e'))
			cobot_rot3 += 5.0f;

		if (wParam == VkKeyScanA('r'))
			cobot_rot4 += 5.0f;

		if (wParam == VkKeyScanA('t'))
			cobot_rot5 += 5.0f;

		if (wParam == VkKeyScanA('y'))
			cobot_rot6 += 5.0f;

		if (wParam == VkKeyScanA('a'))
			cobot_rot1 -= 5.0f;

		if (wParam == VkKeyScanA('s'))
			cobot_rot2 -= 5.0f;

		if (wParam == VkKeyScanA('d'))
			cobot_rot3 -= 5.0f;

		if (wParam == VkKeyScanA('f'))
			cobot_rot4 -= 5.0f;

		if (wParam == VkKeyScanA('g'))
			cobot_rot5 -= 5.0f;

		if (wParam == VkKeyScanA('h'))
			cobot_rot6 -= 5.0f;

		InvalidateRect(hWnd, NULL, FALSE);
	}
	break;

	// A menu command
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
			// Exit the program
		case ID_FILE_EXIT:
			DestroyWindow(hWnd);
			break;
		}
	}
	break;


	default:   // Passes it on if unproccessed
		return (DefWindowProc(hWnd, message, wParam, lParam));

	}

	return (0L);
}
