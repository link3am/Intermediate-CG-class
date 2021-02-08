#include "ColorCorrectionEffect.h"

void ColorCorrectionEffect::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);

	//Setup shaders
	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/color_correction_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();
}

void ColorCorrectionEffect::ApplyEffect(PostEffect* buffer)
{
	BindShader(0);
	buffer->BindColorAsTexture(0, 0, 0);
	_buffers[0]->RenderToFSQ();
	buffer->UnbindTexture(0);
	UnbindShader();
}

float ColorCorrectionEffect::GetIntensity() const
{
	return _intensity;
}

void ColorCorrectionEffect::SetIntensity(float intensity)
{
	_intensity = intensity;
}
