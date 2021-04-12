#include "3amBuffer.h"

void SamBuffer::Init(unsigned width, unsigned height)
{
	int index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);
	
	index = int(_buffers.size());
	_buffers.push_back(new Framebuffer());
	_buffers[index]->AddColorTarget(GL_RGBA8);
	_buffers[index]->AddDepthTarget();
	_buffers[index]->Init(width, height);

	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl",GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/gBuffer_point_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	index = int(_shaders.size());
	_shaders.push_back(Shader::Create());
	_shaders[index]->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	_shaders[index]->LoadShaderPartFromFile("shaders/gBuffer_ambient_frag.glsl", GL_FRAGMENT_SHADER);
	_shaders[index]->Link();

	_sunBuffer.AllocateMemory(sizeof(PointLight));

	if (_sunEnabled) {
		_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(PointLight));
	}

	PostEffect::Init(width, height);
}

void SamBuffer::ApplyEffect(GBuffer* gBuffer)
{

	_sunBuffer.SendData(reinterpret_cast<void*>(&_sun), sizeof(PointLight));

	if (_sunEnabled) {
		_shaders[Lights::DIRECTIONAL]->Bind();
		//_shaders[Lights::POINT]->SetUniformMatrix("u_LightSpaceMatrix", _lightSpaceViewProj);
		_shaders[Lights::DIRECTIONAL]->SetUniform("u_CamPos", _camPos);


		_sunBuffer.Bind(0);

		gBuffer->BindLighting();

		_buffers[1]->RenderToFSQ();

		gBuffer->UnbindLighting();

		_sunBuffer.Unbind(0);

		_shaders[Lights::DIRECTIONAL]->UnBind();
	}

	_shaders[Lights::AMBIENT]->Bind();


	_sunBuffer.Bind(0);

	gBuffer->BindLighting();
	_buffers[1]->BindColorAsTexture(0, 4);
	_buffers[0]->BindColorAsTexture(0, 5);

	_buffers[0]->RenderToFSQ();

	_buffers[0]->UnbindTexture(5);
	_buffers[1]->UnbindTexture(4);
	gBuffer->UnbindLighting();



	_sunBuffer.Unbind(0);

	_shaders[Lights::AMBIENT]->UnBind();
}

void SamBuffer::DrawIllumBuffer()
{
	_shaders[_shaders.size() - 1]->Bind();

	_buffers[1]->BindColorAsTexture(0, 0);

	Framebuffer::DrawFullscreenQuad();

	_buffers[1]->UnbindTexture(0);

	_shaders[_shaders.size() - 1]->UnBind();
}

//void samBuffer::SetLightSpaceViewProj(glm::mat4 lightSpaceViewProj)
//{
//	_lightSpaceViewProj = lightSpaceViewProj;
//}

void SamBuffer::SetCamPos(glm::vec3 camPos)
{
	_camPos = camPos;
}

PointLight& SamBuffer::GetSunRef()
{
	return _sun;
}

void SamBuffer::SetSun(PointLight newSun)
{
	_sun = newSun;
}

void SamBuffer::SetSun(glm::vec4 lightDir)
{
	_sun._lightPos = lightDir;
	
}
void SamBuffer::SetSun2(glm::vec4 lightCol)
{
	
	_sun._lightCol = lightCol;
}
void SamBuffer::EnableSun(bool enabled)
{
	_sunEnabled = enabled;
}
