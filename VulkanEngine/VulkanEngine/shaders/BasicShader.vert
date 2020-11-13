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

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 vertColor;
layout(location = 2) out vec3 normal;
layout(location = 3) out vec3 cameraPosition;
layout(location = 4) out vec2 uv;
layout(location = 5) out Light lights[5];

void main(){
	//Create model view projection matrix
	mat4 mvp = ubo.projection * ubo.view * model;

	//calculate screen position of the fragment
	gl_Position = mvp * vec4(inPosition, 1.0f);
	
	//calculate world position of the fragment
	position = (model * vec4(inPosition, 1.0f)).xyz;

	//Pass variables through to fragment shader
	lights = ubo.lights;
	vertColor = inColor;
	normal = inNormal;
	uv = texCoord.xy;
	cameraPosition = ubo.cameraPosition;
}