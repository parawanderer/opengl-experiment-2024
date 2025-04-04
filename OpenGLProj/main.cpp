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
#include "ConfigConstants.h"
#include "EffectConstants.h"
#include "FileConstants.h"
#include "GameObjectConstants.h"
#include "MilitaryContainer.h"
#include "ModelConstants.h"
#include "NomadCharacter.h"
#include "OrnithopterCharacter.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PlayerInteractionManger.h"
#include "SandWormCharacter.h"
#include "SoundManager.h"
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


#define RENDER_DEBUG_OBJECTS false

constexpr auto SCREEN_OUTPUT_BUFFER_ID = 0;



#pragma region STATE

int currentWidth = INITIAL_WIDTH;
int currentHeight = INITIAL_HEIGHT;

WorldTimeManager timeMgr;

CameraManager camMgr(
	&timeMgr,
	true,
	glm::vec3(0.0f, 410.0f, 0.0f), // glm::vec3(690.0f, 62.0f, -399.0f), //
	glm::vec3(0.0f, 0.0f, -1.0f),
	INITIAL_WIDTH,
	INITIAL_HEIGHT,
	10.0f
);

constexpr float RENDER_DISTANCE = 2000.0f;

glm::vec3 sunPos(1024.0f, 750.0f, -2000.0f);
glm::vec3 sunLightColor = Colors::WHITE;

#pragma endregion


const float attenuationC1 = 0.5f; // max value is 1.0f
const float attenuationC2 = 0.25f; // max value is 1.0f
const float attenuationC3 = 0.25f; // max value is 1.0f


GLFWwindow* init();

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);



// temporary things for the course assignment specifically
// (my idea is to keep this toy project around for personal purposes later and these will be removed after the assignment)

void setupAttenuatedLightSpheres(Shader& targetShader, const float c1, const float c2, const float c3);

std::vector<glm::vec3> computeAttenuatedLightSpheresPos(Terrain& terrain, const float t);

void renderTerrain(Shader& terrainShader, Terrain& terrain, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, const std::vector<glm::vec3>& smallLightSpherePositions);



