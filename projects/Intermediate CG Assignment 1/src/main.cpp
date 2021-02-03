//I am Jin Chan

#include <Logging.h>
#include <iostream>
//
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//
#include <filesystem>
#include <json.hpp>
#include <fstream>
//
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
//vector
#include <vector>
#include "Graphics/IBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Gameplay/Transform.h"
#include "Graphics/MeshBuilder.h"
#include "Graphics/ObjLoader.h"
#include "Graphics/VertexTypes.h"
//textrue
#include "Graphics/Texture2D.h"
#include "Graphics/Texture2DData.h"
//Camera
#include <Gameplay/Camera.h>
//imgui
#include "imgui.h"
#include "imgui_impl_opengl3.cpp"
#include "imgui_impl_glfw.cpp"

using namespace std;

void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
	case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
	case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
	case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
	case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
#ifdef LOG_GL_NOTIFICATIONS
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
#endif
	default: break;
	}
}

GLFWwindow* window;
Camera::sptr camera = nullptr;
void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	//camera->ResizeWindow(width, height);
}
bool initGLFW() {
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return false;
	}

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	//Create a new GLFW window
	window = glfwCreateWindow(800, 800, "TT", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);

	return true;
}
bool initGLAD()
{
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return false;
	}
	return true;
}

void RenderVAO(const Shader::sptr& shader, const VertexArrayObject::sptr& vao, const Camera::sptr& camera, const Transform::sptr& transform, const Texture2D::sptr& Tex)
{
	shader->Bind();
	shader->SetUniform("s_tex", 2);
	Tex->Bind(2);
	shader->SetUniformMatrix("u_ModelViewProjection", camera->GetViewProjection() * transform->LocalTransform());
	shader->SetUniformMatrix("u_Model", transform->LocalTransform());
	vao->Render();
}

void cameraMove (Camera::sptr camera)
{
	glm::vec3 position = camera->GetPosition();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += glm::vec3(0.1, 0.0, 0.0);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position += glm::vec3(-0.1, 0.0, 0.0);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += glm::vec3(0.0, 0.1, 0.0);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position += glm::vec3(0.0, -0.1, 0.0);
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		position += glm::vec3(0.0, 0.0, 0.1);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		position += glm::vec3(0.0, 0.0, -0.1);
	}


	camera->SetPosition(position);
}

glm::vec3 LERP(glm::vec3 p0, glm::vec3 p1, float dt)
{
	
	return p0 * (1.0f - dt) + p1 * dt;
}

