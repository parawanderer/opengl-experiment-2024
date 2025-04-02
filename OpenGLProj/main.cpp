#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION

#include "Colors.h"
#include "ErrorUtils.h"
#include "Font.h"
#include "Shader.h"
#include "stb_image.h"
#include "Terrain.h"
#include "Camera.h"

#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 800


#pragma region STATE

Camera camera(
	glm::vec3(-55, 43, -103), 
	glm::vec3(0.0f, 0.0f, -1.0f), 
	INITIAL_WIDTH, 
	INITIAL_HEIGHT, 
	10.0f
);

const float RENDER_DISTANCE = 1500.0f;

// world:

// sun
glm::vec3 sunPos(0.0f, 2000.0f, 0.0f);
glm::vec3 sunLightColor = Colors::WHITE;

#pragma endregion


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	camera.processMouse(window, xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processScroll(window, xoffset, yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	camera.processInput(window);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

# pragma endregion

	// text
	Shader fontShader = Shader::fromFiles("font.vert", "font.frag");
	glm::mat4 textProjection = glm::ortho(0.0f, (float)INITIAL_WIDTH, 0.0f, (float)INITIAL_HEIGHT);
	fontShader.use();
	fontShader.setMat4("projection", textProjection);
	Font font("resources/calibri.ttf", &fontShader);

	// terrain
	const float yScaleMult = 64.0f;
	const float yShift = 16.0f;
	Terrain sandTerrain("resources/perlin-noise-texture.png", yScaleMult, yShift);
	Shader terrainShader = Shader::fromFiles("terrain.vert", "terrain_basic.frag");
	terrainShader.use();
	const glm::mat4 terrainModel = glm::mat4(1.0f);
	terrainShader.setMat4("model", terrainModel); // model transform (to world coords)
	const glm::mat3 terrainNormalMatrix = glm::mat3(glm::transpose(glm::inverse(terrainModel)));
	// matrix to model the normal if there's non-linear scaling going on in the model matrix
	terrainShader.setMat3("normalMatrix", terrainNormalMatrix);
	// material (terrain)
	terrainShader.setVec3("material.ambient", Colors::SAND);
	terrainShader.setVec3("material.diffuse", Colors::SAND);
	terrainShader.setVec3("material.specular", Colors::WHITE); // no specular
	terrainShader.setFloat("material.shininess", 32); // no shininess
	// light (sun)
	terrainShader.setVec3("light.position", sunPos);
	terrainShader.setVec3("light.ambient", sunLightColor * 0.1f);
	terrainShader.setVec3("light.diffuse", sunLightColor * 0.5f);
	terrainShader.setVec3("light.specular", sunLightColor);
	// view position/camera position
	//terrainShader.setVec3("viewPos", cameraPos);// update in loop

# pragma region MAIN_LOOP

	while(!glfwWindowShouldClose(window))
	{
		camera.onNewFrame();
		glm::vec3 cameraPos = camera.getPos();

		// ** input **
		processInput(window);

		// ** rendering **
		glClearColor(0.45f, 0.49f, 0.61f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// ** view/projection transformations **
		glm::mat4 view = camera.getView(); //glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glm::mat4 projection = glm::perspective(
			glm::radians(camera.getFov()), 
			(float)INITIAL_WIDTH / (float)INITIAL_HEIGHT, 
			0.1f, 
			RENDER_DISTANCE
		);

		// ** terrain **
		terrainShader.use();
		terrainShader.setMat4("view", view);
		terrainShader.setMat4("projection", projection);
		terrainShader.setVec3("viewPos", cameraPos);
		sandTerrain.render();

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