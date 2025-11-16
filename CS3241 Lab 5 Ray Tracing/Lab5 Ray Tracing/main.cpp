// Given that shadows are already implemented, 
// step 5's shadows are dependent on the relatvie position of the objects and 
// thus do not appear like how it is in the example.

// CS3241Lab5.cpp 
#include <cmath>
#include <iostream>
#include "GL\glut.h"
#include "vector3D.h"
#include <chrono>

using namespace std;

#define WINWIDTH 600
#define WINHEIGHT 400
#define NUM_OBJECTS 4
#define MAX_RT_LEVEL 50
#define NUM_SCENE 2

const double EPS = 1e-3;

float* pixelBuffer = new float[WINWIDTH * WINHEIGHT * 3];

class Ray { // a ray that start with "start" and going in the direction "dir"
public:
	Vector3 start, dir;
};

class RtObject {

public:
	virtual double intersectWithRay(Ray, Vector3& pos, Vector3& normal) = 0; // return a -ve if there is no intersection. Otherwise, return the smallest postive value of t

	// Materials Properties
	double ambiantReflection[3] ;
	double diffusetReflection[3] ;
	double specularReflection[3] ;
	double speN = 300;


};

class Sphere : public RtObject {

	Vector3 center_;
	double r_;
public:
	Sphere(Vector3 c, double r) { center_ = c; r_ = r; };
	Sphere() {};
	void set(Vector3 c, double r) { center_ = c; r_ = r; };
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
};

class Pyramid : public RtObject {
	Vector3 vertices[4];  // 4 vertices for tetrahedron
	Vector3 center_;
	
public:
	Pyramid(Vector3 center, double size) {
		center_ = center;
		// Create tetrahedron vertices
		double h = size * sqrt(2.0/3.0);  // height
		vertices[0] = center + Vector3(0, h*0.6, 0);  // top
		vertices[1] = center + Vector3(-size, -h*0.4, -size);  // base
		vertices[2] = center + Vector3(size, -h*0.4, -size);   // base
		vertices[3] = center + Vector3(0, -h*0.4, size);       // base
	}
	
	Pyramid() {};
	
	void set(Vector3 center, double size) {
		center_ = center;
		double h = size * sqrt(2.0/3.0);
		vertices[0] = center + Vector3(0, h*0.6, 0);
		vertices[1] = center + Vector3(-size, -h*0.4, -size);
		vertices[2] = center + Vector3(size, -h*0.4, -size);
		vertices[3] = center + Vector3(0, -h*0.4, size);
	}
	
	double intersectWithRay(Ray, Vector3& pos, Vector3& normal);
	
private:
	// Intersect ray with triangle
	bool intersectTriangle(Ray ray, Vector3 v0, Vector3 v1, Vector3 v2, 
	                       double& t, Vector3& hitPoint, Vector3& hitNormal) {
		Vector3 e1 = v1 - v0;
		Vector3 e2 = v2 - v0;
		Vector3 h = cross_prod(ray.dir, e2);
		double a = dot_prod(e1, h);
		
		if (fabs(a) < EPS) return false;
		
		double f = 1.0 / a;
		Vector3 s = ray.start - v0;
		double u = f * dot_prod(s, h);
		
		if (u < 0.0 || u > 1.0) return false;
		
		Vector3 q = cross_prod(s, e1);
		double v = f * dot_prod(ray.dir, q);
		
		if (v < 0.0 || u + v > 1.0) return false;
		
		t = f * dot_prod(e2, q);
		
		if (t > EPS) {
			hitPoint = ray.start + ray.dir * t;
			hitNormal = cross_prod(e1, e2);
			hitNormal.normalize();
			return true;
		}
		
		return false;
	}
};


RtObject **objList; // The list of all objects in the scene


// Global Variables
// Camera Settings
Vector3 cameraPos(0,0,-500);

// assume the the following two vectors are normalised
Vector3 lookAtDir(0,0,1);
Vector3 upVector(0,1,0);
Vector3 leftVector(1, 0, 0);
float focalLen = 500;

// Light Settings

