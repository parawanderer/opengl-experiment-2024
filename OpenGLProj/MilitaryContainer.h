#ifndef MILITARYCONTAINER_MINE_H
#define MILITARYCONTAINER_MINE_H
#include "DrawableEntity.h"
#include "SphericalBoundingBoxedEntity.h"
#include "SphericalBoxedGameObject.h"

/**
 * \brief A container that will eventually hold items but does nothing for now...
 */
class MilitaryContainer : public SphericalBoundingBoxedEntity, public DrawableEntity
{
public:
	MilitaryContainer(SphericalBoxedGameObject* model);

	void draw(Shader& shader) override;

	bool hasItems() const;

	float getRadiusSphericalBoundingBox() override;
	glm::vec3 getBoundMidPoint() override;

	SphericalBoxedGameObject* getObjectModel() const;

	void setModelTransform(const glm::mat4& transform);

	void open();

private:
	SphericalBoxedGameObject* _model;

	glm::vec3 _currentPos = glm::vec3(0.0, 0.0, 0.0);

};

#endif