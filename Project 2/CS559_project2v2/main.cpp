/* PROJECT 2 by Nik Ingrassia and Jackson Reed
*  For CS559 - Fall 2013 with perry Kivolowitz
*  
*  This program contains five modes: 
*  - A rotating spaceship, lit and built from a mesh
*  - A rotating model of mars, lit and build from a displacement mapped mesh
*  - A first person view flying over the surface of mars
*  - A third person view of a controllable ship flying over the surface of mars
*  - A view from outside the starfield
*/


//Enum increment taken from http://www.cplusplus.com/forum/beginner/41790/
#define _USE_MATH_DEFINES

#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#include "Starfield.h"
#include "Spaceship.h"
#include "Mars.h"

using namespace std;
using namespace glm;

static class Globals
{
public:
	Globals();
	glm::ivec2 window_size;
	float aspect_ratio;
	bool window_closed;

	float near_plane, far_plane, fov;

	bool wireframe_enabled;
	bool normals_enabled;

	bool paused;
	float current_time, time_last_pause_began, total_time_paused;
	int period;
	float rotate_factor;

	int polygon_detail;
	bool starfield_enabled;
	float starfield_depth;
	float starfield_inner_radius;
	int starfield_num_stars;

	bool ship_direction[4];
	float ship_height;
	float ship_side_direction;
	float ship_size;

	bool camera_direction[4];
	float horiz_cam_angle, vert_cam_angle, cam_radius;
	enum DisplayModes {Ship, Mars, FirstPerson, ThirdPerson, Stars};
	enum Direction {Left, Right, Up, Down};
	DisplayModes current_mode;
	vector<std::string> onscreen_text;

	int mouse_x;
	int mouse_y;

	bool mouse_left;



} globals;

// Allows us to use the '++' operator to cycle through display modes
// Function found at http://www.cplusplus.com/forum/beginner/41790/
inline Globals::DisplayModes& operator++(Globals::DisplayModes& disp_modes, int)  // <--- note -- must be a reference
{
   const int i = static_cast<int>(disp_modes);
   disp_modes = static_cast<Globals::DisplayModes>((i + 1) % 5);
   return disp_modes;
}

// Globals class to hold all the global variables
Globals::Globals() 
{
	this->window_size = vec2(1366, 768);
	this->aspect_ratio = (float)(window_size.x) / (float)(window_size.y);
	this->window_closed = true;

	this->near_plane = 0.1f;
	this->far_plane = 150.0f;
	this->fov = 50.0f;

	this->wireframe_enabled = false;
	this->normals_enabled = false;

	this->paused = false;
	this->current_time = 0;
	this->time_last_pause_began = 0;
	this->total_time_paused = 0;
	this->period = 1000 / 60;
	this->rotate_factor = 4.0f;

	this->polygon_detail = 60;
	this->starfield_enabled = true;
	this->starfield_depth = 20.0;
	this->starfield_inner_radius = 25.0;
	this->starfield_num_stars = 10000;

	this->ship_height = 3.2f;
	this->ship_side_direction = 0.0f;
	this->ship_size = 0.03f;

	this->current_mode = Ship;
	this->horiz_cam_angle = 0;
	this->vert_cam_angle = 0;
	this->cam_radius = 10;
}

Starfield starfield;
Spaceship spaceship;
Mars mars;

