#version 450
#extension GL_ARB_separate_shader_objects: enable

struct Light{
	vec3 position;
	vec3 color;
	float range;
	float intensity;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertColor;
layout(binding = 1) uniform sampler2D texSampler;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 cameraPosition;
layout(location = 4) in vec2 uv;
layout(location = 5) in Light lights[5];

layout(location = 0) out vec4 outColor;

void main(){
	outColor = vec4(vertColor, 1.0f);
}