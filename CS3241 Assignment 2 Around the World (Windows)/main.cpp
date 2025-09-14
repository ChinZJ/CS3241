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
	float size;
	float angle;
	float alpha;

	planet()
	{
		distFromRef = 0;
		angularSpeed = 0;
		color[0] = color[1] = color[2] = 0;
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


void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init(void)
{
	glClearColor (0.0, 0.0, 0.3, 1.0);
	glShadeModel (GL_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void generatePlanets()
{
	//The sun
	planetList[0].distFromRef = 0;
	planetList[0].angularSpeed = 0;
	planetList[0].color[0] = 1.0;
	planetList[0].color[1] = 0.7;
	planetList[0].color[2] = 0.0;
	planetList[0].size = 1.5;

	//Mercury
	planetList[1].distFromRef = 3;
	planetList[1].angularSpeed = 5;
	planetList[1].color[0] = 0.7;
	planetList[1].color[1] = 0.7;
	planetList[1].color[2] = 0.7;
	planetList[1].size = 0.5;


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



	// You can modify the following for your assignment
	//draw mercury
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
			planetList[i].angle = ((float)timeinfo->tm_sec  )*6;
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
	glutInitWindowSize (600, 600);
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