// Utility function for conversion from degree to radians
float toRadian(float d)
{
	return (float)(d / 180.0f * M_PI);
}
// used early on to orient in 3D space
// Clashed with modern GL
void DrawAxes()
{
	glBegin(GL_LINES);
		glLineWidth(1.0);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(10.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 10.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 10.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
}

// Uses the vector of strings to draw text onto the screen
void DisplayOnscreenText()
{
	
	mat4 orth = ortho(0.0f, (float)globals.window_size.x, 0.0f, (float)globals.window_size.y, 0.1f, 10.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(orth));

	// Look at the same place constantly, and shift the modelview matrix in front of the near plane
	mat4 text = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	
	glMatrixMode(GL_MODELVIEW);

	// Draw pause text if paused
	glColor3f(.5f, .7f, .9f);
	if(globals.paused)
	{
		text = translate(text, vec3(globals.window_size.x/2, globals.window_size.y/2, -1.0f)); // Center of screen
		text = scale(text, vec3(.40f, .40f, .40f));
		text = translate(text, vec3(-3 * glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, 'P'),  -glutStrokeHeight(GLUT_STROKE_MONO_ROMAN) / 2, 0.0f)); // Center of text
		glLoadMatrixf(value_ptr(text));
		glLineWidth(4.0f);
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *)"PAUSED");
		glLineWidth(1.0f);
		//reset modelview text matrix when done drawing centered text.
		text = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	}
	text = translate(text, vec3(0.0f, 5.0f, -1.0f));
	text = scale(text, vec3(.15f, .15f, .15f));
	
	//glColor3f(.2f, .5f, .9f);
	vector<string> * s = &globals.onscreen_text;
	for(auto i = s->begin(); i < s->end(); i++) 
	{
		glLoadMatrixf(value_ptr(text));
		glutStrokeString(GLUT_STROKE_MONO_ROMAN, (const unsigned char *)(*i).c_str());
		text = translate(text, vec3(0.0f, glutStrokeHeight(GLUT_STROKE_MONO_ROMAN), 0.0f));
	}
}

// EACH "MODE" HAS ITS OWN DRAW FUNCTION CALLED BY DRAWFUNC()

void ShipModeDraw(mat4 proj)
{
	mat4 mv(1.0f);

	vec3 eyePos = vec3(globals.cam_radius * cos(toRadian(globals.vert_cam_angle)) * cos(toRadian(globals.horiz_cam_angle)),
						(globals.cam_radius * sin(toRadian(globals.vert_cam_angle))),
						(globals.cam_radius * cos(toRadian(globals.vert_cam_angle)) * sin(toRadian(globals.horiz_cam_angle))));
	mv = lookAt(eyePos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));

	// also draw a starfield
	if(globals.starfield_enabled)
	{
		starfield.Draw(proj, mv, globals.window_size, 0);
	}

	spaceship.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);
}

void MarsModeDraw(mat4 proj)
{
	//Just using our mesh, replace the mesh with Mars eventually!

	mat4 mv(1.0f);
	//Temporary lookat - always looking at the center point?
	vec3 eyePos = vec3(globals.cam_radius * cos(toRadian(globals.vert_cam_angle)) * cos(toRadian(globals.horiz_cam_angle)),
						(globals.cam_radius * sin(toRadian(globals.vert_cam_angle))),
						(globals.cam_radius * cos(toRadian(globals.vert_cam_angle)) * sin(toRadian(globals.horiz_cam_angle))));
	mv = lookAt(eyePos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));


	// also draw a starfield
	if(globals.starfield_enabled)
	{
		starfield.Draw(proj, mv, globals.window_size, 0);
	}

	// Mars is a little too small normally - scale it up
	mv = scale(mv, vec3(1.3f, 1.3f, 1.3f));
	mars.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);

}

void FirstPersonModeDraw(mat4 proj)
{
	mat4 mv(1.0f);
	//set up our position, view, and up vectors!
	//The angle adjustments are to ensure that default values have us facing a good direction
	vec3 eyePos = vec3(0.0f, 0.0f, 3.5f);
	vec3 lookVec = vec3(cos(toRadian(globals.vert_cam_angle/2.0f - 45.0f)) * sin(toRadian(globals.horiz_cam_angle - 90.0f)),
						(cos(toRadian(globals.vert_cam_angle/2.0f - 45.0f)) * cos(toRadian(globals.horiz_cam_angle - 90.0f))),
						(sin(toRadian(globals.vert_cam_angle/2.0f - 30.0f)))) + eyePos;

	vec3 upVec = vec3(0.0f,	0.0f, 1.0f);
	mv = lookAt(eyePos, lookVec, upVec);

	// also draw a starfield
	if(globals.starfield_enabled)
	{
		starfield.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);
	}
	
	mars.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);
}