Vector3 lightPos(900,1000,-1500);
double ambiantLight[3] = { 0.4,0.4,0.4 };
double diffusetLight[3] = { 0.7,0.7, 0.7 };
double specularLight[3] = { 0.5,0.5, 0.5 };


double bgColor[3] = { 0.1,0.1,0.4 };

int sceneNo = 0;


double Sphere::intersectWithRay(Ray r, Vector3& intersection, Vector3& normal)
// return a -ve if there is no intersection. Otherwise, return the smallest postive value of t
{

	// Step 1
	Vector3 oc = r.start - center_;

	double alpha = dot_prod(r.dir, r.dir); // d · d
	double beta = 2.0 * dot_prod(r.dir, oc); // 2d · (p - c)
	double gamma = dot_prod(oc, oc) - r_ * r_; // |p-c|^2 - r^2

	double disc = beta * beta - 4 * alpha * gamma; // b^2 - 4ac

	// No real roots
	if (disc < 0) 
	{
		return -1;
	}

	double sqrtDisc = sqrt(disc);
	double t1 = (-beta - sqrtDisc) / (2.0 * alpha);
	double t2 = (-beta + sqrtDisc) / (2.0 * alpha);

	double t;
	if (t1 > 0) 
	{
		t = t1;
	} else if (t2 > 0) 
	{
		t = t2;
	} else 
	{
		return -1;
	}

	intersection = r.start + r.dir * t;

	normal = intersection - center_;
	normal.normalize();

	return t;
}

double Pyramid::intersectWithRay(Ray r, Vector3& intersection, Vector3& normal) {
	double minT = DBL_MAX;
	bool hit = false;
	Vector3 tempIntersection, tempNormal;
	double t;
	
	// Check all 4 faces of the tetrahedron
	
	// Face 1: bottom (vertices 1, 2, 3) - counterclockwise from below
	if (intersectTriangle(r, vertices[1], vertices[3], vertices[2], t, tempIntersection, tempNormal)) {
		if (t < minT) {
			minT = t;
			intersection = tempIntersection;
			normal = tempNormal;
			// Normal points outward
			Vector3 toCenter = center_ - intersection;
			if (dot_prod(normal, toCenter) > 0) {
				normal = -normal;
			}
			hit = true;
		}
	}
	
	// Face 2: side 1 (vertices 0, 1, 2)
	if (intersectTriangle(r, vertices[0], vertices[2], vertices[1], t, tempIntersection, tempNormal)) {
		if (t < minT) {
			minT = t;
			intersection = tempIntersection;
			normal = tempNormal;
			Vector3 toCenter = center_ - intersection;
			if (dot_prod(normal, toCenter) > 0) {
				normal = -normal;
			}
			hit = true;
		}
	}
	
	// Face 3: side 2 (vertices 0, 2, 3)
	if (intersectTriangle(r, vertices[0], vertices[3], vertices[2], t, tempIntersection, tempNormal)) {
		if (t < minT) {
			minT = t;
			intersection = tempIntersection;
			normal = tempNormal;
			Vector3 toCenter = center_ - intersection;
			if (dot_prod(normal, toCenter) > 0) {
				normal = -normal;
			}
			hit = true;
		}
	}
	
	// Face 4: side 3 (vertices 0, 3, 1)
	if (intersectTriangle(r, vertices[0], vertices[1], vertices[3], t, tempIntersection, tempNormal)) {
		if (t < minT) {
			minT = t;
			intersection = tempIntersection;
			normal = tempNormal;
			Vector3 toCenter = center_ - intersection;
			if (dot_prod(normal, toCenter) > 0) {
				normal = -normal;
			}
			hit = true;
		}
	}
	
	return hit ? minT : -1;
}


