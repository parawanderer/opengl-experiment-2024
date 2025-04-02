#ifndef PLAYER_MINE_H
#define PLAYER_MINE_H

#include "CarriedGameObject.h"

/**
 * \brief Represents the player's game state
 */
class PlayerState
{
public:
	PlayerState();

	bool hasCarriedItem();

	void setCarriedItem(const CarriedGameObject& item);
	CarriedGameObject& getCarriedItem();

	CarriedGameObject removeCarriedItem();

private:
	bool _hasItem = false;
	CarriedGameObject _carriedItem;
};


#endif