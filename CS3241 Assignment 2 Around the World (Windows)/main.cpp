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

class rocket
{
public:
	float orbitRadius;
	float orbitSpeed;
	float currentAngle;
	float size;
	GLfloat color[3];

	rocket()
	{
		orbitRadius = 0.5;
		orbitSpeed = -100; // Rotate opposite direciton.
		currentAngle = 0;
		size = 0.15;
		color[0] = 0.8;
		color[1] = 0.8;
		color[2] = 0.8;
	}
};

class ufo
{
public:
	float jumpHeight;
	float jumpSpeed;
	float currentHeight;
	float size;
	GLfloat color[3];
	bool goingUp;

	ufo() 
	{
		jumpHeight = 0.3;
		jumpSpeed = 1.0;
		currentHeight = 0.0;
		size = 0.1;
		color[0] = 0.5;  // Gray
		color[1] = 0.5;
		color[2] = 0.5;
		goingUp = true;
	}
};

GLfloat PI = 3.14;
float alpha = 0.0, k=1;
float tx = 0.0, ty=0.0;
planet planetList[numPlanets];
rocket earthRocket;
ufo neptuneUfo;

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

float BASE_ANGULAR_SPEED = 48;
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
	planetList[1].angularSpeed = BASE_ANGULAR_SPEED * 365 / 88;
	planetList[1].color[0] = 0.500;
	planetList[1].color[1] = 0.320;
	planetList[1].color[2] = 0.148;
	planetList[1].size = 0.172;
	planetList[1].angle = 9;

	// Venus
	planetList[2].distFromRef = 2.195;
	planetList[2].angularSpeed = BASE_ANGULAR_SPEED * 365 / 227;
	planetList[2].color[0] = 0.734;
	planetList[2].color[1] = 0.324;
	planetList[2].color[2] = 0.0391;
	planetList[2].size = 0.278;
	planetList[2].angle = 236;

	// Earth
	planetList[3].distFromRef = 2.567;
	planetList[3].angularSpeed = BASE_ANGULAR_SPEED;
	// Water
	planetList[3].color[0] = 0.0352;
	planetList[3].color[1] = 0.738;
	planetList[3].color[2] = 0.891;
	// Land
	planetList[3].colorTwo[0] = 0.379;
	planetList[3].colorTwo[1] = 0.699;
	planetList[3].colorTwo[2] = 0.328;
	planetList[3].size = 0.287;
	planetList[3].angle = 264;

	// Mars
	planetList[4].distFromRef = 2.911;
	planetList[4].angularSpeed = BASE_ANGULAR_SPEED * 365 / 687;
	planetList[4].color[0] = 0.824;
	planetList[4].color[1] = 0.313;
	planetList[4].color[2] = 0.0430;
	planetList[4].size = 0.2;
	planetList[4].angle = 60;

	// Jupiter
	planetList[5].distFromRef = 4.404;
	planetList[5].angularSpeed = BASE_ANGULAR_SPEED * 365 / 4328;
	planetList[5].color[0] = 0.715;
	planetList[5].color[1] = 0.551;
	planetList[5].color[2] = 0.371;
	// Rings
	planetList[5].colorTwo[0] = 0.859;
	planetList[5].colorTwo[1] = 0.496;
	planetList[5].colorTwo[2] = 0.246;
	planetList[5].size = 1.002;
	planetList[5].angle = 73;

	// Saturn
	planetList[6].distFromRef = 6.585;
	planetList[6].angularSpeed = BASE_ANGULAR_SPEED * 365 / 10767;
	planetList[6].color[0] = 0.734;
	planetList[6].color[1] = 0.617;
	planetList[6].color[2] = 0.425;
	// Darker regions
	planetList[6].colorTwo[0] = 0.844;
	planetList[6].colorTwo[1] = 0.656;
	planetList[6].colorTwo[2] = 0.383;
	planetList[6].size = 1.07;
	planetList[6].angle = 261;

	// Uranus
	planetList[7].distFromRef = 8.279;
	planetList[7].angularSpeed = BASE_ANGULAR_SPEED * 365 / 30769;
	planetList[7].color[0] = 0.422;
	planetList[7].color[1] = 0.659;
	planetList[7].color[2] = 0.707;
	planetList[7].size = 0.851;
	planetList[7].angle = 100;

	// Neptune
	planetList[8].distFromRef = 10.104;
	planetList[8].angularSpeed = BASE_ANGULAR_SPEED * 365 / 60225;
	planetList[8].color[0] = 0.281;
	planetList[8].color[1] = 0.625;
	planetList[8].color[2] = 0.820;
	planetList[8].size = 0.828;
	planetList[8].angle = 14;
}

