#version 450
#extension GL_ARB_separate_shader_objects: enable

struct Light{
	vec3 position;
	vec3 color;
	float range;
};

layout(binding = 0) uniform UniformBufferObject{
	mat4 view;
	mat4 projection;
	vec3 cameraPosition;
	Light[5] lights;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 texCoord;

//Instanced Data
layout(location = 4) in mat4 model;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 vertColor;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 cameraPosition;
layout(location = 4) out vec3 uv;
layout(location = 5) out Light lights[5];

void main(){
	
	mat4 viewNoTranslation = ubo.view;
	// scale?
	// viewNoTranslation[0][0] = 2;
	// viewNoTranslation[1][1] = 2;
	// viewNoTranslation[2][2] = 2;
	 
	 // Removing translation
	 viewNoTranslation[3][0] = 0;
	 viewNoTranslation[3][1] = 0;
	 viewNoTranslation[3][2] = 0;
	//Create model view projection matrix
	mat4 vp = ubo.projection * viewNoTranslation;

	//calculate screen position of the fragment
	gl_Position = vp * vec4(inPosition, 1.0f);
	// Always draws at max depth
	gl_Position.z = gl_Position.w;
	
	//calculate world position of the fragment
	position = (vp * vec4(inPosition, 1.0f)).xyz;

	//Pass variables through to fragment shader
	lights = ubo.lights;
	vertColor = inColor;
	normal = inNormal;
	uv = texCoord;
	cameraPosition = ubo.cameraPosition;
}