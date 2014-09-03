/* UW Madison, CS 559 - Fall Semester 2013
*  By Jackson Reed and Zhouyang Michael Ye
*  
*  Project 1: This program contains two windows, each holding a different viewpoint of 
*  an array of spaceship models. The user is given control of the camera and can view
*  the ships from different angles. The third person view shows what the camera sees
*  in a graphical way. 
*/

#include <vector>
#include <iostream>
#include <array>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES 
#include <math.h>
#include "spaceship.h"

using namespace glm;
using namespace std;

#ifndef BAD_GL_VALUE
#define BAD_GL_VALUE	GLuint(-1)
#endif

class GlobalVars
{
public:
	GlobalVars();
	float fov, nearPlane, farPlane;
	bool WindowClosed;
	ivec2 fpvWindowSize, tpvWindowSize;
	int period;
	bool wireframe;
	float cameraHDegree, cameraVDegree, cameraRadius;
	float eyePosX, eyePosY, eyePosZ;
	float fpvAspect, tpvAspect;
	int window1, window2;
	mat4 fpvMv, fpvProj;
	vec2 currentMousePos;
	bool rmClicked;
	vector<Spaceship *> ships;

} globalVars;

GlobalVars::GlobalVars()
{
	this->fov = 50.0f;
	this->nearPlane = 5.0f;
	this->farPlane = 35.0f;
	this->WindowClosed = true;
	this->fpvWindowSize = ivec2(700, 600);
	this->tpvWindowSize = ivec2(900, 400);
	this->period = 1000 / 60;
	this->wireframe = false;
	this->cameraHDegree = 0.0;
	this->cameraVDegree = 45.0;
	this->cameraRadius = 25.0;
	this->rmClicked = false;
	this->fpvAspect = float(globalVars.fpvWindowSize.x) / float(globalVars.fpvWindowSize.y);
	this->tpvAspect = float(globalVars.tpvWindowSize.x / 2.0f) / float(globalVars.tpvWindowSize.y);
}

// Utility function for drawing axis
void drawAxis()
{
	glBegin(GL_LINES);
		glLineWidth(1.0);
		glColor3f(1, 0, 0); 
		glVertex3f(-10, 0, 0); 
		glVertex3f(10, 0, 0);
		glColor3f(0, 1, 0); 
		glVertex3f(0, -10, 0); 
		glVertex3f(0, 10, 0);
		glColor3f(0, 0, 1); 
		glVertex3f(0, 0, -10); 
		glVertex3f(0, 0, 10);
	glEnd();
}

// Utility function for conversion from degree to radians
float toRadian(float d)
{
	return (float)(d / 180.0f * M_PI);
}