void ThirdPersonModeDraw(mat4 proj)
{

	mat4 mv(1.0f);
	//set up our position, view, and up vectors!
	//The angle adjustments are to ensure that default values have us facing a good direction
	vec3 eyePos = vec3(.1f, 0.0f, 4.0f);
	vec3 lookVec = vec3(-cos(toRadian(globals.vert_cam_angle/2.0f - 45.0f)),
						0.0f,
						sin(toRadian(globals.vert_cam_angle/2.0f - 45.0f))
						) + eyePos;

	// Everything will be tilted on its side
	// The ship is flying along the rotating mars - not moving forward itself
	vec3 upVec = vec3(0.0f,	0.0f, 1.0f);
	mv = lookAt(eyePos, lookVec, upVec);
	mat4 saved_mv = mv;
	
	mv = rotate(mv, globals.horiz_cam_angle/2.0f, vec3(0.0f, 0.0f, 1.0f));
	mv = rotate(mv, globals.horiz_cam_angle, vec3(1.0f, 0.0f, 0.0f));

	// also draw a starfield
	if(globals.starfield_enabled)
	{
		starfield.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);
	}
	
	mars.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);


	//restore unrotated modelview so we control the ship!
	mv = saved_mv;

	//Translate the ship based on previous input
	mv = translate(mv, vec3(-1.0f, globals.ship_side_direction / 720.0f, globals.ship_height));

	// the ship is too large compared to mars, scale it down
	mv = scale(mv, vec3(globals.ship_size));

	//rotate ship so it's oriented correctly in world
	mv = rotate(mv, -10.0f, vec3(0.0f, 1.0f, 0.0f));
	mv = rotate(mv, 90.0f, vec3(0.0f, 0.0f, 1.0f));

	spaceship.Draw(proj, mv, globals.window_size, globals.ship_side_direction);

}
void StarsModeDraw(mat4 proj)
{

	mat4 mv(1.0f);
	//make sure we're outside of the starfield!
	vec3 eyePos = vec3(0.0f,0.0f, 2.0f * (globals.starfield_inner_radius + globals.starfield_depth));
	mv = lookAt(eyePos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));

	//rotate based on the time!
	//divide by rotate_factor to control speed of rotation!
	mv = rotate(mv,
		(((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused)
			/globals.rotate_factor),
		vec3(0.0f, 1.0f, 0.0f)); 

	starfield.Draw(proj, mv, globals.window_size, ((globals.paused ? globals.time_last_pause_began : globals.current_time) - globals.total_time_paused) / globals.rotate_factor);

}

// Takes care of the Displaying of ALL modes
void DisplayFunc()
{
	//this is all common setup no matter what mode we're in!
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black for SPACE!
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, (globals.wireframe_enabled == true) ? GL_LINE : GL_FILL);

	glViewport(0, 0, globals.window_size.x, globals.window_size.y);

	mat4 proj = perspective(globals.fov, globals.aspect_ratio, globals.near_plane, globals.far_plane);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(proj));

	//select our mode and draw it! Using an enum
	switch(globals.current_mode)
	{
		case Globals::DisplayModes::Ship:
			ShipModeDraw(proj);
			break;
		case Globals::DisplayModes::Mars:
			MarsModeDraw(proj);
			break;
		case Globals::DisplayModes::FirstPerson:
			FirstPersonModeDraw(proj);
			break;
		case Globals::DisplayModes::ThirdPerson:
			ThirdPersonModeDraw(proj);
			break;
		case Globals::DisplayModes::Stars:
			StarsModeDraw(proj);
			break;
		default:
			ShipModeDraw(proj);
			break;
	}

	DisplayOnscreenText();
	glutSwapBuffers();
	
}

void ReshapeFunc(int w, int h)
{
	if(!globals.window_closed && h > 0) {
		// Just modify the aspect, which effects the viewport
		globals.window_size = vec2(w, h);
		globals.aspect_ratio = (float)(w) / (float)(h);
		glutPostRedisplay();
	}
}

