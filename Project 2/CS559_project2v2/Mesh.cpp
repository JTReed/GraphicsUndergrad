//Based on "Top.cpp" in Perry Kovolovitz's Modern GL Example
#include "Mesh.h"

#include <iostream>

using namespace std;
using namespace glm;

//Just sets up some object variables.
Mesh::Mesh()
{
	this->shader_index = 0;
	this->has_specular = true;
}

Mesh::~Mesh()
{
}

// Sets up the list of normals
void Mesh::BuildNormalVisualizationGeometry()
{
	const float normal_scalar = 0.125f;
	for (int j = 1; j <= 3; ++j)
	{
		// Point of normal ON plane
		this->normal_vertices.push_back(VertexAttributesP(this->vertex_list[this->vertex_indices[this->vertex_indices.size() - j]].position));
		// point of normal AWAY FROM plane
		this->normal_vertices.push_back(VertexAttributesP(this->vertex_list[this->vertex_indices[this->vertex_indices.size() - j]].position + this->vertex_list[this->vertex_indices[this->vertex_indices.size() - j]].normal * normal_scalar));
		// Push point on plane first, then away
		this->normal_indices.push_back(this->normal_vertices.size() - 2);
		this->normal_indices.push_back(this->normal_vertices.size() - 1);
	}
}

//increments through the shaders to use!
void Mesh::StepShader()
{
	this->shader_index = ++this->shader_index % this->shaders.size();
}

//Build a mesh of a given number of slices/stacks, with a single color.
void Mesh::BuildMesh(int slices, int stacks, vec3 color)
{

	// Sanity check - this should never happen
	if(stacks <= 1) 
	{
		stacks = 2;
	}

	if(slices <= 1)
	{
		slices = 2;
	}

	/*
	* This makes vertices and connectivity for a FLAT, RECTANGULAR MESH
	* Possibly used when drawing a sphere, then positions modified
	*
	* 1.	Looping through the height and width, just make points
	* 2.	Go through the points and generate connectivity using a function that finds
	*		adjacent points
	* 3.	Connect a triangle ( /| ) if there is a point to the left, and connect a triangle ( |/ ) if there is a point to the right
	*		(Do not loop through the bottom row of points - already connected to points above it)
	* 4.	Generate normals for each point using a function, e.g. CalcNormal(vec3 position)
	* 5.	Push those normals onto their own list
	*/

	// When doing a flat mesh, all normals are straight on the Z axis - different for sphere and stuff
	const vec3 n = vec3(0.0f, 0.0f, 1.0f);

	// Here we just make the points and push them on our vertex_list
	for(int h = 0; h < stacks; h++)
	{
		for(int w = 0; w < slices; w++) 
		{
			VertexAttributesPCNT vertex;
			vertex.position = vec3((float)w, (float)h, 0.0f);
			vertex.normal = n;
			vertex.texture_coordinate = vec2(float(h) / float(stacks), float(w) / float(slices)); 
			vertex.color = color; 
			this->vertex_list.push_back(vertex);
		}
	}

	// Here we generate connectivity for the triangles
	// We don't need to run on the bottom row because of how we generate the connectivity
	// Fill out the vertex_indices list
	for(int h = 0; h < (stacks-1); h++)
	{
		for(int w = 0; w < slices; w++)
		{
			int index = (w + h * slices); //each row is offset by "width" vertices.
			//Check for left, then generate left-style tri
			if((index % slices) > 0) // If left vertex exists
			{
				// CONNECT LEFT TRIANGLE
				this->vertex_indices.push_back(index);
				this->vertex_indices.push_back(index + slices);
				this->vertex_indices.push_back(index + (slices - 1));

				Mesh::BuildNormalVisualizationGeometry();
			}

			//Check for right, then generate right-style tri
			if((index % slices) < (slices - 1))	// If right vertex exists
			{
				// CONNECT RIGHT TRIANGLE
				this->vertex_indices.push_back(index);
				this->vertex_indices.push_back(index + 1);
				this->vertex_indices.push_back(index + slices);

				Mesh::BuildNormalVisualizationGeometry();
			}
		}
	}
}

//Standard mesh initialize.
//Initializes the object, then sets up our data buffers/inputs/etc
//Initializes the shaders the object will use - Phong or noSpecPhong and the toon shader.
bool Mesh::Initialize()
{
	// CHECK FOR ERRORS!
	if(this->GLReturnedError("Mesh::Initialize - on entry")) 
	{
		return false;
	}

	if(!super::Initialize())
	{
		return false;
	}

	// set up vertex array and the data buffer
	if (!this->PostGLInitialize(&this->vertex_array_handle, &this->vertex_coordinate_handle, this->vertex_list.size() * sizeof(VertexAttributesPCNT), &this->vertex_list[0]))
	{
		return false;
	}

	if(this->GLReturnedError("Mesh::Initialize - after PostGLInit"))
	{
		return false;
	}

	// Put in values for shader's LAYOUT variables
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) (sizeof(vec3) * 2));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesPCNT), (GLvoid *) (sizeof(vec3) * 1));
	// Activate them
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	//free our buffer and vertex array!
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (this->normal_vertices.size() > 0)
	{
		if (!this->PostGLInitialize(&this->normal_array_handle, &this->normal_coordinate_handle, this->normal_vertices.size() * sizeof(VertexAttributesP), &this->normal_vertices[0]))
			return false;

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttributesP), (GLvoid *) 0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	if(this->GLReturnedError("Mesh::Initialize - after attribute poniter"))
	{
		return false;
	}

	if(has_specular)
	{
		// For testing when drawing mesh - used solid shader for everything when in debug mode
		//#ifdef _DEBUG
		//if (!this->shader.Initialize("solid_shader.vert", "solid_shader.frag"))
		//#else
		if (!this->phong_shader.Initialize("phong_shader.vert", "multi_phong_shader.frag"))
		//#endif
		{
			return false;
		}
	} 
	else
	{
		if (!this->phong_shader.Initialize("phong_shader.vert", "NoSpec_shader.frag"))
		{
			return false;
		}
	}

	if (!this->solid_color_shader.Initialize("solid_shader.vert", "solid_shader.frag"))
	{
		return false;
	}

	
	if(!this->toon_shader.Initialize("phong_shader.vert", "toon_shader.frag"))
	{
		return false;
	}

	if(this->GLReturnedError("Mesh::Initialize - after shader Init"))
	{
		return false;
	}

	this->shaders.push_back(&this->phong_shader);
	this->shaders.push_back(&this->toon_shader);

	return true;
}

