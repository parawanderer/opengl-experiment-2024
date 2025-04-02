#ifndef CARRIEDGAMEOBJECT_MINE_H
#define CARRIEDGAMEOBJECT_MINE_H
#include "Thumper.h"

/**
 * \brief A wrapper for Player State current object use.
 * Currently it's pretty empty but in the future I could see this storing
 * more data relating to the object after the player picks this up.
 * If it stays this empty I will end up removing it.
 *
 * It being hardcoded to Thumper is also temporary,
 * I plan to mess around with this project a bit more and add some more items later.
 */
class CarriedGameObject
{
public:
	CarriedGameObject(Thumper* object);

	Thumper* getObject() const;

private:
	Thumper* _object;
};


#endif