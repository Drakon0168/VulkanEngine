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
	float totalTime;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 texCoord;

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
	float scaledTime = ubo.totalTime / 2.5f;
	lights[0] = Light(vec3(0.0f, 1.1f, 0.0f) + vec3(cos(scaledTime), 0.0f, sin(scaledTime)) * 1.5f, vec3(1.0f, 1.0f, 1.0f), 3.0f);
	lights[1] = Light(vec3(0.0f, 2.0f, -1.5f), vec3(1.0f, 0.988f, 0.769f), 3.0f);
	vertColor = inColor;
	normal = inNormal;
	uv = texCoord;
	cameraPosition = ubo.cameraPosition;
}