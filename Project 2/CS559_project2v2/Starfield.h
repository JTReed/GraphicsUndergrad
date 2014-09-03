/* By Nik Ingrassia and Jackson Reed
*  For CS559 - Fall 2013 with perry Kivolowitz
*  
*  This class randomizes the vertex positions in a vertex list
*  to create a random starfield and draws them as points using a solid shader
*/

#pragma once
#include "Object.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Starfield : public Object
{
public:
	Starfield(void);
	void TakeDown();
	bool Initialize(float inner_radius, float depth, int stars);
	virtual void Draw(const glm::mat4 & projection, glm::mat4 modelview, const glm::ivec2 & size, const float time = 0);

	Shader solid_color;

private:

	// Utility function for conversion from degree to radians
	double toRadian(double d);

	std::vector<VertexAttributesP> vertex_list;
	typedef Object super;
};