float lastButtonChoiceAt = 0.0f;
const float buttonCooldown = 0.2f;
// Assignment-only, not particularly great code, but it will make for a nice video :)
// this will be removed after the assignment demo video, which is why I didn't write a nicer dedicated class for this, unlike other project features.
void handleAssignmentOnlyPuppetController(const float t, GLFWwindow* window, SandWormCharacter& sandWormCharacter, NomadCharacter& nomadCharacter, const glm::vec3& cameraPos)
{
	
	if (lastButtonChoiceAt + buttonCooldown < t) // TODO: remove, all of this is temporary stuff for the assignment only
	{
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)
		{
			// trigger earthquake noise
			sandWormCharacter.startQuakingEarth();
			std::cout << "[PUPPETING] PRESS 0: QUAKE EARTH" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		{
			// make nomad run over and say his voice line
			nomadCharacter.runOverAndWhatIsThat(cameraPos, 2.5f);
			std::cout << "[PUPPETING] PRESS 9: Nomad 'what is that?!'" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		{
			// trigger apparance worm -> move towards us
			glm::vec3 directionalVectorTowardsNomad = nomadCharacter.getCurrentPosition() - sandWormCharacter.getCurrentPosition();
			directionalVectorTowardsNomad.y = 0; // notably this will get ignored anyways
			directionalVectorTowardsNomad = glm::normalize(directionalVectorTowardsNomad);
			// travel 1500.0f meters in the direction of the nomad.
			sandWormCharacter.appearAndMoveTowards(1500.0f * directionalVectorTowardsNomad + sandWormCharacter.getCurrentPosition());
			std::cout << "[PUPPETING] PRESS 8: WORM APPEARS!" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		{
			// nomad voice line
			nomadCharacter.reactToSandworm();
			std::cout << "[PUPPETING] PRESS 7: Nomad: 'holy mother of!'" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		{
			// nomad run away (from sandworm!)
			glm::vec3 directionalVectorTowardsNomad = nomadCharacter.getCurrentPosition() - sandWormCharacter.getCurrentPosition();
			directionalVectorTowardsNomad.y = 0; // notably this will get ignored anyways
			directionalVectorTowardsNomad = glm::normalize(directionalVectorTowardsNomad);
			// travel 1500.0f meters away from the direction of the sandworm
			nomadCharacter.runOverTo(1500.0f* directionalVectorTowardsNomad + nomadCharacter.getCurrentPosition(), 0.0f, true);
			std::cout << "[PUPPETING] PRESS 6: Nomad run away" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		{
			// nomad lookbehind while running
			nomadCharacter.playLookBackAnimWhileRunning();
			std::cout << "[PUPPETING] PRESS 5: Nomad look back" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		{
			// nomad fall, worm eats him, then worm disappear
			nomadCharacter.runStopByFallingAndStayDown();
			std::cout << "[PUPPETING] PRESS 4: Nomad fall over and get eaten" << std::endl;
		}

		lastButtonChoiceAt = t;
	}
}

int main()
{
	GLFWwindow* window = init();
	if (window == nullptr) return -1;

	SoundManager sound(AUDIO_BASE_PATH);

#pragma region SHADERS_AND_POSTPROCESSING
	Shader genericShader = Shader::fromFiles(SHADER_MESH_VERT, SHADER_MESH_FRAG);
	genericShader.use();
	genericShader.setVec3("lightPos", sunPos);
	genericShader.setVec3("light.ambient", sunLightColor * 0.5f);
	genericShader.setVec3("light.diffuse", sunLightColor * 1.0f);
	genericShader.setVec3("light.specular", sunLightColor * 1.0f);
	genericShader.setBool("doAnimate", false);

	Shader particlesShader = Shader::fromFiles(SHADER_PARTICLES_VERT, SHADER_PARTICLES_FRAG);

	Quad screen2Dquad = Quad();
	DistanceFieldPostProcessor distanceFieldPostProcessor(&screen2Dquad, currentWidth, currentHeight);
	distanceFieldPostProcessor.setOutlineSize(0.003f);
	distanceFieldPostProcessor.setOutlinePulsate(true);
#pragma endregion

#pragma region GAME_MODELS
	RenderableGameObject ornithopterObject(MODEL_ORNITHOPTER);
	AnimationSet ornithopterAnimations(MODEL_ORNITHOPTER, ornithopterObject.getObjectModel());

	Model thumperModel(MODEL_THUMPER); // reuse the model
	AnimationSet thumperAnimations(MODEL_THUMPER, &thumperModel);
	SphericalBoxedGameObject thumperObject1(&thumperModel, 0.4f);
	SphericalBoxedGameObject thumperObject2(&thumperModel, 0.4f);

	SphericalBoxedGameObject nomadObject(MODEL_NOMAD, 0.6f, glm::vec3(0.0, 1.45f, 0.0), 100.0f);
	AnimationSet nomadAnimations(MODEL_NOMAD, nomadObject.getObjectModel());

	RenderableGameObject sandWormObject(MODEL_SANDWORM);
	AnimationSet sandWormAnimations(MODEL_SANDWORM, sandWormObject.getObjectModel());

	Model containerSmallModel(MODEL_CONTAINER_SMALL);
	SphericalBoxedGameObject containerSObject1(&containerSmallModel, 0.6f, glm::vec3(0.0), 60.0f);
	SphericalBoxedGameObject containerSObject2(&containerSmallModel, 0.6f, glm::vec3(0.0), 60.0f);

	Model containerLargeModel(MODEL_CONTAINER_LARGE);
	RenderableGameObject containerLObject1(&containerLargeModel);
	RenderableGameObject containerLObject2(&containerLargeModel);
	RenderableGameObject containerLObject3(&containerLargeModel);

	// I'll use this as my second light source
	Sphere sphere(20, 20, 1.0f);
#pragma endregion

#pragma region TEXT
	Shader fontShader = Shader::fromFiles(SHADER_FONT_VERT, SHADER_FONT_FRAG);
	glm::mat4 textProjection = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight);
	fontShader.use();
	fontShader.setMat4("projection", textProjection);
	Font font(FONT_PLAY_REGULAR, &fontShader);
#pragma endregion

#pragma region SKYBOX
	Shader skyboxShader = Shader::fromFiles(SHADER_SKYBOX_VERT, SHADER_SKYBOX_FRAG);
	Skybox skybox(&skyboxShader, SKYBOX_FACES);
#pragma endregion

#pragma region SUN
	Shader lightCubeShader = Shader::fromFiles(SHADER_LIGHTSOURCE_VERT, SHADER_LIGHTSOURCE_FRAG);
	const glm::mat4 lightCubeModel = LIGHT_CUBE_MODEL(sunPos);
	Sun sun(&lightCubeShader, lightCubeModel, sunLightColor);
#pragma endregion

#pragma region TERRAIN
	Shader terrainShader = Shader::fromFiles(SHADER_TERRAIN_VERT, SHADER_TERRAIN_FRAG);
	Terrain sandTerrain(
		&terrainShader, 
		TERRAIN_HEIGHTMAP,
		TERRAIN_TEXTURE_PRIMARY,
		TERRAIN_TEXTURE_DARKER,
		TERRAIN_NORMAL_MAP,
		TERRAIN_Y_SCALE_MULTIPLIER,
		TERRAIN_Y_SHIFT,
		sunPos,
		sunLightColor
	);
	setupAttenuatedLightSpheres(terrainShader, attenuationC1, attenuationC2, attenuationC3);
	camMgr.setTerrain(&sandTerrain);
#pragma endregion

#pragma region PARTICLES
	ParticleSystem particles1(
		&timeMgr,
		TEXTURE_PARTICLE_DUST,
		sandTerrain.getWorldHeightVecFor(104, -106),
		PARTCILE_SANDWORMDUST_LIFETIME,
		PARTCILE_SANDWORMDUST_COUNT,
		PARTCILE_SANDWORMDUST_SPAWN_PER_FRAME,
		PARTCILE_SANDWORMDUST_SIZE_W_H
	);

	ParticleSystem particles2(
		&timeMgr,
		TEXTURE_PARTICLE_DUST,
		sandTerrain.getWorldHeightVecFor(55, -106),
		PARTCILE_SANDWORMDUST_LIFETIME,
		PARTCILE_SANDWORMDUST_COUNT,
		PARTCILE_SANDWORMDUST_SPAWN_PER_FRAME,
		PARTCILE_SANDWORMDUST_SIZE_W_H
	);

	std::vector<ParticleSystem*> particles = { &particles1, &particles2 };
#pragma endregion

#pragma region MODELTRANSFORMS
	containerLObject1.setModelTransform(CONTAINER_L1_MODEL(sandTerrain));
	containerLObject2.setModelTransform(CONTAINER_L2_MODEL(sandTerrain));
	containerLObject3.setModelTransform(CONTAINER_L3_MODEL(sandTerrain));
#pragma endregion

	UITextRenderer uiText(&timeMgr, camMgr.getPlayerCamera(), &font);

	// state
	PlayerState player = PlayerState(&sound, camMgr.getPlayerCamera());
	camMgr.getPlayerCamera()->addSubscriber(&player);

	// "characters"
	NomadCharacter nomadCharacter(&timeMgr, &sandTerrain, &sound, &uiText, &nomadObject, &nomadAnimations, 146.12f, -171.42f);
	SandWormCharacter sandWormCharacter(&timeMgr, &sandTerrain, &sound, &sandWormObject, &sandWormAnimations, &particles1, &particles2, 944.37f, -793.97f); //726.44f, -610.75f
	OrnithopterCharacter ornithopterCharacter(&timeMgr, &sound, &ornithopterObject, &ornithopterAnimations);

	// items player can pick up
	Thumper thumper1(&timeMgr, &sound, &thumperObject1, &thumperAnimations);
	thumper1.setPosition(sandTerrain.getWorldHeightVecFor(5.0f, 6.0f) + SMALL_OFFSET_Y);
	Thumper thumper2(&timeMgr, &sound, &thumperObject2, &thumperAnimations);
	thumper2.setPosition(sandTerrain.getWorldHeightVecFor(161.61f, -157.85f) + SMALL_OFFSET_Y);

	// containers the player could open in the future, maybe. They're all empty, though
	MilitaryContainer container1(&containerSObject1);
	container1.setModelTransform(CONTAINER_S1_MODEL(sandTerrain));
	MilitaryContainer container2(&containerSObject2);
	container2.setModelTransform(CONTAINER_S2_MODEL(sandTerrain));

	const std::vector<FrameRequester*> frameRequesters = {
		&nomadCharacter,
		&sandWormCharacter,
		&thumper1,
		&thumper2,
		&ornithopterCharacter,
		&particles1,
		&particles2
	}; 
	std::set<Thumper*> worldItemsThatPlayerCanPickUp = { &thumper1, &thumper2 };
	std::set<NomadCharacter*> charactersThatPlayerCanTalkTo = { &nomadCharacter };
	std::set<MilitaryContainer*> chestsThatPlayerCanOpen = { &container1, &container2 };
	std::vector<RenderableGameObject*> staticGameObjects = {
		container1.getObjectModel(),
		container2.getObjectModel(),
		&containerLObject1,
		&containerLObject2,
		&containerLObject3
	};
	std::vector<AnimatedEntity*> independentAnimatedEntities = { &nomadCharacter, &sandWormCharacter, &ornithopterCharacter };

	PlayerInteractionManger interactionManger(
		&timeMgr, 
		&camMgr, 
		window, 
		&sandTerrain, 
		&player, 
		0.2f, 
		&worldItemsThatPlayerCanPickUp,
		&charactersThatPlayerCanTalkTo,
		&chestsThatPlayerCanOpen
	);

	// ============ [ MAIN LOOP ] ============
	camMgr.beforeLoop();
	while (!glfwWindowShouldClose(window))
	{
		const float t = (float)glfwGetTime();
		processInput(window);

		timeMgr.onNewFrame();
		camMgr.processInput(window);
		uiText.setCurrentWidthHeight(currentWidth, currentHeight);

#pragma region PROJECTING
		const glm::vec3 cameraPos = camMgr.getPos();
		const glm::vec3 cameraFront = camMgr.getCurrentCamera()->getFront();
		const glm::mat4 view = camMgr.getCurrentCamera()->getView();
		const float fov = camMgr.getCurrentCamera()->getFov();
		const glm::mat4 projection = glm::perspective(glm::radians(fov),(float)currentWidth / (float)currentHeight,0.1f, RENDER_DISTANCE);
#pragma endregion

		sound.updateListenerPos(cameraPos, cameraFront);
		for (auto frameRequester : frameRequesters) frameRequester->onNewFrame();

#pragma region MOUSE_RAY_PICKING_AND_PLAYER_INTERACTIONS
		SphericalBoundingBoxedEntity* result = interactionManger.getMouseTarget();
		interactionManger.handleInteractionChecks(result);
		handleAssignmentOnlyPuppetController(t, window, sandWormCharacter, nomadCharacter, cameraPos);
#pragma endregion

#pragma region RENDERING
		glBindFramebuffer(GL_FRAMEBUFFER, SCREEN_OUTPUT_BUFFER_ID);
		glEnable(GL_DEPTH_TEST);
		if (USE_SRGB_COLORS) glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction for better looking colours! https://learnopengl.com/Advanced-Lighting/Gamma-Correction
		glClearColor(Colors::CUSTOM_BLUE.r, Colors::CUSTOM_BLUE.g, Colors::CUSTOM_BLUE.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// WORLD
		skybox.render(view, projection);

		std::vector<glm::vec3> smallLightSpherePositions = computeAttenuatedLightSpheresPos(sandTerrain, t);
		renderTerrain(terrainShader, sandTerrain, view, projection, cameraPos, smallLightSpherePositions);

		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		if (RENDER_DEBUG_OBJECTS) // render a "light cube" at the position of the sun (only really useful for debugging)
		{
			lightCubeShader.setMat4("model", lightCubeModel);
			sun.draw();
		}

		for (auto pos: smallLightSpherePositions)
		{
			// I'll make this interesting because having a diminishing with distance light source is part of the assignment
			// and I can't really think of anything super simple to implement that fits into the game world. So I'll just go with this.
			lightCubeShader.setMat4("model", glm::translate(glm::mat4(1.0f), pos));
			sphere.draw(lightCubeShader);
		}

		// MAIN MODELS
		genericShader.use();
		genericShader.setMat4("projection", projection);
		genericShader.setMat4("view", view);
		genericShader.setVec3("viewPos", cameraPos);

		// static models in the world (non-characters/interacteable items)
		for (auto staticObj: staticGameObjects) staticObj->draw(genericShader);

		// animated entities (not dynamic)
		for (auto entity : independentAnimatedEntities) entity->draw(genericShader);

		// dynamic world items (player can pick these up)
		for (auto thump : worldItemsThatPlayerCanPickUp) thump->draw(genericShader);

		if (player.hasCarriedItem()) // dynamic "in player hand" items
		{
			// (the perspective on this thing doesn't really make sense in the world)
			player.getCarriedItem().getObject()->drawCarried(
				genericShader,
				view, 
				t, 
				camMgr.getCurrentCamera()->isMoving(), 
				camMgr.getCurrentCamera()->isSpeeding()
			);
		}
#pragma endregion

#pragma region PARTICLES
		particlesShader.use();
		particlesShader.setMat4("projection", projection);
		particlesShader.setMat4("view", view);
		for (auto particle : particles) particle->draw(particlesShader, view, cameraPos);
#pragma endregion

#pragma region POST_PROCESSING
		fontShader.use();
		fontShader.setMat4("projection", textProjection);
		if (result != nullptr) {
			if (DrawableEntity* drawableEntity = dynamic_cast<DrawableEntity*>(result)) // render outline (generic for all objects)
			{
				distanceFieldPostProcessor.computeAndRenderOverlay(projection, view, { drawableEntity }, SCREEN_OUTPUT_BUFFER_ID);
			}

			uiText.renderOverlayForTargetItem(result);
		}

		if (player.hasCarriedItem()) uiText.renderCarriedItemInfo(THUMPER);

		uiText.processDialogueRequests();
		uiText.renderCurrentDialogue();
		uiText.renderMainUIOverlay(cameraPos);
		glCheckError();
#pragma endregion

		if (USE_SRGB_COLORS) glDisable(GL_FRAMEBUFFER_SRGB); // only the LAST step is allowed to apply gamma correction, otherwise our colours would get all messed-up: https://learnopengl.com/Advanced-Lighting/Gamma-Correction

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();

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
			terrain.getWorldHeightVecFor(-170, 40) + glm::vec3(sin(t) * 2.0f, 10.5 + sin(t) * 10.0f, cos(t) * 2.0f),
	};

	return smallLightSpherePositions;
}

void renderTerrain(Shader& terrainShader, Terrain& terrain, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos, const std::vector<glm::vec3>& smallLightSpherePositions)
{
	terrainShader.use();
	for (int i = 0; i < smallLightSpherePositions.size(); ++i)
	{
		terrainShader.setVec3("attLightPos[" + std::to_string(i) + "]", smallLightSpherePositions[i]);
	}
	terrain.render(view, projection, cameraPos);
}