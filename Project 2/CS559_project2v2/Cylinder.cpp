#define _USE_MATH_DEFINES

#include "Cylinder.h"
#include <cmath>

using namespace std;
using namespace glm;

Cylinder::Cylinder()
{
}

void Cylinder::BuildNormalVisualizationGeometry()
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

//Builds the mesh and shape for a cylinder, and creates the normals and their visualization.
//Finishes with the standard initialize for any mesh object.
bool Cylinder::Initialize(float base_radius, float top_radius, float height, int slices, int stacks, vec3 color) 
{
	// Initialize a flat mesh - Mesh::Inizialize(slices, stacks + 2)
	//  - stacks + 2 because we need an extra stack on top and bottom to form the flat bits of the cylinder
	// Modify the points of that flat mesh by:
	// - 'wrapping' them around to make the cylinder
	// - bringing the top and bottom together to form the flat sides
	// Make sure everything is centered around the origin or it'll be hard to manage

	BuildMesh(slices, stacks, color);
	BuildShape(base_radius, top_radius, height, slices, stacks);
	this->CalcNormals(slices, stacks);
	this->BuildNormalVisualizationGeometry();
	if(!super::Initialize())
	{
		return false;
	}
	

	return true;

}

//Modifies the vertices so they form a cylinder!
void Cylinder::BuildShape(float base_radius, float top_radius, float height, int slices, int stacks)
{
	mat4 mv(1.0);

	// Loop through EACH INDEX.
	for(int i = 0; i < (slices * stacks); i++)
	{
		int curr_slice = i % slices;
		int curr_stack = i / slices;

		float radius = (top_radius * (float(curr_stack)/float(stacks))) + (base_radius * (1-(float(curr_stack)/float(stacks))));

		this->vertex_list[i].position.x = float(radius * sin(2.0 * M_PI * ((float)curr_slice/((float)slices -1))));
		this->vertex_list[i].position.y = curr_stack * (height / (float)stacks);
		this->vertex_list[i].position.z = float(radius * cos(2.0 * M_PI * ((float)curr_slice/((float)slices -1))));
	}

	
}

Cylinder::~Cylinder()
{

}
