#ifndef FRAMEREQUESTOR_MINE_H
#define FRAMEREQUESTOR_MINE_H

/**
 * \brief Any class that requests new frames. We have a lot of these,
 * and being able to service all of them in a single loop is convenient
 */
class FrameRequester
{
public:
	virtual ~FrameRequester() = default;
	virtual void onNewFrame() = 0;
};

#endif