// Handles the display within the FIRST PERSON window
void fpvDisplayFunc()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	//Determines if the spaceships are rendered as wireframes or solid shapes
	glPolygonMode(GL_FRONT_AND_BACK, (globalVars.wireframe == true) ? GL_LINE : GL_FILL);
	
	//Sets the viewport of the FIRST PERSON window
	glViewport(0, 0, globalVars.fpvWindowSize.x, globalVars.fpvWindowSize.y);

	//Calculates and loads the projection matrix
	mat4 proj = perspective(globalVars.fov, globalVars.fpvAspect, globalVars.nearPlane, globalVars.farPlane);
	globalVars.fpvProj = proj;
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(proj));

	//Calculates the coordinates of the camera and sets the camera position and orientation
	mat4 mv(1.0f);
	globalVars.eyePosX = globalVars.cameraRadius * cos(toRadian(globalVars.cameraHDegree)) * cos(toRadian(globalVars.cameraVDegree));
	globalVars.eyePosY = globalVars.cameraRadius * sin(toRadian(globalVars.cameraVDegree));
	globalVars.eyePosZ = globalVars.cameraRadius * sin(toRadian(globalVars.cameraHDegree)) * cos(toRadian(globalVars.cameraVDegree));
	mv = lookAt(vec3(globalVars.eyePosX, globalVars.eyePosY, globalVars.eyePosZ), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	globalVars.fpvMv = mv;

	//Loads the ModelView Matrix and draws the spaceships
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(mv));
	for (vector<Spaceship *>::iterator iter = globalVars.ships.begin(); iter < globalVars.ships.end(); iter++) {
		(*iter)->Draw();
	}

	// Begin drawing the TEXT:
	// Reset the projection matrix to be orthographic
	// Draw the text in a new lookat matrix that does not change
	// Make sure to disable lighting or it'll look wierd
	mat4 orth = ortho(0.0f, (float)globalVars.fpvWindowSize.x, 0.0f, (float)globalVars.fpvWindowSize.y, 1.0f, 10.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(orth));

	// Look at the same place constantly, and shift the modelview matrix in front of the near plane
	mat4 text = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	text = translate(text, vec3(0.0f, 0.0f, -1.0f));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(text));
	
	// Scale accordingly, the modelview needs to be moved up or else the bottom of the text will be cut off
	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 0.0f);
	text = scale(text, vec3(.4f, .4f, .4f));
	text = translate(text, vec3(glutStrokeWidth(GLUT_STROKE_ROMAN, 'P')/4, glutStrokeHeight(GLUT_STROKE_ROMAN)/4, 0.0f));	// "Magic numbers" to match aspect
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"Perspective View");

	// Scale for smaller text, transform a uniform amount of space up after each line
	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	text = scale(text, vec3(.4f, .4f, .4f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"Left / Right ; Up / Down ; PgUp / PgDown");

	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"x - exits the program");

	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"w - toggle wireframe");

	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"o/p - Near Plane ; k/l - Far Plane ; r - reset");

	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}

//Handles the reshaping of the FIRST PERSON window
void fpvReshapeFunc(int w, int h)
{
	//If window is not closed, resize the window, recalculate the aspect, and redisplay the window
	if(globalVars.WindowClosed == false && h > 0) {
		globalVars.fpvWindowSize = ivec2(w, h);
		globalVars.fpvAspect = float(globalVars.fpvWindowSize.x) / float(globalVars.fpvWindowSize.y);
		glutPostWindowRedisplay(globalVars.window1);
	}
}

//Calculates a modeling coordinate from the clipping space coordinate using the
//inverses of the projection and modelview matrix of the FIRST PERSON window
vec4 calcCoord(vec3 vertex)
{
	vec4 coord = inverse(globalVars.fpvMv) * inverse(globalVars.fpvProj) * vec4(vertex, 1.0f);
	return coord;
}