void CloseFunc()
{
	// MAKE SURE TO TAKE DOWN EVERY OBJECT CREATED
	globals.window_closed = true;
	starfield.TakeDown();
	spaceship.TakeDown();
	mars.TakeDown();
	glutLeaveMainLoop();
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	//ignore keyboard function except "unpause" and "quit" when paused
	if((c != 27) && (c != 'x') && (c != 'p') && globals.paused)
	{
		return;
	}
	switch(c) {
		case 'x':
		case 27: // Esc
			// Close program
			CloseFunc();
			break;
		case 'n':
			// toggle normals
			globals.normals_enabled = !globals.normals_enabled;
			spaceship.EnableNormals(globals.normals_enabled);
			mars.EnableNormals(globals.normals_enabled);
			break;
		case 'w':
			// toggle wireframe
			globals.wireframe_enabled = !globals.wireframe_enabled;
			break;
		case 's':
			// Toggle starfield
			globals.starfield_enabled = !globals.starfield_enabled;
			break;
		case 'p':
			// Pause
			globals.paused = !globals.paused;
			//if we've just paused, set the time we just paused at!
			if(globals.paused)
			{
				globals.time_last_pause_began = globals.current_time;
			}
			//if we've just unpaused, add to the total time paused.
			else
			{
				globals.total_time_paused += (globals.current_time - globals.time_last_pause_began);
			}
			break;
	}
	return;
}

void SpecialFunc(int key, int x, int y)
{
	//ignore special keys when paused.
	if(globals.paused)
	{
		return;
	}

	switch(key)	{

		// For LEF, RIGHT, UP, and DOWN, functionality changes when in Third Person Mode
		// Motion based on booleans - when true, motion occurs - when false, motion stops
		case GLUT_KEY_LEFT:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Left] = true;
			}
			else
			{
				globals.camera_direction[globals.Left] = true;
				globals.ship_direction[globals.Left] = true;
			}
			break;
		case GLUT_KEY_RIGHT:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Right] = true;
			}
			else
			{
				globals.camera_direction[globals.Right] = true;
				globals.ship_direction[globals.Right] = true;
			}
			break;
		case GLUT_KEY_UP:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Up] = true;
			}
			else
			{
				globals.ship_direction[globals.Up] = true;
			}
			break;
		case GLUT_KEY_DOWN:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Down] = true;
			}
			else
			{
				globals.ship_direction[globals.Down] = true;
			}
			break;

		case GLUT_KEY_F1:
			// Cycle to next mode
			globals.current_mode++;
			//reset camera angle, ship rotation, and fov on mode switch
			globals.horiz_cam_angle = globals.vert_cam_angle = 0.0f;
			globals.ship_side_direction = 0.0f;
			globals.fov = 50.0f;
			//update the display text!
			globals.onscreen_text.pop_back();
			switch(globals.current_mode)
			{
				case Globals::DisplayModes::Ship:
					globals.onscreen_text.push_back("Ship Mode");
					return;
				case Globals::DisplayModes::Mars:
					globals.onscreen_text.push_back("Mars Mode");
					break;
				case Globals::DisplayModes::FirstPerson:
					globals.onscreen_text.push_back("First Person Mode");
					break;
				case Globals::DisplayModes::ThirdPerson:
					globals.onscreen_text.push_back("Third Person Mode");
					break;
				case Globals::DisplayModes::Stars:
					globals.onscreen_text.push_back("Starfield Mode");
					break;
			}
			break;
		case GLUT_KEY_F2:
			// Cycle to next shader
			mars.StepShader();
			spaceship.StepShader();
			break;
	}
}

// When a key is realeased, it's boolean is set to false, stopping motion
void SpecialUp(int key, int x, int y)
{
	switch(key)
	{
		// Behaves differently if in third person mode
		case GLUT_KEY_UP:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Up] = false;
			}
			else
			{
				globals.ship_direction[globals.Up] = false;
			}
			break;
		case GLUT_KEY_DOWN:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Down] = false;
			}
			else
			{
				globals.ship_direction[globals.Down] = false;
			}
			break;
		case GLUT_KEY_LEFT:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Left] = false;
			}
			else
			{
				globals.camera_direction[globals.Left] = false;
				globals.ship_direction[globals.Left] = false;
			}
			break;
		case GLUT_KEY_RIGHT:
			if(globals.current_mode != globals.ThirdPerson)
			{
				globals.camera_direction[globals.Right] = false;
			}
			else
			{
				globals.camera_direction[globals.Right] = false;
				globals.ship_direction[globals.Right] = false;
			}
			break;
	}
}


