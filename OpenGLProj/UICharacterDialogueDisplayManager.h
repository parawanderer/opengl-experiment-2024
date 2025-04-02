#ifndef UICHARACTERDIALOGUEDISPLAYMANAGER_MINE_H
#define UICHARACTERDIALOGUEDISPLAYMANAGER_MINE_H
#include <string>
#include <glm/vec3.hpp>

#include "DialogueRequester.h"

/**
 * \brief Represents a manager that can serve dialogue. Dialogue must be shown based on priority rules.
 */
class UICharacterDialogueDisplayManager
{
public:
	/**
	 * \brief Abstract means of requesting dialogue to be shown on the screen
	 * \param speaker					Speaker of the dialogue. e.g. "Bob"
	 * \param spokenDialoge				The dialogue to be shown. e.g. "Hello there!"
	 * \param durationOfShowing			How long the dialogue should be shown for, in seconds!
	 * \param speakerPositionInWorld	The position of the speaker in the world if it is constant. If not use the second option of requestDialogue(). Dialogues will be replaced when a new dialogue is requested that is closer to the player.
	 */
	virtual void requestDialogue(const std::string& speaker, const std::string& spokenDialoge, const float durationOfShowing, const glm::vec3& speakerPositionInWorld) = 0;
	/**
	 * \brief Equivalent to requestDialogue but allows a dynamically positioned speaker.
	 * \param speaker						Speaker of the dialogue. e.g. "Bob"
	 * \param spokenDialoge					The dialogue to be shown. e.g. "Hello there!"
	 * \param durationOfShowing				How long the dialogue should be shown for, in seconds!
	 * \param speakerWithDynamicPosition	A pointer to an instance of a class that can provide its current position. This version of the method allows the requester to move over time,
	 *										thereby getting closer or further away from the player and losing priority to other requests.
	 */
	virtual void requestDialogue(const std::string& speaker, const std::string& spokenDialoge, const float durationOfShowing, DialogueRequester* speakerWithDynamicPosition) = 0;
};

#endif