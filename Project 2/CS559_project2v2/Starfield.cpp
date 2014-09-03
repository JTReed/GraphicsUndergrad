#define _USE_MATH_DEFINES

#include "Starfield.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>

using namespace std;
using namespace glm;


//picking a point on a sphere - technique taken from
//http://mathworld.wolfram.com/SpherePointPicking.html
//or will be eventually, once the dang thing works!


Starfield::Starfield()
{
}


//Initialize our Starfield!
//Generate a whole bunch of random points on a sphere with a randomized radius
//Set up shaders, etc. as well.
bool Starfield::Initialize(float inner_radius, float depth, int stars)
{
	// CHECK FOR ERRORS!
	if(this->GLReturnedError("Starfield::Initialize - on entry")) 
	{
		return false;
	}

	if(!super::Initialize())
	{
		return false;
	}

	//check our inputs for sanity!
	if((depth <= 0) || (inner_radius <= 0) || (stars <= 0))
	{
		return false;
	}

	//make a bunch of points 

	//seed the RNG - constant for testing!
	#ifdef _DEBUG
	srand(1);
	#else
	srand((unsigned int)(time(NULL)));
	#endif

	cout << "Randomizing position of " << stars << " stars..." << endl;

	////generate points for the stars, and place them in the arrays!
	for(int i = 0; i < stars; i++)
	{
		float radius = ((float(rand())/float(RAND_MAX)) * depth) + inner_radius;
		float h_angle = float(rand())/float(RAND_MAX) * M_PI * 2.0;
		float v_angle = acos((2.0 * float(rand())/float(RAND_MAX)) - 1.0);	//stars will be concentrated at poles.

		
		VertexAttributesP vertex = VertexAttributesP(vec3(
			radius * sin(v_angle) * cos(h_angle),
			radius * cos(v_angle),
			radius * sin(v_angle) * sin(h_angle)));
		this->vertex_list.push_back(vertex);
		//since we'll just draw these vertices as points, we don't care about connectivity
		//and just push each index on when we create a vertex
		this->vertex_indices.push_back(i);

	}


	//set up our handles
	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertex_list.size() * sizeof(VertexAttributesP), &this->vertex_list[0]))
	{
		return false;
	}

	if(this->GLReturnedError("Starfield::Initialize - after PostGLInit"))
	{
		return false;
	}

	//set up our layout
	glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, sizeof(VertexAttributesP), (GLvoid *) 0);
	//activate our vertex stuff
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if(this->GLReturnedError("Starfield::Initialize - after attribute pointer"))
	{
		return false;
	}

	if(!this->solid_color.Initialize("solid_shader.vert","solid_shader.frag"))
	{
		return false;
	}

	if(this->GLReturnedError("Starfield::Initialize - after shader init"))
	{
		return false;
	}

	return true;
}

//Draw the starfield - set up the solid color shader and pass in our points!
void Starfield::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, const float time)
{
	if(this->GLReturnedError("Starfield::Draw - on entry"))
	{
		return;
	}

	glEnable(GL_DEPTH_TEST);
	//rotate along y axis based on time!
	modelview = rotate(modelview, time, vec3(0.0f, 1.0f, 0.0f)); 
	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));

	glEnable(GL_PROGRAM_POINT_SIZE);
	solid_color.Use();
	this->GLReturnedError("Starfield::Draw - after use");
	solid_color.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	this->GLReturnedError("Starfield::Draw - after common setup");
	glBindVertexArray(this->vertex_array_handle);
	glDrawElements(GL_POINTS, this->vertex_indices.size(), GL_UNSIGNED_INT, &this->vertex_indices[0]);
	glBindVertexArray(0);
	glUseProgram(0);
}

//Take down the starfield's stuff!
void Starfield::TakeDown()
{
	this->vertex_indices.clear();
	this->vertex_list.clear();	// TODO: Check for memory leaks!!
	this->solid_color.TakeDown();
	super::TakeDown();
}
