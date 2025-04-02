#include "RenderableGameObject.h"

#include <glm/mat4x4.hpp>

RenderableGameObject::RenderableGameObject(const char* modelFilePath)
:
_model(new Model(modelFilePath)),
_isModelExternal(false),
_modelTransform(glm::mat4(1.0f)),
_normalMatrix(glm::mat3(glm::transpose(glm::inverse(this->_modelTransform))))
{}

RenderableGameObject::RenderableGameObject(Model* model):
	_model(model),
	_isModelExternal(true),
	_modelTransform(glm::mat4(1.0f)),
	_normalMatrix(glm::mat3(glm::transpose(glm::inverse(this->_modelTransform))))
{
}

RenderableGameObject::~RenderableGameObject()
{
	if (!this->_isModelExternal) delete this->_model;
}

void RenderableGameObject::setModelTransform(glm::mat4 model)
{
	this->_modelTransform = model;
	this->_normalMatrix = glm::mat3(glm::transpose(glm::inverse(this->_modelTransform)));
}

Model* RenderableGameObject::getObjectModel() const
{
	return this->_model;
}

glm::mat4 RenderableGameObject::getModelTransform() const
{
	return this->_modelTransform;
}

glm::mat3 RenderableGameObject::getNormalMatrix() const
{
	return this->_normalMatrix;
}

void RenderableGameObject::draw(Shader& shader)
{
	this->fillShaderUnifs(shader);
	this->_model->draw(shader);
}

void RenderableGameObject::fillShaderUnifs(Shader& shader)
{
	shader.setMat4("model", this->_modelTransform);
	shader.setMat3("normalMatrix", this->_normalMatrix);
}
