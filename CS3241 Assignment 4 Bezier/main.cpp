// CS3241Lab4.cpp : Defines the entry point for the console application.
//#include <cmath>
#include "math.h"
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <Windows.h>
#include "GL/glut.h"
#define M_PI 3.141592654
#elif __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/GLUT.h>
#endif

#define MAXPTNO 1000
#define NLINESEGMENT 32
#define NOBJECTONCURVE 8
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

using namespace std;

// Global variables that you can use
struct Point {
	int x,y;
};

// Storage of control points
int nPt = 0;
Point ptList[MAXPTNO];

// Display options
bool displayControlPoints = true;
bool displayControlLines = true;
bool displayTangentVectors = false;
bool displayObjects = false;
bool C1Continuity = false;
	
void drawRightArrow() {
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
		glVertex2f(0,0);
		glVertex2f(100,0);
		glVertex2f(95,5);
		glVertex2f(100,0);
		glVertex2f(95,-5);
	glEnd();
}

void drawTriangularLeg(float startY, float direction) {
    glBegin(GL_TRIANGLES);
        // First triangle (largest)
        glVertex2f(0, startY);
        glVertex2f(3 * direction, startY + 15 * direction);
        glVertex2f(-3 * direction, startY + 15 * direction);

        // Second triangle (medium)
        glVertex2f(0, startY + 10 * direction);
        glVertex2f(2 * direction, startY + 22 * direction);
        glVertex2f(-2 * direction, startY + 22 * direction);

        // Third triangle (smallest)
        glVertex2f(0, startY + 18 * direction);
        glVertex2f(1.5 * direction, startY + 28 * direction);
        glVertex2f(-1.5 * direction, startY + 28 * direction);
    glEnd();
}

void drawRoundedRectBody() {
    float width = 20.0;
    float height = 30.0;
    float radius = 10.0;
    
    
	// Orange right half
	glColor3f(0.788, 0.279, 0.022);
    glBegin(GL_POLYGON);
        // Top-left rounded corner
        for (int i = 90; i <= 180; i += 10) {
            float angle = i * M_PI / 180.0;
            glVertex2f(-width/2 + radius + radius * cos(angle), 
                      height/2 - radius + radius * sin(angle));
        }
        // Left side down
        glVertex2f(-width/2, -height/2 + radius);
        // Bottom-left rounded corner
        for (int i = 180; i <= 270; i += 10) {
            float angle = i * M_PI / 180.0;
            glVertex2f(-width/2 + radius + radius * cos(angle), 
                      -height/2 + radius + radius * sin(angle));
        }
        // Bottom middle
        glVertex2f(0, -height/2);
        // Top middle
        glVertex2f(0, height/2);
    glEnd();
    
    // Black left half
	glColor3f(0.373, 0.211, 0.147);
    glBegin(GL_POLYGON);
        // Start at top middle
        glVertex2f(0, height/2);
        // Bottom middle
        glVertex2f(0, -height/2);
        // Bottom-right rounded corner
        for (int i = 270; i <= 360; i += 10) {
            float angle = i * M_PI / 180.0;
            glVertex2f(width/2 - radius + radius * cos(angle), 
                      -height/2 + radius + radius * sin(angle));
        }
        // Right side up
        glVertex2f(width/2, height/2 - radius);
        // Top-right rounded corner
        for (int i = 0; i <= 90; i += 10) {
            float angle = i * M_PI / 180.0;
            glVertex2f(width/2 - radius + radius * cos(angle), 
                      height/2 - radius + radius * sin(angle));
        }
    glEnd();
}

void drawCentipede() {
    drawRoundedRectBody();
    
    // Draw legs
    glColor3f(0.833, 0.505, 0.0);
    glLineWidth(2.0);
    
    // Upper legs (starting closer to the body)
    drawTriangularLeg(15, 1);
    
    // Lower legs (starting closer to the body)
    drawTriangularLeg(-15, -1);
    
    glLineWidth(1.0);
}

