#ifndef CARRIEDGAMEOBJECT_MINE_H
#define CARRIEDGAMEOBJECT_MINE_H
#include "SphericalBoxedGameObject.h"
#include "Thumper.h"

class CarriedGameObject
{
public:
	CarriedGameObject(Thumper* object);

	Thumper* getObject() const;

private:
	Thumper* _object;
};


#endif