//Draws the wireframe representation of the frustum in the THIRD PERSON window
void DrawTpvCamera()
{
	//Sets the color, translate to the position of the camera and draw the representation of the camera
	glColor3f(1.0f, 1.0f, 1.0f);
	glDisable(GL_LIGHTING);
	mat4 m(1.0f);
	mat4 mv(1.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, value_ptr(mv));
	m = translate(mv, vec3(globalVars.eyePosX, globalVars.eyePosY, globalVars.eyePosZ));
	glLoadMatrixf(value_ptr(m));
	glutSolidSphere(0.5, 30, 30);
	glLoadMatrixf(value_ptr(mv));


	//Calculates and stores the coordinates of the verticies of the far clipping plane of the frustum in the THIRD PERSON window
	vec4 farPlaneCoord [4];
	int counter = 0;
	for(int xVal = -1; xVal < 2; xVal += 2)
	{
		for(int yVal = -1; yVal < 2; yVal += 2)
		{
			farPlaneCoord[counter] = calcCoord(vec3(xVal, yVal, 1.0f));
			farPlaneCoord[counter] = farPlaneCoord[counter] / farPlaneCoord[counter].w;
			counter++;
		}
	}

	//Calculates and stores the coordinates of the verticies of the near clipping plane of the frustum in the THIRD PERSON window
	vec4 nearPlaneCoord [4];
	counter = 0;
	for(int xVal = -1; xVal < 2; xVal += 2)
	{
		for(int yVal = -1; yVal < 2; yVal += 2)
		{
			nearPlaneCoord[counter] = calcCoord(vec3(xVal, yVal, -1.0f));
			nearPlaneCoord[counter] = nearPlaneCoord[counter] / nearPlaneCoord[counter].w;
			counter++;
		}
	}

	//Translate to and draw spheres at the verticies of the far clipping plane of the frustum in the THIRD PERSON window
	//sizeof() returns size in bits, divide by sizeof() one element to get size of array
	for(int index = 0; index < sizeof(farPlaneCoord) / sizeof(farPlaneCoord[0]); index++)
	{
		m = translate(mv, vec3(farPlaneCoord[index].x, farPlaneCoord[index].y, farPlaneCoord[index].z));
		glLoadMatrixf(value_ptr(m));
		glutSolidSphere(0.5, 30, 30);
		glLoadMatrixf(value_ptr(mv));
	}

	//Translate to and draw spheres at the verticies of the near clipping plane of the frustum in the THIRD PERSON window
	for(int index = 0; index < sizeof(nearPlaneCoord) / sizeof(nearPlaneCoord[0]); index++)
	{
		m = translate(mv, vec3(nearPlaneCoord[index].x, nearPlaneCoord[index].y, nearPlaneCoord[index].z));
		glLoadMatrixf(value_ptr(m));
		glutSolidSphere(0.5, 30, 30);
		glLoadMatrixf(value_ptr(mv));
	}

	//Draw the wireframe representation of the frustum in the THIRD PERSON window
	glBegin(GL_LINES);
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(2.0f);

		//Draw the rectangle representing the far clipping plane
		glVertex3f(farPlaneCoord[0].x, farPlaneCoord[0].y, farPlaneCoord[0].z);
		glVertex3f(farPlaneCoord[1].x, farPlaneCoord[1].y, farPlaneCoord[1].z);
		glVertex3f(farPlaneCoord[2].x, farPlaneCoord[2].y, farPlaneCoord[2].z);
		glVertex3f(farPlaneCoord[3].x, farPlaneCoord[3].y, farPlaneCoord[3].z);
		glVertex3f(farPlaneCoord[0].x, farPlaneCoord[0].y, farPlaneCoord[0].z);
		glVertex3f(farPlaneCoord[2].x, farPlaneCoord[2].y, farPlaneCoord[2].z);
		glVertex3f(farPlaneCoord[1].x, farPlaneCoord[1].y, farPlaneCoord[1].z);
		glVertex3f(farPlaneCoord[3].x, farPlaneCoord[3].y, farPlaneCoord[3].z);

		//Draw the rectangle representing the near clipping plane
		glVertex3f(nearPlaneCoord[0].x, nearPlaneCoord[0].y, nearPlaneCoord[0].z);
		glVertex3f(nearPlaneCoord[1].x, nearPlaneCoord[1].y, nearPlaneCoord[1].z);
		glVertex3f(nearPlaneCoord[2].x, nearPlaneCoord[2].y, nearPlaneCoord[2].z);
		glVertex3f(nearPlaneCoord[3].x, nearPlaneCoord[3].y, nearPlaneCoord[3].z);
		glVertex3f(nearPlaneCoord[0].x, nearPlaneCoord[0].y, nearPlaneCoord[0].z);
		glVertex3f(nearPlaneCoord[2].x, nearPlaneCoord[2].y, nearPlaneCoord[2].z);
		glVertex3f(nearPlaneCoord[1].x, nearPlaneCoord[1].y, nearPlaneCoord[1].z);
		glVertex3f(nearPlaneCoord[3].x, nearPlaneCoord[3].y, nearPlaneCoord[3].z);

		//Draw the lines between corresponding points of the near and far clipping plane
		glVertex3f(nearPlaneCoord[0].x, nearPlaneCoord[0].y, nearPlaneCoord[0].z);
		glVertex3f(farPlaneCoord[0].x, farPlaneCoord[0].y, farPlaneCoord[0].z);
		glVertex3f(nearPlaneCoord[1].x, nearPlaneCoord[1].y, nearPlaneCoord[1].z);
		glVertex3f(farPlaneCoord[1].x, farPlaneCoord[1].y, farPlaneCoord[1].z);
		glVertex3f(nearPlaneCoord[2].x, nearPlaneCoord[2].y, nearPlaneCoord[2].z);
		glVertex3f(farPlaneCoord[2].x, farPlaneCoord[2].y, farPlaneCoord[2].z);
		glVertex3f(nearPlaneCoord[3].x, nearPlaneCoord[3].y, nearPlaneCoord[3].z);
		glVertex3f(farPlaneCoord[3].x, farPlaneCoord[3].y, farPlaneCoord[3].z);

		//Draw the center line from the camera to the center of the far clipping plane
		glVertex3f(globalVars.eyePosX, globalVars.eyePosY, globalVars.eyePosZ);
		glVertex3f(-globalVars.eyePosX * 0.5f, -globalVars.eyePosY * 0.5f, -globalVars.eyePosZ * 0.5f);
	}
	glEnd();

	glEnable(GL_LIGHTING);
}