// Draws a sphere.
void drawPlanet(planet p, int circle_points, float angle)
{
	glPushMatrix();
	glRotatef(p.angle, 0, 0, 1);
	glTranslatef(0, p.distFromRef, 0);	
	glBegin(GL_POLYGON);
		for (int i = 0; i < circle_points; i++) {
			angle = 2*PI*i/circle_points;
			glColor4f(p.color[0], p.color[1], p.color[2], p.alpha);
			glVertex2f(p.size * cos(angle), p.size * sin(angle));
		}
	glEnd();
	glPopMatrix();
}

void drawSegments(planet p, int segments, int circle_points, float angle)
{
    glPushMatrix();
    glRotatef(p.angle, 0, 0, 1);
    glTranslatef(0, p.distFromRef, 0);
    
    // Height of each horizontal band
    float bandHeight = (2.0f * p.size) / segments;
    
    for (int seg = 0; seg < segments; seg++) {
        // Alternate colors between bands
        if (seg % 2 == 0) {
            glColor4f(p.color[0], p.color[1], p.color[2], p.alpha);
        } else {
            glColor4f(p.colorTwo[0], p.colorTwo[1], p.colorTwo[2], p.alpha);
        }
        
        // Calculate y boundaries for this horizontal band
        float yTop = p.size - (seg * bandHeight);
        float yBottom = p.size - ((seg + 1) * bandHeight);
        
        // Draw the band as a series of quads that follow the circle's curve
        glBegin(GL_QUAD_STRIP);
			// Sample points along the x-axis
			for (int i = 0; i <= circle_points; i++) {
				// Go from left (-p.size) to right (+p.size)
				float x = -p.size + (2.0f * p.size * i / circle_points);
				
				// For each x, calculate the corresponding y values on the circle
				// Circle equation: x^2 + y^2 = r^2, so y = sqrt(r^2 - x^2)
				float x_ratio = x / p.size;
				
				// Check if x is within the circle
				if (x_ratio >= -1.0f && x_ratio <= 1.0f) {
					float y_circle = p.size * sqrt(1.0f - x_ratio * x_ratio);
					
					// Clamp the band to stay within the circle
					float y_top_clamped = yTop;
					float y_bottom_clamped = yBottom;
					
					// Only draw if the band intersects with the circle at this x position
					if (yTop >= -y_circle && yBottom <= y_circle) {
						// Clamp top and bottom to circle boundary
						if (y_top_clamped > y_circle) {
							y_top_clamped = y_circle;
						}
						if (y_bottom_clamped < -y_circle) {
							y_bottom_clamped = -y_circle;
						}
						
						// Add vertices for the quad strip
						glVertex2f(x, y_top_clamped);
						glVertex2f(x, y_bottom_clamped);
					}
				}
			}
        glEnd();
    }
    glPopMatrix();
}

// Draws two triangle land masses
void drawLand(planet p) 
{
    glPushMatrix();
    glRotatef(p.angle, 0, 0, 1);
    glTranslatef(0, p.distFromRef, 0);  // Move to planet position
    glColor4f(p.colorTwo[0], p.colorTwo[1], p.colorTwo[2], p.alpha);
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.02f, 0.0f); // bot
		glVertex2f(0.12f, 0.15f); // top right
		glVertex2f(-0.2f, 0.15f); // top left
    glEnd();
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.02f, 0.0f); // top
        glVertex2f(0.0f, -0.2f); // bot
        glVertex2f(0.15f, -0.075f); // right
    glEnd();
    glPopMatrix();
}

void drawRocket()
{
	glPushMatrix();

	glRotatef(planetList[3].angle, 0, 0, 1);
	glTranslatef(0, planetList[3].distFromRef, 0);

	glRotatef(earthRocket.currentAngle, 0, 0, 1);
	glTranslatef(0, earthRocket.orbitRadius, 0);

	if (clockMode) {
		glRotatef(90, 0, 0, 1);
	} else {
		glRotatef(-90, 0, 0, 1);
	}
	

	// Rocket body.
	glColor4f(earthRocket.color[0], earthRocket.color[1], earthRocket.color[2], 1.0);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, earthRocket.size);           // Tip
        glVertex2f(-earthRocket.size/2, -earthRocket.size/2);  // Left base
        glVertex2f(earthRocket.size/2, -earthRocket.size/2);   // Right base
    glEnd();

	// Rocket flame.
	glColor4f(1.0, 0.5, 0.0, 0.8);  // Orange flame
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, -earthRocket.size/2);
        glVertex2f(-earthRocket.size/3, -earthRocket.size);
        glVertex2f(earthRocket.size/3, -earthRocket.size);
    glEnd();

	glPopMatrix();
}

