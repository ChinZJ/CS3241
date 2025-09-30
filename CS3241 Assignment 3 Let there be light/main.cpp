

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
	glScalef(r, r, r);
	int i, j;
	int n = 80;

	/**
	 * This was part of the original code, I am not sure what its purpose is as it disrupts the 
	 * toggling settings provided via keyboard.
	 */
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	for (i = 0; i < 2 * n; i++) { // 2 * n longitude
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
}

void drawCylinder(double radius, double height) 
{
    int n = 80;
    glBegin(GL_QUAD_STRIP);
		for (int i = 0; i <= n; i++) {
			double angle = 2 * M_PI * i / n;
			double x = radius * cos(angle);
			double z = radius * sin(angle);
			
			if (m_Smooth) {
				glNormal3d(x, 0, z);
			}
			glVertex3d(x, 0, z);
			glVertex3d(x, height, z);
		}
    glEnd();
}

void setColorAndVertex(double x, double y, double z) {
	double grad = (z + 1.0) / 2.0;
	GLfloat r = (1.0 - grad) * 0.6 + grad * 0.8;
	GLfloat g = (1.0 - grad) * 0.2 + grad * 0.4;
	GLfloat b = (1.0 - grad) * 0.4 + grad * 0.6;
	glColor3f(r, g, b);
	glVertex3d(x, y, z);
}

