#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define _USE_MATH_DEFINES
#include "math.h"

#define STB_IMAGE_IMPLEMENTATION

#include <filesystem>
#include <set>
#include <GL/gl.h>

#include "Animation.h"
#include "Animator.h"
#include "Colors.h"
#include "ErrorUtils.h"
#include "Font.h"
#include "UITextRenderer.h"
#include "Shader.h"
#include "stb_image.h"
#include "Terrain.h"
#include "Camera.h"
#include "CameraManager.h"
#include "CameraUtils.h"
#include "DistanceFieldPostProcessor.h"
#include "Quad.h"
#include "RenderableGameObject.h"
#include "Skybox.h"
#include "SphericalBoxedGameObject.h"
#include "Sun.h"
#include "WorldMathUtils.h"
#include "PlayerState.h"
#include "CarriedGameObject.h"
#include "GameObjectConstants.h"
#include "NomadCharacter.h"
#include "SandWormCharacter.h"
#include "Thumper.h"
#include "WorldTimeManager.h"

// keeping this at a power of two to support the outline-rendering JFA algorithm.
// I could make this not a power of two but then I need to perform some annoying buffer size remappings during the JFA algo.
// (take the not power of two, not n x n width and height, insert the generated image into a buffer of n x n with n a
// power of two such that n is the next 2^m with ceil(max(width, height)) = m. Then I would execute the JFA algo in this buffer.
// a question would be if I should upscale or downscale the buffer, and how bad of an effect would we see if e.g. downscaling
// to the previous power of two (2^(m-1)), and then upscaling the generated outline linearly.
// But regardless my computer can handle rendering the current implementation just fine.
// The main problem is that JFA does not generate nice outlines in non power of two. I think the original authors of the JFA
// paper address exactly this (https://www.comp.nus.edu.sg/~tants/jfa/i3d06.pdf) with their generality principle.
// I have also seen this post: https://computergraphics.stackexchange.com/a/2119 but it does not appear to be practically accurate.
// I'm not sure if that is an implementation issue on my behalf or not, as all examples of JFA depth-field outlines that I have seen
// work with powers of two
//
// EDIT: after some testing with my current setup I have found that while the outline lines are jagged when observed too closely
// it doesn't really stand out THAT much, so I'm gonna go back to a resolution that looks prettier for now.
// Above note still stands regarding how to fix the jaggedness.
#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 800

constexpr auto SCREEN_OUTPUT_BUFFER_ID = 0;

#pragma region STATE

int currentWidth = INITIAL_WIDTH;
int currentHeight = INITIAL_HEIGHT;

WorldTimeManager timeMgr;

CameraManager camMgr(
	&timeMgr,
	true,
	glm::vec3(0.0f, 410.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	INITIAL_WIDTH,
	INITIAL_HEIGHT,
	10.0f
);

const float RENDER_DISTANCE = 1500.0f;

// world:

// sun
glm::vec3 sunPos(1024.0f, 750.0f, -2000.0f);
glm::vec3 sunLightColor = Colors::WHITE;

#pragma endregion

GLFWwindow* init();

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);

glm::mat4 computeOrnithroperModelTransform(const float t);


// temporary things for the course assignment specifically

void setupAttenuatedLightSpheres(Shader& targetShader, const float c1, const float c2, const float c3);

std::vector<glm::vec3> computeAttenuatedLightSpheresPos(Terrain& terrain, const float t);

void renderTerrain(Shader& terrainShader, Terrain& terrain, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, const std::vector<glm::vec3>& smallLightSpherePositions);


