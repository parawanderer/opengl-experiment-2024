#include "UITextRenderer.h"

#include "Colors.h"

UITextRenderer::UITextRenderer(Font* uiFont) : _font(uiFont)
{}

void UITextRenderer::renderItemInteractOverlay(const char* itemName, int currentWidth, int currentHeight, bool isActive)
{
	// text overlay (inspired by bethesda games because it is simple to do)
	// obviously the font isn't really all that nice looking.
	// but I don't feel like fiddling with fonts/implementing distance-fields *again*,
	// but this time for fonts.
	this->_font->renderText(
		itemName,
		(currentWidth * (5.0f / 8.0f)),
		(currentHeight / 2.0f),
		0.6f,
		Colors::WHITE
	);

	this->_font->renderText(
		"C) TAKE",
		(currentWidth * (5.0f / 8.0f)),
		(currentHeight / 2.0f) - 40.0f,
		0.5f,
		Colors::WHITE
	);

	this->_font->renderText(
		isActive ? "E) DEACTIVATE" : "E) ACTIVATE",
		(currentWidth * (5.0f / 8.0f)),
		(currentHeight / 2.0f) - 80.0f,
		0.5f,
		Colors::WHITE
	);
}

void UITextRenderer::renderCarriedItemInfo(const char* itemName, int currentWidth, int currentHeight)
{
	this->_font->renderText(
		itemName,
		(currentWidth - 150.0f),
		55.0f,
		0.45f,
		Colors::WHITE
	);

	this->_font->renderText(
		"E) DROP",
		(currentWidth - 150.0f),
		30.0f,
		0.35f,
		Colors::WHITE
	);
}

void UITextRenderer::renderMainUIOverlay(const glm::vec3 cameraPos, int currentWidth, int currentHeight)
{
	this->_font->renderText(
		std::format("X:{:.2f} Y:{:.2f}, Z:{:.2f}", cameraPos.x, cameraPos.y, cameraPos.z),
		25.0f,
		currentHeight - 25.0f,
		0.5f,
		Colors::WHITE
	);

	// actually going to do a trick to get a center-of-the-screen "." indicator like in this game: https://youtu.be/6QZAhsxwNU0?si=J7eN6p2nRvc4Z_tW
	// mostly because I think it is useful/helpful for object-picking purposes
	this->_font->renderText(".", currentWidth / 2.0f, currentHeight / 2.0f, 0.5f, Colors::WHITE);
}