void Mesh::BuildShape()
{
}

//Take down all of the bits and bobs we use when we make the mesh!
void Mesh::TakeDown()
{
	this->vertex_list.clear();	// TODO: Check for memory leaks!!
	this->phong_shader.TakeDown();
	this->solid_color_shader.TakeDown();
	this->toon_shader.TakeDown();
	super::TakeDown();
}

//Draws the object and possibly its normals.
void Mesh::Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, const float time)
{
	if(this->GLReturnedError("Mesh::Draw - on entry"))
	{
		return;
	}

	glEnable(GL_DEPTH_TEST);
	//rotate along y axis based on time!
	modelview = rotate(modelview, time, vec3(0.0f, 1.0f, 0.0f)); 
	mat4 mvp = projection * modelview;
	mat3 nm = inverse(transpose(mat3(modelview)));
	
	this->shaders[this->shader_index]->Use();
	this->GLReturnedError("Mesh::Draw - after use");
	this->shaders[this->shader_index]->CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
	this->GLReturnedError("Mesh::Draw - after common setup");
	glBindVertexArray(this->vertex_array_handle);
	glDrawElements(GL_TRIANGLES, this->vertex_indices.size(), GL_UNSIGNED_INT, &this->vertex_indices[0]);
	glBindVertexArray(0);
	glUseProgram(0);

	if (this->draw_normals)
	{
		this->solid_color_shader.Use();
		this->solid_color_shader.CommonSetup(time, value_ptr(size), value_ptr(projection), value_ptr(modelview), value_ptr(mvp), value_ptr(nm));
		glBindVertexArray(this->normal_array_handle);
		glDrawElements(GL_LINES , this->normal_indices.size(), GL_UNSIGNED_INT , &this->normal_indices[0]);
		glBindVertexArray(0);
		glUseProgram(0);
	}

	if (this->GLReturnedError("Mesh::Draw - on exit"))
	{
		return;
	}	
}

//Calculates the cross product of two lines generated from the center point and the vertices in the two given directions.
vec3 Mesh::CrossProduct(int index, int point_direction1, int point_direction2)
{
	vec3 norm1 = this->vertex_list[index + point_direction1].position - this->vertex_list[index].position;
	vec3 norm2 = this->vertex_list[index + point_direction2].position - this->vertex_list[index].position;

	vec3 normal = cross(norm1, norm2);
	return normal;
}

//Calculates the normals based on the neighboring vertices.
//Checks for all the possible neighbor vertices.
//Averages out the edge normals when they should be the same - for spheres/cylinders.
void Mesh::CalcNormals(int slices, int stacks)
{	
	cout << "Calculating normals..." << endl;

	int up_left = slices - 1;
	int up = slices;
	int up_right = slices + 1;
	int right = 1;
	int down_right = -slices + 1;
	int down = -slices;
	int down_left = -slices - 1;
	int left = -1;

	vec3 normal;
	int num_normals_to_avg;

	for(size_t index = 0; index < this->vertex_list.size(); index++)
	{
		int curr_slice = index % slices;
		int curr_stack = index / slices;

		bool on_top = (curr_stack == stacks - 1);
		bool on_bottom = (curr_stack == 0);
		bool on_left = (curr_slice == 0);
		bool on_right = (curr_slice == slices - 1);

		normal = vec3(0.0f, 0.0f, 0.0f);
		num_normals_to_avg = 0;

		if(!on_top)
		{
			if(!on_right)
			{
				normal += CrossProduct(index, right, up);
				num_normals_to_avg++;
			}
			if(!on_left)
			{
				normal += CrossProduct(index, up, up_left);
				normal += CrossProduct(index, up_left, left);
				num_normals_to_avg += 2;
			}
		}
		if(!on_bottom)
		{
			if(!on_right)
			{
				normal += CrossProduct(index, down_right, right);
				normal += CrossProduct(index, down, down_right);
				num_normals_to_avg += 2;
			}
			if(!on_left)
			{
				normal += CrossProduct(index, left, down);
				num_normals_to_avg++;
			}
		}

		normal /= num_normals_to_avg;
		normal = normalize(normal);
		this->vertex_list[index].normal = normal;
		// cout << this->vertex_list[index].normal.x << " " << this->vertex_list[index].normal.y << " " << this->vertex_list[index].normal.z << endl;
	}

	// Take the average of the left and right (they touch in every case here)
	vec3 avg_edge_normal;
	for(int stack = 0; stack < stacks; stack++)
	{
		avg_edge_normal = this->vertex_list[stack*slices].normal + this->vertex_list[stack*slices + (slices - 1)].normal;
		avg_edge_normal /= 2;
		avg_edge_normal = normalize(avg_edge_normal);

		this->vertex_list[stack*slices].normal = avg_edge_normal;
		this->vertex_list[stack*slices + (slices - 1)].normal = avg_edge_normal;
	}
}