int main()
{
	GLFWwindow* window = init();
	if (window == nullptr) return -1;

#pragma region SHADERS_AND_POSTPROCESSING
	Shader genericShader = Shader::fromFiles("mesh.vert", "mesh.frag");
	genericShader.use();
	genericShader.setVec3("light.position", sunPos);
	genericShader.setVec3("light.ambient", sunLightColor * 0.5f);
	genericShader.setVec3("light.diffuse", sunLightColor * 1.0f);
	genericShader.setVec3("light.specular", sunLightColor * 1.0f);
	genericShader.setBool("doAnimate", false);

	Quad quad;
	DistanceFieldPostProcessor distanceFieldPostProcessor(&quad, currentWidth, currentHeight);
	distanceFieldPostProcessor.setOutlineSize(0.003f);
	distanceFieldPostProcessor.setOutlinePulsate(true);

#pragma endregion

#pragma region GAME_MODELS
	// backpack
	//Model backpack("resources/models/backpack/backpack.obj");
	// glm::mat4 backpackModel = glm::mat4(1.0f);
	// backpackModel = glm::translate(backpackModel, glm::vec3(0.0f, 80.0f, 0.0f));
	// backpackModel = glm::scale(backpackModel, glm::vec3(1.0f, 1.0f, 1.0f));	

	RenderableGameObject ornithopter("resources/models/dune-ornithopter/ornithopter_edit.dae");

	Model thumperModel("resources/models/thumper_dune/Thumper.fbx"); // reuse the model
	AnimationManager thumperAnimations("resources/models/thumper_dune/Thumper.fbx", &thumperModel);
	SphericalBoxedGameObject thumperObj1(&thumperModel, 0.4f);
	SphericalBoxedGameObject thumperObj2(&thumperModel, 0.4f);

	RenderableGameObject nomad("resources/models/rust-nomad/RustNomad.fbx");
	AnimationManager nomadAnimations("resources/models/rust-nomad/RustNomad.fbx", nomad.getObjectModel());

	RenderableGameObject sandWorm("resources/models/sandworm2/Sandworm.fbx");
	AnimationManager sandWormAnimations("resources/models/sandworm2/Sandworm.fbx", sandWorm.getObjectModel());

	Model containerSmall("resources/models/military-container-free/Military_Container.dae");
	RenderableGameObject containerS1(&containerSmall);
	RenderableGameObject containerS2(&containerSmall);

	Model containerLarge("resources/models/cargo-container/Container.dae");
	RenderableGameObject containerL1(&containerLarge);
	RenderableGameObject containerL2(&containerLarge);
	RenderableGameObject containerL3(&containerLarge);


	// I'll use this as my second light source
	Sphere sphere(20, 20, 1.0f);
	const float attenuationC1 = 0.5f; // max value is 1.0f
	const float attenuationC2 = 0.25f; // max value is 1.0f
	const float attenuationC3 = 0.25f; // max value is 1.0f

#pragma endregion

#pragma region TEXT
	Shader fontShader = Shader::fromFiles("font.vert", "font.frag");
	glm::mat4 textProjection = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight);
	fontShader.use();
	fontShader.setMat4("projection", textProjection);
	Font font("resources/font/play/Play-Regular.ttf", &fontShader);
	UITextRenderer uiText(&font);
#pragma endregion

#pragma region SKYBOX
	std::vector<std::string> skyboxFaces{
		"resources/skybox2/right.jpg",
		"resources/skybox2/left.jpg",
		"resources/skybox2/top.jpg",
		"resources/skybox2/bottom.jpg",
		"resources/skybox2/front.jpg",
		"resources/skybox2/back.jpg",
	};
	Shader skyboxShader = Shader::fromFiles("skybox.vert", "skybox.frag");
	Skybox skybox(&skyboxShader, skyboxFaces);
#pragma endregion

#pragma region SUN
	// this is currently a cube which is not necessarily ideal (however it's far out of range to see when spawning in the map)
	Shader lightCubeShader = Shader::fromFiles("shader_lightsource.vert", "shader_lightsource.frag");
	glm::mat4 lightCubeModel = glm::mat4(1.0f);
	lightCubeModel = glm::translate(lightCubeModel, sunPos);
	lightCubeModel = glm::scale(lightCubeModel, glm::vec3(10.0f));
	Sun sun(&lightCubeShader, lightCubeModel, sunLightColor);
