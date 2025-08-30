////////////////////////////////////////////////////////////
// Name: Chin Zhen Jie
// Extra functions I used: glEnable(), glStencilFunc(), glStencilOp(), glColorMask(), glutStrokeCharacter()
// Theres probably more...
///////////////////////////////////////////////////////////

// CS3241 Assignment 1: Doodle
#include <cmath>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include "GL/glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

using namespace std;

GLfloat PI = 3.14;
float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0;

// Hardcode the coordinates of a pentagon for computational efficiency.
// Drawn in clockwise manner.
float PENTAGON_CENTER_X = 4.0f;
float PENTAGON_CENTER_Y = 16.0f;
float PENTAGON_RADIUS = 5.0f;
float PENTAGON_ANGLE_ONE = 18.0f * M_PI / 180.0f;
float PENTAGON_ANGLE_TWO = 54.0f * M_PI / 180.0f;

float PENTAGON_COORDS[5][2] = {
	{0.0f, PENTAGON_RADIUS},
	{PENTAGON_RADIUS * cos(PENTAGON_ANGLE_ONE), PENTAGON_RADIUS * sin(PENTAGON_ANGLE_ONE)},
	{PENTAGON_RADIUS * cos(PENTAGON_ANGLE_TWO), -PENTAGON_RADIUS * sin(PENTAGON_ANGLE_TWO)},
	{-PENTAGON_RADIUS * cos(PENTAGON_ANGLE_TWO), -PENTAGON_RADIUS * sin(PENTAGON_ANGLE_TWO)},
	{-PENTAGON_RADIUS * cos(PENTAGON_ANGLE_ONE), PENTAGON_RADIUS * sin(PENTAGON_ANGLE_ONE)},
};

// Variables for iris.
int SEGMENTS = 100;

// Function pointer.
typedef void (*DrawShapeFunc)(float centerX, float centerY, float radius, int segments,
	float red, float green, float blue);

void drawCircleOutline(float centerX, float centerY, float radius, int segments,
        float red, float green, float blue) {

    glLineWidth(3.0f);  // Set line thickness
    glBegin(GL_LINE_LOOP);
    	glColor3f(red, green, blue);
		for (int a = 0; a < 360; a += 360 / segments) {
			float position = a * M_PI / 180.0f;
			float x = centerX + radius * cos(position);
			float y = centerY + radius * sin(position);
			glVertex2f(x, y);
		}
    glEnd();

    glLineWidth(1.0f);
}

void drawCircle(float centerX, float centerY, float radius, int segments,
		float red, float green, float blue) {
    glBegin(GL_POLYGON);  
		glColor3f(red, green, blue);
        for (int a = 0; a < 360; a += 360 / segments) {
			float position = a * M_PI / 180.0f;
			float x = centerX + radius * cos(position);
			float y = centerY + radius * sin(position);
			glVertex2f(x, y);
		}
    glEnd();
}

// Draws four layers of iris,, one overlaying the other
void drawIris() {
	drawCircle(PENTAGON_CENTER_X, PENTAGON_CENTER_Y, 6.5, SEGMENTS,
			1.0f, 1.0f, 1.0f); // Last layer
	drawCircle(PENTAGON_CENTER_X, PENTAGON_CENTER_Y, 5.0, SEGMENTS,
			0.77f, 0.14f, 0.09f); // Third layer
	drawCircle(PENTAGON_CENTER_X, PENTAGON_CENTER_Y, 4.5, SEGMENTS,
			0.89f, 0.56f, 0.54f); // Second layer
	drawCircle(PENTAGON_CENTER_X, PENTAGON_CENTER_Y, 2.0, SEGMENTS,
			0.7f, 0.15f, 0.1f); // Inner layer
}

// Variables for eye white.
int EYE_WHITE_LENGTH = 24;
int EYE_WHITE_HEIGHT = 16;
int EYE_WHITE_RADIUS = 13;

int EYE_WHITE_X = 0;
int EYE_WHITE_FIRST_Y = 5;
int EYE_WHITE_SECOND_Y = -5;

