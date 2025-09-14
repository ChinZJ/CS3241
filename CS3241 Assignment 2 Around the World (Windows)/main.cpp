// CS3241Lab1.cpp : Defines the entry point for the console application.
#include <cmath>
#include <iostream>
#include <time.h>
#include <iostream>
#include <thread> // Required for std::this_thread::sleep_for
#include <chrono> // Required for std::chrono::milliseconds
#define GL_SILENCE_DEPRECATION


#ifdef _WIN32
#include <Windows.h>
#include "GL/glut.h"
#define M_PI 3.141592654

#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

using namespace std;

#define numStars 100
#define numPlanets 9


class planet
{
public:
	float distFromRef;
	float angularSpeed;
	GLfloat color[3];
	GLfloat colorTwo[3];
	float size;
	float angle;
	float alpha;

	planet()
	{
		distFromRef = 0;
		angularSpeed = 0;
		color[0] = color[1] = color[2] = 0;
		colorTwo[0] = colorTwo[1] = colorTwo[2] = 0;
		size = 0;
		angle = 0;
		alpha = 1.0;
	}
};

GLfloat PI = 3.14;
float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0;
planet planetList[numPlanets];


bool clockMode = false;


time_t seconds = 0;
struct tm * timeinfo;
float timer = 0.1; // this is a dumb way to control the speed of rotations


// Window resizing and viewing parameters.
void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	float aspect = (float)w / (float)h;

	if (aspect >= 1.0) {
		glOrtho(-10 * aspect, 10 * aspect, -10, 10, -10, 10);
	} else {
		glOrtho(-10, 10, -10 / aspect, 10 / aspect, -10, 10);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// State and graphic settings.
void init(void)
{
	glClearColor (0.00391, 0.0469, 0.172, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


// Data is generated and scaled from https://www.jpl.nasa.gov/edu/pdfs/scaless_reference.pdf
void generatePlanets()
{
	//The sun
	planetList[0].distFromRef = 0;
	planetList[0].angularSpeed = 0;
	planetList[0].color[0] = 0.988;
	planetList[0].color[1] = 0.594;
	planetList[0].color[2] = 0.0313;
	planetList[0].size = 1.5;

	//Mercury
	planetList[1].distFromRef = 2;
	planetList[1].angularSpeed = 5;
	planetList[1].color[0] = 0.500;
	planetList[1].color[1] = 0.320;
	planetList[1].color[2] = 0.148;
	planetList[1].size = 0.172;

	// Venus
	planetList[2].distFromRef = 2.195;
	planetList[2].angularSpeed = 5;
	planetList[2].color[0] = 0.734;
	planetList[2].color[1] = 0.324;
	planetList[2].color[2] = 0.0391;
	planetList[2].size = 0.278;

	// Earth
	planetList[3].distFromRef = 2.267;
	planetList[3].angularSpeed = 5;
	// Water
	planetList[3].color[0] = 0.0352;
	planetList[3].color[1] = 0.738;
	planetList[3].color[2] = 0.891;
	// Land
	planetList[3].colorTwo[0] = 0.379;
	planetList[3].colorTwo[1] = 0.699;
	planetList[3].colorTwo[2] = 0.328;
	planetList[3].size = 0.287;

	// Mars
	planetList[4].distFromRef = 2.411;
	planetList[4].angularSpeed = 5;
	planetList[4].color[0] = 0.824;
	planetList[4].color[1] = 0.313;
	planetList[4].color[2] = 0.0430;
	planetList[4].size = 0.2;

	// Jupiter
	planetList[5].distFromRef = 3.404;
	planetList[5].angularSpeed = 5;
	planetList[5].color[0] = 0.715;
	planetList[5].color[1] = 0.551;
	planetList[5].color[2] = 0.371;
	// Rings
	planetList[3].colorTwo[0] = 0.859;
	planetList[3].colorTwo[1] = 0.496;
	planetList[3].colorTwo[2] = 0.246;
	planetList[5].size = 2.202;

	// Saturn
	planetList[6].distFromRef = 4.585;
	planetList[6].angularSpeed = 5;
	planetList[6].color[0] = 0.844;
	planetList[6].color[1] = 0.656;
	planetList[6].color[2] = 0.383;
	// Darker regions
	planetList[3].colorTwo[0] = 0.734;
	planetList[3].colorTwo[1] = 0.617;
	planetList[3].colorTwo[2] = 0.425;
	planetList[6].size = 1.87;

	// Uranus
	planetList[7].distFromRef = 7.179;
	planetList[7].angularSpeed = 5;
	planetList[7].color[0] = 0.422;
	planetList[7].color[1] = 0.659;
	planetList[7].color[2] = 0.707;
	planetList[7].size = 0.851;

	// Neptune
	planetList[8].distFromRef = 10.104;
	planetList[8].angularSpeed = 5;
	planetList[8].color[0] = 0.281;
	planetList[8].color[1] = 0.625;
	planetList[8].color[2] = 0.820;
	planetList[8].size = 0.828;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glPushMatrix();

	//controls transformation
	glScalef(k, k, k);	
	glTranslatef(tx, ty, 0);	
	glRotatef(alpha, 0, 0, 1);

	int circle_points = 50;
	float angle = 0;

	// Draw sun.
	glPushMatrix();
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[0].color[0], planetList[0].color[1], planetList[0].color[2], planetList[0].alpha);
			glVertex2f(planetList[0].size * cos(angle), planetList[0].size * sin(angle));
		}
	glEnd();
	glPopMatrix();

	//Draw Mercury
	glPushMatrix();
	glRotatef(planetList[1].angle, 0, 0, 1);
	glTranslatef(0, planetList[1].distFromRef, 0);	
	glBegin(GL_POLYGON);

		for (int i = 0; i < circle_points; i++) 
		{
			angle = 2*PI*i/circle_points;
			if(i>circle_points/2)
				glColor4f(planetList[1].color[0], planetList[1].color[1], planetList[1].color[2], planetList[1].alpha);
			else
				glColor4f(planetList[1].color[0]-0.2, planetList[1].color[1]-0.2, planetList[1].color[2]-0.2, planetList[1].alpha);
			glVertex2f(planetList[1].size * cos(angle), planetList[1].size *sin(angle));
		}

	glEnd();
	glPopMatrix();

	// Draw Venus
	glPushMatrix();
	glRotatef(planetList[2].angle, 0, 0, 1);
	glTranslatef(0, planetList[2].distFromRef, 0);	
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[2].color[0], planetList[2].color[1], planetList[2].color[2], planetList[2].alpha);
			glVertex2f(planetList[2].size * cos(angle), planetList[2].size * sin(angle));
		}
	glEnd();
	glPopMatrix();

	// Draw Earth
	glPushMatrix();
	glRotatef(planetList[3].angle, 0, 0, 1);
	glTranslatef(0, planetList[3].distFromRef, 0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[3].color[0], planetList[3].color[1], planetList[3].color[2], planetList[3].alpha);
			glVertex2f(planetList[3].size * cos(angle), planetList[3].size * sin(angle));
		}
	glEnd();
	
	glPopMatrix();

	// Draw Mars
	glPushMatrix();
	glRotatef(planetList[4].angle, 0, 0, 1);
	glTranslatef(0, planetList[4].distFromRef, 0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[4].color[0], planetList[4].color[1], planetList[4].color[2], planetList[4].alpha);
			glVertex2f(planetList[4].size * cos(angle), planetList[4].size * sin(angle));
		}
	glEnd();
	glPopMatrix();

	// Draw Jupiter
	glPushMatrix();
	glRotatef(planetList[5].angle, 0, 0, 1);
	glTranslatef(0, planetList[5].distFromRef, 0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[5].color[0], planetList[5].color[1], planetList[5].color[2], planetList[5].alpha);
			glVertex2f(planetList[5].size * cos(angle), planetList[5].size * sin(angle));
		}
	glEnd();
	glPopMatrix();

	// Draw Saturn
	glPushMatrix();
	glRotatef(planetList[6].angle, 0, 0, 1);
	glTranslatef(0, planetList[6].distFromRef, 0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[6].color[0], planetList[6].color[1], planetList[6].color[2], planetList[6].alpha);
			glVertex2f(planetList[6].size * cos(angle), planetList[6].size * sin(angle));
		}
	glEnd();
	glPopMatrix();

	// Draw Uranus
	glPushMatrix();
	glRotatef(planetList[7].angle, 0, 0, 1);
	glTranslatef(0, planetList[7].distFromRef, 0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[7].color[0], planetList[7].color[1], planetList[7].color[2], planetList[7].alpha);
			glVertex2f(planetList[7].size * cos(angle), planetList[7].size * sin(angle));
		}
	glEnd();
	glPopMatrix();

	// Draw Neptune
	glPushMatrix();
	glRotatef(planetList[8].angle, 0, 0, 1);
	glTranslatef(0, planetList[8].distFromRef, 0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(planetList[8].color[0], planetList[8].color[1], planetList[8].color[2], planetList[8].alpha);
			glVertex2f(planetList[8].size * cos(angle), planetList[8].size * sin(angle));
		}
	glEnd();
	glPopMatrix();


	glPopMatrix();
	glFlush ();
}

