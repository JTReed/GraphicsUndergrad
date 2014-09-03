// Method of producing sphere points from http://stackoverflow.com/questions/4081898/procedurally-generate-a-sphere-mesh

#define _USE_MATH_DEFINES

#include <math.h>
#include <iostream>
#include <glm/glm.hpp>
#include "Sphere.h"

using namespace glm;
using namespace std;

Sphere::Sphere()
{
}

//Special normal geometry builder for spheres/mars.
void Sphere::BuildNormalVisualizationGeometry()
{
	const float normal_scalar = 0.125f;
	int vertex_index = 0;
	for(uint i = 0; i < normal_vertices.size(); i++)
	{
		this->normal_vertices[i] = this->vertex_list[this->vertex_indices[vertex_index]].position;
		i++;
		this->normal_vertices[i] = this->vertex_list[this->vertex_indices[vertex_index]].position + this->vertex_list[vertex_indices[vertex_index]].normal * normal_scalar;
		vertex_index++;
	}
	
}

//Calculates the normal for a sphere.
//Set all the normals for the top/bottom to straight up/down!
void Sphere::CalcNormals(int slices, int stacks)
{
	super::CalcNormals(slices, stacks);

	for(int slice = 0; slice < slices; slice++)
	{
		this->vertex_list[slice].normal = vec3(0.f, -1.0f, 0.0f);
		this->vertex_list[slice + slices*(stacks - 1)].normal = vec3(0.f, 1.0f, 0.0f);
	}
}

//Build the mesh/shape, set up normals, and initialize the mesh bits.
bool Sphere::Initialize(float radius, int slices, int stacks, vec3 color)
{


	BuildMesh(slices, stacks, color);
	BuildShape(radius, slices, stacks);
	CalcNormals(slices, stacks);
	this->BuildNormalVisualizationGeometry();
	if(!super::Initialize())
	{
		return false;
	}
	

	return true;
}

//Modify the points on the previously generated mesh so that we have a sphere.
void Sphere::BuildShape(float radius, int slices, int stacks)
{
	// Loop through EACH INDEX.
	for(int i = 0; i < (slices * stacks); i++)
	{
		int curr_slice = i % slices;
		int curr_stack = i / slices;

		//right now we're not adding the fan at top/bottom so may cause problems for texturing?
		float v_angle = -(float(M_PI) / 2.0f) + (float(curr_stack) * ((float(M_PI) / float(stacks - 1))));
		float h_angle = (2.0f * float(M_PI) * (float(curr_slice) / float(slices - 1)));

		this->vertex_list[i].position.x = radius * cos(v_angle) * sin(h_angle);
		this->vertex_list[i].position.y = radius * sin(v_angle);
		this->vertex_list[i].position.z = radius * cos(v_angle) * cos(h_angle);
	}
}

Sphere::~Sphere()
{
}
