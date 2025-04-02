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
	/**
	 * \brief Create an instance by loading the model from the specified path. Does not reuse the model.
	 *
	 * \param modelFilePath			Pointer to object model that was instantiated separately
	 * \param radius				radius of the bounding sphere in world-space. This will ignore any scaling done to the object.
	 * \param localSpaceMidPoint	local space mid-point of the object. Defaults to [0, 0, 0].
	 *								This is from where the bounding sphere will be extended, and is transformed
	 *								along with the object when the object is moved using a model transform.
	 */
	SphericalBoxedGameObject(const char* modelFilePath, float radius, glm::vec3 localSpaceMidPoint = glm::vec3(0.0f), float boundingSphereScale = 1.0f);

	/**
	 * \brief Same as SphericalBoxedGameObject(modelFilePath, radius, localSpaceMidPoint), except this does not load the model, allowing the same model to be reused between instances.
	 *
	 * \param model					Pointer to object model that was instantiated separately
	 *
	 * \param radius				radius of the bounding sphere in world-space. This will ignore any scaling done to the object.
	 *
	 * \param localSpaceMidPoint	local space mid-point of the object. Defaults to [0, 0, 0].
	 *								This is from where the bounding sphere will be extended, and is transformed
	 *								along with the object when the object is moved using a model transform.
	 */
	SphericalBoxedGameObject(Model* model, float radius, glm::vec3 localSpaceMidPoint = glm::vec3(0.0f), float boundingSphereScale = 1.0f);

	float getBoundRadius() const;

	/**
	 * \return The bounding sphere's mid point in normalized homogeneous coordinates (x, y, z, w = 1.0)
	 */
	glm::vec4 getWorldMidPoint() const;

	/**
	 * \brief Makes it so that when the object is drawn using draw(shader), its bounding sphere will be drawn around it, if this is set to true.
	 * **Note** no specific shaders have been implemented for the sphere so it will just take a random texture as its colour, probably not being transparent. 
	 */
	void setShowBoundingSphere(bool doShow);

	void draw(Shader& shader) override;

private:
	float _boundingSphereRadius;
	glm::vec3 _boundingSphereMidPoint;
	float _localBoundingSphereScale;

	Sphere _sphere;
	bool _showBoundingSphere = false;
};

#endif