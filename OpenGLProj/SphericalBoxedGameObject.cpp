#include "SphericalBoxedGameObject.h"

#include <glm/ext/matrix_transform.hpp>

#include "glm//vec4.hpp"
#include "glm//mat4x4.hpp"

SphericalBoxedGameObject::SphericalBoxedGameObject(
	const char* modelFilePath, 
	float radius, 
	glm::vec3 localSpaceMidPoint,
	float boundingSphereScale
)
: RenderableGameObject(modelFilePath),
_boundingSphereRadius(radius), // TODO: allow changing this? Scaling this would mean we're no longer working with a bounding sphere but a "bounding capsule" afaik?
_boundingSphereMidPoint(localSpaceMidPoint),
_localBoundingSphereScale(boundingSphereScale),
_sphere(Sphere(16, 8, radius))
{
}

SphericalBoxedGameObject::SphericalBoxedGameObject(Model* model, float radius, glm::vec3 localSpaceMidPoint, float boundingSphereScale)
	: RenderableGameObject(model),
	_boundingSphereRadius(radius), // TODO: allow changing this? Scaling this would mean we're no longer working with a bounding sphere but a "bounding capsule" afaik?
	_boundingSphereMidPoint(localSpaceMidPoint),
	_localBoundingSphereScale(boundingSphereScale),
	_sphere(Sphere(16, 8, radius))
{
}

float SphericalBoxedGameObject::getBoundRadius() const
{
	return this->_boundingSphereRadius;
}

glm::vec4 SphericalBoxedGameObject::getWorldMidPoint() const
{
	glm::mat4 adaptedModel = this->getModelTransform();
	adaptedModel = glm::scale(adaptedModel, glm::vec3(this->_localBoundingSphereScale));
	adaptedModel = glm::translate(adaptedModel, this->_boundingSphereMidPoint);

	return adaptedModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void SphericalBoxedGameObject::setShowBoundingSphere(bool doShow)
{
	this->_showBoundingSphere = doShow;
}

void SphericalBoxedGameObject::draw(Shader& shader)
{
	RenderableGameObject::draw(shader);
	if (this->_showBoundingSphere)
	{
		// keep using the same transform matrices as we used above ^
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glm::mat4 adaptedModel = this->getModelTransform();
		adaptedModel = glm::scale(adaptedModel, glm::vec3(this->_localBoundingSphereScale));
		adaptedModel = glm::translate(adaptedModel, this->_boundingSphereMidPoint);
		shader.setMat4("model", adaptedModel);
		this->_sphere.draw(shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