// Handles all display for the THIRD PERSON window
void tpvDisplayFunc()
{
	glClearColor(.1f, .1f, .1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	//Set the Z axis viewport to the left side of the THIRD PERSON window
	glViewport(0, 0, (int)(globalVars.tpvWindowSize.x / 2), (int)globalVars.tpvWindowSize.y);

	//Calcualates and loads the projection matrix for the Z axis view
	mat4 proj = ortho(-35.0f * globalVars.tpvAspect, 35.0f * globalVars.tpvAspect, -35.0f, 35.0f, 1.0f, 150.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(proj));

	//Calculates and loads the ModelView matrix of the Z axis view
	mat4 mv(1.0f);
	mv = lookAt(vec3(0.0, 0.0, 30.0), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(mv));
	
	//Draw the grid for the camera
	glDisable(GL_LIGHTING|GL_DEPTH_TEST);
	mat4 m(1.0f);
	glLoadMatrixf(value_ptr(m));
	glColor3f(3.2f, 3.2f, 3.2f);
	glutWireSphere(globalVars.cameraRadius, 20, 20);
	glEnable(GL_LIGHTING|GL_DEPTH_TEST);
	glLoadMatrixf(value_ptr(mv));
	
	//Draw frustum representation and spaceships for the Z axis view
	DrawTpvCamera();
	for (vector<Spaceship *>::iterator iter = globalVars.ships.begin(); iter < globalVars.ships.end(); iter++)
	{
		(*iter)->Draw();
	}
	
	// First, draw the text on the Left (Z View) side of the window
	proj = ortho(0.0f, (float)globalVars.tpvWindowSize.x, 0.0f, (float)globalVars.tpvWindowSize.y, 1.0f, 10.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(proj));

	// Look at the same place constantly, and shift the modelview matrix in front of the near plane
	mat4 text(1.0);
	text = lookAt(vec3(0.0, 0.0, 3.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	text = translate(text, vec3(0.0, 0.0, -1));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(text));

	// Scale accordingly, the modelview needs to be moved up or else the bottom of the text will be cut off
	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 0.0f);
	text = scale(text, vec3(.4f, .2f, .4f));
	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN)/4, .0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"Z Axis View");

	// Scale for smaller text, transform a uniform amount of space up after each line
	text = translate(text, vec3(0.0, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0));
	text = scale(text, vec3(.7f, .7f, .7f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"Left / Right ; Up / Down ; PgUp / PgDown");

	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"x - exits the program");

	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"w - toggle wireframe");

	
	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"o/p - Near Plane ; k/l - Far Plane");

	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN), 0.0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"r - reset");
	

	glEnable(GL_LIGHTING);
	// End drawing Z VIEW text

	//Set the X axis view to the right side of the THIRD PERSON window
	glViewport(globalVars.tpvWindowSize.x / 2, 0, globalVars.tpvWindowSize.x / 2, (int)globalVars.tpvWindowSize.y);

	//Calculate and load the projection matrix of the X axis view
	proj = ortho(-35.0f * globalVars.tpvAspect, 35.0f * globalVars.tpvAspect, -35.0f, 35.0f, 1.0f, 150.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(proj));

	//Calculate and load the ModelView matrix of the X axis view
	mv = lookAt(vec3(30.0, 0.0, 0.0), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(mv));

	//Draw the grid for the camera
	glDisable(GL_LIGHTING|GL_DEPTH_TEST);
	m = mat4(1.0f);
	glColor3f(3.2f, 3.2f, 3.2f);
	m = rotate(m, 90.0f, vec3(0, 1, 0));
	glLoadMatrixf(value_ptr(m));
	glutWireSphere(globalVars.cameraRadius, 20, 20);
	glEnable(GL_LIGHTING|GL_DEPTH_TEST);
	glLoadMatrixf(value_ptr(mv));


	//Draw the representation of the frustum and the spaceships
	for (vector<Spaceship *>::iterator iter = globalVars.ships.begin(); iter < globalVars.ships.end(); iter++)
	{
		(*iter)->Draw();
	}
	DrawTpvCamera();

	// Then, draw the text on the X VIEW side
	proj = ortho(0.0f, (float)globalVars.tpvWindowSize.x, 0.0f, (float)globalVars.tpvWindowSize.y, 1.0f, 10.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(proj));

	// Scale accordingly, the modelview needs to be moved up or else the bottom of the text will be cut off
	text = lookAt(vec3(0.0, 0.0, 3.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	text = translate(text, vec3(0.0, 0.0, -1));
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(text));

	// Scale for smaller text
	glDisable(GL_LIGHTING);
	glColor3f(0.0f, 1.0f, 0.0f);
	text = scale(text, vec3(.4f, .2f, .4f));
	text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_ROMAN)/4, .0f));
	glLoadMatrixf(value_ptr(text));
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*)"X Axis View");

	glEnable(GL_LIGHTING);
	// End X VIEW Text

	glutSwapBuffers();

}

