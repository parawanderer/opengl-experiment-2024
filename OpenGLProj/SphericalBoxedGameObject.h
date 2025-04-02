#ifndef CIRCULARBOXEDGAMEOBJECT_MINE_H
#define CIRCULARBOXEDGAMEOBJECT_MINE_H
#include "RenderableGameObject.h"
#include "Sphere.h"

/**
 * \brief A SphericalBoxedGameObject is a RenderableGameObject with additional functionality surrounding intersections for ray casting/picking
 */
class SphericalBoxedGameObject : public RenderableGameObject
{
public:
	SphericalBoxedGameObject(const char* modelFilePath, float radius, glm::vec3 localSpaceMidPoint = glm::vec3(0.0f));

	SphericalBoxedGameObject(Model* model, float radius, glm::vec3 localSpaceMidPoint = glm::vec3(0.0f));

	float getBoundRadius() const;

	/**
	 * \return The bounding sphere's mid point in normalized homogeneous coordinates (x, y, z, w = 1.0)
	 */
	glm::vec4 getWorldMidPoint() const;

	void setShowBoundingSphere(bool doShow);

	void draw(Shader& shader) override;

private:
	float _boundingSphereRadius;
	glm::vec3 _boundingSphereMidPoint;

	Sphere _sphere;
	bool _showBoundingSphere = false;
};

#endif