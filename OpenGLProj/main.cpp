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

#include "Colors.h"
#include "ErrorUtils.h"
#include "Font.h"
#include "Shader.h"
#include "stb_image.h"
#include "Terrain.h"
#include "Camera.h"
#include "CameraManager.h"
#include "DistanceFieldPostProcessor.h"
#include "Quad.h"
#include "RenderableGameObject.h"
#include "Skybox.h"
#include "SphericalBoxedGameObject.h"
#include "Sun.h"
#include "WorldMathUtils.h"
#include "PlayerState.h"
#include "CarriedGameObject.h"

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

CameraManager camMgr(
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


void framebufferSizeCallback(GLFWwindow* window, int width, int height);

void mouseCallback(GLFWwindow* window, double xpos, double ypos);

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void processInput(GLFWwindow* window);

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods);


int main()
{
# pragma region INIT
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
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, INITIAL_WIDTH, INITIAL_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CW);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, processKey);

# pragma endregion

#pragma region SHADERS_AND_POSTPROCESSING
	Shader genericShader = Shader::fromFiles("mesh.vert", "mesh.frag");
	genericShader.use();
	genericShader.setVec3("light.position", sunPos);
	genericShader.setVec3("light.ambient", sunLightColor * 0.3f);
	genericShader.setVec3("light.diffuse", sunLightColor * 1.0f);
	genericShader.setVec3("light.specular", sunLightColor * 0.1f);

	Quad quad;
	DistanceFieldPostProcessor distanceFieldPostProcessor(
		&quad,
		currentWidth,
		currentHeight
	);
	distanceFieldPostProcessor.setOutlineSize(0.003f);

#pragma endregion

#pragma region GAME_MODELS
	

	// backpack
	//Model backpack("resources/models/backpack/backpack.obj");
	// glm::mat4 backpackModel = glm::mat4(1.0f);
	// backpackModel = glm::translate(backpackModel, glm::vec3(0.0f, 80.0f, 0.0f));
	// backpackModel = glm::scale(backpackModel, glm::vec3(1.0f, 1.0f, 1.0f));	

	// some models
	RenderableGameObject ornithopter("resources/models/dune-ornithopter/ornithopter_edit.dae");


	Model thumperModel("resources/models/thumper_dune/thumper_dune.dae"); // reuse the model
	SphericalBoxedGameObject thumper(&thumperModel, 0.4f);
	SphericalBoxedGameObject thumper2(&thumperModel, 0.4f);
	//thumper.setShowBoundingSphere(true);
	//thumper2.setShowBoundingSphere(true);

	RenderableGameObject nomad("resources/models/rust-nomad/RustNomad.fbx");

	RenderableGameObject sandWorm("resources/models/dune-sandworm/sandworm_edit.dae");
#pragma endregion

#pragma region TEXT
	Shader fontShader = Shader::fromFiles("font.vert", "font.frag");
	glm::mat4 textProjection = glm::ortho(0.0f, (float)currentWidth, 0.0f, (float)currentHeight);
	fontShader.use();
	fontShader.setMat4("projection", textProjection);
	Font font("resources/font/play/Play-Regular.ttf", &fontShader);
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
	// this is currently a cube which is not necessarily ideal
	Shader lightCubeShader = Shader::fromFiles("shader_lightsource.vert", "shader_lightsource.frag");
	glm::mat4 lightCubeModel = glm::mat4(1.0f);
	lightCubeModel = glm::translate(lightCubeModel, sunPos);
	lightCubeModel = glm::scale(lightCubeModel, glm::vec3(10.0f));
	Sun sun(&lightCubeShader, lightCubeModel, sunLightColor);
#pragma endregion

#pragma region TERRAIN
	const float yScaleMult = 192.0f;
	const float yShift = 32.0f;
	Shader terrainShader = Shader::fromFiles("terrain.vert", "terrain.frag");
	Terrain sandTerrain(
		&terrainShader, 
		"resources/terrain/heightmap/yetanothermap2.png",
		"resources/terrain/texture/sand_texture.jpg",
		"resources/terrain/texture/sand_texture2.jpg",
		"resources/terrain/texture/testtt.jpg",
		yScaleMult, 
		yShift,
		sunPos,
		sunLightColor
	);
	camMgr.setTerrain(&sandTerrain);
#pragma endregion

