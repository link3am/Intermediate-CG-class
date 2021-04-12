#include "Utilities/BackendHandler.h"
#include <filesystem>
#include <json.hpp>
#include <fstream>

//TODO: New for this tutorial
#include <DirectionalLight.h>
#include <PointLight.h>
#include <UniformBuffer.h>
/////////////////////////////

#include <Texture2D.h>
#include <Texture2DData.h>
#include <MeshBuilder.h>
#include <MeshFactory.h>
#include <NotObjLoader.h>
#include <ObjLoader.h>
#include <VertexTypes.h>
#include <ShaderMaterial.h>

#include <RendererComponent.h>
#include <TextureCubeMap.h>
#include <TextureCubeMapData.h>

#include <Timing.h>
#include <GameObjectTag.h>
#include <InputHelpers.h>

#include <IBehaviour.h>
#include <CameraControlBehaviour.h>
#include <FollowPathBehaviour.h> 
#include <SimpleMoveBehaviour.h>
#include <Player.h>
#include <Bullet.h>
#include <Graphics\GBuffer.h>
//#include <Graphics\IlluminationBuffer.h>
#include <Graphics\3amBuffer.h>
using namespace std;

int main() {
	int frameIx = 0;
	float fpsBuffer[128];
	float minFps, maxFps, avgFps;
	int selectedVao = 0; // select cube by default
	std::vector<GameObject> controllables;
	//key toggle
	//std::vector<GameObject> controllables;

	bool drawGBuffer = false;
	bool drawIllumBuffer = false;

	BackendHandler::InitAll();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(BackendHandler::GlDebugMessage, nullptr);
	glEnable(GL_TEXTURE_2D);


#pragma region Shader and ImGui 

	//Shader::sptr passthroughShader = Shader::Create();
	//passthroughShader->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
	//passthroughShader->LoadShaderPartFromFile("shaders/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
	//passthroughShader->Link();  

	Shader::sptr simpleDepthShader = Shader::Create();
	simpleDepthShader->LoadShaderPartFromFile("shaders/simple_depth_vert.glsl", GL_VERTEX_SHADER);
	simpleDepthShader->LoadShaderPartFromFile("shaders/simple_depth_frag.glsl", GL_FRAGMENT_SHADER);
	simpleDepthShader->Link();
	 
	//Init gBuffer shader
	Shader::sptr gBufferShader = Shader::Create();
	gBufferShader->LoadShaderPartFromFile("Shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	gBufferShader->LoadShaderPartFromFile("Shaders/gBuffer_pass_frag.glsl", GL_FRAGMENT_SHADER); 
	gBufferShader->Link();

	//Shader::sptr shader = Shader::Create();
	//shader->LoadShaderPartFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
	//Directional Light Shader
	//shader->LoadShaderPartFromFile("shaders/directional_blinn_phong_frag.glsl", GL_FRAGMENT_SHADER);
	//shader->Link();


	//Creates our directional Light
	//DirectionalLight theSun;
	//UniformBuffer directionalLightBuffer;

	PostEffect* basicEffect;
	Framebuffer* shadowBuffer;
	GBuffer* gBuffer;
	//IlluminationBuffer* illuminationBuffer;
	SamBuffer* samBuffer;
	int activeEffect = 0;

	int toggleScene = 0;

	glm::vec4 lightDir = glm::vec4(0.0, 0.0, -1.0,0.0);
	float lightD[4] = { 0.0,0.0,0.0,0.0 };
	glm::vec4 lightCol = glm::vec4(1.0, 0.0, 0.0,0.0);
	std::vector<PostEffect*> effects;
	SepiaEffect* sepiaEffect;
	GreyscaleEffect* greyscaleEffect;
	PixelEffect* pixelEffect;
	BloomEffect* bloomEffect;
	FilmEffect* filmEffect;

	//Allocates enough memory for one directional light (we can change this easily, but we only need 1 directional light)
	//directionalLightBuffer.AllocateMemory(sizeof(DirectionalLight));
	//Casts our sun as "data" and sends it to the shader
	//directionalLightBuffer.SendData(reinterpret_cast<void*>(&theSun), sizeof(DirectionalLight));

	//directionalLightBuffer.Bind(0);


//glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 0.0f);
//glm::vec3 lightCol = glm::vec3(0.9f, 0.85f, 0.5f);
//float     lightAmbientPow = 0.05f;
//float     lightSpecularPow = 1.0f;
//glm::vec3 ambientCol = glm::vec3(1.0f);
//float     ambientPow = 0.1f;
//float     lightLinearFalloff = 0.09f;
//float     lightQuadraticFalloff = 0.032f;
//bool CelShading = false;
//bool Normal = false;

	//shader->SetUniform("u_LightPos", lightPos);
	//shader->SetUniform("u_LightCol", lightCol);
	//shader->SetUniform("u_AmbientLightStrength", lightAmbientPow);
	//shader->SetUniform("u_SpecularLightStrength", lightSpecularPow);
	//shader->SetUniform("u_AmbientCol", ambientCol);
	//shader->SetUniform("u_AmbientStrength", ambientPow);
	//shader->SetUniform("u_LightAttenuationConstant", 1.0f);
	//shader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
	//shader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);
	//shader->SetUniform("u_CelShading", (int)CelShading);
	//shader->SetUniform("u_Normal", (int)Normal);

	//3am
	//BloomEffect* bloomEffect;


	vector<Bullet> BulletList;

	vector<GameObject> BulletObjList;

#pragma endregion

#pragma region ImGui 
	BackendHandler::imGuiCallbacks.push_back([&]() {
		if (ImGui::CollapsingHeader("toggles "))
		{
			ImGui::SliderInt("Chosen Effect", &activeEffect, 0, effects.size() - 1);

			if (activeEffect == 0)
			{
				ImGui::Text("Active Effect: basic Effect");

				PostEffect* temp = (PostEffect*)effects[activeEffect];
			}
			if (activeEffect == 1)
			{
				ImGui::Text("Active Effect: Sepia Effect");

				SepiaEffect* temp = (SepiaEffect*)effects[activeEffect];
				float intensity = temp->GetIntensity();

				if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 1.0f))
				{
					temp->SetIntensity(intensity);
				}
			}
			if (activeEffect == 2)
			{
				ImGui::Text("Active Effect: Greyscale Effect");

				GreyscaleEffect* temp = (GreyscaleEffect*)effects[activeEffect];
				float intensity = temp->GetIntensity();

				if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 1.0f))
				{
					temp->SetIntensity(intensity);
				}
			}
			if (activeEffect == 3)
			{
				ImGui::Text("Active Effect: Pixel Effect");

				PixelEffect* temp = (PixelEffect*)effects[activeEffect];
				float intensity = temp->GetIntensity();

				if (ImGui::SliderFloat("Intensity", &intensity, 0.001f, 0.01f))
				{
					temp->SetIntensity(intensity);
				}
			}
			if (activeEffect == 4)
			{
				ImGui::Text("Active Effect: bloom Effect");

				BloomEffect* temp = (BloomEffect*)effects[activeEffect];
				float intensity = temp->GetThreshold();

				if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 1.0f))
				{
					temp->SetThreshold(intensity);
				}
			}
			if (activeEffect == 5)
			{
				ImGui::Text("Active Effect: Film Effect");

				FilmEffect* temp = (FilmEffect*)effects[activeEffect];
				float intensity = temp->GetIntensity();

				if (ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f))
				{
					temp->SetIntensity(intensity);
				}
			}
		}

		if (ImGui::CollapsingHeader("Switch Scene"))
		{
			ImGui::SliderInt("toggleScene", &toggleScene, 0, 4);
			 
			if (toggleScene == 0)
			{
				ImGui::Text(" deferred light source");
				drawGBuffer = false;
				effects[activeEffect]->ApplyEffect(samBuffer);
				effects[activeEffect]->DrawToScreen();
			} 
			if (toggleScene == 1)
			{
				ImGui::Text(" Position Buffer");
				gBuffer->PositionBuffer();
				drawGBuffer = false; 
			}
			if (toggleScene == 2)
			{
				ImGui::Text(" Normal Buffer");
				gBuffer->NormalBuffer();
				drawGBuffer = false;
			}
			if (toggleScene == 3)
			{
				ImGui::Text(" Color Buffer");
				gBuffer->ColorBuffer();
				drawGBuffer = false;
			}
			if (toggleScene == 4)
			{
				ImGui::Text(" Color Buffer");
				samBuffer->DrawIllumBuffer();
				drawGBuffer = false;
			}
		}
		

		if (ImGui::CollapsingHeader("Light Level Lighting Settings"))
		{
			if (ImGui::SliderFloat4("Light Direction/Position", lightD,-5,5))
			{
				samBuffer->SetSun(glm::vec4(lightD[0], lightD[1], lightD[2], lightD[3]));
			}
		
			if (ImGui::ColorPicker3("Light Col", glm::value_ptr(samBuffer->GetSunRef()._lightCol)))
			{
				samBuffer->SetSun(glm::vec4(lightD[0], lightD[1], lightD[2], lightD[3]));
			}
		}
		

		ImGui::Text("Q/E -> Yaw\nLeft/Right -> Roll\nUp/Down -> Pitch\nY -> Toggle Mode");

		minFps = FLT_MAX;
		maxFps = 0;
		avgFps = 0;
		for (int ix = 0; ix < 128; ix++) {
			if (fpsBuffer[ix] < minFps) { minFps = fpsBuffer[ix]; }
			if (fpsBuffer[ix] > maxFps) { maxFps = fpsBuffer[ix]; }
			avgFps += fpsBuffer[ix];
		}
		ImGui::PlotLines("FPS", fpsBuffer, 128);
		ImGui::Text("MIN: %f MAX: %f AVG: %f", minFps, maxFps, avgFps / 128.0f);
		});