void drawLotusPetal(double length) 
{
    GLfloat mat_ambient[] = { 0.8f, 0.4f, 0.7f, 1.0f };
    GLfloat mat_diffuse[] = { 0.9f, 0.5f, 0.8f, 1.0f };
    GLfloat mat_specular[] = { 
        m_Highlight ? 0.05f : 0.0f,
        m_Highlight ? 0.05f : 0.0f,
        m_Highlight ? 0.05f : 0.0f,
        1.0f 
    };
    GLfloat mat_shininess[] = { m_Highlight ? 50.0f : 0.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Create petal using scaled ellipsoid
    glPushMatrix();
    glScalef(0.3 * length, length, 0.15 * length); // Elongated shape
    
    int n = 80;
    for (int i = 0; i < 2 * n; i++) {
        for (int j = 0; j < n / 2; j++) {
            glBegin(GL_POLYGON);
            
				double x1 = sin(i * M_PI / n) * sin(j * M_PI / n);
				double y1 = cos(i * M_PI / n) * sin(j * M_PI / n);
				double z1 = cos(j * M_PI / n);

				double x2 = sin((i + 1) * M_PI / n) * sin(j * M_PI / n);
				double y2 = cos((i + 1) * M_PI / n) * sin(j * M_PI / n);
				double z2 = cos(j * M_PI / n);

				double x3 = sin((i + 1) * M_PI / n) * sin((j + 1) * M_PI / n);
				double y3 = cos((i + 1) * M_PI / n) * sin((j + 1) * M_PI / n);
				double z3 = cos((j + 1) * M_PI / n);

				double x4 = sin(i * M_PI / n) * sin((j + 1) * M_PI / n);
				double y4 = cos(i * M_PI / n) * sin((j + 1) * M_PI / n);
				double z4 = cos((j + 1) * M_PI / n);

				// Invert normals to make the inside bright instead.
				if (m_Smooth) {
					glNormal3d(-x1, -y1, -z1);
					glNormal3d(-x2, -y2, -z2);
					glNormal3d(-x3, -y3, -z3);
					glNormal3d(-x4, -y4, -z4);
				}

				setColorAndVertex(x1, y1, z1);
				setColorAndVertex(x2, y2, z2);
				setColorAndVertex(x3, y3, z3);
				setColorAndVertex(x4, y4, z4);
            
            glEnd();
        }
    }
    glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}

void drawLotus(double cylRad, double cylHeight, double sphRad) {
	// Stem
	// Green stem material
    GLfloat mat_ambient[] = { 0.1f, 0.4f, 0.1f, 1.0f };
    GLfloat mat_diffuse[] = { 0.2f, 0.6f, 0.2f, 1.0f };
    GLfloat mat_specular[] = { 
        m_Highlight ? 0.1f : 0.0f,
        m_Highlight ? 0.1f : 0.0f,
        m_Highlight ? 0.1f : 0.0f,
        1.0f 
    };
    GLfloat mat_shininess[] = { m_Highlight ? 10.0f : 0.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glPushMatrix();
    glTranslatef(0, -1.7, 0);
    drawCylinder(cylRad, cylHeight);
    glPopMatrix();

	// Stamen
	// Draw a yellow sphere
	GLfloat stamen_ambient[] = { 0.7f, 0.6f, 0.1f, 1.0f };
	GLfloat stamen_diffuse[] = { 0.8f, 0.7f, 0.2f, 1.0f };
	GLfloat stamen_specular[] = { 
		m_Highlight ? 1.0f : 0.0f,
		m_Highlight ? 1.0f : 0.0f,
		m_Highlight ? 1.0f : 0.0f,
		1.0f 
	};
	GLfloat stamen_shininess[] = { m_Highlight ? 50.0f : 0.0f };

	glMaterialfv(GL_FRONT, GL_AMBIENT, stamen_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, stamen_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, stamen_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, stamen_shininess);

	glPushMatrix();
    glTranslatef(0, -0.3, 0);
    drawSphere(sphRad);
    glPopMatrix();

	// Petals
	for (int i = 0; i < 5; i++) {
        glPushMatrix();
        glRotatef(35 + (i * 72.0), 0, 1, 0);
        glTranslatef(0, -0.3, 0.5);
        glRotatef(70, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
        drawLotusPetal(0.8);
        glPopMatrix();
    }

	for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glRotatef(35 + (i * 60.0), 0, 1, 0);
        glTranslatef(0, -0.1, 0.5);
        glRotatef(45, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
        drawLotusPetal(0.6);
        glPopMatrix();
    }

	for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(i * 90.0, 0, 1, 0);
        glTranslatef(0, -0.2, 0.3);
		glRotatef(30, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
        drawLotusPetal(0.45);
        glPopMatrix();
    }

	for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glRotatef(45 + (i * 90.0), 0, 1, 0);
        glTranslatef(0, -0.1, 0.25);
		glRotatef(15, 1, 0, 0);
		glRotatef(180, 0, 0, 1);
        drawLotusPetal(0.45);
        glPopMatrix();
    }
}

void drawSpiderWeb() {
	glPushMatrix();
	glScalef(2.0, 2.0, 2.0);
	glTranslatef(0.0, 0.0, -0.08);

    glColor3f(1.0f, 1.0f, 1.0f);
	GLfloat mat_white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_shininess[] = { m_Highlight ? 50.0f : 0.0f };

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_white);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_white);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_white);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    double octagonRadius = 4.0;
    double strandThickness = 0.005;
    int numCircles = 30;
    int numRadialLines = 80;

	glScalef(0.5, 0.5, 0.5);
    
    // Draw octagon outer frame (8 sides)
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 8; i++) {
        double angle = (2.0 * M_PI * i) / 8.0;
        double x = octagonRadius * cos(angle);
        double y = octagonRadius * sin(angle);
        glVertex3d(x, y, 0);
    }
    glEnd();
    
    // Core binders
	double innerRadius = octagonRadius / 1.2;
    for (int i = 0; i < 8; i++) {
		double angle1 = (2.0 * M_PI * i) / 8.0;
		double angle2 = (2.0 * M_PI * i) / 8.0 + M_PI / 8.0;  // Inner octagon is rotated
		
		double x1 = octagonRadius * cos(angle1);
		double y1 = octagonRadius * sin(angle1);
		
		double x2 = innerRadius * cos(angle2);
		double y2 = innerRadius * sin(angle2);
		
		glBegin(GL_LINES);
			glVertex3d(x1, y1, 0);
			glVertex3d(x2, y2, 0);
		glEnd();
	}

	// Octagon inner frame
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 8; i++) {
		double angle = (2.0 * M_PI * i) / 8.0 + M_PI / 8.0;
		double x = innerRadius * cos(angle);
		double y = innerRadius * sin(angle);
		glVertex3d(x, y, 0);
	}
	glEnd();
    
    // Draw 30 concentric circles with varying radii
    double minRadius = 0.005;
	double maxRadius = innerRadius;
	double power = 2.0;

	for (int circle = 0; circle <= numCircles; circle++) {
		// Stop before inner octagon
		double t = (double)circle / (numCircles + 2);
		
		// Non uniform spacing
		double t_scaled = pow(t, power);
		
		// Map to actual radius range
		double radius = minRadius + t_scaled * (maxRadius - minRadius);
		
		glBegin(GL_LINE_LOOP);
		int segments = 80;  // Number of segments per circle for smoothness
		for (int j = 0; j < segments; j++) {
			double angle = (2.0 * M_PI * j) / segments;
			double x = radius * cos(angle);
			double y = radius * sin(angle);
			glVertex3d(x, y, 0);
		}
		glEnd();
	}
    
    for (int side = 0; side < 8; side++) {
		// Get the two vertices of this octagon side
		double angle1 = (2.0 * M_PI * side) / 8.0 + M_PI / 8.0;
		double angle2 = (2.0 * M_PI * (side + 1)) / 8.0 + M_PI / 8.0;
		
		double x1 = innerRadius * cos(angle1);
		double y1 = innerRadius * sin(angle1);
		double x2 = innerRadius * cos(angle2);
		double y2 = innerRadius * sin(angle2);
		
		// Draw 10 lines to points along this edge
		for (int i = 0; i <= 10; i++) {
			// Interpolate along the edge
			double t = (double)i / 10.0;
			double x = x1 + t * (x2 - x1);
			double y = y1 + t * (y2 - y1);
			
			glBegin(GL_LINES);
				glVertex3d(0, 0, 0);  // Origin
				glVertex3d(x, y, 0);   // Point on octagon edge
			glEnd();
		}
	}
	glPopMatrix();
}

