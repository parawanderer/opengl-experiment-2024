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
#include "PlayerCamera.h"
#include "Skybox.h"

#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 800


#pragma region STATE

Camera camera(glm::vec3(0.0f, 118.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	INITIAL_WIDTH,
	INITIAL_HEIGHT,
	40.0f
);

PlayerCamera playerCamera(
	glm::vec3(0.0f, 118.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f), 
	INITIAL_WIDTH, 
	INITIAL_HEIGHT, 
	10.0f,
	2.0f,
	5.0f
);

const float RENDER_DISTANCE = 1500.0f;

// world:

// sun
glm::vec3 sunPos(1024.0f, 750.0f, -2000.0f);
//glm::vec3 sunPos(50.0f, 750.0f, 0.0f);
glm::vec3 sunLightColor = Colors::WHITE;

#pragma endregion


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	playerCamera.processMouse(window, xpos, ypos);
	// camera.processMouse(window, xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	playerCamera.processScroll(window, xoffset, yoffset);
	// camera.processScroll(window, xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	playerCamera.processInput(window);
	// camera.processInput(window);
}

void processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	playerCamera.processKey(window, key, scancode, action, mods);
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

	playerCamera.setTerrain(&sandTerrain);
#pragma endregion

# pragma region MAIN_LOOP

	playerCamera.teleportToFloor();
	while(!glfwWindowShouldClose(window))
	{
		// ** view/projection transformations **
		// camera.onNewFrame();
		// const glm::vec3 cameraPos = camera.getPos();
		// const glm::mat4 view = camera.getView(); 
		// const float fov = camera.getFov();
		playerCamera.onNewFrame();
		const glm::vec3 cameraPos = playerCamera.getPos();
		const glm::vec3 cameraPosForDisplay = playerCamera.getPosIncludingJump();
		const glm::mat4 view = playerCamera.getView();
		const float fov = playerCamera.getFov();

		// ** input **
		processInput(window);

		// ** rendering **
		glClearColor(0.45f, 0.49f, 0.61f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glm::mat4 projection = glm::perspective(
			glm::radians(fov),
			(float)INITIAL_WIDTH / (float)INITIAL_HEIGHT, 
			0.1f, 
			RENDER_DISTANCE
		);

		// ======== RENDERING ========

		// ** skybox **
		skybox.render(view, projection);

		// ** terrain **
		sandTerrain.render(view, projection, cameraPos);

		// ** light cube **
		lightCubeShader.use();
		lightCubeShader.setVec3("lightColor", sunLightColor);
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);


		// ** text overlay **
		font.renderText(
			std::format("X:{:.2f} Y:{:.2f}, Z:{:.2f}", cameraPosForDisplay.x, cameraPosForDisplay.y, cameraPosForDisplay.z),
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