#pragma endregion

#pragma region TERRAIN
	Shader terrainShader = Shader::fromFiles("terrain.vert", "terrain.frag");
	Terrain sandTerrain(
		&terrainShader, 
		"resources/terrain/heightmap/yetanothermap2.png",
		"resources/terrain/texture/sand_texture.jpg",
		"resources/terrain/texture/sand_texture2.jpg",
		"resources/terrain/texture/testtt.jpg",
		192.0f,
		32.0f,
		sunPos,
		sunLightColor
	);
	setupAttenuatedLightSpheres(terrainShader, attenuationC1, attenuationC2, attenuationC3);
	camMgr.setTerrain(&sandTerrain);
#pragma endregion

#pragma region MODELTRANSFORMS

	const glm::vec3 smallOffsetY = glm::vec3(0.0, 0.1, 0.0);

	glm::mat4 thumper1Model = glm::mat4(1.0f);
	thumper1Model = glm::translate(thumper1Model, sandTerrain.getWorldHeightVecFor(5.0f, 6.0f) + smallOffsetY);

	glm::mat4 thumper2Model = glm::mat4(1.0f);
	thumper2Model = glm::translate(thumper2Model, sandTerrain.getWorldHeightVecFor(9.0f, 10.0f) + smallOffsetY);

	glm::mat4 containerS1Model = glm::mat4(1.0f);
	containerS1Model = glm::translate(containerS1Model, sandTerrain.getWorldHeightVecFor(150.0f, -150.0f) + glm::vec3(0.0, -0.1f, 0.0f));
	containerS1Model = glm::rotate(containerS1Model, glm::radians(4.0f), glm::vec3(0.0, 0.0, 1.0));
	containerS1Model = glm::scale(containerS1Model, glm::vec3(0.01f));
	containerS1.setModelTransform(containerS1Model);

	glm::mat4 containerS2Model = glm::mat4(1.0f);
	containerS2Model = glm::translate(containerS2Model, sandTerrain.getWorldHeightVecFor(155.0, -155.0f) + glm::vec3(0.0, -0.1f, 0.0f));
	containerS2Model = glm::rotate(containerS2Model, glm::radians(-32.0f), glm::vec3(0.0, 0.0, 1.0));
	containerS2Model = glm::scale(containerS2Model, glm::vec3(0.01f));
	containerS2.setModelTransform(containerS2Model);

	glm::mat4 containerL1Model = glm::mat4(1.0f);
	containerL1Model = glm::translate(containerL1Model, sandTerrain.getWorldHeightVecFor(160.0f, -160.0f) + glm::vec3(0.0, -1.8f, 0.0f));
	containerL1Model = glm::rotate(containerL1Model, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
	containerL1Model = glm::rotate(containerL1Model, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0));
	containerL1Model = glm::scale(containerL1Model, glm::vec3(0.02f));
	containerL1.setModelTransform(containerL1Model);

	glm::mat4 containerL2Model = glm::mat4(1.0f);
	containerL2Model = glm::translate(containerL2Model, sandTerrain.getWorldHeightVecFor(182.0f, -175.0f) + glm::vec3(0.0, -2.0f, 0.0f));
	containerL2Model = glm::rotate(containerL2Model, glm::radians(74.0f), glm::vec3(1.0, 0.0, 0.0));
	containerL2Model = glm::rotate(containerL2Model, glm::radians(112.0f), glm::vec3(0.0, 1.0, 0.0));
	containerL2Model = glm::rotate(containerL2Model, glm::radians(-12.0f), glm::vec3(0.0, 0.0, 1.0));
	containerL2Model = glm::scale(containerL2Model, glm::vec3(0.02f));
	containerL2.setModelTransform(containerL2Model);

	glm::mat4 containerL3Model = glm::mat4(1.0f);
	containerL3Model = glm::translate(containerL3Model, sandTerrain.getWorldHeightVecFor(182, -227) + glm::vec3(0.0, -1.5f, 0.0f));
	containerL3Model = glm::rotate(containerL3Model, glm::radians(-21.0f), glm::vec3(1.0, 0.0, 0.0));
	containerL3Model = glm::rotate(containerL3Model, glm::radians(-3.0f), glm::vec3(0.0, 1.0, 0.0));
	containerL3Model = glm::rotate(containerL3Model, glm::radians(15.0f), glm::vec3(0.0, 0.0, 1.0));
	containerL3Model = glm::scale(containerL3Model, glm::vec3(0.02f));
	containerL3.setModelTransform(containerL3Model);

