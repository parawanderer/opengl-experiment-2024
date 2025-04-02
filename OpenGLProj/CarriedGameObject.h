#ifndef CARRIEDGAMEOBJECT_MINE_H
#define CARRIEDGAMEOBJECT_MINE_H
#include "SphericalBoxedGameObject.h"

class CarriedGameObject
{
public:
	CarriedGameObject(SphericalBoxedGameObject* objectModel);

	SphericalBoxedGameObject* getModel() const;

private:
	SphericalBoxedGameObject* _objectModel;
};


#endif