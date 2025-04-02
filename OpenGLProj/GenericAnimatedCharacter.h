#ifndef GENERICANIMATEDCHARACTER_MINE_H
#define GENERICANIMATEDCHARACTER_MINE_H
#include "AnimatedEntity.h"
#include "DialogueRequester.h"
#include "RenderableGameObject.h"
#include "WorldTimeManager.h"

/**
 * \brief Provides common behaviours for animated characters
 */
class GenericAnimatedCharacter : public AnimatedEntity, public DialogueRequester
{
public:
	GenericAnimatedCharacter(
		const WorldTimeManager* time, 
		RenderableGameObject* model, 
		AnimationSet* animations,
		const float animationTransitionTime,
		const glm::vec3& initialPosition,
		const float initialYaw, // we assume pitch is always 0 for now
		const float yawModelOffset,
		const glm::vec3& rotateYawOver
		);

	void draw(Shader& shader) override;

	glm::vec3 getCurrentPosition() const override;
	virtual const glm::vec3& getCurrentFront() const;
	virtual float getYaw() const;
	virtual float getPitch() const;
	virtual void rotateTowards(const glm::vec3& position);

protected:
	Animator& getAnimator();
	RenderableGameObject* getRenderableGameModel() const;
	const WorldTimeManager* getTime() const;

	void setCurrentPosition(const glm::vec3& newPosition);
	void setCurrentFront(const glm::vec3& newFront);
	void setYaw(const float newYaw);


	virtual void playAnimation(const std::string& animationName);
	virtual void playAnimationWithTransition(const std::string& animationName);
	virtual void updateAnimationInterpolationForFrame();

	virtual void updateModelTransform() = 0;

private:
	const WorldTimeManager* _time;

	RenderableGameObject* _model;
	Animator _animator;

	glm::vec3 _currentPosition;
	glm::vec3 _currentFront;
	float _yaw;

	const float _yawModelOffset; // TODO: look into why these aren't properly aligned between characters?
	const glm::vec3 _rotateYawOver; // TODO: ^

	const float _animationTransitionTime;

	float _animationInterpolationStart = 0.0f;
	std::string _animation1 = "";
	std::string _animation2 = "";
};

#endif