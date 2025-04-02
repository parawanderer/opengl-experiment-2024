#ifndef WORLDTIMEMANAGER_MINE_H
#define WORLDTIMEMANAGER_MINE_H

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