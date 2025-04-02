#ifndef COLORS_MINE_H
#define COLORS_MINE_H
#include <regex>
#include <glm/vec3.hpp>

namespace Colors
{
	constexpr glm::vec3 WHITE(1.0f, 1.0f, 1.0f);
	constexpr glm::vec3 BLACK(0.0f, 0.0f, 0.0f);
	constexpr glm::vec3 GREY(0.5f, 0.5f, 0.5f);
	constexpr glm::vec3 RED(1.0f, 0.0f, 0.0f);
	constexpr glm::vec3 GREEN(0.0f, 1.0f, 0.0f);
	constexpr glm::vec3 BLUE(0.0f, 0.0f, 1.0f);
	constexpr glm::vec3 SAND(0.81f, 0.54f, 0.28f);
	constexpr glm::vec3 YELLOW(1.0f, 1.0f, 0.0f);
	constexpr glm::vec3 CUSTOM_BLUE(0.45f, 0.49f, 0.61f);

	/**
	 * \brief	Convert a hex color code https://htmlcolorcodes.com/color-picker/ to a 0-1 range color vector. For convenience/copy pasting purposes
	 *
	 * \param hex  color code string. The format must match `#xxxxxx` and be valid, else an exception will be thrown
	 *
	 * \return		color vector
	 */
	inline glm::vec3 hexToVec3(const char* hex)
	{
		static const std::regex formatRegex("^#[a-fA-F0-9]{6}$");
		if (!std::regex_match(hex, formatRegex))
			throw std::exception("Invalid hex color code provided for conversion");

		char rPart[2] = { hex[1], hex[2] };
		auto r = std::stoi(rPart, nullptr, 16) / 255.0f;

		char gPart[2] = { hex[3], hex[4] };
		auto g = std::stoi(gPart, nullptr, 16) / 255.0f;

		char bPart[2] = { hex[5], hex[6] };
		auto b = std::stoi(bPart, nullptr, 16) / 255.0f;

		return glm::vec3(r, g, b);
	}
}

#endif