#pragma endregion
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL); // New 

	// Load some textures from files
	Texture2D::sptr noSpec = Texture2D::LoadFromFile("images/grassSpec.png");
	TextureCubeMap::sptr environmentMap = TextureCubeMap::LoadFromImages("images/cubemaps/skybox/ToonSky.jpg");
	//melon
	Texture2D::sptr melonUV = Texture2D::LoadFromFile("images/melon UV.png");
	Texture2D::sptr tableUV = Texture2D::LoadFromFile("images/table UV.png");
	//stuff
	Texture2D::sptr uv1 = Texture2D::LoadFromFile("images/clock UV.png");
	Texture2D::sptr uv2 = Texture2D::LoadFromFile("images/cutting UV.png");
	Texture2D::sptr uv3 = Texture2D::LoadFromFile("images/microwave UV.png");
	Texture2D::sptr uv4 = Texture2D::LoadFromFile("images/can UV.png");
	Texture2D::sptr uv5 = Texture2D::LoadFromFile("images/dish UV.png");
	Texture2D::sptr uv6 = Texture2D::LoadFromFile("images/pan UV.png");

	///////////////////////////////////// Scene Generation //////////////////////////////////////////////////
#pragma region Scene Generation

// We need to tell our scene system what extra component types we want to support
	GameScene::RegisterComponentType<RendererComponent>();
	GameScene::RegisterComponentType<BehaviourBinding>();
	GameScene::RegisterComponentType<Camera>();

	// Create a scene, and set it to be the active scene in the application
	GameScene::sptr scene = GameScene::Create("scene 1");
	Application::Instance().ActiveScene = scene;

	// We can create a group ahead of time to make iterating on the group faster
	entt::basic_group<entt::entity, entt::exclude_t<>, entt::get_t<Transform>, RendererComponent> renderGroup
		= scene->Registry().group<RendererComponent>(entt::get_t<Transform>());

	// Create a material and set some properties for it
	

	ShaderMaterial::sptr melonMat = ShaderMaterial::Create();
	melonMat->Shader = gBufferShader;
	melonMat->Set("s_Diffuse", melonUV);
	melonMat->Set("s_Specular", noSpec);
	melonMat->Set("u_Shininess", 8.0f);
	melonMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr tableMat = ShaderMaterial::Create();
	tableMat->Shader = gBufferShader;
	tableMat->Set("s_Diffuse", tableUV);
	tableMat->Set("s_Specular", noSpec);
	tableMat->Set("u_Shininess", 8.0f);
	tableMat->Set("u_TextureMix", 0.0f);
	//stuff material

	ShaderMaterial::sptr clockMat = ShaderMaterial::Create();
	clockMat->Shader = gBufferShader;
	clockMat->Set("s_Diffuse", uv1);
	clockMat->Set("s_Specular", noSpec);
	clockMat->Set("u_Shininess", 8.0f);
	clockMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr cuttingMat = ShaderMaterial::Create();
	cuttingMat->Shader = gBufferShader;
	cuttingMat->Set("s_Diffuse", uv2);
	cuttingMat->Set("s_Specular", noSpec);
	cuttingMat->Set("u_Shininess", 8.0f);
	cuttingMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr microwaveMat = ShaderMaterial::Create();
	microwaveMat->Shader = gBufferShader;
	microwaveMat->Set("s_Diffuse", uv3);
	microwaveMat->Set("s_Specular", noSpec);
	microwaveMat->Set("u_Shininess", 8.0f);
	microwaveMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr canMat = ShaderMaterial::Create();
	canMat->Shader = gBufferShader;
	canMat->Set("s_Diffuse", uv4);
	canMat->Set("s_Specular", noSpec);
	canMat->Set("u_Shininess", 8.0f);
	canMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr dishMat = ShaderMaterial::Create();
	dishMat->Shader = gBufferShader;
	dishMat->Set("s_Diffuse", uv5);
	dishMat->Set("s_Specular", noSpec);
	dishMat->Set("u_Shininess", 8.0f);
	dishMat->Set("u_TextureMix", 0.0f);

	ShaderMaterial::sptr panMat = ShaderMaterial::Create();
	panMat->Shader = gBufferShader;
	panMat->Set("s_Diffuse", uv6);
	panMat->Set("s_Specular", noSpec);
	panMat->Set("u_Shininess", 8.0f);
	panMat->Set("u_TextureMix", 0.0f);

	VertexArrayObject::sptr melonvao = ObjLoader::LoadFromFile("models/watermelon.obj");
	GameObject melonObj = scene->CreateEntity("melon");
	{
		melonObj.emplace<RendererComponent>().SetMesh(melonvao).SetMaterial(melonMat);
		melonObj.get<Transform>().SetLocalPosition(0.0f, 0.0f, 0.0f);
		melonObj.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	Player P1 = Player(melonObj);

	GameObject table = scene->CreateEntity("table");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/table.obj");
		table.emplace<RendererComponent>().SetMesh(vao).SetMaterial(tableMat);
		table.get<Transform>().SetLocalPosition(0.0f, 0.0f, 0.0f);
		table.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff1 = scene->CreateEntity("s1");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/clock.obj");
		stuff1.emplace<RendererComponent>().SetMesh(vao).SetMaterial(clockMat);
		stuff1.get<Transform>().SetLocalPosition(2.0f, 0.0f, -0.7f);
		stuff1.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff2 = scene->CreateEntity("s2");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/cutting.obj");
		stuff2.emplace<RendererComponent>().SetMesh(vao).SetMaterial(cuttingMat);
		stuff2.get<Transform>().SetLocalPosition(8.0f, 0.0f, -0.8f);
		stuff2.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff3 = scene->CreateEntity("s3");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/microwave.obj");
		stuff3.emplace<RendererComponent>().SetMesh(vao).SetMaterial(microwaveMat);
		stuff3.get<Transform>().SetLocalPosition(15.0f, 0.0f, -1.0f);
		stuff3.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff4 = scene->CreateEntity("s4");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/can.obj");
		stuff4.emplace<RendererComponent>().SetMesh(vao).SetMaterial(canMat);
		stuff4.get<Transform>().SetLocalPosition(19.4f, 0.0f, -0.5f);
		stuff4.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff5 = scene->CreateEntity("s5");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/dish.obj");
		stuff5.emplace<RendererComponent>().SetMesh(vao).SetMaterial(dishMat);
		stuff5.get<Transform>().SetLocalPosition(23.f, 0.0f, -1.5f);
		stuff5.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff6 = scene->CreateEntity("s6");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/dish.obj");
		stuff6.emplace<RendererComponent>().SetMesh(vao).SetMaterial(dishMat);
		stuff6.get<Transform>().SetLocalPosition(23.f, 0.5f, -1.5f);
		stuff6.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	GameObject stuff7 = scene->CreateEntity("s7");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/can.obj");
		stuff7.emplace<RendererComponent>().SetMesh(vao).SetMaterial(canMat);
		stuff7.get<Transform>().SetLocalPosition(26.0f, 0.0f, -0.5f);
		stuff7.get<Transform>().SetLocalRotation(0.0f, 40.0f, 0.0f);
	}
	GameObject stuff8 = scene->CreateEntity("s8");
	{
		VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/pan.obj");
		stuff8.emplace<RendererComponent>().SetMesh(vao).SetMaterial(panMat);
		stuff8.get<Transform>().SetLocalPosition(30.0f, 0.0f, -3.0f);
		stuff8.get<Transform>().SetLocalRotation(0.0f, 0.0f, 0.0f);
	}
	// Create an object to be our camera
	GameObject cameraObject = scene->CreateEntity("Camera");
	{
		cameraObject.get<Transform>().SetLocalPosition(0, 2.5, 6).LookAt(glm::vec3(0, 1, 0)).SetLocalRotation(glm::vec3(0, 0, 0));

		// We'll make our camera a component of the camera object
		Camera& camera = cameraObject.emplace<Camera>();// Camera::Create();
		camera.SetPosition(glm::vec3(0, 2.5, 6));
		camera.SetUp(glm::vec3(0, 1, 0));
		camera.LookAt(glm::vec3(0, 1, 0));
		camera.SetFovDegrees(90.0f); // Set an initial FOV
		camera.SetOrthoHeight(3.0f);
		//BehaviourBinding::Bind<CameraControlBehaviour>(cameraObject);
	}


	int width, height;
	glfwGetWindowSize(BackendHandler::window, &width, &height);

	GameObject gBufferObject = scene->CreateEntity("G Buffer");
	{
		gBuffer = &gBufferObject.emplace<GBuffer>();
		gBuffer->Init(width, height);
	}
	
	GameObject illuminationbufferObject = scene->CreateEntity("Illumination Buffer");
	{
		//illuminationBuffer = &illuminationbufferObject.emplace<IlluminationBuffer>();
		//illuminationBuffer->Init(width, height);
	}
	GameObject sambufferObject = scene->CreateEntity("3am Buffer");
	{
		samBuffer = &sambufferObject.emplace<SamBuffer>();
		samBuffer->Init(width, height);
	}
	int shadowWitch = 4096;
	int shadowHeight = 4096;

	GameObject shadowBufferObject = scene->CreateEntity("Shadow Buffer");
	{
		shadowBuffer = &shadowBufferObject.emplace<Framebuffer>();
		shadowBuffer->AddDepthTarget();
		shadowBuffer->Init(shadowWitch, shadowHeight);
	}

	GameObject framebufferObject = scene->CreateEntity("Basic Effect");
	{
		basicEffect = &framebufferObject.emplace<PostEffect>();
		basicEffect->Init(width, height);
	}
	effects.push_back(basicEffect);

	GameObject sepiaEffectObject = scene->CreateEntity("Sepia Effect");
	{
		sepiaEffect = &sepiaEffectObject.emplace<SepiaEffect>();
		sepiaEffect->Init(width, height);
	}
	effects.push_back(sepiaEffect);

	GameObject greyscaleEffectObject = scene->CreateEntity("Greyscale Effect");
	{
		greyscaleEffect = &greyscaleEffectObject.emplace<GreyscaleEffect>();
		greyscaleEffect->Init(width, height);
	}
	effects.push_back(greyscaleEffect);

	GameObject pixelEffectObject = scene->CreateEntity("Pixel Effect");
	{
		pixelEffect = &pixelEffectObject.emplace<PixelEffect>();
		pixelEffect->Init(width, height);
	}
	effects.push_back(pixelEffect);

	GameObject bloomEffectObject = scene->CreateEntity("Bloom Effect");
	{
		bloomEffect = &bloomEffectObject.emplace<BloomEffect>();
		bloomEffect->Init(width, height);
	}
	effects.push_back(bloomEffect);

	GameObject filmEffectObject = scene->CreateEntity("Film Effect");
	{
		filmEffect = &filmEffectObject.emplace<FilmEffect>();
		filmEffect->Init(width, height);
	}
	effects.push_back(filmEffect);
