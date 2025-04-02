#ifndef PLAYERINTERACTIONMANAGER_MINE_H
#define PLAYERINTERACTIONMANAGER_MINE_H
#include <set>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "CameraManager.h"
#include "PlayerState.h"
#include "Thumper.h"
#include "WorldTimeManager.h"

class PlayerInteractionManger
{
public:
	PlayerInteractionManger(
		const WorldTimeManager* time,
		const CameraManager* cameraManager,
		GLFWwindow* window,
		const Terrain* terrain,
		PlayerState* player, 
		const float worldInteractionCooldownSecs,
		std::set<Thumper*>* worldItemsThatPlayerCanPickUp
	);

	Thumper* getMouseTarget();

	void handleInteractionChecks(Thumper* mouseRayTarget);

private:
	const WorldTimeManager* _time;
	const CameraManager* _camera;
	GLFWwindow* _window;

	const Terrain* _terrain;
	PlayerState* _player;
	std::set<Thumper*>* _worldItemsThatPlayerCanPickUp;

	const float _worldInteractionCooldownSecs;

	bool _interactionCooldownPassed = false;
	float _lastInteractionAt = 0.0f;

	bool handleActivateItemInteraction(Thumper* mouseRayTarget);
	bool handlePickUpItemInteraction(Thumper* mouseRayTarget);
	bool handleDropItemInteraction(const glm::vec3& cameraPos, const glm::vec3& cameraFront);
};

#endif