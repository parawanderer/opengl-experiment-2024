#ifndef WORLDTIMEMANAGER_MINE_H
#define WORLDTIMEMANAGER_MINE_H

/**
 * \brief Utility state wrapper for world time. onNewFrame() Should be called at the start of the main loop
 */
class WorldTimeManager
{
public:
	WorldTimeManager();

	void onNewFrame();

	float getDeltaTime() const;
	/**
	 * \return Current time in seconds
	 */
	float getCurrentTime() const;

private:
	float _deltaTime = 0.0f; // time between current and last frame
	float _lastFrame = 0.0f; // time of last frame
};

#endif