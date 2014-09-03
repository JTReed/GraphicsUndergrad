//Taken from Perry Kivolowitz's Modern GL Example
//use the phong_shader.vert vertex shader.

#version 400

layout (location = 0) out vec4 FragColor;

flat in vec3 color;
in vec3 position;
in vec3 normal;

const float shininess = 100.0f;
const vec3 light_position = vec3(10.0, 15.0, 0.0);
const int levels = 5;
const float scaleFactor = 1.0/levels;

vec3 toonShade( )
{
  vec3 n = normal;

  if (!gl_FrontFacing)
	n = -n;

  vec3 s = normalize(light_position - position);
  float cosine = max(0.0, dot(s, n));
  vec3 diffuse = vec3(0.5, 0.5, 0.5) * floor(cosine * levels) * scaleFactor;

  //multiply vertex color by sum of diffuse(cel shading) and ambient light
  return color * (vec3(0.3, 0.3, 0.3) + diffuse);
	
}

void main()
{
	FragColor = vec4(toonShade(), 1.0);
}
