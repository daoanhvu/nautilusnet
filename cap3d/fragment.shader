#version 330 core
// https://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Multiple_Lights
// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 Color_vertex;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection1_cameraspace;
in vec3 LightDirection2_cameraspace;
in vec2 oUseNormal;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
// uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 lightPos1_worldspace;
uniform vec3 lightColor1;
uniform vec3 lightPos2_worldspace;
uniform vec3 lightColor2;

struct LightSource {
  	vec3 position_world;
	vec3 direction_camera;
	vec3 lightColor;
  	vec3 diffuseColor;
  	vec3 ambientColor;
	vec3 specularColor;
};
const int numLights = 2;
LightSource lights[numLights];

void main(){
	// Light emission properties
	// You probably want to put them as uniforms
	float LightPower = 20.0f;
	float specularPower = 5.0f;

	//Need lighting????
	if(oUseNormal[1] > 0.5f) {
		// Material properties
		// vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
		vec3 MaterialDiffuseColor = Color_vertex;
		// vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;

		// light 1
		lights[0].position_world = lightPos1_worldspace;
		lights[0].direction_camera = LightDirection1_cameraspace;
		lights[0].specularColor = vec3(0.3,0.3,0.3);
	  	lights[0].ambientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
		lights[0].lightColor = lightColor1;

		// light 2
		lights[1].position_world = lightPos2_worldspace;
		lights[1].direction_camera = LightDirection2_cameraspace;
		lights[1].specularColor = vec3(0.5,0.5,0.5);
	  	lights[1].ambientColor = vec3(0.5,0.3,0.1) * MaterialDiffuseColor;
		lights[1].lightColor = lightColor2;

		vec3 totalLighting = vec3(0.0f, 0.0f, 0.0f);
		if(oUseNormal[0] >= 0.5) {
			// Normal of the computed fragment, in camera space
			vec3 n = normalize( Normal_cameraspace );
			// Eye vector (towards the camera)
			vec3 E = normalize(EyeDirection_cameraspace);
			// vec3 totalLighting = MaterialAmbientColor * 5.0f;
			for(int i=0; i<numLights; i++) {
				float distance = length( lights[i].position_world - Position_worldspace );
				float distance2 = distance * distance;
				vec3 l = normalize( lights[i].direction_camera );
				float cosTheta = clamp(dot(n, l), 0, 1);
				vec3 R = reflect(-l,n);
				float cosAlpha = clamp( dot( E,R ), 0,1 );
				vec3 diffuseReflection = MaterialDiffuseColor * lights[i].lightColor * LightPower * cosTheta / distance2;
				vec3 specularReflection = lights[i].specularColor * lights[i].lightColor * specularPower * pow(cosAlpha,5) / distance2;
				totalLighting = totalLighting + lights[i].ambientColor * 5.0f + diffuseReflection + specularReflection;
			}
		} else {
			for(int i=0; i<numLights; i++) {
				totalLighting = totalLighting + lights[i].ambientColor * 5.0f;
			}
		}

		color = totalLighting;
	} else {
		color = Color_vertex;
	}
}