#pragma endregion 
	//////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////// SKYBOX ///////////////////////////////////////////////

		// Load our shaders
	Shader::sptr skybox = std::make_shared<Shader>();
	skybox->LoadShaderPartFromFile("shaders/skybox-shader.vert.glsl", GL_VERTEX_SHADER);
	skybox->LoadShaderPartFromFile("shaders/skybox-shader.frag.glsl", GL_FRAGMENT_SHADER);
	skybox->Link();

	ShaderMaterial::sptr skyboxMat = ShaderMaterial::Create();
	skyboxMat->Shader = skybox;
	skyboxMat->Set("s_Environment", environmentMap);
	skyboxMat->Set("u_EnvironmentRotation", glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0))));
	skyboxMat->RenderLayer = 100;

	MeshBuilder<VertexPosNormTexCol> mesh;
	MeshFactory::AddIcoSphere(mesh, glm::vec3(0.0f), 1.0f);
	MeshFactory::InvertFaces(mesh);
	VertexArrayObject::sptr meshVao = mesh.Bake();

	GameObject skyboxObj = scene->CreateEntity("skybox");
	skyboxObj.get<Transform>().SetLocalPosition(0.0f, 0.0f, 0.0f);
	//skyboxObj.get_or_emplace<RendererComponent>().SetMesh(meshVao).SetMaterial(skyboxMat);




