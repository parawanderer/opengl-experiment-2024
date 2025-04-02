#ifndef TERRAIN_MINE_H
#define TERRAIN_MINE_H
#include <string>
#include <vector>

class Terrain
{
public:
	Terrain(const std::string& sourceHeightMapPath, float yScaleMult, float yShift);

	/**
	 * \brief render mesh strip by strip
	 */
	void render();
private:
	std::vector<float> _vertices;
	std::vector<unsigned int> _indices;
	unsigned int _numStrips;
	unsigned int _numVertsPerStrip;
	unsigned int _VAO;
	unsigned int _VBO;
	unsigned int _EBO;
};

#endif