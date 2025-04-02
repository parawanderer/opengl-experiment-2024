#include "CarriedGameObject.h"

CarriedGameObject::CarriedGameObject(Thumper* object): _object(object)
{}

Thumper* CarriedGameObject::getObject() const
{
	return this->_object;
}
