#ifndef SOUNDMANAGER_MINE_H
#define SOUNDMANAGER_MINE_H

#include <string>
#include <glm/vec3.hpp>
#include <irrKlang/irrKlang.h>

class SoundManager
{
public:
	SoundManager(const std::string& basePath);

	void playOnce(const std::string& trackName);

	void updateListenerPos(const glm::vec3& listenerPosition, const glm::vec3& lookDirection);

	irrklang::ISound* playTracked(const std::string& trackName, bool loop);

	irrklang::ISound* playTracked3D(const std::string& trackName, bool loop, const glm::vec3& soundPos);

	static irrklang::vec3df convert(const glm::vec3& position);
private:
	const std::string _basePath;
	irrklang::ISoundEngine* _soundEngine;

	irrklang::vec3df _listenerPos;
	irrklang::vec3df _lookDir;
	irrklang::vec3df _velPerSecond;
	irrklang::vec3df _upVector;

	std::string getFullPath(const std::string& fileName);
};

#endif