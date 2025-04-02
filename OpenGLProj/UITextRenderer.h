#ifndef UITEXTRENDERER_MINE_H
#define UITEXTRENDERER_MINE_H
#include <vector>

#include "Font.h"
#include "PlayerCamera.h"
#include "SphericalBoundingBoxedEntity.h"
#include "UICharacterDialogueDisplayManager.h"
#include "WorldTimeManager.h"

struct DialogueRequest {
	std::string speaker;
	std::string spokenDialogue;
	float durationOfShowing;
	glm::vec3 speakerPositionInWorld;
	DialogueRequester* dialogueRequester;
};

/**
 * \brief Renders UI-related elements for the screen. Should be one of the final elements to be rendered to the screen.
 */
class UITextRenderer : public UICharacterDialogueDisplayManager
{
public:
	UITextRenderer(const WorldTimeManager* time, const PlayerCamera* camera, Font* uiFont);

	void setCurrentWidthHeight(int currentWidth, int currentHeight);

	void renderCarriedItemInfo(const char* itemName);

	void renderMainUIOverlay(const glm::vec3 cameraPos);

	void requestDialogue(
		const std::string& speaker, 
		const std::string& spokenDialoge, 
		const float durationOfShowing,
		const glm::vec3& speakerPositionInWorld) override;

	void requestDialogue(
		const std::string& speaker, 
		const std::string& spokenDialoge, 
		const float durationOfShowing,
		DialogueRequester* speakerWithDynamicPosition) override;

	void processDialogueRequests();

	void renderCurrentDialogue();

	void renderOverlayForTargetItem(SphericalBoundingBoxedEntity* target);

private:
	const WorldTimeManager* _time;
	const PlayerCamera* _camera;

	Font* _font;
	int _currentWidth;
	int _currentHeight;

	float _lastDialogueAt = 0.0f;
	float _distanceSquaredLastRequest = 0.0f;
	DialogueRequest _currentDialogue;

	std::vector<DialogueRequest> _requestsForFrame;

	void renderItemInteractOverlay(const char* itemName, bool isActive);

	void renderSpeakToCharacterOverlay(const char* characterName);

	void renderContainerOverlay(const char* containerName, int containerItemCount);
};

#endif