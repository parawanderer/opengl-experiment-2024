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

CarriedGameObject PlayerState::removeCarriedItem()
{
	this->_hasItem = false;
	const auto item = this->_carriedItem;
	this->_carriedItem = CarriedGameObject(nullptr);
	return item;
}