void drawUfo()
{
	glPushMatrix();

	// Calculate Neptune's actual position in world coordinates
	float neptuneX = -planetList[8].distFromRef * sin(planetList[8].angle * PI / 180.0);
	float neptuneY = planetList[8].distFromRef * cos(planetList[8].angle * PI / 180.0);
	
	// Move directly to Neptune's world position
	glTranslatef(neptuneX, neptuneY, 0);
	
	// Move to top of Neptune (add planet size + jump height)
	glTranslatef(0, planetList[8].size + neptuneUfo.currentHeight, 0);

	glScalef(2.0, 2.0, 1.0);  

	// UFO main body (ellipse)
	glColor4f(neptuneUfo.color[0], neptuneUfo.color[1], neptuneUfo.color[2], 1.0);
	glBegin(GL_POLYGON);
		for (int i = 0; i < 20; i++) {
			float angle = 2 * PI * i / 20;
			glVertex2f(neptuneUfo.size * 1.5 * cos(angle), neptuneUfo.size * 0.5 * sin(angle));
		}
	glEnd();

	// UFO dome (smaller circle on top)
	glColor4f(0.7, 0.7, 0.9, 0.8);
	glBegin(GL_POLYGON);
		for (int i = 0; i < 15; i++) {
			float angle = 2 * PI * i / 15;
			glVertex2f(neptuneUfo.size * 0.8 * cos(angle), neptuneUfo.size * 0.3 * sin(angle) + neptuneUfo.size * 0.2);
		}
	glEnd();

	glPopMatrix();
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
	drawPlanet(planetList[0], circle_points, angle);

	//Draw Mercury
	drawPlanet(planetList[1], circle_points, angle);

	// Draw Venus
	drawPlanet(planetList[2], circle_points, angle);

	// Draw Earth
	drawPlanet(planetList[3], circle_points, angle);
	drawLand(planetList[3]);
	drawRocket();

	// Draw Mars
	drawPlanet(planetList[4], circle_points, angle);

	// Draw Jupiter
	drawSegments(planetList[5], 5, circle_points, angle);

	// Draw Saturn
	drawSegments(planetList[6], 3, circle_points, angle);

	// Draw Uranus
	drawPlanet(planetList[7], circle_points, angle);

	// Draw Neptune
	drawPlanet(planetList[8], circle_points, angle);
	drawUfo();


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

		earthRocket.currentAngle += earthRocket.orbitSpeed * timer;
	}
	else
	{
		// draw your clock here
		seconds = time(NULL);
		timeinfo = localtime(&seconds);

		for (int i = 0; i < numPlanets; i++)
		{
			planetList[i].alpha = 1;
			
			// Odd indices forms minute and second hand.
			if (i == 1 || i == 3) {
				planetList[i].angle = -((float)timeinfo->tm_sec)*6; // 6 degrees per second
			} else if (i == 2 || i == 5 || i == 7) {
				planetList[i].angle = -((float)timeinfo->tm_min)*6; // 6 degrees per minute
			} else {
				// Rest forms hour hand, add minute increments using minutes.
				planetList[i].angle = -(((float)(timeinfo->tm_hour % 12)) * 30 + ((float)timeinfo->tm_min) * 0.5);
			}
		}

		// Milliseconds
		earthRocket.currentAngle = ((float)timeinfo->tm_sec) * 6 + (clock() % 1000) * 0.36;
	}

	if (neptuneUfo.goingUp) {
		neptuneUfo.currentHeight += neptuneUfo.jumpSpeed * timer;
		if (neptuneUfo.currentHeight >= neptuneUfo.jumpHeight) {
			neptuneUfo.goingUp = false;
		}
	} else {
		neptuneUfo.currentHeight -= neptuneUfo.jumpSpeed * timer;
		if (neptuneUfo.currentHeight <= 0.0) {
			neptuneUfo.goingUp = true;
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
