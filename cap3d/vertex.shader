#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec3 vertexColor;

// Output data ; will be interpolated for each fragment.
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 Color_vertex;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection1_cameraspace;
out vec3 LightDirection2_cameraspace;
out vec2 oUseNormal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos1_worldspace;
uniform vec3 lightColor1;
uniform vec3 lightPos2_worldspace;
uniform vec3 lightColor2;

uniform int useNormal;
uniform int useLighting;
uniform float pointSize;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// Position of the vertex, in worldspace : M * position
	Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;

	// Vector that goes from the vertex to the camera, in camera space.
	// In camera space, the camera is at the origin (0,0,0).
	vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	// Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
	vec3 LightPosition1_cameraspace = ( V * vec4(lightPos1_worldspace,1)).xyz;
	LightDirection1_cameraspace = LightPosition1_cameraspace + EyeDirection_cameraspace;
	vec3 LightPosition2_cameraspace = ( V * vec4(lightPos2_worldspace,1)).xyz;
	LightDirection2_cameraspace = LightPosition2_cameraspace + EyeDirection_cameraspace;

	oUseNormal[0] = 0;
	if(useNormal >= 1) {
		// Normal of the the vertex, in camera space
		Normal_cameraspace = ( V * M * vec4(vertexNormal_modelspace,0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.
		oUseNormal[0] = 1;
	}

	oUseNormal[1] = 0;
	if(useLighting >= 1) {
		oUseNormal[1] = 1;
	}

	//Set point size
	gl_PointSize = pointSize;

	Color_vertex = vertexColor;
}
