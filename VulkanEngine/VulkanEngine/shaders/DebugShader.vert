#version 450
#extension GL_ARB_separate_shader_objects: enable

struct Light{
	vec3 position;
	vec3 color;
	float range;
	float intensity;
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
layout(location = 8) in vec3 inWireColor;

layout(location = 0) out vec3 color;

void main(){
	//Create model view projection matrix
	mat4 mvp = ubo.projection * ubo.view * model;

	//calculate screen position of the fragment
	gl_Position = mvp * vec4(inPosition, 1.0f);
	
	//Pass variables through to fragment shader
	color = inWireColor;
}