void drawSpiralAntenna(float baseX, float baseY, float direction) {
    int numSegments = 12;
    float angleStep = 360.0 / numSegments;
    
    for (int i = 0; i < numSegments; i++) {
        float angle = direction * i * angleStep * M_PI / 180.0;
        float nextAngle = direction * (i + 1) * angleStep * M_PI / 180.0;
        
        // Spiral parameters: start far from body, spiral inward (scaled up by 30%)
        float startRadius = 32.5 - (i * 2.34);  // Scaled from 25.0 - (i * 1.8)
        float endRadius = 32.5 - ((i + 1) * 2.34);
        float thickness = 2.6;  // Scaled from 2.0
        
        glBegin(GL_TRIANGLES);
            // Center point of spiral
            glVertex2f(baseX + startRadius * cos(angle), 
                      baseY + startRadius * sin(angle));
            // Outer edge
            glVertex2f(baseX + (startRadius + thickness) * cos(angle), 
                      baseY + (startRadius + thickness) * sin(angle));
            // Next point in spiral
            glVertex2f(baseX + endRadius * cos(nextAngle), 
                      baseY + endRadius * sin(nextAngle));
        glEnd();
    }
}

void drawCentipedeHead() {
    drawRoundedRectBody();
    
    // Spiral antennae using triangles
    glColor3f(0.286, 0.041, 0.0);
    
    // Left antenna - spiral clockwise
    drawSpiralAntenna(-13, 10.4, 1);
    
    // Right antenna - spiral counter-clockwise
    drawSpiralAntenna(-13, -10.4, -1);
    
    glLineWidth(1.0);
}

void drawTailLeg(float startX, float legYOffset, int numSegments, float segmentLength, 
                 float baseWidth, float curveFactor, float curveDirection) {
    for (int i = 0; i < numSegments; i++) {
        float progress = (float)i / (float)(numSegments - 1);
        
        float curveOffset = curveDirection * curveFactor * progress * progress * segmentLength * numSegments;

        float x1 = startX + i * segmentLength;
        float x2 = x1 + segmentLength;
        float y_offset = legYOffset + curveOffset;
        float y_offset_next = legYOffset + curveOffset * (1 + 1.5/numSegments);
        
        glVertex2f(x1, y_offset - baseWidth/2); 
        glVertex2f(x1, y_offset + baseWidth/2); 
        glVertex2f(x2, y_offset_next);
    }
}

void drawCentipedeTail() {
    drawRoundedRectBody();
    
    glColor3f(0.645, 0.369, 0.0);
    
    int numSegments = 4;
    float segmentLength = 12.0;
    float baseWidth = 4.0;
    float curveFactor = 0.05;
    float legSeparation = 20.0;
    
    // Starting position closer to the body to eliminate gap
    float startX = 10;  // Moved closer from 15 to connect with body
    
    glBegin(GL_TRIANGLES);
		// Draw upper leg 
		drawTailLeg(startX, legSeparation/2, numSegments, segmentLength, 
					baseWidth, curveFactor, -1.0);
		
		// Draw lower leg
		drawTailLeg(startX, -legSeparation/2, numSegments, segmentLength, 
					baseWidth, curveFactor, 1.0);
    glEnd();
}

Point evalCubicBezier(Point p00, Point p01, Point p02, Point p03, float t) {
	float t2 = t * t;
	float t3 = t2 * t;
	float invT = (1-t);
	float invT2 = invT * invT;
	float invT3 = invT2 * invT;

	Point result;
	result.x = (invT3 * p00.x) + (3 * t * invT2 * p01.x) + (3 * t2 * invT * p02.x) + (t3 * p03.x);
	result.y = (invT3 * p00.y) + (3 * t * invT2 * p01.y) + (3 * t2 * invT * p02.y) + (t3 * p03.y);

	return result;
}

/**
 * Condition required is p0n - p0n_1 = q01 - p0n.
 * Thus q01 = 2p0n - p0n_1.
 */
Point evalC1Point(Point p0n, Point p0n_1) {
	Point result;
	result.x = (2 * p0n.x) - p0n_1.x;
	result.y = (2 * p0n.y) - p0n_1.y;

	return result;
}

Point evalTangentVector(Point p00, Point p01, Point p02, Point p03, float t) {
	float t2 = t * t;
	float invT = 1 - t;
	float invT2 = invT * invT;

	Point tangent;
	tangent.x = (3 * invT2 * (p01.x - p00.x)) 
			+ (6 * t * invT * (p02.x - p01.x)) 
			+ (3 * t2 * (p03.x - p02.x));
	tangent.y = (3 * invT2 * (p01.y - p00.y)) 
			+ (6 * t * invT * (p02.y - p01.y)) 
			+ (3 * t2 * (p03.y - p02.y));
	return tangent;
}

