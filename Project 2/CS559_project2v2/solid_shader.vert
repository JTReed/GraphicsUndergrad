//Taken from Perry Kivolowitz's Modern GL Example

#version 400

layout (location = 0) in vec3 vertex_position;

uniform mat4 mvp;

void main()
{
	// "random number generator" for GLSL from: http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
	gl_Position = mvp * vec4(vertex_position, 1.0);
	gl_PointSize = 2.0 * fract(sin(dot(vertex_position.xy, vec2(12.9898, 78.233)))* 43758.5453);
}