void rayTrace(Ray ray, double& r, double& g, double& b, int fromObj = -1 ,int level = 0)
{
	// Step 4
	// Stop at max level
	if (level >= MAX_RT_LEVEL) {
		r = bgColor[0];
		g = bgColor[1];
		b = bgColor[2];
		return;
	}

	int goBackGround = 1, i = 0;

	Vector3 intersection, normal;
	Vector3 lightV;
	Vector3 viewV;
	Vector3 lightReflectionV;
	Vector3 rayReflectionV;

	Ray newRay;
	double mint = DBL_MAX, t;

	int closestObj = -1;
	Vector3 closestIntersection, closestNormal;


	for (i = 0; i < NUM_OBJECTS; i++)
	{
		if (i == fromObj) // Skip current object
		{
			continue;
		}

		if ((t = objList[i]->intersectWithRay(ray, intersection, normal)) > 0)
		{
			// Step 2 
			// r = g = b = 1.0; 			
			if (t < mint) {
				mint = t;
				closestObj = i;
				closestIntersection = intersection;
				closestNormal = normal;
			}
		}
	}

	// Intersection found
	if (closestObj != -1) {
		// Start with ambient component (IPhong)
		r = objList[closestObj]->ambiantReflection[0] * ambiantLight[0];
		g = objList[closestObj]->ambiantReflection[1] * ambiantLight[1];
		b = objList[closestObj]->ambiantReflection[2] * ambiantLight[2];

		// Step 3
		// Shadow ray (intersection point to light source)
		Vector3 lightDir = lightPos - closestIntersection;
		double lightDist = lightDir.length();
		lightDir.normalize();
		
		Ray shadowRay;
		shadowRay.start = closestIntersection;
		shadowRay.dir = lightDir;

		// Shadow ray hit any object
		bool inShadow = false;
		Vector3 tempIntersection, tempNormal;
		for (int j = 0; j < NUM_OBJECTS; j++) {
			if (j == closestObj) continue;  // Skip current object
			
			double shadowT = objList[j]->intersectWithRay(shadowRay, tempIntersection, tempNormal);
			if (shadowT > EPS && shadowT < (lightDist - EPS)) {
				inShadow = true;
				break;
			}
		}
		
		// Diffuse and specular components (not shadow)
		if (!inShadow) {
			// Diffuse component: kd * Id * (N·L)
			double NdotL = dot_prod(closestNormal, lightDir);
			if (NdotL > 0) {
				r += objList[closestObj]->diffusetReflection[0] * diffusetLight[0] * NdotL;
				g += objList[closestObj]->diffusetReflection[1] * diffusetLight[1] * NdotL;
				b += objList[closestObj]->diffusetReflection[2] * diffusetLight[2] * NdotL;
			}
			
			// Specular component: ks * Is * (R·V)^n
			// R = 2(N·L)N - L
			Vector3 reflectDir = closestNormal * (2.0 * NdotL) - lightDir;
			reflectDir.normalize();
			
			// View direction (surface to camera)
			Vector3 viewDir = ray.start - closestIntersection;
			viewDir.normalize();
			
			double RdotV = dot_prod(reflectDir, viewDir);
			if (RdotV > 0) {
				double specular = pow(RdotV, objList[closestObj]->speN);
				r += objList[closestObj]->specularReflection[0] * specularLight[0] * specular;
				g += objList[closestObj]->specularReflection[1] * specularLight[1] * specular;
				b += objList[closestObj]->specularReflection[2] * specularLight[2] * specular;
			}
		}

		// Step 4
		// ray, r = 2(N · (-i))N + i
		// i = incoming ray direction
		Vector3 incomingDir = ray.dir;
		double NdotI = dot_prod(closestNormal, -incomingDir);
		Vector3 reflectionDir = closestNormal * (2.0 * NdotI) + incomingDir;
		reflectionDir.normalize();


		// Reflection ray
		Ray reflectionRay;
		reflectionRay.start = closestIntersection;
		reflectionRay.dir = reflectionDir;

		// Recursive
		double reflectR = 0, reflectG = 0, reflectB = 0;
		rayTrace(reflectionRay, reflectR, reflectG, reflectB, closestObj, level + 1);

		// Add weighted reflection ks * Ir
		r += objList[closestObj]->specularReflection[0] * reflectR;
		g += objList[closestObj]->specularReflection[1] * reflectG;
		b += objList[closestObj]->specularReflection[2] * reflectB;
		
		// Clamp values to [0, 1]
		if (r > 1.0) r = 1.0;
		if (g > 1.0) g = 1.0;
		if (b > 1.0) b = 1.0;
		
		goBackGround = 0;
	}

	if (goBackGround)
	{
		r = bgColor[0];
		g = bgColor[1];
		b = bgColor[2];
	}

}


