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
	int lightCount = lights.length();
	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < lightCount; i++){
		vec3 direction = position - lights[i].position;
		vec3 cameraDirection = normalize(cameraPosition - position);
		float strength = length(direction) / lights[i].range;
		strength = 1.0f - clamp(strength, 0.0f, 1.0f);
		direction = normalize(direction);

		//Calculate diffuse color
		vec3 diffuse = lights[i].color * clamp(dot(normal, -1 * direction), 0.0f, 1.0f);

		//Calculate specular color
		float specular = pow(clamp(dot(reflect(direction, normal), cameraDirection), 0.0f, 1.0f), 128) * 0.5f;

		//Calculate final color
		finalColor +=  (diffuse + specular) * strength * lights[i].intensity;
	}

	//Add ambient light
	finalColor += vec3(0.015f, 0.015f, 0.015f);
	vec4 textureColor = texture(texSampler, uv);

	outColor = vec4(finalColor * vertColor, 1.0f) * textureColor;
}