//Handles the reshaping of the THIRD PERSON window
void tpvReshapeFunc(int w, int h)
{
	//Sets the window size, recalculate the aspect of the viewports, and redisplays the window
	if(globalVars.WindowClosed == false && h > 0) {
		globalVars.tpvWindowSize = ivec2(w, h);
		globalVars.tpvAspect = float(globalVars.tpvWindowSize.x/2.0f) / float(globalVars.tpvWindowSize.y);
		glutPostWindowRedisplay(globalVars.window2);
	}
}

// Deletes all the allocated memory when the project is closed - IMPORTANT
void CloseFunc()
{
	globalVars.WindowClosed = true;

	// Delete DISPLAYLISTS
	glDeleteLists(1, 1); // SphereHandle
	glDeleteLists(2, 1); // Cylinderhandle
	glDeleteLists(3, 1); // FinHandle

	// Delete SPACESHIP objects, any GEOMETRY drawn(?), and TEXT(?)
	glutLeaveMainLoop();
	while (globalVars.ships.size() > 0)
	{
		delete *(globalVars.ships.end() - 1);	/// .end() is not a pointer to end of vector, but an iterator
												/// Use * to delete the OBJECT, not the interator (that's a crash)
		globalVars.ships.pop_back();
	}
}

//Implements keyboard commands for toggling of wireframe and closing the program
void KeyboardFunc(unsigned char c, int x, int y)
{
	switch(c) {
		case 'w': 
			//toggles wireframe
			globalVars.wireframe = !globalVars.wireframe;
			break;
		case 'x':
		case 27: // Esc
			// Closes program
			CloseFunc();
			return;
		case 'o': 
			// decreases near plane
			if(globalVars.nearPlane == .5f) {
				break;
			}
			globalVars.nearPlane -= .5f;
			break;
		
		case 'p':
			// increases near plane
			if(globalVars.nearPlane == globalVars.farPlane - .5f) {
				break;
			}
			globalVars.nearPlane += .5f;
			break;
		case 'l':
			// increases far plane
			if(globalVars.farPlane == 50.0f) {
				break;
			}
			globalVars.farPlane += .5f;
			break;
		case 'k':
			// deacreases far plane
			if(globalVars.farPlane == globalVars.nearPlane + .5f) {
				break;
			}
			globalVars.farPlane -= .5f;
			break;
		case 'r':
			//RESET
			globalVars.nearPlane = 5;
			globalVars.farPlane = 35;
			
	}

	//Redisplays windows
	glutPostWindowRedisplay(globalVars.window1);
	glutPostWindowRedisplay(globalVars.window2);
}


