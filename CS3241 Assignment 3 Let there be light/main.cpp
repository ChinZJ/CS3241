

// CS3241 Assignment 2: Let there be light
#include <cmath>
#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#include "GL/glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

using namespace std;

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

// global variable

bool m_Smooth = FALSE;
bool m_Highlight = FALSE;
GLfloat angle = 0;   /* in degrees */
GLfloat angle2 = 0;   /* in degrees */
GLfloat zoom = 1.0;
int mouseButton = 0;
int moving, startx, starty;

#define NO_OBJECT 4
int current_object = 0;

void setupLighting()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// Lights, material properties
	GLfloat	ambientProperties[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat	diffuseProperties[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat	specularProperties[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat lightPosition[] = { -100.0f,100.0f,100.0f,1.0f };

	glClearDepth(1.0);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientProperties);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseProperties);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularProperties);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);

	// Default : lighting
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

}


void drawSphere(double r)
{
	// Draw a blue sphere
	GLfloat mat_ambient[] = { 0.3f, 0.5f, 0.7f, 1.0f }; // I_a (ambient)
	GLfloat mat_diffuse[] = { 0.3f, 0.5f, 0.7f, 1.0f }; // I_p (diffuse)
	
	// I_p (specular)
	GLfloat mat_specular[] = { 
		m_Highlight ? 1.0f : 0.0f,
		m_Highlight ? 1.0f : 0.0f,
		m_Highlight ? 1.0f : 0.0f,
		1.0f 
	};
	GLfloat mat_shininess[] = { m_Highlight ? 50.0f : 0.0f }; // n, max 50

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glScalef(r, r, r);
	int i, j;
	int n = 20;

	
	/**
	 * This was part of the original code, I am not sure what its purpose is as it disrupts the 
	 * toggling settings provided via keyboard.
	 */
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	
	for (i = 0; i < 2 * n; i++) // 2 * n longitude
		for (j = 0; j < n; j++) // n latitude
		{
			glBegin(GL_POLYGON);
				// Draw spherical coordinates and convert to catesian coordinates.
				// Vertex 1
				double x1 = sin(i * M_PI / n) * sin(j * M_PI / n);
				double y1 = cos(i * M_PI / n) * sin(j * M_PI / n);
				double z1 = cos(j * M_PI / n);

				// Vertex 2
				double x2 = sin((i + 1) * M_PI / n) * sin(j * M_PI / n);
				double y2 = cos((i + 1) * M_PI / n) * sin(j * M_PI / n);
				double z2 = cos(j * M_PI / n);

				// Vertex 3
				double x3 = sin((i + 1) * M_PI / n) * sin((j + 1) * M_PI / n);
				double y3 = cos((i + 1) * M_PI / n) * sin((j + 1) * M_PI / n);
				double z3 = cos((j + 1) * M_PI / n);

				// Vertex 4
				double x4 = sin(i * M_PI / n) * sin((j + 1) * M_PI / n);
				double y4 = cos(i * M_PI / n) * sin((j + 1) * M_PI / n);
				double z4 = cos((j + 1) * M_PI / n);
				
				/**
				 * I was unable to start up sample.exe, thus I apologise if I have misinterpreted 
				 * any of the instructions.
				 * 
				 * For a vertex (i, j). The normal vector is the normalized direction vector from
				 * the center (origin) to the point.
				 * Since the given radius is 1, the position vector itself is the unit normal vector.
				 * Regardless, glEnable(GL_NORMALIZE) is already called in setupLighting, 
				 * thus normals does not need to be manually calculated
				 */
				// Toggle smooth shading
				if (m_Smooth) {
					glNormal3d(x1, y1, z1);
					glNormal3d(x2, y2, z2);
					glNormal3d(x3, y3, z3);
					glNormal3d(x4, y4, z4);
				}

				glVertex3d(x1, y1, z1);
				glVertex3d(x2, y2, z2);
				glVertex3d(x3, y3, z3);
				glVertex3d(x4, y4, z4);
			glEnd();
		}

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glTranslatef(0, 0, -6);

	glRotatef(angle2, 1.0, 0.0, 0.0);
	glRotatef(angle, 0.0, 1.0, 0.0);

	glScalef(zoom, zoom, zoom);

	switch (current_object) {
	case 0:
		drawSphere(1);
		break;
	case 1:
		// draw your second primitive object here
		break;
	case 2:
		// draw your first composite object here
		break;
	case 3:
		// draw your second composite object here
		break;
	default:
		break;
	};
	glPopMatrix();
	glutSwapBuffers();
}




void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
	case 'P':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'w':
	case 'W':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'v':
	case 'V':
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		break;
	case 's':
	case 'S':
		m_Smooth = !m_Smooth;
		break;
	case 'h':
	case 'H':
		m_Highlight = !m_Highlight;
		break;

	case '1':
	case '2':
	case '3':
	case '4':
		current_object = key - '1';
		break;

	case 'Q':
	case 'q':
		exit(0);
		break;

	default:
		break;
	}

	glutPostRedisplay();
}



void
mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		mouseButton = button;
		moving = 1;
		startx = x;
		starty = y;
	}
	if (state == GLUT_UP) {
		mouseButton = button;
		moving = 0;
	}
}

void motion(int x, int y)
{
	if (moving) {
		if (mouseButton == GLUT_LEFT_BUTTON)
		{
			angle = angle + (x - startx);
			angle2 = angle2 + (y - starty);
		}
		else zoom += ((y - starty) * 0.001);
		startx = x;
		starty = y;
		glutPostRedisplay();
	}

}

int main(int argc, char** argv)
{
	cout << "CS3241 Lab 3" << endl << endl;

	cout << "1-4: Draw different objects" << endl;
	cout << "S: Toggle Smooth Shading" << endl;
	cout << "H: Toggle Highlight" << endl;
	cout << "W: Draw Wireframe" << endl;
	cout << "P: Draw Polygon" << endl;
	cout << "V: Draw Vertices" << endl;
	cout << "Q: Quit" << endl << endl;

	cout << "Left mouse click and drag: rotate the object" << endl;
	cout << "Right mouse click and drag: zooming" << endl;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("CS3241 Assignment 3");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	setupLighting();
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 40.0,
		/* aspect ratio */ (double) WINDOW_WIDTH / WINDOW_HEIGHT,
		/* Z near */ 1.0, /* Z far */ 80.0);
	glMatrixMode(GL_MODELVIEW);
	glutMainLoop();

	return 0;
}
