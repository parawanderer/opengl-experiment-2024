#include "SoundManager.h"

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
	this->_listenerPos = convert(listenerPosition);
	this->_lookDir = convert(lookDirection);

	this->_soundEngine->setListenerPosition(
		this->_listenerPos,
		this->_lookDir,
		this->_velPerSecond,
		this->_upVector
	);
}

irrklang::ISound* SoundManager::playTracked(const std::string& trackName, bool loop)
{
	auto fullPath = this->getFullPath(trackName);
	irrklang::ISound* result = this->_soundEngine->play2D(fullPath.c_str(), loop, false, true);
	return result;
}

irrklang::ISound* SoundManager::playTracked3D(const std::string& trackName, bool loop, const glm::vec3& soundPos)
{
	auto fullPath = this->getFullPath(trackName);
	irrklang::ISound* result = this->_soundEngine->play3D(
		fullPath.c_str(), 
		convert(soundPos),
		loop,
		false,
		true
		);
	return result;
}

irrklang::vec3df SoundManager::convert(const glm::vec3& position)
{
	return irrklang::vec3df(position.x, position.y, -position.z);
}

std::string SoundManager::getFullPath(const std::string& fileName)
{
	return this->_basePath + "/" + fileName;
}
