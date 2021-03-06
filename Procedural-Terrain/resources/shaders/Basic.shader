#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 colour;
//Make sure the offset is the last layout location and the location matches InstanceBuffer
layout(location = 3) in vec4 offset;
//layout(location = 4) in bool isVisable; //This causes an error in the shader? Need to figure out how to not send vertice data you don't want to render

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_offset;

out vec3 v_Colour;
out vec3 v_Normal;
out vec3 v_FragPos;

void main()
{
	gl_Position = u_Projection * u_View * u_Model * (vec4(position.xyz + u_offset, 1.0f));
	v_Colour = colour;
	v_Normal = normal;
	v_FragPos = vec3(u_Model * vec4(position.xyz + u_offset, 1.0f));
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 colour;

in vec3 v_Colour;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_Colour;
uniform vec3 u_lightColour;
uniform vec3 u_lightPos;

void main()
{
	vec3 ambient = 0.1f * u_lightColour;
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(u_lightPos - v_FragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * u_lightColour;
	vec3 result = (ambient + diffuse) * v_Colour;
	colour = vec4(result, 1.0f);
};