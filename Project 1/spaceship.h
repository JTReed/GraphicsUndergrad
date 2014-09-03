#pragma once	
#include <GL/freeglut.h>
#include <glm/glm.hpp>

#ifndef	BAD_GL_VALUE
#define	BAD_GL_VALUE	GLuint(-1)
#endif

class Spaceship
{
public:
	Spaceship(glm::vec3 positions);
	bool Initialize();
	void Draw();
	bool isInitialized;
	GLuint sphereHandle;
	GLuint cylinderHandle;
	GLuint finHandle;
	glm::vec3 position;
	static int slices;
	static int stacks;

private:
	// Make a position non-optional
	Spaceship() {};
};