//Implements camera controls of the program
void SpecialFunc(int key, int x, int y)
{
	switch(key) {
		//Rotate camera left
		case GLUT_KEY_LEFT:
			globalVars.cameraHDegree = fmod((globalVars.cameraHDegree + 1.0f), 360.0f);
			break;
		//Rotate camera up, to a limit
		case GLUT_KEY_UP:
			if(globalVars.cameraVDegree >= 89.0f)
				break;
			globalVars.cameraVDegree = fmod((globalVars.cameraVDegree + 1.0f), 360.0f);
			break;
		//Rotate camera down, to a limit
		case GLUT_KEY_DOWN:
			if(globalVars.cameraVDegree <= -89.0f)
				break;
			globalVars.cameraVDegree = fmod((globalVars.cameraVDegree - 1.0f), 360.0f);
			break;
		//Rotate camera right
		case GLUT_KEY_RIGHT:
			globalVars.cameraHDegree = fmod((globalVars.cameraHDegree - 1.0f), 360.0f);
			break;
		//Decreases the field of view, zoom in
		case GLUT_KEY_PAGE_DOWN:
			if(globalVars.fov == 10.0)
				break;
			globalVars.fov = globalVars.fov - 0.5f;
			break;
		//Increases the field of view, zoom out
		case GLUT_KEY_PAGE_UP:
			if(globalVars.fov == 80.0)
				break;
			globalVars.fov = globalVars.fov + 0.5f;
			break;
	}
}

// Handles mouse buttons
void MouseFunc(int button, int state, int x, int y)
{
	// The boolean rmClicked prevents movement when middle or right buttons are clicked
	if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) {
			globalVars.currentMousePos = vec2(x, y);
		}
	}else {
		if(state == GLUT_DOWN) {
			globalVars.rmClicked = true;
		} else {
			globalVars.rmClicked = false;
		}
	}
}

// Handles mouse wheel control
void MouseWheelFunc(int button, int dir, int x, int y) 
{
	if(dir > 0) {
		if(globalVars.fov > 10) {
			globalVars.fov--;
		}
	} else {
		if(globalVars.fov < 80) {
			globalVars.fov++;
		}
	}
}

