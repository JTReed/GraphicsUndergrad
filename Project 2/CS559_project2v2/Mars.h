/* By Nik Ingrassia and Jackson Reed
*  For CS559 - Fall 2013 with perry Kivolowitz
*  
*  This class modifies the vertex positions and normals in a sphere
*  mesh to create a mars model based on a text file of altitudes
*/

#pragma once
#include "sphere.h"
#include <string>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Mars :
	public Sphere
{
public:
	Mars(void);
	bool Initialize(char * filename, float radius, float max_offset, vec3 color);
	void ReadFile(char * filename, vector<float> mars_radius, int &slices, int &stacks);
	void BuildShape(vector<float> mars_radius, int slices, int stacks);
	~Mars(void);

private:
	typedef Sphere super;
};

