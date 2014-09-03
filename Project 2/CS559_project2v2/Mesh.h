//Based on "Top.h" in Perry Kivolowitz's Modern GL Example
/* By Nik Ingrassia and Jackson Reed
*  For CS559 - Fall 2013 with perry Kivolowitz
*  
*  This class creates a flat mesh of some dimension and creates a 
*  vector of their connectedness in order to draw triangles. Also
*  contains the functionality to draw and set up shaders of a mesh.
*  This class is the base of Sphere, Cylinder, and Mars
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object.h"
#include "Shader.h"

class Mesh: public Object
{
public:
	Mesh();
	void StepShader();
	void TakeDown();
	void BuildMesh(int slices, int stacks, glm::vec3 color); // height will end up as stacks, width will end up as slices
	bool Initialize();
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);

	Shader phong_shader;
	Shader texture_shader;
	Shader toon_shader;
	Shader solid_color_shader;

	~Mesh();

protected:
	bool has_specular;
	virtual void BuildShape();
	virtual void BuildNormalVisualizationGeometry();
	glm::vec3 CrossProduct(int index, int vertex_position1, int vertex_position2);
	virtual void CalcNormals(int slices, int stacks);
	std::vector<VertexAttributesPCNT> vertex_list;
	std::vector<Shader *> shaders;

private:
	typedef Object super;
	int shader_index;

};