#ifndef MODELCONSTANTS_MINE_H
#define MODELCONSTANTS_MINE_H
#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Terrain.h"

// collection of placements for static object in the world
// I consider these "close to" being constants

constexpr auto SMALL_OFFSET_Y = glm::vec3(0.0, 0.1, 0.0);

auto CONTAINER_S1_MODEL = [](const Terrain& terrain) {
	glm::mat4 containerS1Model = glm::mat4(1.0f);
	containerS1Model = glm::translate(containerS1Model, terrain.getWorldHeightVecFor(150.0f, -150.0f) + glm::vec3(0.0, -0.1f, 0.0f));
	containerS1Model = glm::rotate(containerS1Model, glm::radians(4.0f), glm::vec3(0.0, 0.0, 1.0));
	containerS1Model = glm::scale(containerS1Model, glm::vec3(0.005f));
	return containerS1Model;
};

auto CONTAINER_S2_MODEL = [](const Terrain& terrain) {
	glm::mat4 containerS2Model = glm::mat4(1.0f);
	containerS2Model = glm::translate(containerS2Model, terrain.getWorldHeightVecFor(167.8f, -163.0f) + glm::vec3(0.0, -0.1f, 0.0f));
	containerS2Model = glm::rotate(containerS2Model, glm::radians(-12.0f), glm::vec3(0.0, 1.0, 1.0));
	containerS2Model = glm::rotate(containerS2Model, glm::radians(-39.0f), glm::vec3(0.0, 0.0, 1.0));
	containerS2Model = glm::scale(containerS2Model, glm::vec3(0.005f));
	return containerS2Model;
};

auto CONTAINER_L1_MODEL = [](const Terrain& terrain) {
	glm::mat4 containerL1Model = glm::mat4(1.0f);
	containerL1Model = glm::translate(containerL1Model, terrain.getWorldHeightVecFor(160.0f, -160.0f) + glm::vec3(0.0, -1.8f, 0.0f));
	containerL1Model = glm::rotate(containerL1Model, glm::radians(35.0f), glm::vec3(1.0, 0.0, 0.0));
	containerL1Model = glm::rotate(containerL1Model, glm::radians(10.0f), glm::vec3(0.0, 1.0, 0.0));
	containerL1Model = glm::scale(containerL1Model, glm::vec3(0.02f));
	return containerL1Model;
};

auto CONTAINER_L2_MODEL = [](const Terrain& terrain) {
	glm::mat4 containerL2Model = glm::mat4(1.0f);
	containerL2Model = glm::translate(containerL2Model, terrain.getWorldHeightVecFor(182.0f, -175.0f) + glm::vec3(0.0, -2.0f, 0.0f));
	containerL2Model = glm::rotate(containerL2Model, glm::radians(74.0f), glm::vec3(1.0, 0.0, 0.0));
	containerL2Model = glm::rotate(containerL2Model, glm::radians(112.0f), glm::vec3(0.0, 1.0, 0.0));
	containerL2Model = glm::rotate(containerL2Model, glm::radians(-12.0f), glm::vec3(0.0, 0.0, 1.0));
	containerL2Model = glm::scale(containerL2Model, glm::vec3(0.02f));
	return containerL2Model;
};

auto CONTAINER_L3_MODEL = [](const Terrain& terrain) {
	glm::mat4 containerL3Model = glm::mat4(1.0f);
	containerL3Model = glm::translate(containerL3Model, terrain.getWorldHeightVecFor(182, -227) + glm::vec3(0.0, -1.5f, 0.0f));
	containerL3Model = glm::rotate(containerL3Model, glm::radians(-21.0f), glm::vec3(1.0, 0.0, 0.0));
	containerL3Model = glm::rotate(containerL3Model, glm::radians(-3.0f), glm::vec3(0.0, 1.0, 0.0));
	containerL3Model = glm::rotate(containerL3Model, glm::radians(15.0f), glm::vec3(0.0, 0.0, 1.0));
	containerL3Model = glm::scale(containerL3Model, glm::vec3(0.02f));
	return containerL3Model;
};

auto LIGHT_CUBE_MODEL = [](const glm::vec3& sunPos)
{
	glm::mat4 lightCubeModel = glm::mat4(1.0f);
	lightCubeModel = glm::translate(lightCubeModel, sunPos);
	lightCubeModel = glm::scale(lightCubeModel, glm::vec3(10.0f));
	return lightCubeModel;
};


#endif