// Takes care of Mouse control when button down
void Motion(int x, int y)
{
	//track position like the passive function if left button isn't pressed!
	if(!globals.mouse_left)
	{
		globals.mouse_x = x;
		globals.mouse_y = y;
		return;
	}
	// only do motion when not paused or in Third person mode!
	if(!globals.paused && globals.current_mode != globals.ThirdPerson)
	{
		// invert y movement if in First person mode
		int invert_vert = 1;
		if(globals.current_mode == globals.FirstPerson)
			invert_vert *= -1;

		//horizontal mouse motion
		float x_delta = float(x - globals.mouse_x) / 4.0f;
		globals.horiz_cam_angle = fmod((globals.horiz_cam_angle + x_delta), 360.0f);

		//vertical mouse motion
		float y_delta = float(y - globals.mouse_y) / 4.0f;
		globals.vert_cam_angle += (invert_vert * y_delta);
		//restrict vertical motion to +- 89.0f degrees.
		if(globals.vert_cam_angle > 89.0f)
			globals.vert_cam_angle = 89.0f;
		if(globals.vert_cam_angle < -89.0f)
			globals.vert_cam_angle = -89.0f;
	}

	//store current mouse position to calculate delta!
	globals.mouse_x = x;
	globals.mouse_y = y;
}

// takes care of mouse motion when button is not pressed
void PassiveMotion(int x, int y)
{
	//keep track of mouse position when no button is pressed
	globals.mouse_x = x;
	globals.mouse_y = y;
}

void MouseWheel(int wheel, int direction, int x, int y)
{
	//don't do anything if paused or in third person mode!
	if(globals.paused || globals.current_mode == globals.ThirdPerson)
		return;

	// Modifies FOV
	switch (direction)
	{
		//wheel down
	case -1:
		if(globals.fov < 80.0f)
			globals.fov += 2.0f;
		break;
		//wheel up
	case 1:
		if(globals.fov > 10.0f)
			globals.fov -= 2.0f;
		break;
	}
}

// handles mouse clicks
void Mouse(int button, int state, int x, int y)
{
	//reset mouse position on button up/down.
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		globals.mouse_left = true;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		globals.mouse_left = false;
	}
}

// TimerFunc updates every frame - takes care of REDRAWING and CONTROL
void TimerFunc(int value)
{
	// Control implementation for the CAMERA
	// Right and left movement work differently in third person mode
	if(globals.camera_direction[globals.Left])
	{
		if(globals.current_mode != globals.ThirdPerson)
		{
			globals.horiz_cam_angle = fmod((globals.horiz_cam_angle - 0.5f), 360.0f);
		}
		else
		{
			globals.horiz_cam_angle = fmod((globals.horiz_cam_angle - 0.2f), 360.0f);
		}
	}
	if(globals.camera_direction[globals.Right])
	{
		if(globals.current_mode != globals.ThirdPerson)
		{
			globals.horiz_cam_angle = fmod((globals.horiz_cam_angle + 0.5f), 360.0f);
		}
		else
		{
			globals.horiz_cam_angle = fmod((globals.horiz_cam_angle + 0.2f), 360.0f);
		}
	}
	if(globals.camera_direction[globals.Up])
	{
		if(globals.vert_cam_angle < 89.0f)
			globals.vert_cam_angle += 0.5f;
	}
	if(globals.camera_direction[globals.Down])
	{
		if(globals.vert_cam_angle > -89.0f)
			globals.vert_cam_angle -= 0.5f;
	}

	//Control Implementation for the SHIP

	if(globals.ship_direction[globals.Left] && globals.ship_side_direction > -50.0f)
	{
		globals.ship_side_direction -= float(M_PI)/4.0f;
	}
	if(globals.ship_direction[globals.Right] && globals.ship_side_direction < 50.0f)
	{
		globals.ship_side_direction += float(M_PI)/4.0f;
	}
	// if not moving RIGHT or LEFT, slowly move its direction  back to the center so it drifts back
	if(!globals.ship_direction[globals.Left] && !globals.ship_direction[globals.Right])
	{
		if(globals.ship_side_direction < 1 && globals.ship_side_direction > -1)
		{
			// Stay still
		}
		else if(globals.ship_side_direction > 1)
		{
			globals.ship_side_direction -= float(M_PI)/2.0f;
		}
		else if(globals.ship_side_direction < -1)
		{
			globals.ship_side_direction += float(M_PI)/2.0f;
		}
	}
	if(globals.ship_direction[globals.Up])
	{
		if(globals.ship_height > 3.05)
			globals.ship_height -= 0.005f;
	}
	if(globals.ship_direction[globals.Down])
	{
		if(globals.ship_height < 3.5f)
			globals.ship_height += 0.005f;
	}

	//update our current time.
	globals.current_time++;
	glutTimerFunc(globals.period, TimerFunc, 0);
	//make sure our window is open when we draw again!
	if(!globals.window_closed)
	{
		glutPostRedisplay();
	}
}

