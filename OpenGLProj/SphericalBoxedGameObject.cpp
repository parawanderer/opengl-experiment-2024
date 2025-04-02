#include "SphericalBoxedGameObject.h"

#include "glm//vec4.hpp"
#include "glm//mat4x4.hpp"

SphericalBoxedGameObject::SphericalBoxedGameObject(
	const char* modelFilePath, 
	float radius, 
	glm::vec3 localSpaceMidPoint
)
: RenderableGameObject(modelFilePath),
_boundingSphereRadius(radius), // TODO: allow changing this? Scaling this would mean we're no longer working with a bounding sphere but a "bounding capsule" afaik?
_boundingSphereMidPoint(localSpaceMidPoint),
_sphere(Sphere(16, 8, radius))
{
}

SphericalBoxedGameObject::SphericalBoxedGameObject(Model* model, float radius, glm::vec3 localSpaceMidPoint)
	: RenderableGameObject(model),
	_boundingSphereRadius(radius), // TODO: allow changing this? Scaling this would mean we're no longer working with a bounding sphere but a "bounding capsule" afaik?
	_boundingSphereMidPoint(localSpaceMidPoint),
	_sphere(Sphere(16, 8, radius))
{
}

float SphericalBoxedGameObject::getBoundRadius() const
{
	return this->_boundingSphereRadius;
}

glm::vec4 SphericalBoxedGameObject::getWorldMidPoint() const
{
	return this->getModelTransform() * glm::vec4(this->_boundingSphereMidPoint, 1.0f);
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
		this->_sphere.draw(shader);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

