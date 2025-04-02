#ifndef FONT_MINE_H
#define FONT_MINE_H

#include <map>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Shader.h"

struct Character
{
	unsigned int textureId; // id handle of the glyph texture
	glm::ivec2 size; // size of the glyph
	glm::ivec2 bearing; // offset from baseline to left/top of the glyph
	unsigned int advance; // offset to advance to next glyph
};


class Font
{
public:
	Font(const std::string &fontPath, Shader* fontShader);

	void renderText(std::string text, float x, float y, float scale, glm::vec3 color);

private:
	std::map<char, Character> _characters;
	unsigned int _VBO;
	unsigned int _VAO;
	Shader* _fontShader;
};

#endif