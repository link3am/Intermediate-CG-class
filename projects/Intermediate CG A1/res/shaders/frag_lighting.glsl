#version 410

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

uniform sampler2D s_tex;
uniform sampler2D s_Specular;

uniform vec3  u_AmbientCol;
uniform float u_AmbientStrength;

uniform vec3  u_LightPos;
uniform vec3  u_LightCol;
uniform float u_AmbientLightStrength;
uniform float u_SpecularLightStrength;
uniform float u_Shininess;

uniform vec3  u_CamPos;


uniform	int u_ambB;
uniform	int u_difB;
uniform	int u_specB;
uniform	int u_lightB;
out vec4 frag_color;


void main() {

	// Lecture 5
	vec3 ambient = u_AmbientLightStrength * u_LightCol;
	if( u_ambB > 5 )
	{ambient =vec3(1.0);
	}
	// Diffuse
	vec3 N = normalize(inNormal);
	vec3 lightDir = normalize(u_LightPos - inPos);

	float dif = max(dot(N, lightDir), 0.0);
	vec3 diffuse = dif * u_LightCol;
	if(u_difB>5){
	diffuse = vec3(0.0f);
	}
	
	// Specular
	vec3 viewDir  = normalize(u_CamPos - inPos);
	vec3 h        = normalize(lightDir + viewDir);

	float spec = pow(max(dot(N, h), 0.0), u_Shininess); // Shininess coefficient (can be a uniform)

	vec3 specular = u_SpecularLightStrength * spec * u_LightCol; 
	if( u_specB > 5 )
	{specular=vec3(0.0,0.0,0.0);}
	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(s_tex, inUV);

	vec3 result = (
		(u_AmbientCol * u_AmbientStrength) + // global ambient light
		ambient + diffuse + specular // light factors from our single light
		) * inColor * textureColor.rgb; // Object color

	frag_color = vec4(result, textureColor.a);
	if(u_lightB>5)
	{
	frag_color = vec4(textureColor.rgb, textureColor.a);
	}

}