void drawEyeWhite() {
	glEnable(GL_STENCIL_TEST);
	glClear(GL_STENCIL_BUFFER_BIT);

	glPushMatrix();

	glTranslatef(PENTAGON_CENTER_X, PENTAGON_CENTER_Y, 0.0f);
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f); // Rotate fractal.

	// Stencil the entire first circle.
	glStencilFunc(GL_ALWAYS, 1, 0xFF); // There is some depth mask for applying layers.
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Only replace the value when the stencil test and depth passes.
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not write to all RGBA channels.
	drawCircle(EYE_WHITE_X, EYE_WHITE_FIRST_Y, EYE_WHITE_RADIUS, SEGMENTS, 1.0f, 1.0f, 1.0f);

	// Keep values where second circle intersects the first circle.
	// Fill where second circle intersects the first circle
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    drawCircle(EYE_WHITE_X, EYE_WHITE_SECOND_Y, EYE_WHITE_RADIUS, SEGMENTS, 0.69f, 0.47f, 0.47f);
    
    // Draw parts of second circle outline that intersect with first circle
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    drawCircleOutline(EYE_WHITE_X, EYE_WHITE_SECOND_Y, EYE_WHITE_RADIUS, SEGMENTS, 0.0f, 0.0f, 0.0f);

	// Clear and draw the bottom outline
	glClear(GL_STENCIL_BUFFER_BIT);

	glStencilFunc(GL_ALWAYS, 1, 0xFF); // There is some depth mask for applying layers.
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Only replace the value when the stencil test and depth passes.
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // Do not write to all RGBA channels.
	drawCircle(EYE_WHITE_X, EYE_WHITE_SECOND_Y, EYE_WHITE_RADIUS, SEGMENTS, 1.0f, 1.0f, 1.0f);

	// Draw parts of first circle outline that intersect with second circle
	glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    drawCircleOutline(EYE_WHITE_X, EYE_WHITE_FIRST_Y, EYE_WHITE_RADIUS, SEGMENTS, 0.0f, 0.0f, 0.0f);

    glPopMatrix();
    glDisable(GL_STENCIL_TEST);
}

// Variables for fractal.
vector<pair<float, float>> fracPoints;
int NUM_ITERATIONS = 80000;

// Checks if two vertices are neighbors in a pentagon
bool areNeighbors(int v1, int v2) {
	int diff = abs(v1 - v2);
	return (diff == 1) || (diff == 4);
}

// Adapted from https://fronkonstin.com/tag/fractals/
void generateFracPoints(int n) {
	// Take the center point in the pentagon.
	float currentX = PENTAGON_CENTER_X; // hardcoded
	float currentY = PENTAGON_CENTER_Y; // hardcoded


	// Choose a vertex randomly and take the midpoint between both of them.
	int vertexOne = rand() % 5; // [0,5)
	currentX = (currentX + PENTAGON_COORDS[vertexOne][0]) / 2.0f;
    currentY = (currentY + PENTAGON_COORDS[vertexOne][1]) / 2.0f;
    fracPoints.push_back({currentX, currentY});

	// Repeat one more time.
	int lastVertex = rand() % 5;
    currentX = (currentX + PENTAGON_COORDS[lastVertex][0]) / 2.0f;
    currentY = (currentY + PENTAGON_COORDS[lastVertex][1]) / 2.0f;
    fracPoints.push_back({currentX, currentY});

	// Repeat step 2 for n number of times.
	for (int i = 2; i < n; i++) {
        int newVertex;
        
        // Choose vertices until we find one that's not a neighbor of the last vertex
        do {
            newVertex = rand() % 5;
        } while (areNeighbors(lastVertex, newVertex));
        
        // Take the midpoint between current point and new vertex
        currentX = (currentX + PENTAGON_COORDS[newVertex][0]) / 2.0f;
        currentY = (currentY + PENTAGON_COORDS[newVertex][1]) / 2.0f;
        fracPoints.push_back({currentX, currentY});
        
        // Update last vertex for next iteration
        lastVertex = newVertex;
    }
}

void drawFractal(int n) {
	generateFracPoints(n); // Populate fractal points

	glPushMatrix();

	glTranslatef(PENTAGON_CENTER_X, PENTAGON_CENTER_Y, 0.0f);
	glRotatef(30.0f, 0.0f, 0.0f, 1.0f); // Rotate fractal.

	glColor3f(0.15f, 0.05f, 0.05f);
	// glColor3f(1.0, 0.9, 0.858);
	glPointSize(1.0f); // If need to change, else default is 1.
	glBegin(GL_POINTS);
		for (auto point : fracPoints) {
			glVertex2f(point.first, point.second);
		}
	glEnd();

	glPopMatrix();
}

