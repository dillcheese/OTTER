//Just a simple handler for simple initialization stuffs
#include "Utilities/BackendHandler.h"

#include <filesystem>
#include <json.hpp>
#include <fstream>

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

int main() {
	int frameIx = 0;
	float fpsBuffer[128];
	float minFps, maxFps, avgFps;
	int selectedVao = 0; // select cube by default
	std::vector<GameObject> controllables;

	BackendHandler::InitAll();

	// Let OpenGL know that we want debug output, and route it to our handler function
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(BackendHandler::GlDebugMessage, nullptr);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Push another scope so most memory should be freed *before* we exit the app
	{
#pragma region Shader and ImGui
		Shader::sptr passthroughShader = Shader::Create();
		passthroughShader->LoadShaderPartFromFile("shaders/passthrough_vert.glsl", GL_VERTEX_SHADER);
		passthroughShader->LoadShaderPartFromFile("shaders/passthrough_frag.glsl", GL_FRAGMENT_SHADER);
		passthroughShader->Link();

		// Load our shaders
		Shader::sptr shader = Shader::Create();
		shader->LoadShaderPartFromFile("shaders/vertex_shader.glsl", GL_VERTEX_SHADER);
		shader->LoadShaderPartFromFile("shaders/frag_blinn_phong_textured.glsl", GL_FRAGMENT_SHADER);
		shader->Link();

		glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 lightCol = glm::vec3(0.9f, 0.85f, 0.5f);
		float     lightAmbientPow = 0.05f;
		float     lightSpecularPow = 1.0f;
		glm::vec3 ambientCol = glm::vec3(1.0f);
		float     ambientPow = 0.1f;
		float     lightLinearFalloff = 0.09f;
		float     lightQuadraticFalloff = 0.032f;
		bool	option1 = true;
		bool	option2 = false;
		bool	option3 = false;
		bool	option4 = false;
		bool	option5 = true; //textures on
		bool	option6 = false;
		bool	option7 = false; // a + s + bloom

		// These are our application / scene level uniforms that don't necessarily update
		// every frame
		shader->SetUniform("u_LightPos", lightPos);
		shader->SetUniform("u_LightCol", lightCol);
		shader->SetUniform("u_AmbientLightStrength", lightAmbientPow);
		shader->SetUniform("u_SpecularLightStrength", lightSpecularPow);
		shader->SetUniform("u_AmbientCol", ambientCol);
		shader->SetUniform("u_AmbientStrength", ambientPow);
		shader->SetUniform("u_LightAttenuationConstant", 1.0f);
		shader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
		shader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);
		shader->SetUniform("u_Option1", (int)option1);
		shader->SetUniform("u_Option2", (int)option2);
		shader->SetUniform("u_Option3", (int)option3);
		shader->SetUniform("u_Option4", (int)option4);
		shader->SetUniform("u_Option5", (int)option5);
		shader->SetUniform("u_Option6", (int)option6);
		shader->SetUniform("u_Option7", (int)option7);

		PostEffect* basicEffect;

		int activeEffect = 0;
		std::vector<PostEffect*> effects;

		Bloom* BloomEffect;

		int bands = 2;
		shader->SetUniform("bands", bands);

		// We'll add some ImGui controls to control our shader
		BackendHandler::imGuiCallbacks.push_back([&]() {
			if (ImGui::CollapsingHeader("Effect Controls"))
			{
				if (activeEffect == 1)
				{
					ImGui::Text("Active Effect: Bloom Effect");

					Bloom* temp = (Bloom*)effects[activeEffect];
					float intensity = temp->GetIntensity();
					float threshold = temp->GetThreshold();
					int amount = temp->GetAmount();
					float blur = temp->GetBlur();
					float gamma = temp->GetGamma();

					if (ImGui::SliderFloat("Exposure", &intensity, 0.10f, 2.0f))
					{
						temp->SetIntensity(intensity);
					}

					if (ImGui::SliderFloat("Threshold", &threshold, 0.0f, 1.0f))
					{
						temp->SetThreshold(threshold);
					}
					if (ImGui::SliderInt("Blur Amounts", &amount, 1, 20))
					{
						temp->SetAmount(amount);
					}
					if (ImGui::SliderFloat("Blur Weights", &blur, 0.001f, 0.1f))
					{
						temp->SetBlur(blur);
					}
					if (ImGui::SliderFloat("Gamma", &gamma, 1.0f, 5.0f))
					{
						temp->SetGamma(gamma);
					}
				}

				if (activeEffect == 0)
				{
					ImGui::Text("Active Effect: NONE");
				}
			}
			if (ImGui::CollapsingHeader("A1 Light Options")) {
				if (ImGui::SliderInt("Bands", &bands, 2, 12))
				{
					shader->SetUniform("bands", bands);
				}

				if (ImGui::Checkbox("No Light", &option1)) {
					option2 = false;
					option3 = false;
					option4 = false;
					option6 = false;
					option7 = false;
					activeEffect = 0;
				}

				if (ImGui::Checkbox("Ambient Only", &option2)) {
					option1 = false;
					option3 = false;
					option4 = false;
					option6 = false;
					option7 = false;
					activeEffect = 0;
				}
				if (ImGui::Checkbox("Specular Only", &option3)) {
					option1 = false;
					option2 = false;
					option4 = false;
					option6 = false;
					option7 = false;
					activeEffect = 0;
				}
				if (ImGui::Checkbox("Ambient + Specular + Bloom", &option7)) {
					option1 = false;
					option2 = false;
					option3 = false;
					option6 = false;
					option4 = false;
					activeEffect = 1;
				}
				if (ImGui::Checkbox("Ambient + Specular + Bloom + Cel Shading", &option4)) {
					option1 = false;
					option2 = false;
					option3 = false;
					option6 = false;
					option7 = false;
					activeEffect = 1;
				}
				if (ImGui::Checkbox("Bloom + Cell Shading", &option6)) {
					option1 = false;
					option2 = false;
					option3 = false;
					option4 = false;
					option7 = false;
					activeEffect = 1;
				}
				if (ImGui::Checkbox("Textures On", &option5)) {
					shader->SetUniform("u_Option5", (int)option5);
				}
			}

			if (ImGui::CollapsingHeader("Other Stuff")) {
				if (ImGui::CollapsingHeader("Environment generation"))
				{
					if (ImGui::Button("Regenerate Environment", ImVec2(200.0f, 40.0f)))
					{
						EnvironmentGenerator::RegenerateEnvironment();
					}
				}
				if (ImGui::CollapsingHeader("Scene Level Lighting Settings"))
				{
					if (ImGui::ColorPicker3("Ambient Color", glm::value_ptr(ambientCol))) {
						shader->SetUniform("u_AmbientCol", ambientCol);
					}
					if (ImGui::SliderFloat("Fixed Ambient Power", &ambientPow, 0.01f, 1.0f)) {
						shader->SetUniform("u_AmbientStrength", ambientPow);
					}
				}
				if (ImGui::CollapsingHeader("Light Level Lighting Settings"))
				{
					if (ImGui::DragFloat3("Light Pos", glm::value_ptr(lightPos), 0.01f, -10.0f, 10.0f)) {
						shader->SetUniform("u_LightPos", lightPos);
					}
					if (ImGui::ColorPicker3("Light Col", glm::value_ptr(lightCol))) {
						shader->SetUniform("u_LightCol", lightCol);
					}
					if (ImGui::SliderFloat("Light Ambient Power", &lightAmbientPow, 0.0f, 1.0f)) {
						shader->SetUniform("u_AmbientLightStrength", lightAmbientPow);
					}
					if (ImGui::SliderFloat("Light Specular Power", &lightSpecularPow, 0.0f, 1.0f)) {
						shader->SetUniform("u_SpecularLightStrength", lightSpecularPow);
					}
					if (ImGui::DragFloat("Light Linear Falloff", &lightLinearFalloff, 0.01f, 0.0f, 1.0f)) {
						shader->SetUniform("u_LightAttenuationLinear", lightLinearFalloff);
					}
					if (ImGui::DragFloat("Light Quadratic Falloff", &lightQuadraticFalloff, 0.01f, 0.0f, 1.0f)) {
						shader->SetUniform("u_LightAttenuationQuadratic", lightQuadraticFalloff);
					}
				}
			}
			shader->SetUniform("u_Option1", (int)option1);
			shader->SetUniform("u_Option2", (int)option2);
			shader->SetUniform("u_Option3", (int)option3);
			shader->SetUniform("u_Option4", (int)option4);
			shader->SetUniform("u_Option6", (int)option6);
			shader->SetUniform("u_Option7", (int)option7);

			/*auto behaviour = BehaviourBinding::Get<SimpleMoveBehaviour>(controllables[selectedVao]);
			ImGui::Checkbox("Relative Rotation", &behaviour->Relative);*/

			ImGui::Text("Cosmic Cat");
		});

#pragma endregion
		// GL states
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glDepthFunc(GL_LEQUAL); // New

#pragma region TEXTURE LOADING
// Load some textures from files

		Texture2D::sptr grass = Texture2D::LoadFromFile("images/grassy.jpg");
		Texture2D::sptr noSpec = Texture2D::LoadFromFile("images/grassSpec.png");
		Texture2D::sptr wood = Texture2D::LoadFromFile("images/wodden.jpg");
		Texture2D::sptr building = Texture2D::LoadFromFile("images/tree12.png");
		Texture2D::sptr rock = Texture2D::LoadFromFile("images/rock1.png");
		Texture2D::sptr tree4 = Texture2D::LoadFromFile("images/tree4.png");
		Texture2D::sptr space = Texture2D::LoadFromFile("images/star.jpg");

		// Load the cube map
		TextureCubeMap::sptr environmentMap = TextureCubeMap::LoadFromImages("images/cubemaps/skybox/ToonSky.jpg");

		// Creating an empty texture
		Texture2DDescription desc = Texture2DDescription();
		desc.Width = 1;
		desc.Height = 1;
		desc.Format = InternalFormat::RGB8;
		Texture2D::sptr texture2 = Texture2D::Create(desc);
		// Clear it with a white colour
		texture2->Clear();
#pragma endregion

		///////////////////////////////////// Scene Generation //////////////////////////////////////////////////
#pragma region Scene Generation

// We need to tell our scene system what extra component types we want to support
		GameScene::RegisterComponentType<RendererComponent>();
		GameScene::RegisterComponentType<BehaviourBinding>();
		GameScene::RegisterComponentType<Camera>();

		// Create a scene, and set it to be the active scene in the application
		GameScene::sptr scene = GameScene::Create("test");
		Application::Instance().ActiveScene = scene;

		// We can create a group ahead of time to make iterating on the group faster
		entt::basic_group<entt::entity, entt::exclude_t<>, entt::get_t<Transform>, RendererComponent> renderGroup =
			scene->Registry().group<RendererComponent>(entt::get_t<Transform>());

		// Create a material and set some properties for it
		ShaderMaterial::sptr stoneMat = ShaderMaterial::Create();
		stoneMat->Shader = shader;
		stoneMat->Set("s_Diffuse", space);
		stoneMat->Set("s_Specular", noSpec);
		stoneMat->Set("u_Shininess", 2.0f);
		stoneMat->Set("u_TextureMix", 0.0f);

		ShaderMaterial::sptr grassMat = ShaderMaterial::Create();
		grassMat->Shader = shader;
		grassMat->Set("s_Diffuse", grass);
		grassMat->Set("s_Specular", noSpec);
		grassMat->Set("u_Shininess", 2.0f);
		grassMat->Set("u_TextureMix", 0.0f);

		ShaderMaterial::sptr woodMat = ShaderMaterial::Create();
		woodMat->Shader = shader;
		woodMat->Set("s_Diffuse", wood);
		woodMat->Set("s_Specular", noSpec);
		woodMat->Set("u_Shininess", 8.0f);
		woodMat->Set("u_TextureMix", 0.0f);

		ShaderMaterial::sptr spiderMat = ShaderMaterial::Create();
		spiderMat->Shader = shader;
		spiderMat->Set("s_Diffuse", building);
		spiderMat->Set("s_Specular", noSpec);
		spiderMat->Set("u_Shininess", 8.0f);
		spiderMat->Set("u_TextureMix", 0.0f);

		ShaderMaterial::sptr rockMat = ShaderMaterial::Create();
		rockMat->Shader = shader;
		rockMat->Set("s_Diffuse", rock);
		rockMat->Set("s_Specular", rock);
		rockMat->Set("u_Shininess", 8.0f);
		rockMat->Set("u_TextureMix", 0.0f);

		ShaderMaterial::sptr tree4Mat = ShaderMaterial::Create();
		tree4Mat->Shader = shader;
		tree4Mat->Set("s_Diffuse", tree4);
		tree4Mat->Set("s_Specular", tree4);
		tree4Mat->Set("u_Shininess", 8.0f);
		tree4Mat->Set("u_TextureMix", 0.0f);

		GameObject obj1 = scene->CreateEntity("Ground");
		{
			VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/plane.obj");
			obj1.emplace<RendererComponent>().SetMesh(vao).SetMaterial(grassMat);
			obj1.get<Transform>().SetLocalScale(2.10f, 2.10f, 2.10f);
		}

		GameObject obj2 = scene->CreateEntity("Cat");
		{
			VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/cat.obj");
			obj2.emplace<RendererComponent>().SetMesh(vao).SetMaterial(stoneMat);
			obj2.get<Transform>().SetLocalPosition(0.0f, -6.0f, 0.0f);
			obj2.get<Transform>().SetLocalRotation(-90.0f, 180.0f, 0.0f); //y is towards screen
			obj2.get<Transform>().SetLocalScale(0.18f, 0.18f, 0.18f);
			//BehaviourBinding::BindDisabled<SimpleMoveBehaviour>(obj2);
			auto pathing = BehaviourBinding::Bind<FollowPathBehaviour>(obj2);
			// Set up a path for the object to follow
			pathing->Points.push_back({ 0.0f, -7.0f, 0.0f });
			pathing->Points.push_back({ 0.0f, -8.0f, 0.0f });
			pathing->Speed = 0.5f;
		}

		GameObject obj4 = scene->CreateEntity("tree");
		{
			VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/tree 12.obj");
			obj4.emplace<RendererComponent>().SetMesh(vao).SetMaterial(spiderMat);
			obj4.get<Transform>().SetLocalPosition(-84.0f, 0.0f, 0.0f); //z height
			obj4.get<Transform>().SetLocalRotation(90.0f, 0.0f, 0.0f);
			obj4.get<Transform>().SetLocalScale(8.0f, 8.0f, 8.0f);
			BehaviourBinding::BindDisabled<SimpleMoveBehaviour>(obj4);
		}

		GameObject obj5 = scene->CreateEntity("rock");
		{
			VertexArrayObject::sptr vao = ObjLoader::LoadFromFile("models/rock1.obj");
			obj5.emplace<RendererComponent>().SetMesh(vao).SetMaterial(rockMat);
			obj5.get<Transform>().SetLocalPosition(-30.0f, -6.0f, 0.0f); //z height
			obj5.get<Transform>().SetLocalRotation(90.0f, 0.0f, 0.0f);
			obj5.get<Transform>().SetLocalScale(2.00f, 2.0f, 2.0f);
			//BehaviourBinding::BindDisabled<SimpleMoveBehaviour>(obj5);
			auto pathing = BehaviourBinding::Bind<FollowPathBehaviour>(obj5);
			// Set up a path for the object to follow
			pathing->Points.push_back({ -30.0f, -6.0f, 0.10f });
			pathing->Points.push_back({ -30.0f, -6.0f, 3.0f });
			pathing->Speed = 1.0f;
		}

		std::vector<glm::vec2> allAvoidAreasFrom = { glm::vec2(-10.0f, -10.0f) };
		std::vector<glm::vec2> allAvoidAreasTo = { glm::vec2(10.0f, 10.0f) };

		glm::vec2 spawnFromHere = glm::vec2(-40.0f, -40.0f);
		glm::vec2 spawnToHere = glm::vec2(40.0f, 40.0f);

		EnvironmentGenerator::AddObjectToGeneration("models/tree4.obj", tree4Mat, 135,
			spawnFromHere, spawnToHere, allAvoidAreasFrom, allAvoidAreasTo);

		EnvironmentGenerator::GenerateEnvironment();

		// Create an object to be our camera
		GameObject cameraObject = scene->CreateEntity("Camera");
		{
			cameraObject.get<Transform>().SetLocalPosition(0, 3, 3).LookAt(glm::vec3(0, 0, 0));

			// We'll make our camera a component of the camera object
			Camera& camera = cameraObject.emplace<Camera>();// Camera::Create();
			camera.SetPosition(glm::vec3(0, 3, 3));
			camera.SetUp(glm::vec3(0, 0, 1));
			camera.LookAt(glm::vec3(0));
			camera.SetFovDegrees(90.0f); // Set an initial FOV
			camera.SetOrthoHeight(3.0f);
			BehaviourBinding::Bind<CameraControlBehaviour>(cameraObject);
		}

		int width, height;
		glfwGetWindowSize(BackendHandler::window, &width, &height);

		GameObject framebufferObject = scene->CreateEntity("Basic Effect");
		{
			basicEffect = &framebufferObject.emplace<PostEffect>();
			basicEffect->Init(width, height);
		}
		effects.push_back(basicEffect);

		GameObject bloomEffectObject = scene->CreateEntity("Bloom Effect");
		{
			BloomEffect = &bloomEffectObject.emplace<Bloom>();
			BloomEffect->Init(width, height);
		}
		effects.push_back(BloomEffect);

#pragma endregion
		//////////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////// SKYBOX ///////////////////////////////////////////////
		{
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
			skyboxObj.get_or_emplace<RendererComponent>().SetMesh(meshVao).SetMaterial(skyboxMat);
		}
		////////////////////////////////////////////////////////////////////////////////////////

		// We'll use a vector to store all our key press events for now (this should probably be a behaviour eventually)
		std::vector<KeyPressWatcher> keyToggles;
		{
			// This is an example of a key press handling helper. Look at InputHelpers.h an .cpp to see
			// how this is implemented. Note that the ampersand here is capturing the variables within
			// the scope. If you wanted to do some method on the class, your best bet would be to give it a method and
			// use std::bind
			keyToggles.emplace_back(GLFW_KEY_T, [&]() { cameraObject.get<Camera>().ToggleOrtho(); });

			controllables.push_back(obj2);

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

		// Initialize our timing instance and grab a reference for our use
		Timing& time = Timing::Instance();
		time.LastFrame = glfwGetTime();

		///// Game loop /////
		while (!glfwWindowShouldClose(BackendHandler::window)) {
			glfwPollEvents();

			// Update the timing
			time.CurrentFrame = glfwGetTime();
			time.DeltaTime = static_cast<float>(time.CurrentFrame - time.LastFrame);

			time.DeltaTime = time.DeltaTime > 1.0f ? 1.0f : time.DeltaTime;

			// Update our FPS tracker data
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

			glClearColor(0.08f, 0.17f, 0.31f, 1.0f);
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

			basicEffect->BindBuffer(0);

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

			basicEffect->UnbindBuffer();

			effects[activeEffect]->ApplyEffect(basicEffect);

			effects[activeEffect]->DrawToScreen();

			// Draw our ImGui content
			BackendHandler::RenderImGui();

			scene->Poll();
			glfwSwapBuffers(BackendHandler::window);
			time.LastFrame = time.CurrentFrame;
		}

		// Nullify scene so that we can release references
		Application::Instance().ActiveScene = nullptr;
		//Clean up the environment generator so we can release references
		EnvironmentGenerator::CleanUpPointers();
		BackendHandler::ShutdownImGui();
	}

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}