#pragma endregion

	// state
	PlayerState player;

	NomadCharacter nomadCharacter(&timeMgr, &sandTerrain, &nomad, &nomadAnimations, 20.0f, -20.0f);
	SandWormCharacter sandWormCharacter(&timeMgr, &sandTerrain, &sandWorm, &sandWormAnimations, 80.0f, 50.0f);

	Thumper thumper1(&timeMgr, &thumperObj1, &thumperAnimations);
	thumper1.setModelTransform(thumper1Model);
	Thumper thumper2(&timeMgr, &thumperObj2, &thumperAnimations);
	thumper2.setModelTransform(thumper2Model);


	std::set<Thumper*> worldItemsThatPlayerCanPickUp = { &thumper1, &thumper2 };
	std::vector<RenderableGameObject*> staticGameObjects = { &containerS1, &containerS2, &containerL1, &containerL2, &containerL3 };
	std::vector<AnimatedEntity*> animatedEntitiesWithFrames = { &nomadCharacter, &sandWormCharacter, &thumper1, &thumper2 };
	std::vector<AnimatedEntity*> independentAnimatedEntities = { &nomadCharacter, &sandWormCharacter };


	const float worldInteractionCooldownSecs = 0.2f;
	float lastInteractionAt = 0.0f;

	// ============ [ MAIN LOOP ] ============
	camMgr.beforeLoop();
	while (!glfwWindowShouldClose(window))
	{
		const float t = (float)glfwGetTime();
		processInput(window);
		timeMgr.onNewFrame();
		camMgr.processInput(window);

		for (auto animatedEntity : animatedEntitiesWithFrames) animatedEntity->onNewFrame();

#pragma region PROJECTING
		const glm::vec3 cameraPos = camMgr.getPos();
		const glm::vec3 cameraFront = camMgr.getCurrentCamera()->getFront(); // direction
		const glm::mat4 view = camMgr.getCurrentCamera()->getView();
		const float fov = camMgr.getCurrentCamera()->getFov();
		const glm::mat4 projection = glm::perspective(glm::radians(fov),(float)currentWidth / (float)currentHeight,0.1f, RENDER_DISTANCE);
#pragma endregion

#pragma region MOUSE_RAY_PICKING
		Thumper* result = nullptr;
		if (!player.hasCarriedItem()) {
			std::vector<SphericalBoundingBoxedEntity*> considered(worldItemsThatPlayerCanPickUp.begin(), worldItemsThatPlayerCanPickUp.end());
			result = (Thumper*) WorldMathUtils::findClosestIntersection(considered, cameraPos, cameraFront,5.0f); // TODO: if I add more items later, then this type cast is not valid
		}
#pragma endregion

#pragma region PLAYER_INTERACTIONS
		bool interactionCooldownPassed = t - lastInteractionAt > worldInteractionCooldownSecs;
		if (interactionCooldownPassed && result != nullptr && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			switch(result->getState())
			{
			case Thumper::STATE::DISABLED:
				result->setState(Thumper::STATE::ACTIVATED); // start animating!
				break;
			case Thumper::STATE::ACTIVATED:
				result->setState(Thumper::STATE::DISABLED);
				break;
			}
			lastInteractionAt = t;
			interactionCooldownPassed = false;
		}

		if (interactionCooldownPassed && !player.hasCarriedItem() && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // pick up the item
		{
			result->setState(Thumper::STATE::DISABLED); // disable when picking up
			worldItemsThatPlayerCanPickUp.erase(result);
			player.setCarriedItem(CarriedGameObject(result));
			lastInteractionAt = t;
			interactionCooldownPassed = false;
		}

		if (interactionCooldownPassed && camMgr.isPlayerCamera() && player.hasCarriedItem() && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) // drop the item
		{
			// drop the item
			Thumper* thump = player.removeCarriedItem().getObject();
			glm::mat4 newModel = glm::mat4(1.0f);
			// translate position just comes out to a nice "in front of the player" position
			newModel = glm::translate(newModel, sandTerrain.getWorldHeightVecFor(cameraPos.x + (cameraFront.x * 2.5f), cameraPos.z + (cameraFront.z * 2.5f)) + smallOffsetY);
			thump->setModelTransform(newModel);
			worldItemsThatPlayerCanPickUp.insert(thump);
			lastInteractionAt = t;
			interactionCooldownPassed = false;
		}
#pragma endregion

#pragma region RENDERING
		glBindFramebuffer(GL_FRAMEBUFFER, SCREEN_OUTPUT_BUFFER_ID);
		glEnable(GL_DEPTH_TEST);
		glClearColor(Colors::CUSTOM_BLUE.r, Colors::CUSTOM_BLUE.g, Colors::CUSTOM_BLUE.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// WORLD
		skybox.render(view, projection);

		std::vector<glm::vec3> smallLightSpherePositions = computeAttenuatedLightSpheresPos(sandTerrain, t);
		renderTerrain(terrainShader, sandTerrain, view, projection, cameraPos, smallLightSpherePositions);

		// ------ ** light cube ** ------
		lightCubeShader.use();
		lightCubeShader.setMat4("model", lightCubeModel);
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		sun.draw();

		// I'll make this interesting because having a diminishing with distance light source is part of the assignment
		// and I can't really think of anything super simple to implement that fits into the game world. So I'll just go with this.
		for (auto pos: smallLightSpherePositions)
		{
			lightCubeShader.setMat4("model", glm::translate(glm::mat4(1.0f), pos));
			sphere.draw(lightCubeShader);
		}

		// ------ ** models ** ------
		genericShader.use();
		genericShader.setMat4("projection", projection);
		genericShader.setMat4("view", view);
		genericShader.setVec3("viewPos", cameraPos);

		//backpack
		// genericShader.setMat4("model", backpackModel);
		// genericShader.setMat4("normalMatrix", backpackNormalMatrix);
		// backpack.draw(genericShader);

		ornithopter.setModelTransform(computeOrnithroperModelTransform(t));
		ornithopter.draw(genericShader);

		// static models in the world (non-characters/interacteable items)
		for (auto staticObj: staticGameObjects) staticObj->draw(genericShader);

		// animated entities (not dynamic)
		for (auto entity : independentAnimatedEntities) entity->draw(genericShader);

		// dynamic world items (player can pick these up)
		for (auto thump : worldItemsThatPlayerCanPickUp) thump->draw(genericShader);

		if (player.hasCarriedItem()) // dynamic "in player hand" items
		{
			// (the perspective on this thing doesn't really make sense in the world)
			glm::mat4 model = CameraUtils::getCarriedItemModelTransform(view, t, camMgr.getCurrentCamera()->isMoving(), camMgr.getCurrentCamera()->isSpeeding());
			Thumper* item = player.getCarriedItem().getObject();
			item->setModelTransform(model);
			item->draw(genericShader);
		}

#pragma region POST_PROCESSING
		fontShader.use();
		fontShader.setMat4("projection", textProjection);

		if (result != nullptr) {
			distanceFieldPostProcessor.computeAndRenderOverlay(projection, view, { result }, SCREEN_OUTPUT_BUFFER_ID);
			uiText.renderItemInteractOverlay(THUMPER, currentWidth, currentHeight, result->getState() == Thumper::STATE::ACTIVATED); // highlight selectable item
		}

		if (player.hasCarriedItem())
		{
			uiText.renderCarriedItemInfo(THUMPER, currentWidth, currentHeight);
		}

		uiText.renderMainUIOverlay(cameraPos, currentWidth, currentHeight);
		glCheckError();
#pragma endregion

#pragma endregion

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

# pragma region CLEANUP
	glfwTerminate();
#pragma endregion

	return 0;
}

GLFWwindow* init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// only for debugging:
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	GLFWwindow* window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "ComputerGraphics Proj :)", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW	window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}

	glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CW);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, processKey);

	return window;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	currentWidth = width;
	currentHeight = height;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	camMgr.mouseCallback(window, xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camMgr.scrollCallback(window, xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	camMgr.processKey(window, key, scancode, action, mods);
}


glm::mat4 computeOrnithroperModelTransform(const float t)
{
	// ornithopter "fly effect" (it's not the best animation but it's interesting as a placeholder)
	glm::mat4 orniModel = glm::mat4(1.0f);
	orniModel = glm::translate(orniModel, glm::vec3(sin(t) * -10.f, 400.0f + ((cos(t) - 1) * 25.0f), sin(-t / 3.0f) * 1500.f));
	orniModel = glm::rotate(orniModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	orniModel = glm::scale(orniModel, glm::vec3(2.0f));
	return orniModel;
}


void setupAttenuatedLightSpheres(Shader& targetShader, const float c1, const float c2, const float c3)
{
	for (int i = 0; i < 3; ++i)
	{
		targetShader.setVec3("attLights[" + std::to_string(i) + "].ambient", Colors::WHITE * 0.00f);
		targetShader.setVec3("attLights[" + std::to_string(i) + "].diffuse", Colors::WHITE * 1.0f);
		targetShader.setVec3("attLights[" + std::to_string(i) + "].specular", Colors::WHITE * 1.0f);
		targetShader.setFloat("attConsts[" + std::to_string(i) + "].c1", c1);
		targetShader.setFloat("attConsts[" + std::to_string(i) + "].c2", c2);
		targetShader.setFloat("attConsts[" + std::to_string(i) + "].c3", c3);
	}
	targetShader.setInt("numAttLights", 3);
}

std::vector<glm::vec3> computeAttenuatedLightSpheresPos(Terrain& terrain, const float t)
{
	std::vector<glm::vec3> smallLightSpherePositions = { // MAX size = 4 (as per terrain.frag)
			terrain.getWorldHeightVecFor(-160, 50) + glm::vec3(sin(t) * 10.0f, 7.0 + cos(t / 2) * 3.0f, cos(t) * 10.0f),
			terrain.getWorldHeightVecFor(-170, 60) + glm::vec3(sin(t) * 80.0f, 6.0 + sin(t) * 3.0f, cos(t) * 3.0f),
			terrain.getWorldHeightVecFor(-170, 40) + glm::vec3(sin(t) * 2.0f, 6.0 + sin(t) * 10.0f, cos(t) * 2.0f),
	};

	return smallLightSpherePositions;
}

void renderTerrain(Shader& terrainShader, Terrain& terrain, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, const std::vector<glm::vec3>& smallLightSpherePositions)
{
	terrainShader.use();
	for (int i = 0; i < smallLightSpherePositions.size(); ++i)
	{
		terrainShader.setVec3("attLights[" + std::to_string(i) + "].position", smallLightSpherePositions[i]);
	}
	terrain.render(view, projection, cameraPos);
}