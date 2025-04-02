#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <vector>

#include "Colors.h"
#include "ErrorUtils.h"
#include "Font.h"
#include "Shader.h"
#include "stb_image.h"

#define INITIAL_WIDTH 1280
#define INITIAL_HEIGHT 800


#pragma region STATE

glm::vec3 cameraPos = glm::vec3(-31.281975, 20.137228, -99.600090);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

const float RENDER_DISTANCE = 1500.0f;

float deltaTime = 0.0f; // time between current and last frame
float lastFrame = 0.0f; // time of last frame

float pitch = 0.0f;
float yaw = -90.0f;

float lastX = INITIAL_WIDTH / 2;
float lastY = INITIAL_HEIGHT / 2;

bool firstMouse = true;

float fov = 45.0f;

#pragma endregion


void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed as y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) pitch = 89.0f;
	if (pitch < -89.0f) pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	fov -= (float)yoffset;
	if (fov < 1.0f) fov = 1.0f;
	if (fov > 45.0f) fov = 45.0f;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	const float cameraSpeed = deltaTime * 10.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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
	Shader terrainShader = Shader::fromFiles("terrain.vert", "terrain.frag");

	int width, height, nChannels;
	unsigned char* data = stbi_load("iceland_heightmap.png",&width, &height, &nChannels, 0);

	// vertex generation
	std::vector<float> vertices;
	float yScale = 64.0f / 256.0f;
	float yShift = 16.0f;

	for (unsigned int i = 0; i < height; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			// retrieve texel for (i, j) tex coord
			unsigned char* texel = data + (j + width * i) * nChannels;
			// raw height at coordinate
			unsigned char y = texel[0];

			// vertex
			vertices.push_back(-height / 2.0f + i); // v.x
			vertices.push_back((int)y * yScale - yShift); // v.y
			vertices.push_back(-width / 2.0f + j ); // v.z
		}
	}
	stbi_image_free(data);

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < height - 1; i++ )
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			for (unsigned int k = 0; k < 2; ++k)
			{
				indices.push_back(j + width * (i + k));
			}
		}
	}

	const unsigned int NUM_STRIPS = height - 1;
	const unsigned int NUM_VERTS_PER_STRIP = width * 2;

	unsigned int terrainVAO, terrainVBO, terrainEBO;
	glGenVertexArrays(1, &terrainVAO);
	glBindVertexArray(terrainVAO);

	glGenBuffers(1, &terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &terrainEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

# pragma region MAIN_LOOP

	while(!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// ** input **
		processInput(window);

		// ** rendering **
		glClearColor(0.45f, 0.49f, 0.61f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// model/view/projection transformations
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//printf("Cam: %f, %f, %f\n", cameraPos.x, cameraPos.y, cameraPos.z);
		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f), 
			(float)INITIAL_WIDTH / (float)INITIAL_HEIGHT, 
			0.1f, 
			RENDER_DISTANCE
		);

		terrainShader.use();
		terrainShader.setMat4("model", model);
		terrainShader.setMat4("view", view);
		terrainShader.setMat4("projection", projection);

		// render mesh strip by strip
		glBindVertexArray(terrainVAO);
		for(unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
		{
			glDrawElements(GL_TRIANGLE_STRIP, NUM_VERTS_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * NUM_VERTS_PER_STRIP * strip));
		}

		// text
		font.renderText(
			std::format("X:{:.2f} Y:{:.2f}, Z:{:.2f}", cameraPos.x, cameraPos.y, cameraPos.z),
			25.0f,
			INITIAL_HEIGHT - 25.0f,
			0.5f,
			Colors::WHITE
		);
		// font.renderText(fontShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		// font.renderText(fontShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

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