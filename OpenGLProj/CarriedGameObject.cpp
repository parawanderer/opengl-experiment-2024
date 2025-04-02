#include "CarriedGameObject.h"

CarriedGameObject::CarriedGameObject(SphericalBoxedGameObject* objectModel): _objectModel(objectModel)
{}

SphericalBoxedGameObject* CarriedGameObject::getModel() const
{
	return this->_objectModel;
}
