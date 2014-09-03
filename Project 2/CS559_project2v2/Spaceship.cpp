#include "Spaceship.h"

#include <iostream>

using namespace std;

Spaceship::Spaceship(void)
{
	
}

// Initialize() initializes a sphere and cylinder and our wing.
bool Spaceship::Initialize(int sliceDetail, int stackDetail, vec3 color)
{
	cout << "Initilizing primitives to build ship..." << endl;
	if(!sphere.Initialize(1, sliceDetail, stackDetail, color))
	{
		return false;
	}

	if(!cylinder.Initialize(1, 1, 1, sliceDetail, stackDetail, color))
	{
		return false;
	}
	if(!wing.Initialize(.5, .1, 1, sliceDetail, stackDetail, color))
	{
		return false;
	}
	return true;
}


//draws the body of the spaceship!
void Spaceship::DrawBody(const mat4 & projection, mat4 mv, const ivec2 & size, const float time)
{
	sphere.Draw(projection, mv, size, time);

	mv = scale(mv, vec3(1.0f, 4.0f, 1.0f));
	cylinder.Draw(projection, mv, size, time);

	mv = translate(mv, vec3(0.0f, 1.0f, 0.0f));
	mv = scale(mv, vec3(1.0f, 0.5f, 1.0f));
	sphere.Draw(projection, mv, size, time);
}

//draws the wing of the spaceship.
void Spaceship::DrawWing(const mat4 & projection, mat4 mv, const ivec2 & size, const float time)
{
	mv = scale(mv, vec3(3.0f, 2.0f, .2f));
	mv = rotate(mv, -90.0f, vec3(0.0f, 0.0f, 1.0f));
	wing.Draw(projection, mv, size, time);

	mv = translate(mv, vec3(0.0f, 1.0f, 0.0f));
	mv = rotate(mv, -90.0f, vec3(0.0f, 0.0f, 1.0f));
	mv = scale(mv, vec3(1.0f/3.0f, .5f, 1.0f/.2f));
	mv = scale(mv, vec3(.2f, .2f, .2f));
	mv = translate(mv, vec3(0.0f, -2.0f, 0.0f));
	DrawBody(projection, mv, size, time);

}

//Draws a spaceship body, and four wings around it. 
void Spaceship::Draw(const mat4 & projection, mat4 modelview, const ivec2 & size, const float time)
{
	mat4 mv = translate(modelview, vec3(0.0f, -2.0f, 0.0f));
	//rotate here!
	mv = rotate(mv, fmod(time, 360.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 origin_matrix = mv;

	mv = scale(mv, vec3(.6f, 1.0f, .6f));
	DrawBody(projection, mv, size, 0);

	mv = origin_matrix;

	mv = translate(mv, vec3(0.0f, 0.0f, 0.0f));
	mv = rotate(mv, 22.5f, vec3(0.0f, 1.0f, 0.0f));
	for(int i = 0; i < 4; i++)
	{

		DrawWing(projection, mv, size, 0);
		if(i % 2 == 0)
		{
			mv = rotate(mv, 135.0f, vec3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			mv = rotate(mv, 45.0f, vec3(0.0f, 1.0f, 0.0f));
		}
	}

	mv = origin_matrix;
}

//take down the simple shapes that are used for the ship.
void Spaceship::TakeDown()
{
	cylinder.TakeDown();
	sphere.TakeDown();
	wing.TakeDown();
}

//steps through each mesh's shaders.
void Spaceship::StepShader()
{
	cylinder.StepShader();
	sphere.StepShader();
	wing.StepShader();
}

//sets the normal enable for each mesh.
void Spaceship::EnableNormals(bool dn)
{
	sphere.EnableNormals(dn);
	cylinder.EnableNormals(dn);
	wing.EnableNormals(dn);
}

Spaceship::~Spaceship()
{
	
}
