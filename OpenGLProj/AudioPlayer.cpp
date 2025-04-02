#include "AudioPlayer.h"

#include "AudioUtilities.h"

AudioPlayer::AudioPlayer() : _sound(nullptr)
{}

AudioPlayer::AudioPlayer(irrklang::ISound* sound) : _sound(sound)
{}

AudioPlayer::AudioPlayer(const AudioPlayer& other): _sound(other._sound)
{}

AudioPlayer::AudioPlayer(AudioPlayer&& other) noexcept : _sound(other._sound)
{}

AudioPlayer& AudioPlayer::operator=(AudioPlayer&& other) noexcept
{
	if (this == &other)
		return *this;
	this->stopAndRelease();
	this->_sound = other._sound;
	other._sound = nullptr;
	return *this;
}

AudioPlayer::~AudioPlayer()
{
	this->stopAndRelease();
}

bool AudioPlayer::hasAudio() const
{
	return this->_sound != nullptr;
}

void AudioPlayer::setMinimumDistance(const float minimumDistanceToHearSoundAtMaxVolume)
{
	if (!this->_sound) return;

	this->_sound->setMinDistance(minimumDistanceToHearSoundAtMaxVolume);
}

void AudioPlayer::setVolume(const float volume)
{
	if (!this->_sound) return;

	this->_sound->setVolume(volume);
}

void AudioPlayer::stop()
{
	if (!this->_sound) return;

	this->_sound->stop();
}

void AudioPlayer::setPosition(const glm::vec3& soundPosition)
{
	if (!this->_sound) return;

	this->_sound->setPosition(AudioUtilities::convert(soundPosition));
}

void AudioPlayer::stopAndRelease()
{
	if (this->_sound != nullptr)
	{
		this->_sound->stop();
		this->_sound->drop();
		this->_sound = nullptr;
	}
}