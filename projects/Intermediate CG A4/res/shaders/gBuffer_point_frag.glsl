#version 420

layout(location = 0) in vec2 inUV;


struct PointLight
{
	//***SAME TYPES SHOULD BE GROUPED TOGETHER***\\
	//VEC4s 
	//SHOULD ALWAYS USE VEC4s (Vec3s get upscaled to Vec4s anyways, using anything less is a waste of memory)
	vec4 _lightPos;
	vec4 _lightCol;
	vec4 _ambientCol;
	//FLOATS
	float _lightLinearFalloff;
	float _lightQuadraticFalloff;
	float _ambientPow;
	float _lightAmbientPow;
	float _lightSpecularPow; 
};

layout (std140, binding = 0) uniform u_Lights
{
	PointLight _sun;
};


layout (binding = 0) uniform sampler2D s_albedoTex;
layout (binding = 1) uniform sampler2D s_normalsTex;
layout (binding = 2) uniform sampler2D s_specularTex;
layout (binding = 3) uniform sampler2D s_positionTex;

//layout(binding = 4) uniform sampler2D s_lightAccumTex;

uniform vec3 u_camPos;
out vec4 frag_color;


// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting
void main() {
	//Albedo
	vec4 textureColor = texture(s_albedoTex,inUV);
	//Normals
	vec3 inNormal = (normalize(texture(s_normalsTex, inUV).rgb)*2.0) - 1.0;
	//Specular
	float texSpec = texture(s_specularTex,inUV).r;
	//Positions
	vec3 fragPos = texture(s_positionTex,inUV).rgb;

;

	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(_sun._lightPos.rgb - fragPos);
	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * _sun._lightCol.xyz;// add diffuse intensity
		
	//Attenuation
	float dist = length(_sun._lightPos.rgb - fragPos);

	float attenuation = 1.0f / (
		 
		_sun._lightLinearFalloff * dist +
		_sun._lightQuadraticFalloff * dist * dist);

	// Specular
	vec3 viewDir  = normalize(u_camPos - fragPos);
	vec3 h        = normalize(lightDir + viewDir);

	float spec = pow(max(dot(N, h), 0.0), 4.0); // Shininess coefficient (can be a uniform)
	vec3 specular = _sun._lightSpecularPow * texSpec * spec * _sun._lightCol.xyz; // Can also use a specular color

	
	vec3 result = (
		(_sun._ambientPow * _sun._ambientCol.rgb) + // global ambient light
		(_sun._ambientCol.rgb+diffuse+specular)*attenuation)*textureColor.rgb;
		

	if(textureColor.a < 0.10){
		result = vec3(1.0,1.0,1.0);
	}

	frag_color = vec4(result, 1.0);
}