void drawInPixelBuffer(int x, int y, double r, double g, double b)
{
	pixelBuffer[(y*WINWIDTH + x) * 3] = (float)r;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 1] = (float)g;
	pixelBuffer[(y*WINWIDTH + x) * 3 + 2] = (float)b;
}

void renderScene()
{
	int x, y;
	Ray ray;
	double r, g, b;

	cout << "Rendering Scene " << sceneNo << " with resolution " << WINWIDTH << "x" << WINHEIGHT << "........... ";
	__int64 time1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the starting time

	ray.start = cameraPos;

	Vector3 vpCenter = cameraPos + lookAtDir * focalLen;  // viewplane center
	Vector3 startingPt = vpCenter + leftVector * (-WINWIDTH / 2.0) + upVector * (-WINHEIGHT / 2.0);
	Vector3 currPt;

	for(x=0;x<WINWIDTH;x++)
		for (y = 0; y < WINHEIGHT; y++)
		{
			currPt = startingPt + leftVector*x + upVector*y;
			ray.dir = currPt-cameraPos;
			ray.dir.normalize();
			rayTrace(ray, r, g, b);
			drawInPixelBuffer(x, y, r, g, b);
		}

	__int64 time2 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count(); // marking the ending time

	cout << "Done! \nRendering time = " << time2 - time1 << "ms" << endl << endl;
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |GL_DOUBLEBUFFER);
	glDrawPixels(WINWIDTH, WINHEIGHT, GL_RGB, GL_FLOAT, pixelBuffer);
	glutSwapBuffers();
	glFlush ();
} 

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-10, 10, -10, 10, -10, 10);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void setScene(int i = 0)
{
	if (i > NUM_SCENE)
	{
		cout << "Warning: Invalid Scene Number" << endl;
		return;
	}

	if (i == 0)
	{

		((Sphere*)objList[0])->set(Vector3(-130, 80, 120), 100);
		((Sphere*)objList[1])->set(Vector3(130, -80, -80), 100);
		delete objList[2];
		objList[2] = new Sphere(Vector3(-130, -80, -80), 100);
		((Sphere*)objList[3])->set(Vector3(130, 80, 120), 100);

		objList[0]->ambiantReflection[0] = 0.1;
		objList[0]->ambiantReflection[1] = 0.4;
		objList[0]->ambiantReflection[2] = 0.4;
		objList[0]->diffusetReflection[0] = 0;
		objList[0]->diffusetReflection[1] = 1;
		objList[0]->diffusetReflection[2] = 1;
		objList[0]->specularReflection[0] = 0.2;
		objList[0]->specularReflection[1] = 0.4;
		objList[0]->specularReflection[2] = 0.4;
		objList[0]->speN = 300;

		objList[1]->ambiantReflection[0] = 0.6;
		objList[1]->ambiantReflection[1] = 0.6;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 1;
		objList[1]->diffusetReflection[1] = 1;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.0;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 50;

		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.6;
		objList[2]->ambiantReflection[2] = 0.1;
		objList[2]->diffusetReflection[0] = 0.1;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 0.1;
		objList[2]->specularReflection[0] = 0.3;
		objList[2]->specularReflection[1] = 0.7;
		objList[2]->specularReflection[2] = 0.3;
		objList[2]->speN = 650;

		objList[3]->ambiantReflection[0] = 0.3;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.7;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.6;
		objList[3]->specularReflection[1] = 0.6;
		objList[3]->specularReflection[2] = 0.6;
		objList[3]->speN = 650;

	}

	if (i == 1)
	{

		// Step 5
		// Green sphere (left)
		((Sphere*)objList[0])->set(Vector3(-60, 20, -200), 70);
		
		// Yellow sphere (right back)
		((Sphere*)objList[1])->set(Vector3(150, 50, 250), 120);
		
		// Blue pyramid (right front)
		delete objList[2];
		objList[2] = new Pyramid(Vector3(160, 40, 60), 50);
		
		// Purple (bottom)
		((Sphere*)objList[3])->set(Vector3(0, -550, 80), 450);

		// Green sphere
		objList[0]->ambiantReflection[0] = 0.1;
		objList[0]->ambiantReflection[1] = 0.6;
		objList[0]->ambiantReflection[2] = 0.1;
		objList[0]->diffusetReflection[0] = 0.1;
		objList[0]->diffusetReflection[1] = 1;
		objList[0]->diffusetReflection[2] = 0.1;
		objList[0]->specularReflection[0] = 0.3;
		objList[0]->specularReflection[1] = 0.7;
		objList[0]->specularReflection[2] = 0.3;
		objList[0]->speN = 650;

		// Yellow sphere
		objList[1]->ambiantReflection[0] = 0.6;
		objList[1]->ambiantReflection[1] = 0.6;
		objList[1]->ambiantReflection[2] = 0.2;
		objList[1]->diffusetReflection[0] = 1;
		objList[1]->diffusetReflection[1] = 1;
		objList[1]->diffusetReflection[2] = 0;
		objList[1]->specularReflection[0] = 0.0;
		objList[1]->specularReflection[1] = 0.0;
		objList[1]->specularReflection[2] = 0.0;
		objList[1]->speN = 50;

		// Blue sphere
		objList[2]->ambiantReflection[0] = 0.1;
		objList[2]->ambiantReflection[1] = 0.4;
		objList[2]->ambiantReflection[2] = 0.4;
		objList[2]->diffusetReflection[0] = 0;
		objList[2]->diffusetReflection[1] = 1;
		objList[2]->diffusetReflection[2] = 1;
		objList[2]->specularReflection[0] = 0.2;
		objList[2]->specularReflection[1] = 0.4;
		objList[2]->specularReflection[2] = 0.4;
		objList[2]->speN = 300;

		// Purple sphere
		objList[3]->ambiantReflection[0] = 0.3;
		objList[3]->ambiantReflection[1] = 0.3;
		objList[3]->ambiantReflection[2] = 0.3;
		objList[3]->diffusetReflection[0] = 0.7;
		objList[3]->diffusetReflection[1] = 0.7;
		objList[3]->diffusetReflection[2] = 0.7;
		objList[3]->specularReflection[0] = 0.6;
		objList[3]->specularReflection[1] = 0.6;
		objList[3]->specularReflection[2] = 0.6;
		objList[3]->speN = 650;
	}
}