// We'll use a vector to store all our key press events for now (this should probably be a behaviour eventually)
	std::vector<KeyPressWatcher> keyToggles;
	{
		// This is an example of a key press handling helper. Look at InputHelpers.h an .cpp to see
		// how this is implemented. Note that the ampersand here is capturing the variables within
		// the scope. If you wanted to do some method on the class, your best bet would be to give it a method and
		// use std::bind
		keyToggles.emplace_back(GLFW_KEY_T, [&]() { cameraObject.get<Camera>().ToggleOrtho(); });

		//Toggles drawing
		keyToggles.emplace_back(GLFW_KEY_F1, [&]() {drawGBuffer = !drawGBuffer; });
		keyToggles.emplace_back(GLFW_KEY_F2, [&]() {drawIllumBuffer = !drawIllumBuffer; });
		controllables.push_back(melonObj);

		keyToggles.emplace_back(GLFW_KEY_KP_ADD, [&]() {
			BehaviourBinding::Get<SimpleMoveBehaviour>(controllables[selectedVao])->Enabled = false;
			selectedVao++;
			if (selectedVao >= controllables.size())
				selectedVao = 0;
			BehaviourBinding::Get<SimpleMoveBehaviour>(controllables[selectedVao])->Enabled = true;
			});
		keyToggles.emplace_back(GLFW_KEY_KP_SUBTRACT, [&]() {
			BehaviourBinding::Get<SimpleMoveBehaviour>(controllables[selectedVao])->Enabled = false;
			selectedVao--;
			if (selectedVao < 0)
				selectedVao = controllables.size() - 1;
			BehaviourBinding::Get<SimpleMoveBehaviour>(controllables[selectedVao])->Enabled = true;
			});

		keyToggles.emplace_back(GLFW_KEY_Y, [&]() {
			auto behaviour = BehaviourBinding::Get<SimpleMoveBehaviour>(controllables[selectedVao]);
			behaviour->Relative = !behaviour->Relative;
			});
	}


	// time
	Timing& time = Timing::Instance();
	time.LastFrame = glfwGetTime();
	double lastfpsTime = glfwGetTime();//60hz


	///// Game loop /////
	while (!glfwWindowShouldClose(BackendHandler::window)) {
		glfwPollEvents();
		// time,FPS, etc
#pragma region time and FPS 
		time.CurrentFrame = glfwGetTime();
		time.DeltaTime = static_cast<float>(time.CurrentFrame - time.LastFrame);
		time.DeltaTime = time.DeltaTime > 1.0f ? 1.0f : time.DeltaTime;
		fpsBuffer[frameIx] = 1.0f / time.DeltaTime;
		frameIx++;
		if (frameIx >= 128)
			frameIx = 0;

		// We'll make sure our UI isn't focused before we start handling input for our game
		if (!ImGui::IsAnyWindowFocused()) {
			// We need to poll our key watchers so they can do their logic with the GLFW state
			// Note that since we want to make sure we don't copy our key handlers, we need a const
			// reference!
			for (const KeyPressWatcher& watcher : keyToggles) {
				watcher.Poll(BackendHandler::window);
			}
		}
#pragma endregion 

#pragma region 

		//for (vector<Bullet>::iterator it = BulletList.begin(); it != BulletList.end();)
		//{
		//	it->projectile();
		//	if (it->isDeath() == true) {
		//		scene->RemoveEntity(it->getBullet());
		//		it = BulletList.erase(it);
		//	}
		//	else {
		//		it++;
		//	}
		//}

		for (int i = 0; i < BulletList.size(); i++) {
			BulletList[i].projectile();

			if (BulletList[i].isDeath()) {
				scene->RemoveEntity(BulletList[i].getBullet());
				BulletList.erase(BulletList.begin() + i);
				BulletObjList.erase(BulletObjList.begin() + i);
				i--;
			}
		}

#pragma endregion
		//////////////////////////////Gmae logic lololol////////////////////////////////////////
		if (glfwGetKey(BackendHandler::window, GLFW_KEY_J) == GLFW_PRESS) {

			BulletObjList.push_back(scene->CreateEntity("bullet"));
			GameObject& bullet = BulletObjList[BulletObjList.size() - 1];
			{
				bullet.emplace<RendererComponent>().SetMesh(melonvao).SetMaterial(melonMat);
				bullet.get<Transform>().SetLocalPosition(P1.getPosition());
				bullet.get<Transform>().SetLocalRotation(90.0f, 0.0f, 0.0f);
			}
			Bullet B1 = Bullet(bullet, P1.returnFace());
			BulletList.push_back(B1);
		}

		if ((time.CurrentFrame - lastfpsTime) >= 1.0 / 90.0) {//HZ limit
			P1.PlayerPhy(BackendHandler::window, time.DeltaTime);

			cameraObject.get<Transform>().SetLocalPosition(P1.getPosition() + glm::vec3(0, 2.5, 6));
			lastfpsTime = time.CurrentFrame;
		}




		///////////////////////////////////////////////////////////////////////////////////////////	
#pragma region sort and render and registry something exolol



	// Iterate over all the behaviour binding components
		scene->Registry().view<BehaviourBinding>().each([&](entt::entity entity, BehaviourBinding& binding) {
			// Iterate over all the behaviour scripts attached to the entity, and update them in sequence (if enabled)
			for (const auto& behaviour : binding.Behaviours) {
				if (behaviour->Enabled) {
					behaviour->Update(entt::handle(scene->Registry(), entity));
				}
			}
			});

		// Clear the screen
		basicEffect->Clear();
		for (int i = 0; i < effects.size(); i++)
		{
			effects[i]->Clear();
		}
		shadowBuffer->Clear();
		gBuffer->Clear();
		//illuminationBuffer->Clear();
		samBuffer->Clear();
		glClearColor(1.0f, 1.0f, 1.0f, 0.3f);
		glEnable(GL_DEPTH_TEST);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Update all world matrices for this frame
		scene->Registry().view<Transform>().each([](entt::entity entity, Transform& t) {
			t.UpdateWorldMatrix();
			});

		// Grab out camera info from the camera object
		Transform& camTransform = cameraObject.get<Transform>();
		glm::mat4 view = glm::inverse(camTransform.LocalTransform());
		glm::mat4 projection = cameraObject.get<Camera>().GetProjection();
		glm::mat4 viewProjection = projection * view;

		//Set up light space martix
		//glm::mat4 lightProjectionMartix = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -30.0f, 30.0f);
		//glm::mat4 lightViewMatrix = glm::lookAt(glm::vec3(-illuminationBuffer->GetSunRef()._lightDirection), glm::vec3(), glm::vec3(0.0f, 0.0f, 1.0f));
		//glm::mat4 lightSpaceViewProj = lightProjectionMartix * lightViewMatrix;
		 
		//Set shadow stuff
		//illuminationBuffer->SetLightSpaceViewProj(lightSpaceViewProj);
		glm::vec3 camPos = glm::inverse(view) * glm::vec4(0, 0, 0, 1);
		//illuminationBuffer->SetCamPos(camPos);
		samBuffer->SetCamPos(camPos);

		// Sort the renderers by shader and material, we will go for a minimizing context switches approach here,
		// but you could for instance sort front to back to optimize for fill rate if you have intensive fragment shaders
		renderGroup.sort<RendererComponent>([](const RendererComponent& l, const RendererComponent& r) {
			// Sort by render layer first, higher numbers get drawn last
			if (l.Material->RenderLayer < r.Material->RenderLayer) return true;
			if (l.Material->RenderLayer > r.Material->RenderLayer) return false;

			// Sort by shader pointer next (so materials using the same shader run sequentially where possible)
			if (l.Material->Shader < r.Material->Shader) return true;
			if (l.Material->Shader > r.Material->Shader) return false;

			// Sort by material pointer last (so we can minimize switching between materials)
			if (l.Material < r.Material) return true;
			if (l.Material > r.Material) return false;

			return false;
			});
		// Start by assuming no shader or material is applied
		Shader::sptr current = nullptr;
		ShaderMaterial::sptr currentMat = nullptr;

		glViewport(0, 0, shadowWitch, shadowHeight);
		//shadowBuffer->Bind();

		//renderGroup.each([&](entt::entity e, RendererComponent& renderer, Transform& transform) {
			// Render the mesh
		//	if (renderer.CastShadows)
			//{
			//	BackendHandler::RenderVAO(simpleDepthShader, renderer.Mesh, viewProjection, transform, lightSpaceViewProj);
		//	}
		//	});

		//shadowBuffer->Unbind();

		glfwGetWindowSize(BackendHandler::window, &width, &height);

		glViewport(0, 0, width, height);

		gBuffer->Bind();

		// Iterate over the render group components and draw them
		renderGroup.each([&](entt::entity e, RendererComponent& renderer, Transform& transform) {
			// If the shader has changed, set up it's uniforms
			if (current != renderer.Material->Shader) {
				current = renderer.Material->Shader;
				current->Bind();
				BackendHandler::SetupShaderForFrame(current, view, projection);
			}
			// If the material has changed, apply it
			if (currentMat != renderer.Material) {
				currentMat = renderer.Material;
				currentMat->Apply();
			}

			
			// Render the mesh
			BackendHandler::RenderVAO(renderer.Material->Shader, renderer.Mesh, viewProjection, transform);
			});

		gBuffer->Unbind();
		//illuminationBuffer->BindBuffer(0);

		//skybox->Bind();
		//BackendHandler::SetupShaderForFrame(skybox, view, projection);
		//skyboxMat->Apply();
		//BackendHandler::RenderVAO(skybox, meshVao, viewProjection, skyboxObj.get<Transform>(), lightSpaceViewProj);
		//skybox->UnBind();

		//illuminationBuffer->UnbindBuffer();

		//shadowBuffer->BindDepthAsTexture(30);

		//illuminationBuffer->ApplyEffect(gBuffer);
		samBuffer->ApplyEffect(gBuffer);
		//shadowBuffer->UnbindTexture(30);

		if (drawGBuffer) {
			gBuffer->DrawBuffersToScreen();
		}
		else if (drawIllumBuffer) {
			samBuffer->DrawIllumBuffer();
		}
		else {
			effects[activeEffect]->ApplyEffect(samBuffer);
			effects[activeEffect]->DrawToScreen();
		}
		filmEffect->SetTime(time.CurrentFrame);

		/*
		if (activeEffect == 0) {
			effects[0]->DrawToScreen();

		}
		else
		{
			effects[activeEffect]->ApplyEffect(basicEffect);
			effects[activeEffect]->DrawToScreen();
		}
		*/
#pragma endregion


		// Draw our ImGui content
		BackendHandler::RenderImGui();

		scene->Poll();
		glfwSwapBuffers(BackendHandler::window);
		time.LastFrame = time.CurrentFrame;

	}

	//directionalLightBuffer.Unbind(0);

	// Nullify scene so that we can release references
	Application::Instance().ActiveScene = nullptr;
	//Clean up the environment generator so we can release references
	EnvironmentGenerator::CleanUpPointers();
	BackendHandler::ShutdownImGui();


	Logger::Uninitialize();
	return 0;
}