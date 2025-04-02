#ifndef UITEXTRENDERER_MINE_H
#define UITEXTRENDERER_MINE_H
#include "Font.h"

/**
 * \brief Renders UI-related elements for the screen. Should be one of the final elements to be rendered to the screen.
 */
class UITextRenderer
{
public:
	UITextRenderer(Font* uiFont);

	void renderItemInteractOverlay(const char* itemName, int currentWidth, int currentHeight, bool isActive);

	void renderCarriedItemInfo(const char* itemName, int currentWidth, int currentHeight);

	void renderMainUIOverlay(const glm::vec3 cameraPos, int currentWidth, int currentHeight);
private:
	Font* _font;
};

#endif