void keyboard (unsigned char key, int x, int y)
{
	//keys to control scaling - k
	//keys to control rotation - alpha
	//keys to control translation - tx, ty
	switch (key) {
	case 's':
	case 'S':
		sceneNo = (sceneNo + 1 ) % NUM_SCENE;
		setScene(sceneNo);
		renderScene();
		glutPostRedisplay();
		break;
	case 'q':
	case 'Q':
		exit(0);

		default:
		break;
	}
}

int main(int argc, char **argv)
{

	
	cout<<"<<CS3241 Lab 5>>\n\n"<<endl;
	cout << "S to go to next scene" << endl;
	cout << "Q to quit"<<endl;
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (WINWIDTH, WINHEIGHT);

	glutCreateWindow ("CS3241 Lab 5: Ray Tracing");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard);

	objList = new RtObject*[NUM_OBJECTS];

	// create four spheres
	objList[0] = new Sphere(Vector3(-130, 80, 120), 100);
	objList[1] = new Sphere(Vector3(130, -80, -80), 100);
	objList[2] = new Sphere(Vector3(-130, -80, -80), 100);
	objList[3] = new Sphere(Vector3(130, 80, 120), 100);

	setScene(0);

	setScene(sceneNo);
	renderScene();

	glutMainLoop();

	for (int i = 0; i < NUM_OBJECTS; i++)
		delete objList[i];
	delete[] objList;

	delete[] pixelBuffer;

	return 0;
}