void drawSpider() {
    double bodyRadius = 0.15;
    double legLength = 0.25;
    double legThickness = 0.01;
    
    // Black widow body material - shiny black
    GLfloat body_ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    GLfloat body_diffuse[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat body_specular[] = { 
        m_Highlight ? 0.2f : 0.0f,
        m_Highlight ? 0.2f : 0.0f,
        m_Highlight ? 0.2f : 0.0f,
        1.0f 
    };
    GLfloat body_shininess[] = { m_Highlight ? 50.0f : 0.0f };
    
    // Draw main body (abdomen)
    glMaterialfv(GL_FRONT, GL_AMBIENT, body_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, body_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, body_shininess);
    
    glPushMatrix();
    glScalef(1.0, 1.2, 0.8);  // Slightly elongated
    drawSphere(bodyRadius);
    glPopMatrix();
    
    // Draw red hourglass on abdomen
    GLfloat red_ambient[] = { 0.3f, 0.0f, 0.0f, 1.0f };
    GLfloat red_diffuse[] = { 0.8f, 0.0f, 0.0f, 1.0f };
    GLfloat red_specular[] = { 
        m_Highlight ? 1.0f : 0.0f,
        m_Highlight ? 0.4f : 0.0f,
        m_Highlight ? 0.4f : 0.0f,
        1.0f 
    };
    GLfloat red_shininess[] = { m_Highlight ? 50.0f : 0.0f };
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, red_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, red_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, red_shininess);
    
    // Upper triangle of hourglass
    glPushMatrix();
    glTranslatef(0, -0.05, -bodyRadius * 0.85);
    glBegin(GL_TRIANGLES);
		glNormal3d(0, 0, 1);
        glVertex3d(0, 0.06, 0.25);
        glVertex3d(-0.04, -0.02, 0.25);
        glVertex3d(0.04, -0.02, 0.25);
    glEnd();
    glPopMatrix();
    
    // Lower triangle of hourglass
    glPushMatrix();
    glTranslatef(0, -0.05, -bodyRadius * 0.85);
    glBegin(GL_TRIANGLES);
        glVertex3d(0, 0.00, 0.25);
        glVertex3d(-0.04, 0.08, 0.25);
        glVertex3d(0.04, 0.08, 0.25);
    glEnd();
    glPopMatrix();
    
    // Smaller head (cephalothorax)
    glMaterialfv(GL_FRONT, GL_AMBIENT, body_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, body_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, body_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, body_shininess);
    
    glPushMatrix();
    glTranslatef(0, bodyRadius * 1.3, 0);
    glScalef(0.6, 0.7, 0.6);
    drawSphere(bodyRadius);
    glPopMatrix();    

	// Draw 8 legs (4 on each side)
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		
		glTranslatef(-0.08, -0.2, 0.2);
		glTranslatef(0, bodyRadius * 1.5, 0);
		glRotatef(i * 45.0, 0, 0, 1);  // Distribute legs around Y axis
		glRotatef(100, 0, 1, 0);
		glTranslatef(bodyRadius * 1.5, 0, 0);  // Move out along X axis
		
		// First segment (upper leg)
		glPushMatrix();
		glRotatef(-25, 0, 0, 1);  // Angle slightly down
		drawCylinder(legThickness, legLength);
		glPopMatrix();
		
		glPopMatrix();
	}

	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		
		glScalef(-1, 1, 1);
		glTranslatef(-0.08, -0.2, 0.2);
		glTranslatef(0, bodyRadius * 1.5, 0);
		glRotatef(i * 45.0, 0, 0, 1);  // Distribute legs around Y axis
		glRotatef(100, 0, 1, 0);
		glTranslatef(bodyRadius * 1.5, 0, 0);  // Move out along X axis
		
		// First segment (upper leg)
		glPushMatrix();
		glRotatef(-25, 0, 0, 1);  // Angle slightly down
		drawCylinder(legThickness, legLength);
		glPopMatrix();
		
		glPopMatrix();
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
	case 0: {
		// Draw a yellow sphere
		GLfloat mat_ambient[] = { 0.7f, 0.6f, 0.1f, 1.0f }; // I_a (ambient)
		GLfloat mat_diffuse[] = { 0.8f, 0.7f, 0.2f, 1.0f }; // I_p (diffuse)
		
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
		drawSphere(1);
		break;
	}
	case 1:
		drawLotusPetal(1);
		break;
	case 2:
		// draw your first composite object here
		drawLotus(0.07, 1.5, 0.22);
		break;
	case 3:
		// draw your second composite object here
		drawSpiderWeb();
		drawSpider();
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
	glClearColor(0.1, 0.2, 0.3, 1.0);
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