#pragma region MODELTRANSFORMS

	const glm::vec3 smallOffsetY = glm::vec3(0.0, 0.1, 0.0);


	glm::mat4 thumper1Model = glm::mat4(1.0f);
	thumper1Model = glm::translate(thumper1Model, sandTerrain.getWorldHeightVecFor(5.0f, 6.0f) + smallOffsetY);
	thumper1Model = glm::scale(thumper1Model, glm::vec3(0.7f));
	thumper.setModelTransform(thumper1Model);

	glm::mat4 thumper2Model = glm::mat4(1.0f);
	thumper2Model = glm::translate(thumper2Model, sandTerrain.getWorldHeightVecFor(9.0f, 10.0f) + smallOffsetY);
	thumper2Model = glm::scale(thumper2Model, glm::vec3(0.7f));
	thumper2.setModelTransform(thumper2Model);

	glm::mat4 nomadModel = glm::mat4(1.0f);
	nomadModel = glm::translate(nomadModel, sandTerrain.getWorldHeightVecFor(6.0f, 6.0f) + smallOffsetY);
	nomadModel = glm::rotate(nomadModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	nomad.setModelTransform(nomadModel);

	// droneModel = glm::translate(droneModel, glm::vec3(-24.0f, sandTerrain.getWorldHeightAt(-24.0f, 24.0f) + 2.0f, 24.0f));
	// droneModel = glm::scale(droneModel, glm::vec3(0.003f));

	// sandRocksModel = glm::translate(sandRocksModel, glm::vec3(980.0f, sandTerrain.getWorldHeightAt(980.0f, 15.0f) - 20.0f, 15.0f));
	// sandRocksModel = glm::scale(sandRocksModel, glm::vec3(0.25f, 0.5f, 0.25f));

	glm::mat4 sandWormModel = glm::mat4(1.0f);
	sandWormModel = glm::translate(sandWormModel, sandTerrain.getWorldHeightVecFor(80.0f, 50.0f));
	sandWormModel = glm::scale(sandWormModel, glm::vec3(3.0f));
	sandWorm.setModelTransform(sandWormModel);

#pragma endregion


	// state
	PlayerState player;

	std::set dynamicItems = { &thumper, &thumper2 };

	// ============ [ MAIN LOOP ] ============
	camMgr.beforeLoop();
	while (!glfwWindowShouldClose(window))
	{
		camMgr.onNewFrame();

		// ------ ** input ** ------
		processInput(window);
		camMgr.processInput(window);

		// ------ ** model/view/projection matrices ** ------
#pragma region PROJECTING
		const glm::vec3 cameraPos = camMgr.getPos();
		const glm::vec3 cameraFront = camMgr.getCurrentCamera()->getFront(); // direction
		const glm::mat4 view = camMgr.getCurrentCamera()->getView();
		const float fov = camMgr.getCurrentCamera()->getFov();

		const glm::mat4 projection = glm::perspective(
			glm::radians(fov),
			(float)currentWidth / (float)currentHeight,
			0.1f,
			RENDER_DISTANCE
		);
#pragma endregion

		// ------ ** mouse ray picking ** ------
#pragma region MOUSE_RAY_PICKING

		SphericalBoxedGameObject* result = nullptr;
		if (!player.hasCarriedItem()) {
			std::vector considered(dynamicItems.begin(), dynamicItems.end());
			result = WorldMathUtils::findClosestIntersection(
				considered,
				cameraPos,
				cameraFront,
				5.0f
			);
		}

#pragma endregion

#pragma region RENDERING

		glBindFramebuffer(GL_FRAMEBUFFER, SCREEN_OUTPUT_BUFFER_ID); // back to default (output to screen)
		glEnable(GL_DEPTH_TEST);
		// ------ ** clear previous image ** ------
		glClearColor(Colors::CUSTOM_BLUE.r, Colors::CUSTOM_BLUE.g, Colors::CUSTOM_BLUE.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCheckError();

		// ------ ** skybox ** ------
		skybox.render(view, projection);

		// ------ ** terrain ** ------
		sandTerrain.render(view, projection, cameraPos);

		// ------ ** light cube ** ------
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		sun.draw();

		// ------ ** models ** ------
		genericShader.use();
		genericShader.setMat4("projection", projection);
		genericShader.setMat4("view", view);
		genericShader.setVec3("viewPos", cameraPos);

		//backpack
		// genericShader.setMat4("model", backpackModel);
		// genericShader.setMat4("normalMatrix", backpackNormalMatrix);
		// backpack.draw(genericShader);

		// ornithopter
		float orniZDisplacement = sin(-glfwGetTime() / 3.0f) * 1500.f;
		float orniYDisplacement = (cos(glfwGetTime()) - 1) * 25.0f;
		float orniXDisplacement = sin(glfwGetTime()) * -10.f;
		glm::mat4 orniModel = glm::mat4(1.0f);
		orniModel = glm::translate(orniModel, glm::vec3(orniXDisplacement, 400.0f + orniYDisplacement, orniZDisplacement));
		orniModel = glm::rotate(orniModel, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		orniModel = glm::scale(orniModel, glm::vec3(2.0f));
		ornithopter.setModelTransform(orniModel);
		ornithopter.draw(genericShader);

		// nomad
		nomad.draw(genericShader);

		// sand worm
		sandWorm.draw(genericShader);

		// *dynamic items*
		// thumpers
		// thumper.draw(genericShader);
		// thumper2.draw(genericShader);
		for (auto thump : dynamicItems) // dynamic world items
		{
			thump->draw(genericShader);
		}

		if (player.hasCarriedItem()) // dynamic "in player hand" items
		{
			// (the perspective on this thing doesn't really make sense in the world)
			CarriedGameObject& item = player.getCarriedItem();
			glm::mat4 model = view;
			model = glm::inverse(model); // cancel view transformation (makes object stick to the camera)
			const float t = glfwGetTime();
			const float speedMultiplier = camMgr.getCurrentCamera()->isSpeeding() ? 1.5f : 1.0f;
			const float xWobble = camMgr.getCurrentCamera()->isMoving() ? (sin(t * 2.0f * M_PI * speedMultiplier) / 200.0f) * speedMultiplier : 0.0;
			const float yWobble = camMgr.getCurrentCamera()->isMoving() ? sin(t * 4.0f * M_PI * speedMultiplier) / 200.0f : 0.0;
			const float xRotate = camMgr.getCurrentCamera()->isMoving() ? (float)glm::radians(sin(t * 2.0f * M_PI * speedMultiplier) * 2.0f * speedMultiplier) : 0.0;
			model = glm::translate(model, glm::vec3(0.5f + xWobble, -0.4f + yWobble, -1.0f)); // move "forwards", "right" and "down"
			model = glm::scale(model, glm::vec3(0.7f)); // scale it down
			model = glm::rotate(model, -xRotate, glm::vec3(0.0f, 0.0f, 1.0f)); // wobble-related rotate
			model = glm::rotate(model, glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate to make position look better perspective-wise

			item.getModel()->setModelTransform(model);
			item.getModel()->draw(genericShader);
		}

		glCheckError();

		// ------ ** post-processing ** ------


		fontShader.use();
		fontShader.setMat4("projection", textProjection);

		// highlight selectable item
		if (result != nullptr) {
			distanceFieldPostProcessor.computeAndRenderOverlay(
				projection, 
				view, 
				{ result }, 
				SCREEN_OUTPUT_BUFFER_ID
			);

			// text overlay (inspired by bethesda games because it is simple to do)
			// obviously the font isn't really all that nice looking.
			// but I don't feel like fiddling with fonts/implementing distance-fields *again*,
			// but this time for fonts.
			font.renderText(
				"Thumper",
				(currentWidth * (5.0f / 8.0f)), 
				(currentHeight / 2.0f), 
				0.6f, 
				Colors::WHITE
			);

			font.renderText(
				"C) TAKE",
				(currentWidth * (5.0f / 8.0f)),
				(currentHeight / 2.0f) - 40.0f,
				0.5f,
				Colors::WHITE
			);
			// TODO: this should toggle an animation with sound effects, I'd say
			font.renderText(
				"E) ACTIVATE",
				(currentWidth * (5.0f / 8.0f)),
				(currentHeight / 2.0f) - 80.0f,
				0.5f,
				Colors::WHITE * 0.8f
			);

			if (!player.hasCarriedItem() && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // only pick up if "hands are free"
			{
				// player is picking this specific one up
				dynamicItems.erase(result);
				player.setCarriedItem(CarriedGameObject(result));
			}
		}

		if (player.hasCarriedItem())
		{
			font.renderText(
				"Thumper",
				(currentWidth - 150.0f),
				55.0f,
				0.45f,
				Colors::WHITE
			);

			font.renderText(
				"E) DROP",
				(currentWidth - 150.0f),
				30.0f,
				0.35f,
				Colors::WHITE
			);

			if (camMgr.isPlayer() && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			{
				// drop the item
				SphericalBoxedGameObject* model = player.getCarriedItem().getModel();
				player.removeCarriedItem();

				// update its position
				glm::mat4 newModel = glm::mat4(1.0f);

				newModel = glm::translate(newModel, sandTerrain.getWorldHeightVecFor(cameraPos.x + (cameraFront.x * 2.5f), cameraPos.z + (cameraFront.z * 2.5f)) + smallOffsetY);
				newModel = glm::scale(newModel, glm::vec3(0.7f));
				model->setModelTransform(newModel);

				// add it back to the dynamic items (for next loop)
				dynamicItems.insert(model);
			}
		}


		// ------ ** text overlay ** ------
		
		font.renderText(
			std::format("X:{:.2f} Y:{:.2f}, Z:{:.2f}", cameraPos.x, cameraPos.y, cameraPos.z),
			25.0f,
			currentHeight - 25.0f,
			0.5f,
			Colors::WHITE
		);

		// actually going to do a trick to get a center-of-the-screen "." indicator like in this game: https://youtu.be/6QZAhsxwNU0?si=J7eN6p2nRvc4Z_tW
		// mostly because I think it is useful/helpful for object-picking purposes
		font.renderText(".", currentWidth / 2.0f, currentHeight / 2.0f, 0.5f, Colors::WHITE);

		glCheckError();

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

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) // for "observer"
		camMgr.switchToNoClip();

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) // for "player"
		camMgr.switchToPlayer();
}

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	camMgr.processKey(window, key, scancode, action, mods);
}