int main()
{
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it
	if (!initGLFW())
		return 1;
	if (!initGLAD())
		return 1;

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDebugMessageCallback(GlDebugMessage, nullptr);
	glEnable(GL_TEXTURE_2D);//textrue
	glEnable(GL_DEPTH_TEST);	// GL states
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);


	//////////////////////////////////////////mod and transform

	//obj load

	VertexArrayObject::sptr melonVAO = ObjLoader::LoadFromFile("watermelon.obj");
	Texture2D::sptr melonTex = Texture2D::LoadFromFile("images/melon UV.png");

	VertexArrayObject::sptr canVAO = ObjLoader::LoadFromFile("can.obj");
	Texture2D::sptr canTex = Texture2D::LoadFromFile("images/can UV.png");

	VertexArrayObject::sptr clockVAO = ObjLoader::LoadFromFile("clock.obj");
	Texture2D::sptr clockTex = Texture2D::LoadFromFile("images/clock UV.png");

	
	//shader
	Shader::sptr melonShader = Shader::Create();
	melonShader->LoadShaderPartFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	melonShader->LoadShaderPartFromFile("shaders/frag_lighting.glsl", GL_FRAGMENT_SHADER);
	//melonShader->LoadShaderPartFromFile("shaders/basic.glsl", GL_FRAGMENT_SHADER);
	melonShader->Link();
	
	Shader::sptr canShader = Shader::Create();
	canShader->LoadShaderPartFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	canShader->LoadShaderPartFromFile("shaders/basic.glsl", GL_FRAGMENT_SHADER);
	canShader->Link();
	
	
	Shader::sptr clockShader = Shader::Create();
	clockShader->LoadShaderPartFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	clockShader->LoadShaderPartFromFile("shaders/basic.glsl", GL_FRAGMENT_SHADER);
	clockShader->Link();
	



	//transform
	Transform::sptr melonTrans = Transform::Create();
	//melonTrans->SetLocalPosition(glm::vec3(0.0, 0.0, 0.0));
	melonTrans->SetLocalRotation(glm::vec3(0.0, -90.0, 0.0));
	Transform::sptr canTrans = Transform::Create();
	canTrans->SetLocalPosition(glm::vec3(5.0, 0.0, -5.0));

	Transform::sptr clockTrans = Transform::Create();
	clockTrans->SetLocalPosition(glm::vec3(-5.0, 0.0, -5.0));

	//////////////////////////////////////////camera
	camera = Camera::Create();
	glm::vec3 cameraPosition = glm::vec3(0, 1, 3);
	camera->SetPosition(cameraPosition); // Set initial position
	camera->SetUp(glm::vec3(0, 1, 0)); // Use a z-up coordinate system
	camera->LookAt(glm::vec3(0.0f, 1.0f, 0.0f)); // Look at center of the screen
	camera->SetFovDegrees(90.0f); 
	camera->SetOrthoHeight(3.0f);
	
	//lighting stuff
	//shader light effect
	glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 lightCol = glm::vec3(3.0f, 3.0f, 3.0f);
	float     lightAmbientPow = 0.05f;
	float     lightSpecularPow = 0.8f;
	glm::vec3 ambientCol = glm::vec3(1.0f);
	float     ambientPow = 0.1f;
	float     shininess = 2.0f;
	int ambB = 0;
	int difB = 0;
	int specB = 0;
	int lightB = 0;
	
	melonShader->SetUniform("u_LightPos", lightPos);
	melonShader->SetUniform("u_LightCol", lightCol);
	melonShader->SetUniform("u_AmbientLightStrength", lightAmbientPow);
	melonShader->SetUniform("u_SpecularLightStrength", lightSpecularPow);
	melonShader->SetUniform("u_AmbientCol", ambientCol);
	melonShader->SetUniform("u_AmbientStrength", ambientPow);
	melonShader->SetUniform("u_Shininess", shininess);
	melonShader->SetUniform("u_ambB", ambB);
	melonShader->SetUniform("u_difB", difB);
	melonShader->SetUniform("u_specB", specB);
	melonShader->SetUniform("u_lightB", lightB);
	

	//delta time and fps limit 
	double lastFrame = glfwGetTime();
	double lastFrameTime = glfwGetTime();
	//double timer = glfwGetTime();
	const double fpsLimit = 1.0 / 60.0;

	float lerpTimer = 0.0;
	bool forward = true;

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClearColor(0.08f, 0.17f, 0.31f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);// delta time
		//lerp
		if (forward == true)
		{
			lerpTimer += dt/2;
		}
		else {
			lerpTimer -= dt/2;
		}

		if (lerpTimer >= 1)
		{
			lerpTimer = 1.0;
			forward = false;
		}
		if (lerpTimer <= 0.0)
		{
			lerpTimer = 0.0;
			forward = true;
		}

	
		
		
		//render
		clockTrans->SetLocalPosition( LERP(glm::vec3(-5.0, 0.0, -5.0), glm::vec3(0.0, 0.0, -5.0), lerpTimer));
		//tester
		melonShader->Bind();
		//melonShader->SetUniformMatrix("u_View", camera->GetView());
		melonShader->SetUniform("u_CamPos", camera->GetPosition());
		melonShader->SetUniform("s_tex", 0);
		
		melonTex->Bind(0);
		
		
		melonShader->SetUniform("u_Shininess", shininess);
	
		//end
		RenderVAO(melonShader, melonVAO, camera, melonTrans,melonTex);
		RenderVAO(canShader, canVAO, camera, canTrans,canTex);
		RenderVAO(clockShader, clockVAO, camera, clockTrans,clockTex);
		

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		{
			melonShader->Bind();
			ambB = 10;
			difB = 10;
			specB = 10;
			lightB = 10;
			melonShader->SetUniform("u_ambB", ambB);
			melonShader->SetUniform("u_difB", difB);
			melonShader->SetUniform("u_specB", specB);
			melonShader->SetUniform("u_lightB", lightB);
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		{
			melonShader->Bind();
			ambB = 0;
			difB = 10;
			specB = 10;
			lightB = 0;
			melonShader->SetUniform("u_ambB", ambB);
			melonShader->SetUniform("u_difB", difB);
			melonShader->SetUniform("u_specB", specB);
			melonShader->SetUniform("u_lightB", lightB);
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		{
			melonShader->Bind();
			ambB = 10;
			difB = 0;
			specB = 0;
			lightB = 0;
			melonShader->SetUniform("u_ambB", ambB);
			melonShader->SetUniform("u_difB", difB);
			melonShader->SetUniform("u_specB", specB);
			melonShader->SetUniform("u_lightB", lightB);
		}
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		{
			melonShader->Bind();
			ambB = 0;
			difB = 0;
			specB = 0;
			lightB = 0;
			melonShader->SetUniform("u_ambB", ambB);
			melonShader->SetUniform("u_difB", difB);
			melonShader->SetUniform("u_specB", specB);
			melonShader->SetUniform("u_lightB", lightB);
		}
		//fps limit 
		if ((thisFrame - lastFrameTime) >= fpsLimit)
		{
			cameraMove(camera);
			lastFrameTime = thisFrame;
		}

		lastFrame = thisFrame;
		//timer = glfwGetTime();
		glfwSwapBuffers(window);
	}


	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
} 