void idle()
{
	// this idle function is to adjust the parameters for display() to draw
	// You can modify the following for your assignment

	if(!clockMode)
	{
		
		//animate planets
		for(int i=0;i<numPlanets;i++)
		{
			planetList[i].alpha = 1;
			planetList[i].angle += planetList[i].angularSpeed*timer;
		}

	}
	else
	{
		// draw your clock here

		seconds = time(NULL);
		timeinfo = localtime(&seconds);

		for (int i = 0; i < numPlanets; i++)
		{
			planetList[i].alpha = 1;
			planetList[i].angle = ((float)timeinfo->tm_sec  )*6; // 6 degrees per second
		}
	}
	
	glutPostRedisplay();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {

        case 27: // press ESC to exit
		case 'q':
		case 'Q':
            exit(0);
	
		case 't':
			clockMode = !clockMode;
			if (clockMode)
				cout << "Current Mode: Clock mode." << endl;
			else
				cout << "Current Mode: Solar mode." << endl;
			break;

		default:
			break;
	}
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 2\n\n";
	cout<<"+++++CONTROL BUTTONS+++++++\n\n";
	cout<<"Toggle Time Mode: T\n";
    cout<<"Exit: ESC or q/Q\n";
	cout << "Current Mode: Clock mode." << endl;


	generatePlanets();

	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (1600, 900);
	glutInitWindowPosition (50, 50);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);	
	//glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
