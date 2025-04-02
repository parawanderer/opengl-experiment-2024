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
	std::set<Thumper*>* worldItemsThatPlayerCanPickUp,
	std::set<NomadCharacter*>* charactersThatPlayerCanTalkTo
):
_time(time),
_camera(cameraManager),
_window(window),
_terrain(terrain),
_player(player),
_worldItemsThatPlayerCanPickUp(worldItemsThatPlayerCanPickUp),
_worldInteractionCooldownSecs(worldInteractionCooldownSecs),
_charactersThatPlayerCanTalkTo(charactersThatPlayerCanTalkTo)
{}

SphericalBoundingBoxedEntity* PlayerInteractionManger::getMouseTarget()
{
	const glm::vec3& cameraPos = this->_camera->getPos();
	const glm::vec3& cameraFront = this->_camera->getCurrentCamera()->getFront();

	std::vector<SphericalBoundingBoxedEntity*> considered(this->_worldItemsThatPlayerCanPickUp->begin(), this->_worldItemsThatPlayerCanPickUp->end());
	considered.insert(considered.end(), this->_charactersThatPlayerCanTalkTo->begin(), this->_charactersThatPlayerCanTalkTo->end()); // textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	return WorldMathUtils::findClosestIntersection(considered, cameraPos, cameraFront, 5.0f);
}

void PlayerInteractionManger::handleInteractionChecks(SphericalBoundingBoxedEntity* mouseRayTarget)
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

bool PlayerInteractionManger::handleActivateItemInteraction(SphericalBoundingBoxedEntity* mouseRayTarget)
{
	// TODO: rewrite interactions using a pattern like *Visitor* to keep this generic

	if (Thumper* thumper = dynamic_cast<Thumper*>(mouseRayTarget))
	{
		if (glfwGetKey(this->_window, GLFW_KEY_E) == GLFW_PRESS)
		{
			switch (thumper->getState())
			{
			case Thumper::STATE::DISABLED:
				thumper->setState(Thumper::STATE::ACTIVATED); // start animating!
				return true;
			case Thumper::STATE::ACTIVATED:
				thumper->setState(Thumper::STATE::DISABLED);
				return true;
			}
		}
	}
	else if (NomadCharacter* character = dynamic_cast<NomadCharacter*>(mouseRayTarget))
	{
		if (glfwGetKey(this->_window, GLFW_KEY_C) == GLFW_PRESS)
		{
			character->saySomething(this->_camera->getCurrentCamera()->getPos());
			return true;
		}
	}
	
	return false;
}

bool PlayerInteractionManger::handlePickUpItemInteraction(SphericalBoundingBoxedEntity* mouseRayTarget)
{
	if (Thumper* thumper = dynamic_cast<Thumper*>(mouseRayTarget))
	{
		if (thumper != nullptr && !this->_player->hasCarriedItem() && glfwGetKey(this->_window, GLFW_KEY_C) == GLFW_PRESS) // pick up the item
		{
			thumper->setState(Thumper::STATE::DISABLED); // disable when picking up
			thumper->setIsCarried(true);
			this->_worldItemsThatPlayerCanPickUp->erase(thumper);
			this->_player->setCarriedItem(CarriedGameObject(thumper));
			return true;
		}
	}

	return false;
}

bool PlayerInteractionManger::handleDropItemInteraction(const glm::vec3& cameraPos, const glm::vec3& cameraFront)
{
	if (this->_camera->isPlayerCamera() && this->_player->hasCarriedItem() && glfwGetKey(this->_window, GLFW_KEY_E) == GLFW_PRESS) // drop the item
	{
		// drop the item
		Thumper* thump = this->_player->removeCarriedItem().getObject();
		thump->setPosition(this->_terrain->getWorldHeightVecFor(cameraPos.x + (cameraFront.x * 2.5f), cameraPos.z + (cameraFront.z * 2.5f)) + ITEM_PLACEMENT_SMALL_OFFSET_Y);
		thump->setIsCarried(false);

		this->_worldItemsThatPlayerCanPickUp->insert(thump);
		return true;
	}
	return false;
}