// Handles mouse movement when mouse button is down only
void MotionFunc(int x, int y)
{
	// Mousefunc only sets position at time of click
	// Need to reset that position after each movement
	// Scalar just scales down the movement speed
	int scalar = 5;

	if(!globalVars.rmClicked) {
		if(x > globalVars.currentMousePos.x) {
			globalVars.cameraHDegree = fmod((globalVars.cameraHDegree + (x - globalVars.currentMousePos.x) / scalar), 360.0f);
		}
		if(x < globalVars.currentMousePos.x) {
			globalVars.cameraHDegree = fmod((globalVars.cameraHDegree + (x - globalVars.currentMousePos.x) / scalar), 360.0f);
		}
		if(y > globalVars.currentMousePos.y) {
			globalVars.cameraVDegree = fmod((globalVars.cameraVDegree + (y - globalVars.currentMousePos.y) / scalar), 360.0f);
		}
		if(y < globalVars.currentMousePos.y) {
			globalVars.cameraVDegree = fmod((globalVars.cameraVDegree + (y - globalVars.currentMousePos.y) / scalar), 360.0f);
		}
		globalVars.currentMousePos = vec2(x, y);
	}
}

//Timer function of the program, automatically redisplays window 
void TimerFunc(int value)
{
	if (globalVars.WindowClosed != true)
	{
		glutTimerFunc(globalVars.period, TimerFunc, 0);
		glutPostWindowRedisplay(globalVars.window1);
		glutPostWindowRedisplay(globalVars.window2);
	}
}


bool InitializeGL()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);

	// yOffset determines where the ships draw on the y-axis
	float yOffset = -3.0f;

	// Depending on which Z "plane" tehe ships are on, there will be a different number of them
	// Draws ships in a square rhombus shape
	for(float z = -15.0f; z < 16.0f; z += 5.0f) 
	{
		if(abs(z) == 15.0f)
		{
			Spaceship * s = new Spaceship(vec3(0.0, yOffset, z));
			if(!s->Initialize()) 
			{
				return false;
			}
			globalVars.ships.push_back(s);
		}
		else if(abs(z) == 10.0f)
		{
			for(float x = -5.0; x < 6.0; x += 10.0f)
			{
				Spaceship * s = new Spaceship(vec3(x, yOffset, z));
				if(!s->Initialize()) 
				{
					return false;
				}
				globalVars.ships.push_back(s);
			}
		}
		else if(abs(z) == 5.0f)
		{
			for(float x = -10.0; x < 11.0; x += 10.0f)
			{
				Spaceship * s = new Spaceship(vec3(x, yOffset, z));
				if(!s->Initialize()) 
				{
					return false;
				}
				globalVars.ships.push_back(s);
			}
		}
		else if(abs(z) == 0.0f)
		{
			for(float x = -15.0; x < 16.0; x += 10.0f)
			{
				Spaceship * s = new Spaceship(vec3(x, yOffset, z));
				if(!s->Initialize()) 
				{
					return false;
				}
				globalVars.ships.push_back(s);
			}
		}
	}
	return true;
}

int main(int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutSetOption(GLUT_RENDERING_CONTEXT, GLUT_USE_CURRENT_CONTEXT);

	// Create and handle FPV window
	glutInitWindowPosition(100, 50);
	glutInitWindowSize(globalVars.fpvWindowSize.x, globalVars.fpvWindowSize.y);
	globalVars.window1 = glutCreateWindow("First Person");
	globalVars.WindowClosed = false;
	glutDisplayFunc(fpvDisplayFunc);
	glutReshapeFunc(fpvReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	// Mouse functions only available in FPV window
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	glutMouseWheelFunc(MouseWheelFunc);
	glutCloseFunc(CloseFunc);
	
	// Create and handle TPV window
	glutInitWindowPosition(500, 200);
	glutInitWindowSize(globalVars.tpvWindowSize.x, globalVars.tpvWindowSize.y);
	globalVars.window2 = glutCreateWindow("Third Person");
	glutDisplayFunc(tpvDisplayFunc);
	glutReshapeFunc(tpvReshapeFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutCloseFunc(CloseFunc);
	if(!InitializeGL())
	{
		CloseFunc();
		return -1;
	}

	glutTimerFunc(globalVars.period, TimerFunc, 0);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	glutMainLoop();
	return 0;
}
