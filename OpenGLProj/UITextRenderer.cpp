#include "UITextRenderer.h"

#include "CameraManager.h"
#include "Colors.h"
#include "GameObjectConstants.h"
#include "MilitaryContainer.h"
#include "NomadCharacter.h"
#include "Thumper.h"
#include "WorldMathUtils.h"

UITextRenderer::UITextRenderer(const WorldTimeManager* time, const PlayerCamera* camera, Font* uiFont)
: _time(time), _camera(camera), _font(uiFont), _currentDialogue{.speaker = "", .spokenDialogue = "", .durationOfShowing = 0.0f, .speakerPositionInWorld = glm::vec3(0.0f), .dialogueRequester = nullptr}
{}

void UITextRenderer::setCurrentWidthHeight(int currentWidth, int currentHeight)
{
	this->_currentWidth = currentWidth;
	this->_currentHeight = currentHeight;
}

void UITextRenderer::renderItemInteractOverlay(const char* itemName, bool isActive)
{
	// text overlay (inspired by bethesda games because it is simple to do)
	// obviously the font isn't really all that nice looking.
	// but I don't feel like fiddling with fonts/implementing distance-fields *again*,
	// but this time for fonts.
	this->_font->renderText(
		itemName,
		(this->_currentWidth * (5.0f / 8.0f)),
		(this->_currentHeight / 2.0f),
		0.6f,
		Colors::WHITE
	);

	this->_font->renderText(
		"E) TAKE",
		(this->_currentWidth * (5.0f / 8.0f)),
		(this->_currentHeight / 2.0f) - 40.0f,
		0.5f,
		Colors::WHITE
	);

	this->_font->renderText(
		isActive ? "C) DEACTIVATE" : "C) ACTIVATE",
		(this->_currentWidth * (5.0f / 8.0f)),
		(this->_currentHeight / 2.0f) - 80.0f,
		0.5f,
		Colors::WHITE
	);
}

void UITextRenderer::renderSpeakToCharacterOverlay(const char* characterName)
{
	this->_font->renderText(
		characterName,
		(this->_currentWidth * (5.0f / 8.0f)),
		(this->_currentHeight / 2.0f),
		0.6f,
		Colors::WHITE
	);

	this->_font->renderText(
		"E) SPEAK",
		(this->_currentWidth * (5.0f / 8.0f)),
		(this->_currentHeight / 2.0f) - 40.0f,
		0.5f,
		Colors::WHITE
	);
}

void UITextRenderer::renderContainerOverlay(const char* containerName, int containerItemCount)
{
	// TODO: expand
	if (containerItemCount == 0)
	{
		this->_font->renderText(
			containerName,
			(this->_currentWidth * (5.0f / 8.0f)),
			(this->_currentHeight / 2.0f),
			0.6f,
			Colors::WHITE
		);

		this->_font->renderText(
			"E) OPEN (Empty)",
			(this->_currentWidth * (5.0f / 8.0f)),
			(this->_currentHeight / 2.0f) - 40.0f,
			0.5f,
			glm::vec3(0.7) // grey
		);
	}
	else
	{
		// TODO: implement
		throw std::exception("Not implemented!");
	}
}

void UITextRenderer::renderCarriedItemInfo(const char* itemName)
{
	this->_font->renderText(
		itemName,
		(this->_currentWidth - 150.0f),
		55.0f,
		0.45f,
		Colors::WHITE
	);

	this->_font->renderText(
		"C) DROP",
		(this->_currentWidth - 150.0f),
		30.0f,
		0.35f,
		Colors::WHITE
	);
}

void UITextRenderer::renderMainUIOverlay(const glm::vec3 cameraPos)
{
	this->_font->renderText(
		std::format("X:{:.2f} Y:{:.2f}, Z:{:.2f}", cameraPos.x, cameraPos.y, cameraPos.z),
		25.0f,
		this->_currentHeight - 25.0f,
		0.5f,
		Colors::WHITE
	);

	// actually going to do a trick to get a center-of-the-screen "." indicator like in this game: https://youtu.be/6QZAhsxwNU0?si=J7eN6p2nRvc4Z_tW
	// mostly because I think it is useful/helpful for object-picking purposes
	this->_font->renderText(".", this->_currentWidth / 2.0f, this->_currentHeight / 2.0f, 0.5f, Colors::WHITE);
}

