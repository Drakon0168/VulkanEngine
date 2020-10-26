#version 450
#extension GL_ARB_separate_shader_objects: enable

struct Light{
	vec3 position;
	vec3 color;
	float range;
};

 
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertColor;
layout(binding = 1) uniform samplerCube texSampler;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 cameraPosition;
layout(location = 4) in vec3 uv;
layout(location = 5) in Light lights[5];

layout(location = 0) out vec4 outColor;

void main(){
	/*int lightCount = lights.length();
	vec3 finalColor = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < lightCount; i++){
		vec3 direction = position - lights[i].position;
		vec3 cameraDirection = normalize(cameraPosition - position);
		vec3 lightDistance = vec3(abs(direction.x), abs(direction.y), abs(direction.z));
		float strength = length(lightDistance) / lights[i].range;
		strength = 1.0f - clamp(strength, 0.0f, 1.0f);
		direction = normalize(direction);

		//Calculate diffuse color
		vec3 diffuse = lights[i].color * clamp(dot(normal, -1 * direction), 0.0f, 1.0f);

		//Calculate specular color
		float specular = pow(clamp(dot(reflect(direction, normal), cameraDirection), 0.0f, 1.0f), 128) * 0.5f;

		//Calculate final color
		finalColor +=  (diffuse + specular) * strength;
	} */

//	finalColor += vec3(0.015f, 0.015f, 0.015f);
	
	outColor = vec4(vertColor, 1.0f);
	vec3 temp =  (uv * 2) - 1;
	// vec3 temp = uv;
	// temp.b = 0;
	// outColor = vec4(finalColor * vertColor, 1.0f);
    outColor = texture(texSampler, temp);
	// outColor = vec4(temp, 1.0);
	// outColor = vec4(position, 1.0);
}