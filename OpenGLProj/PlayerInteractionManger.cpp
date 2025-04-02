#include "PlayerInteractionManger.h"

#include <GLFW/glfw3.h>

#include "WorldMathUtils.h"

const glm::vec3 ITEM_PLACEMENT_SMALL_OFFSET_Y = glm::vec3(0.0, 0.1, 0.0);

PlayerInteractionManger::PlayerInteractionManger(
	const WorldTimeManager* time,
	const CameraManager* cameraManager,
	GLFWwindow* window,
	const Terrain* terrain,
	PlayerState* player, 
	const float worldInteractionCooldownSecs,
	std::set<Thumper*>* worldItemsThatPlayerCanPickUp
):
_time(time),
_camera(cameraManager),
_window(window),
_terrain(terrain),
_player(player),
_worldItemsThatPlayerCanPickUp(worldItemsThatPlayerCanPickUp),
_worldInteractionCooldownSecs(worldInteractionCooldownSecs)
{}

Thumper* PlayerInteractionManger::getMouseTarget()
{
	const glm::vec3& cameraPos = this->_camera->getPos();
	const glm::vec3& cameraFront = this->_camera->getCurrentCamera()->getFront();

	if (!this->_player->hasCarriedItem()) { // only if not carrying an item already
		std::vector<SphericalBoundingBoxedEntity*> considered(this->_worldItemsThatPlayerCanPickUp->begin(), this->_worldItemsThatPlayerCanPickUp->end());
		Thumper* result = (Thumper*)WorldMathUtils::findClosestIntersection(considered, cameraPos, cameraFront, 5.0f);
		// TODO: if I add more item types later, then this type cast is not valid
		return result;
	}
	return nullptr;
}

void PlayerInteractionManger::handleInteractionChecks(Thumper* mouseRayTarget)
{
	const float t = this->_time->getCurrentTime();
	const glm::vec3& cameraPos = this->_camera->getPos();
	const glm::vec3& cameraFront = this->_camera->getCurrentCamera()->getFront();

	const bool interactionCooldownPassed = t - this->_lastInteractionAt > this->_worldInteractionCooldownSecs;

	if (!interactionCooldownPassed) return;

	// order here is order in which the interaction takes priority.
	// "OR" results in short-circuit where the next operations won't be checked/executed anymore
	bool complete = this->handleActivateItemInteraction(mouseRayTarget)
		|| this->handlePickUpItemInteraction(mouseRayTarget)
		|| this->handleDropItemInteraction(cameraPos, cameraFront);

	if (complete)
	{
		this->_lastInteractionAt = t;
	}
}

bool PlayerInteractionManger::handleActivateItemInteraction(Thumper* mouseRayTarget)
{
	if (mouseRayTarget != nullptr && glfwGetKey(this->_window, GLFW_KEY_E) == GLFW_PRESS)
	{
		switch (mouseRayTarget->getState())
		{
		case Thumper::STATE::DISABLED:
			mouseRayTarget->setState(Thumper::STATE::ACTIVATED); // start animating!
			return true;
		case Thumper::STATE::ACTIVATED:
			mouseRayTarget->setState(Thumper::STATE::DISABLED);
			return true;
		}
	}
	return false;
}

bool PlayerInteractionManger::handlePickUpItemInteraction(Thumper* mouseRayTarget)
{
	if (mouseRayTarget != nullptr && !this->_player->hasCarriedItem() && glfwGetKey(this->_window, GLFW_KEY_C) == GLFW_PRESS) // pick up the item
	{
		mouseRayTarget->setState(Thumper::STATE::DISABLED); // disable when picking up
		mouseRayTarget->setIsCarried(true);
		this->_worldItemsThatPlayerCanPickUp->erase(mouseRayTarget);
		this->_player->setCarriedItem(CarriedGameObject(mouseRayTarget));
		return true;
	}
	return false;
}

bool PlayerInteractionManger::handleDropItemInteraction(const glm::vec3& cameraPos, const glm::vec3& cameraFront)
{
	if (this->_camera->isPlayerCamera() && this->_player->hasCarriedItem() && glfwGetKey(this->_window, GLFW_KEY_E) == GLFW_PRESS) // drop the item
	{
		// drop the item
		Thumper* thump = this->_player->removeCarriedItem().getObject();

		// glm::mat4 newModel = glm::mat4(1.0f);
		// // translate position just comes out to a nice "in front of the player" position
		// newModel = glm::translate(newModel, 
		// 	this->_terrain->getWorldHeightVecFor(cameraPos.x + (cameraFront.x * 2.5f), cameraPos.z + (cameraFront.z * 2.5f)) 
		// 	+ ITEM_PLACEMENT_SMALL_OFFSET_Y);
		//thump->updateModelTransform(newModel);

		thump->setPosition(this->_terrain->getWorldHeightVecFor(cameraPos.x + (cameraFront.x * 2.5f), cameraPos.z + (cameraFront.z * 2.5f)) + ITEM_PLACEMENT_SMALL_OFFSET_Y);
		thump->setIsCarried(false);

		this->_worldItemsThatPlayerCanPickUp->insert(thump);
		return true;
	}
	return false;
}
