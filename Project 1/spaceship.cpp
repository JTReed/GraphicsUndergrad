/* The spaceship oject class
*  You can build a displaylist for the ships and draw them from this class
*/

#include <iostream>
#include "spaceship.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace glm;
using namespace std;

int Spaceship::slices = 20;
int Spaceship::stacks = 20;

Spaceship::Spaceship(vec3 position)
{
	this->position = position;
	this->sphereHandle = BAD_GL_VALUE;
	this->cylinderHandle = BAD_GL_VALUE;
	this->finHandle = BAD_GL_VALUE;
	this->isInitialized = false;
}

void Spaceship::Draw()
{
	// TODO - REORGANIZE SCALE, ROTATE, AND TRANSLATE FUNCTIONS TO BE MORE EFFICIENT

	glColor3f(1, 0, 0);
	mat4 m(1.0f);
	mat4 mv(1.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, value_ptr(mv));

	// Draw the cylinder that forms the body where the function call specifies
	m = translate(mv, this->position);
	m = rotate(m, 270.0f, vec3(1, 0, 0));
	m = rotate(m, 45.0f, vec3(0, 0, 1));
	m = scale(m, vec3(1, 1, 4));
	glLoadMatrixf(value_ptr(m));
	glCallList(this->cylinderHandle);
	m = scale(m, vec3(1, 1, .25));
	
	// Draw the sphere at the base
	glColor3f(1, 1, 1);
	glLoadMatrixf(value_ptr(m));
	glCallList(this->sphereHandle);	

	// Draw the stretched sphere that forms the nose of the rocket
	glColor3f(.5f, 0, 0);
	m = translate(m, vec3(0.0f, 0.0f, 4.0f));
	m = scale(m, vec3(1.0f, 1.0f, 2.0f));
	glLoadMatrixf(value_ptr(m));
	glCallList(this->sphereHandle);

	//Move back down to the original position and scale
	m = scale(m, vec3(1.0f, 1.0f, 0.5f));
	m = translate(m, vec3(0.0f, 0.0f, -4.0f));
	m = rotate(m, -270.0f, vec3(1, 0, 0));

	// move to position of first booster and scale appropriately
	m = scale(m, vec3(0.5f, 0.5f, 0.5f));
	m = translate(m, vec3(4.0f, -1.0f, 0.0f));
	m = rotate(m, 270.0f, vec3(1, 0, 0));

	// Draw each booster
	for(int i = 0; i < 4; i++) {
		// Draw the cylinder
		glColor3f(1, 0, 0);
		m = scale(m, vec3(1, 1, 3));
		glLoadMatrixf(value_ptr(m));
		glCallList(this->cylinderHandle);
		m = scale(m, vec3(1, 1, 1.0/3.0));

		//Draw the stretched sphere on bottom
		glColor3f(1, 1, 1);
		m = scale(m, vec3(1.0f, 1.0f, 2.0f));
		glLoadMatrixf(value_ptr(m));
		glCallList(this->sphereHandle);
		m = scale(m, vec3(1.0f, 1.0f, 0.5f));

		// Draw the sphere on top
		glColor3f(.3f, 0, 0);
		m = translate(m, vec3(0.0f, 0.0f, 3.0f));
		glLoadMatrixf(value_ptr(m));
		glCallList(this->sphereHandle);
		m = translate(m, vec3(0.0f, 0.0f, -3.0f));

		// Move position to where the next booster should be drawn
		m = rotate(m, 90.0f, vec3(0, 0, 1));
		m = translate (m, vec3(4.0f, 4.0f, 0.0f));
	}

	// back to origin of ship
	m = rotate(m, -270.0f, vec3(1, 0, 0));
	m = translate(m, vec3(-4.0f, 1.0f, 0.0f));
	m = scale(m, vec3(2.0f, 2.0f, 2.0f));

	// Draw two of the fins in one scale
	glColor3f(1, .5f, .1);
	m = scale(m, vec3(0.5f, 1.5f, 1.0f));
	for(int i = 0; i < 2; i++) {
		glLoadMatrixf(value_ptr(m));
		glCallList(this->finHandle);
		m = rotate(m, 180.0f, vec3(0, 1, 0));
	}
	m = scale(m, vec3(2.0f, (1.0f/1.5f), 1.0f));

	// Draw the other two fins in another scale
	m = rotate(m, 90.0f, vec3(0, 1, 0));
	m = scale(m, vec3(.5f, 1.5f, 1.0f));
	for(int i = 0; i < 2; i++) {
		glLoadMatrixf(value_ptr(m));
		glCallList(this->finHandle);
		m = rotate(m, 180.0f, vec3(0, 1, 0));
	}

	glLoadMatrixf(value_ptr(mv));
}

// Creates displayLists for each shape: Spheres, Cylinders, Fins (modified cylinders)
// MAKE SURE TO DELETE EACH LIST
bool Spaceship::Initialize()
{
	if(this->sphereHandle ==  BAD_GL_VALUE || this->cylinderHandle == BAD_GL_VALUE) {

		GLUquadric * q = gluNewQuadric();

		if(q != NULL) {
			if((this->sphereHandle = glGenLists(1)) == 0) {
				cout << "Spaceship::Initialize() - Failed to GenLists() for sphereHandle" << endl;
				return false;
			}

			glNewList(this->sphereHandle, GL_COMPILE);		// List at DisplayList(1)
			gluSphere(q, 1.0, this->slices, this->stacks);
			glEndList();

			if((this->cylinderHandle = glGenLists(1)) == 0) {
				cout << "Spaceship::Initialize() - Failed to GenLists() for cylinderHandle" << endl;
				return false;
			}

			glNewList(this->cylinderHandle, GL_COMPILE);	// List at DisplayList(2)
			gluCylinder(q, 1.0, 1.0, 1.0, this->slices, this->stacks);
			glEndList();

			if((this->finHandle = glGenLists(1)) == 0) {
				cout << "Spaceship::Initialize() - Failed to GenLists() for finHandle" << endl;
				return false;
			}

			glNewList(this->finHandle, GL_COMPILE);			// List at DisplayList(3)
			gluCylinder(q, 0.6, 0.25, 2.0, this->slices, this->stacks);
			glEndList();

			gluDeleteQuadric(q);
			this->isInitialized = true;
		} else {
			return false;
		}
	}
	return true;
}