// Completely hardcoded.
void drawSmile() {
	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-19.0, -25.0);
		glVertex2f(-12.938, -26.309);
		glVertex2f(-10.051, -27.488);
		glVertex2f(-9.474, -28.797);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-12.938, -26.309);
		glVertex2f(-6.154, -30.369);
		glVertex2f(-7.020, -25.130);
		glVertex2f(-12.649, -25.0);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-3.268, -30.238);
		glVertex2f(-9.041, -26.571);
		glVertex2f(-8.896, -24.738);
		glVertex2f(-4.278, -28.011);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-6.298, -26.964);
		glVertex2f(-5.865, -24.476);
		glVertex2f(-2.835, -25.785);
		glVertex2f(-0.958, -29.714);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-0.381, -29.845);
		glVertex2f(-4.134, -22.773);
		glVertex2f(-1.391, -24.607);
		glVertex2f(-0.525, -27.195);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-0.525, -27.195);
		glVertex2f(0.340, -21.726);
		glVertex2f(3.94, -23.035);
		glVertex2f(6.546, -29.452);
		glVertex2f(-0.525, -27.095);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(-0.525, -27.195);
		glVertex2f(4.381, -19.630);
		glVertex2f(7.412, -20.940);
		glVertex2f(8.134, -27.226);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(7.412, -20.940);
		glVertex2f(7.989, -17.535);
		glVertex2f(11.16, -18.845);
		glVertex2f(11.742, -26.571);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(11.164, -15.440);
		glVertex2f(15.639, -17.666);
		glVertex2f(15.494, -25.130);
		glVertex2f(11.309, -22.642);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(13.329, -17.011);
		glVertex2f(13.185, -13.869);
		glVertex2f(16.072, -15.309);
		glVertex2f(17.515, -19.5);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(16.072, -15.309);
		glVertex2f(16.072, -15.440);
		glVertex2f(18.525, -14.392);
		glVertex2f(18.381, -18.452);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(17.226, -12.952);
		glVertex2f(17.082, -10.071);
		glVertex2f(19.536, -10.857);
		glVertex2f(19.824, -15.702);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(19.536, -10.857);
		glVertex2f(20.402, -6.273);
		glVertex2f(21.845, -7.321);
		glVertex2f(21.123, -12.559);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.9f, 0.008f, 0.004f);
		glVertex2f(21.412, -9.416);
		glVertex2f(22.134, -10.071);
		glVertex2f(22.711, -3.261);
		glVertex2f(21.845, -7.321);
	glEnd();
}

// Adapted from https://stackoverflow.com/questions/538661/how-do-i-draw-text-with-glut-opengl-in-c
// Adapted from https://stackoverflow.com/questions/544079/how-do-i-use-glutbitmapstring-in-c-to-draw-text-to-the-screen
// Turns out bitmapstring cannot be rotated according to 
// https://stackoverflow.com/questions/55808841/rotate-a-glut-bitmap-stroke-string-around-its-center
void drawVerticalText(float x, float y, const char* text, bool upsideDown = false) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(5.0f);

    glPushMatrix();
    
    if (upsideDown) {
        glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
    }
    
    glTranslatef(x, y, 0.0f);
    glScalef(0.04f, 0.04f, 1.0f);
    
    float textY = -50.0f;
    for (int i = 0; text[i] != '\0'; i++) {
        glPushMatrix();

        glTranslatef(0.0f, textY, 0.0f);
        glutStrokeCharacter(GLUT_STROKE_ROMAN, text[i]);

        glPopMatrix();
        textY -= 150.0f; 
    }

    glPopMatrix();
}

void drawJokerText() {
	drawVerticalText(-29.0f, 46.0f, "JOKER", false);
    drawVerticalText(-29.0f, 46.0f, "JOKER", true);
}

void drawBackgroundGradient() {
	glBegin(GL_QUADS);
    
    // Top vertices - one color (e.g., dark red)
    glColor3f(0.3f, 0.0f, 0.1f);
    glVertex2f(-35.0f, 50.0f);
    glVertex2f(35.0f, 50.0f);
    
    // Bottom vertices - another color (e.g., black)
    glColor3f(0.0f, 0.0f, 0.0f);
    glVertex2f(35.0f, -50.0f);
    glVertex2f(-35.0f, -50.0f);
    
    glEnd();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);
	
	//draw your stuff here (Erase the triangle code)
	drawBackgroundGradient();

	drawEyeWhite();
	drawIris();
	drawFractal(NUM_ITERATIONS);
	drawSmile();
	drawJokerText();

	glPopMatrix();
	glFlush ();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-35, 35, -50, 50, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init(void)
{
	glClearColor (1.0, 1.0, 1.0, 1.0);
	glShadeModel (GL_SMOOTH);
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

		case 'a':
			alpha+=10;
			glutPostRedisplay();
		break;

		case 'd':
			alpha-=10;
			glutPostRedisplay();
		break;

		case 'q':
			k+=0.1;
			glutPostRedisplay();
		break;

		case 'e':
			if(k>0.1)
				k-=0.1;
			glutPostRedisplay();
		break;

		case 'z':
			tx-=0.1;
			glutPostRedisplay();
		break;

		case 'c':
			tx+=0.1;
			glutPostRedisplay();
		break;

		case 's':
			ty-=0.1;
			glutPostRedisplay();
		break;

		case 'w':
			ty+=0.1;
			glutPostRedisplay();
		break;

		default:
		break;
	}
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 1\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Scale Up/Down: Q/E\n";
	cout<<"Rotate Clockwise/Counter-clockwise: A/D\n";
	cout<<"Move Up/Down: W/S\n";
	cout<<"Move Left/Right: Z/C\n";

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize (565, 815);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
