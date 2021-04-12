#pragma once

#include "Post/PostEffect.h"
#include "UniformBuffer.h"
#include "GBuffer.h"
#include "PointLight.h"
//#include "DirectionalLight.h"

enum Lights
{
	DIRECTIONAL,
	AMBIENT
};

//This is a post effect to make our job easier
class SamBuffer : public PostEffect
{
public:
	//Initializes framebuffer
	//Overrides post effect Init
	void Init(unsigned width, unsigned height) override;
	
	//Makes it so apply effect with a PostEffect does nothing for this object
	void ApplyEffect(PostEffect* buffer) override { };
	//Can only apply effect using GBuffer object
	void ApplyEffect(GBuffer* gBuffer);

	void DrawIllumBuffer();

	//void SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj);
	void SetCamPos(glm::vec3 camPos);

	PointLight& GetSunRef();
	

	//Sets the sun in the scene
	void SetSun(PointLight newSun);
	void SetSun(glm::vec4 lightDir);
	void SetSun2(glm::vec4 lightCol);
	void EnableSun(bool enabled);

private:
	//glm::mat4 _lightSpaceViewProj;
	glm::vec3 _camPos;
	glm::vec4 _sunPosition;
	
	UniformBuffer _sunBuffer;


	bool _sunEnabled = true;


	PointLight _sun;
};