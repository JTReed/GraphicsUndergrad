/* By Nik Ingrassia and Jackson Reed
*  For CS559 - Fall 2013 with perry Kivolowitz
*  
*  This class modifies the vertex positions and normals in a mesh to
*  create a sphere
*/

#pragma once

#include "Mesh.h"

class Sphere:public Mesh
{
public:
	Sphere();
	bool Initialize(float radius, int slices, int stacks, glm::vec3 color);
	void BuildNormalVisualizationGeometry();
	void CalcNormals(int slices, int stacks);
	void BuildShape(float radius, int slices, int stacks);
	~Sphere();
private:
	typedef Mesh super;
};