int main(int argc, char * argv[])
{
	//make sure we have a filename in argv! argc must equal 2
	if(argc != 2)
	{
		cout << "specify a mars file por favor!" << endl;
		return -1;
	}
	

	//initialize and set options
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutSetOption(GLUT_RENDERING_CONTEXT, GLUT_USE_CURRENT_CONTEXT);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
	//create our window and set up callbacks
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(globals.window_size.x, globals.window_size.y);
	glutCreateWindow("MARS");
	globals.window_closed = false;
	glutDisplayFunc(DisplayFunc);
	glutReshapeFunc(ReshapeFunc);
	glutCloseFunc(CloseFunc);
	glutKeyboardFunc(KeyboardFunc);
	glutSpecialFunc(SpecialFunc);
	glutSpecialUpFunc(SpecialUp);
	glutMotionFunc(Motion);
	glutMouseFunc(Mouse);
	glutPassiveMotionFunc(PassiveMotion);
	glutMouseWheelFunc(MouseWheel);

	// Add onscreen text to string vector - we always start in ship mode!
	globals.onscreen_text.push_back("Nik Ingrassia and Jackson Reed for CS559");
	globals.onscreen_text.push_back("Esc to close");
	globals.onscreen_text.push_back("w to toggle wireframe - n to toggle normals");
	globals.onscreen_text.push_back("s to toggle starfield");
	globals.onscreen_text.push_back("F1 to switch modes - F2 to toggle shader");
	globals.onscreen_text.push_back("Ship Mode");

	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		return -1;
	}

	cout << "Initializing Objects...\n" << endl;
	
	// Initialize the mars model
	cout << "Initizalizing Mars..." << endl;
	if(!mars.Initialize(argv[1], 3.0f, 0.15f, vec3(0.6f, 0.1f, 0.1f)))
	{
		return -1;
	}
	cout << "Mars initialized\n" << endl;

	// Initialize the spaceship model
	cout << "Initializing Ship..." << endl;
	if(!spaceship.Initialize(globals.polygon_detail, globals.polygon_detail, vec3(1.0f, 0.0f, 0.0f)))
	{
		return -1;
	}
	cout << "Ship initialized\n" << endl;

	// initialize a starfield - lots of stars!
	cout << "Initialing Starfield..." << endl;
	if(!starfield.Initialize(globals.starfield_inner_radius,globals.starfield_depth,globals.starfield_num_stars))
	{
		return -1;
	}
	cout << "Starfield initialized" << "\n\n" << "Starting program..." << "\n\n" << "Welcome to the Final Frontier." << endl;

	glutTimerFunc(globals.period, TimerFunc, 0);
	glutMainLoop();
	return 0;
}
