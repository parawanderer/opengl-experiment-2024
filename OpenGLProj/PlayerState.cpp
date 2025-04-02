#include "PlayerState.h"

PlayerState::PlayerState(): _carriedItem(nullptr)
{}

bool PlayerState::hasCarriedItem()
{
	return this->_hasItem;
}

void PlayerState::setCarriedItem(const CarriedGameObject& item)
{
	this->_hasItem = true;
	this->_carriedItem = item;
}

CarriedGameObject& PlayerState::getCarriedItem()
{
	return this->_carriedItem;
}

void PlayerState::removeCarriedItem()
{
	this->_hasItem = false;
	this->_carriedItem = CarriedGameObject(nullptr);
}
