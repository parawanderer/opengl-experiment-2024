#include "SoundManager.h"

#include "AudioUtilities.h"

SoundManager::SoundManager(const std::string& basePath)
:
_soundEngine(irrklang::createIrrKlangDevice()),
_basePath(basePath),
_listenerPos(irrklang::vec3df(0.0, 0.0, 0.0)),
_lookDir(irrklang::vec3df(0.0, 0.0, -1.0)),
_velPerSecond(irrklang::vec3df(0.0, 0.0, 0.0)),
_upVector(irrklang::vec3df(0.0, 1.0, 0.0))
{
	this->_soundEngine->setListenerPosition(
		this->_listenerPos, // to be updated
		this->_lookDir, // to be updated
		this->_velPerSecond, // TODO: needed?
		this->_upVector
	);
}

void SoundManager::playOnce(const std::string& trackName)
{
	auto fullPath = this->getFullPath(trackName);
	this->_soundEngine->play2D(fullPath.c_str());
}

void SoundManager::updateListenerPos(const glm::vec3& listenerPosition, const glm::vec3& lookDirection)
{
	this->_listenerPos = AudioUtilities::convert(listenerPosition);
	this->_lookDir = AudioUtilities::convert(lookDirection);

	this->_soundEngine->setListenerPosition(
		this->_listenerPos,
		this->_lookDir,
		this->_velPerSecond,
		this->_upVector
	);
}

AudioPlayer SoundManager::playTracked3D(const std::string& trackName, bool loop, const glm::vec3& soundPos)
{
	auto fullPath = this->getFullPath(trackName);
	irrklang::ISound* result = this->_soundEngine->play3D(
		fullPath.c_str(),
		AudioUtilities::convert(soundPos),
		loop,
		false,
		true
	);
	return AudioPlayer(result);
}

AudioPlayer SoundManager::playTracked2D(const std::string& trackName, bool loop)
{
	auto fullPath = this->getFullPath(trackName);
	irrklang::ISound* result = this->_soundEngine->play2D(fullPath.c_str(), loop, false, true);
	return AudioPlayer(result);
}

std::string SoundManager::getFullPath(const std::string& fileName)
{
	return this->_basePath + "/" + fileName;
}