void drawControlLines(void) {
	glColor3f(0,1,0);
	glBegin(GL_LINE_STRIP);
		for (int i = 0; i < nPt; i++) {
			if (C1Continuity && i >= 4 && (i - 1) % 3 == 0) {
				Point p0n_1 = ptList[i - 2];
				Point p0n = ptList[i - 1];
				Point c1Point = evalC1Point(p0n, p0n_1);
				glVertex2d(c1Point.x, c1Point.y);
			} else {
				glVertex2d(ptList[i].x, ptList[i].y);
			}
		}
	glEnd();
}

void drawCubicBezier(void) {
	glColor3f(0, 0, 1);
	for (int i = 0; i + 3 < nPt; i += 3) {
		Point p00 = ptList[i];
		Point p01 = ptList[i + 1];
		Point p02 = ptList[i + 2];
		Point p03 = ptList[i + 3];

		if (C1Continuity && i >= 3) {
			Point p0n_1 = ptList[i - 1];
			Point p0n = ptList[i];
			p01 = evalC1Point(p0n, p0n_1);
		}

		glBegin(GL_LINE_STRIP);
			for (int j = 0; j <= NLINESEGMENT; j++) {
				float t = (float)j / (float)NLINESEGMENT;
				Point pt = evalCubicBezier(p00, p01, p02, p03, t);
				glVertex2d(pt.x, pt.y);
			}
		glEnd();
	}
}

void drawTangentVectors(void) {
	glColor3f(0, 0, 0);
	for (int i = 0; i + 3 < nPt; i += 3) {
		Point p00 = ptList[i];
		Point p01 = ptList[i + 1];
		Point p02 = ptList[i + 2];
		Point p03 = ptList[i + 3];

		if (C1Continuity && i >= 3) {
			Point p0n_1 = ptList[i - 1];
			Point p0n = ptList[i];
			p01 = evalC1Point(p0n, p0n_1);
		}

		for (int j = 0; j < NOBJECTONCURVE; j++) {
			float t = (float)j / (float)(NOBJECTONCURVE - 1);

			Point pt = evalCubicBezier(p00, p01, p02, p03, t);
			Point tangent = evalTangentVector(p00, p01, p02, p03, t);

			float angle  = atan2(tangent.y, tangent.x) * 180.0 / M_PI;

			glPushMatrix();
				glTranslatef(pt.x, pt.y, 0);
				glRotatef(angle, 0, 0, 1);
				drawRightArrow();
			glPopMatrix();
		}
	}
}

void drawObjects(void) {
    int totalObjects = 0;
    int numCurves = 0;
    for (int i = 0; i + 3 < nPt; i += 3) {
        totalObjects += NOBJECTONCURVE;
        numCurves++;
    }
    totalObjects -= (numCurves - 1);
    
    int objectIndex = 0;
    
    for (int i = 0; i + 3 < nPt; i += 3) {
        Point p00 = ptList[i];
        Point p01 = ptList[i + 1];
        Point p02 = ptList[i + 2];
        Point p03 = ptList[i + 3];

        if (C1Continuity && i >= 3) {
            Point p0n_1 = ptList[i - 1];
            Point p0n = ptList[i];
            p01 = evalC1Point(p0n, p0n_1);
        }

        for (int j = 0; j < NOBJECTONCURVE; j++) {
            if (i > 0 && j == 0) {
                continue;
            }
            
            float t = (float)j / (float)(NOBJECTONCURVE - 1);

            Point pt = evalCubicBezier(p00, p01, p02, p03, t);
            Point tangent = evalTangentVector(p00, p01, p02, p03, t);

            float angle = atan2(tangent.y, tangent.x) * 180.0 / M_PI;

            glPushMatrix();
                glTranslatef(pt.x, pt.y, 0);
                glRotatef(angle, 0, 0, 1);
                
                if (objectIndex == 0) {
                    drawCentipedeHead();
                } else if (objectIndex == totalObjects - 1) {
                    drawCentipedeTail();
                } else {
                    drawCentipede();
                }
            glPopMatrix();
            
            objectIndex++;
        }
    }
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	if(displayControlPoints)
	{

		glPointSize(5);
		glBegin(GL_POINTS);
		for(int i=0;i<nPt; i++) {
			bool isChange = false;
			if (C1Continuity && i >= 4 && (i - 1) % 3 == 0) {
				glColor3f(0.7, 0.7, 0.7); // Original control point
				isChange = true;
			} else {
				glColor3f(0,0,0);
			}
			glVertex2d(ptList[i].x,ptList[i].y);
			
			if (isChange) {
				Point p0n_1 = ptList[i - 2];
				Point p0n = ptList[i - 1];
				Point c1Point = evalC1Point(p0n, p0n_1);

				glColor3f(1, 0, 0);
				glVertex2d(c1Point.x, c1Point.y);
			}
		}
		glEnd();
		glPointSize(1);

	}

	if(displayControlLines)
	{
		drawControlLines();
		drawCubicBezier();
	}

	if (displayTangentVectors) {
		drawTangentVectors();
	}

	if (displayObjects) {
		drawObjects();
	}

	glPopMatrix();
	glutSwapBuffers ();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,w,h,0);  
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
}

