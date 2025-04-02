#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION

#include <GL/gl.h>

#include "Colors.h"
#include "ErrorUtils.h"
#include "Font.h"
#include "Shader.h"
#include "stb_image.h"
#include "Terrain.h"
#include "Camera.h"
#include "CameraManager.h"
#include "RenderableGameObject.h"
#include "Skybox.h"
#include "SphericalBoxedGameObject.h"
#include "WorldMathUtils.h"

#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 800


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

	Shader genericShader = Shader::fromFiles("mesh.vert", "mesh.frag");
	genericShader.use();
	genericShader.setVec3("light.position", sunPos); // glm::vec3(-19, 160, 19)
	genericShader.setVec3("light.ambient", sunLightColor * 0.3f);
	genericShader.setVec3("light.diffuse", sunLightColor * 1.0f);
	genericShader.setVec3("light.specular", sunLightColor * 0.1f);

	// backpack
	//Model backpack("resources/models/backpack/backpack.obj");
	// glm::mat4 backpackModel = glm::mat4(1.0f);
	// backpackModel = glm::translate(backpackModel, glm::vec3(0.0f, 80.0f, 0.0f));
	// backpackModel = glm::scale(backpackModel, glm::vec3(1.0f, 1.0f, 1.0f));	

	// some models
	RenderableGameObject ornithopter("resources/models/dune-ornithopter/ornithopter_edit.dae");


	Model thumperModel("resources/models/thumper_dune/thumper_dune.dae");
	SphericalBoxedGameObject thumper(&thumperModel, 1.0f);
	SphericalBoxedGameObject thumper2(&thumperModel, 1.0f);
	thumper.setShowBoundingSphere(true);
	thumper2.setShowBoundingSphere(true);
	//Model thumper("resources/models/thumper_dune/thumper_dune.dae");
	//glm::mat4 thumperModel = glm::mat4(1.0f);


	RenderableGameObject nomad("resources/models/rust-nomad/RustNomad.fbx");

	// Model drone("resources/models/scifi-drone-11/drone.fbx");
	// glm::mat4 droneModel = glm::mat4(1.0f);

	// Model sandRocks("resources/models/sand-rock-pack-08/Mesher2.dae");
	// glm::mat4 sandRocksModel = glm::mat4(1.0f);

	RenderableGameObject sandWorm("resources/models/dune-sandworm/sandworm_edit.dae");

#pragma region TEXT
	Shader fontShader = Shader::fromFiles("font.vert", "font.frag");
	glm::mat4 textProjection = glm::ortho(0.0f, (float)INITIAL_WIDTH, 0.0f, (float)INITIAL_HEIGHT);
	fontShader.use();
	fontShader.setMat4("projection", textProjection);
	Font font("resources/calibri.ttf", &fontShader);
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
	Shader lightCubeShader = Shader::fromFiles("shader_lightsource.vert", "shader_lightsource.frag");
	glm::mat4 lightCubeModel = glm::mat4(1.0f);
	lightCubeModel = glm::translate(lightCubeModel, sunPos);
	lightCubeModel = glm::scale(lightCubeModel, glm::vec3(10.0f));
	lightCubeShader.use();
	lightCubeShader.setMat4("model", lightCubeModel);
	lightCubeShader.setVec3("lightColor", sunLightColor);
	float lightCubeVerts[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};
	unsigned int VBO;
	unsigned int lightCubeVAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightCubeVerts), lightCubeVerts, GL_STATIC_DRAW);
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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

	//glm::mat3 backpackNormalMatrix = glm::mat3(glm::transpose(glm::inverse(backpackModel)));

	thumper.setModelTransform(
		glm::translate(glm::mat4(1.0f), sandTerrain.getWorldHeightVecFor(5.0f, 6.0f) + glm::vec3(0.0, 0.1, 0.0))
	);

	thumper2.setModelTransform(
		glm::translate(glm::mat4(1.0f), sandTerrain.getWorldHeightVecFor(9.0f, 10.0f) + glm::vec3(0.0, 0.1, 0.0))
	);

	glm::mat4 nomadModel = glm::mat4(1.0f);
	nomadModel = glm::translate(nomadModel, sandTerrain.getWorldHeightVecFor(-20.0f, 15.0f) + glm::vec3(0.0, 0.1, 0.0));
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

# pragma region MAIN_LOOP

	camMgr.beforeLoop();
	while(!glfwWindowShouldClose(window))
	{
		// ** view/projection transformations **
		camMgr.onNewFrame();

		// ** input **
		processInput(window);
		camMgr.processInput(window);

		const glm::vec3 cameraPos = camMgr.getPos();
		const glm::mat4 view = camMgr.getCurrentCamera()->getView();
		const float fov = camMgr.getCurrentCamera()->getFov();

		// ** rendering **
		glClearColor(0.45f, 0.49f, 0.61f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		const glm::mat4 projection = glm::perspective(
			glm::radians(fov),
			(float)INITIAL_WIDTH / (float)INITIAL_HEIGHT, 
			0.1f, 
			RENDER_DISTANCE
		);

		// ** mouse picking **
		const SphericalBoxedGameObject* result = WorldMathUtils::findClosestIntersection(
			{ &thumper, &thumper2 }, 
			cameraPos, 
			camMgr.getCurrentCamera()->getFront()
		);

		if (result != nullptr)
		{
			std::cout << "now pointing at: " << (result == &thumper ? "thumper 1" : "tumper2") << "\n";
		}


		// ======== RENDERING ========

		// ** skybox **
		skybox.render(view, projection);

		// ** terrain **
		sandTerrain.render(view, projection, cameraPos);

		// ** light cube **
		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// ** model loading **

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

		// thumper
		thumper.draw(genericShader);
		thumper2.draw(genericShader);

		// nomad
		nomad.draw(genericShader);

		// drone
		// genericShader.setMat4("model", droneModel);
		// drone.draw(genericShader);

		// sand rocks
		// genericShader.setMat4("model", sandRocksModel);
		// genericShader.setMat3("normalMatrix", sandRocksNormalMatrix);
		// sandRocks.draw(genericShader);

		// sand worm
		sandWorm.draw(genericShader);



		// ** text overlay **
		font.renderText(
			std::format("X:{:.2f} Y:{:.2f}, Z:{:.2f}", cameraPos.x, cameraPos.y, cameraPos.z),
			25.0f,
			INITIAL_HEIGHT - 25.0f,
			0.5f,
			Colors::WHITE
		);

		// check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

# pragma endregion

# pragma region CLEANUP
	glfwTerminate();
#pragma endregion

	return 0;
}