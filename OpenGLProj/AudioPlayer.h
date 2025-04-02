#ifndef AUDIOPLAYER_MINE_H
#define AUDIOPLAYER_MINE_H
#include <glm/vec3.hpp>
#include <irrKlang/ik_ISound.h>

/**
 * \brief 
 */
class AudioPlayer
{
public:
	AudioPlayer();

	AudioPlayer(irrklang::ISound* sound);

	AudioPlayer(const AudioPlayer& other);

	AudioPlayer(AudioPlayer&& other) noexcept;

	AudioPlayer& operator=(AudioPlayer&& other) noexcept;

	~AudioPlayer();

	void setPosition(const glm::vec3& soundPosition);

	bool hasAudio() const;

	/**
	 * \brief (For 3D sounds) Set minimum distance at which the sound can be heard
	 */
	void setMinimumDistance(const float minimumDistanceToHearSoundAtMaxVolume);

	/**
	 * \brief	Set volume of current sound
	 * \param volume	in range [0, 1]
	 */
	void setVolume(const float volume);

	/**
	 * \brief Stop the current sound, if any
	 */
	void stop();

	/**
	 * \brief Stop the current sound and release associated resources
	 */
	void stopAndRelease();

private:
	irrklang::ISound* _sound;
};

#endif
