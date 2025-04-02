#include "MilitaryContainer.h"

MilitaryContainer::MilitaryContainer(SphericalBoxedGameObject* model):
_model(model)
{}

void MilitaryContainer::draw(Shader& shader)
{
	//this->_model->setShowBoundingSphere(true);
	this->_model->draw(shader);
}

bool MilitaryContainer::hasItems() const
{
	return false; // TODO: update...
}

float MilitaryContainer::getRadiusSphericalBoundingBox()
{
	return this->_model->getBoundRadius();
}

glm::vec3 MilitaryContainer::getBoundMidPoint()
{
	return this->_model->getWorldMidPoint();
}

SphericalBoxedGameObject* MilitaryContainer::getObjectModel() const
{
	return this->_model;
}

void MilitaryContainer::setModelTransform(const glm::mat4& transform)
{
	this->_model->setModelTransform(transform);
}

void MilitaryContainer::open()
{
	// TODO: update...
	if (this->hasItems())
	{
		throw std::exception("Not implemented");
	}
}
