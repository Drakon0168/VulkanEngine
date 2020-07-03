#version 450
#extension GL_ARB_separate_shader_objects: enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertColor;
layout(location = 2) in vec2 uv;
layout(location = 3) in Light{
	vec3 position;
	vec3 color;
	float range;
} lights[5];

layout(location = 0) out vec4 outColor;

void main(){
	int lightCount = lights.length();
	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < lightCount; i++){
		vec3 lightDistance = vec3(abs(position.x - lights[i].position.x), abs(position.y - lights[i].position.y), abs(position.z - lights[i].position.z));
		float strength = length(lightDistance) / lights[i].range;
		strength = 1.0f - clamp(strength, 0.0f, 1.0f);

		finalColor += lights[i].color * strength;
	}

	finalColor += vec3(0.015f, 0.015f, 0.015f);

	outColor = vec4(finalColor * vertColor, 1.0f);
}