void init(void)
{
	glClearColor (1.0,1.0,1.0, 1.0);
}

void readFile()
{

	std::ifstream file;
    file.open("savefile.txt");
	file >> nPt;

	if(nPt>MAXPTNO)
	{
		cout << "Error: File contains more than the maximum number of points." << endl;
		nPt = MAXPTNO;
	}

	for(int i=0;i<nPt;i++)
	{
		file >> ptList[i].x;
		file >> ptList[i].y;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void writeFile()
{
	std::ofstream file;
    file.open("savefile.txt");
    file << nPt << endl;

	for(int i=0;i<nPt;i++)
	{
		file << ptList[i].x << " ";
		file << ptList[i].y << endl;
	}
    file.close();// is not necessary because the destructor closes the open file by default
}

void keyboard (unsigned char key, int x, int y)
{
	switch (key) {
		case 'r':
		case 'R':
			readFile();
		break;

		case 'w':
		case 'W':
			writeFile();
		break;

		case 'T':
		case 't':
			displayTangentVectors = !displayTangentVectors;
		break;

		case 'o':
		case 'O':
			displayObjects = !displayObjects;
		break;

		case 'p':
		case 'P':
			displayControlPoints = !displayControlPoints;
		break;

		case 'L':
		case 'l':
			displayControlLines = !displayControlLines;
		break;

		case 'C':
		case 'c':
			C1Continuity = !C1Continuity;
		break;

		case 'e':
		case 'E':
			nPt = 0; // No need to clear ptList, simply overwrite

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

void mouse(int button, int state, int x, int y)
{
	/*button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON */
	/*state: GLUT_UP or GLUT_DOWN */
	enum
	{
		MOUSE_LEFT_BUTTON = 0,
		MOUSE_MIDDLE_BUTTON = 1,
		MOUSE_RIGHT_BUTTON = 2,
		MOUSE_SCROLL_UP = 3,
		MOUSE_SCROLL_DOWN = 4
	};
	if((button == MOUSE_LEFT_BUTTON)&&(state == GLUT_UP))
	{
		if(nPt==MAXPTNO)
		{
			cout << "Error: Exceeded the maximum number of points." << endl;
			return;
		}
		ptList[nPt].x=x;
		ptList[nPt].y=y;
		nPt++;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	cout<<"CS3241 Lab 4"<< endl<< endl;
	cout << "Left mouse click: Add a control point"<<endl;
	cout << "Q: Quit" <<endl;
	cout << "P: Toggle displaying control points" <<endl;
	cout << "L: Toggle displaying control lines" <<endl;
	cout << "E: Erase all points (Clear)" << endl;
	cout << "C: Toggle C1 continuity" <<endl;	
	cout << "T: Toggle displaying tangent vectors" <<endl;
	cout << "O: Toggle displaying objects" <<endl;
	cout << "R: Read in control points from \"savefile.txt\"" <<endl;
	cout << "W: Write control points to \"savefile.txt\"" <<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition (50, 50);
	glutCreateWindow ("CS3241 Assignment 4");
	init ();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}