void UITextRenderer::requestDialogue(const std::string& speaker, const std::string& spokenDialoge,
	const float durationOfShowing, const glm::vec3& speakerPositionInWorld)
{
	this->_requestsForFrame.push_back({
		.speaker = speaker,
		.spokenDialogue = spokenDialoge,
		.durationOfShowing = durationOfShowing,
		.speakerPositionInWorld = speakerPositionInWorld,
		.dialogueRequester = nullptr
	});
}

void UITextRenderer::requestDialogue(const std::string& speaker, const std::string& spokenDialoge,
	const float durationOfShowing, DialogueRequester* speakerWithDynamicPosition)
{
	this->_requestsForFrame.push_back({
		.speaker = speaker,
		.spokenDialogue = spokenDialoge,
		.durationOfShowing = durationOfShowing,
		.speakerPositionInWorld = speakerWithDynamicPosition->getCurrentPosition(),
		.dialogueRequester = speakerWithDynamicPosition
	});
}

void UITextRenderer::processDialogueRequests()
{
	const float now = this->_time->getCurrentTime();
	const glm::vec3 playerPos = this->_camera->getPos();
	const bool isLastDialogueComplete = now > (this->_lastDialogueAt + this->_currentDialogue.durationOfShowing);

	float bestDistance = isLastDialogueComplete ? FLT_MAX
						: this->_currentDialogue.dialogueRequester != nullptr
							? WorldMathUtils::distance2(this->_currentDialogue.dialogueRequester->getCurrentPosition(), playerPos)
							: this->_distanceSquaredLastRequest;

	DialogueRequest* bestRequest = isLastDialogueComplete ? nullptr : &this->_currentDialogue;
	float bestDialogueAt = isLastDialogueComplete ? 0.0f : this->_lastDialogueAt;

	for (DialogueRequest& req : this->_requestsForFrame)
	{
		// any request better than the current one? Based on distance
		const float d2 = WorldMathUtils::distance2(playerPos, req.speakerPositionInWorld);
		if (d2 <= bestDistance)
		{
			bestDistance = d2;
			bestRequest = &req;
			bestDialogueAt = now;
		}
	}

	if (bestRequest != nullptr) // update if we have a solution
	{
		this->_currentDialogue = *bestRequest;
		this->_lastDialogueAt = bestDialogueAt;
		this->_distanceSquaredLastRequest = bestDistance;
	}

	this->_requestsForFrame.clear(); // clear requests
}

void UITextRenderer::renderCurrentDialogue()
{
	const bool isLastDialogueComplete = this->_time->getCurrentTime() > (this->_lastDialogueAt + this->_currentDialogue.durationOfShowing);
	if (isLastDialogueComplete) return;

	const std::string dialogueLine = this->_currentDialogue.speaker + ": " + this->_currentDialogue.spokenDialogue;

	this->_font->renderTextCenter(
		dialogueLine,
		(this->_currentWidth * 0.5f),
		30.0f,
		0.5f,
		Colors::WHITE
	);
}

void UITextRenderer::renderOverlayForTargetItem(SphericalBoundingBoxedEntity* target)
{
	// TODO: update this to make it more generic
	if (Thumper* thumperResult = dynamic_cast<Thumper*>(target)) // render interaction text for item
	{
		this->renderItemInteractOverlay(THUMPER, thumperResult->getState() == Thumper::STATE::ACTIVATED); // highlight selectable item
	}

	if (NomadCharacter* character = dynamic_cast<NomadCharacter*>(target)) // render interaction text for a character
	{
		this->renderSpeakToCharacterOverlay(NOMAD);
	}

	if (MilitaryContainer* container = dynamic_cast<MilitaryContainer*>(target)) // render interaction text for a container
	{
		this->renderContainerOverlay(CONTAINER, 0);
	}
}
