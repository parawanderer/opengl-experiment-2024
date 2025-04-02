#ifndef BONE_MINE_H
#define BONE_MINE_H

#include <string>
#include <vector>
#include <assimp/anim.h>
#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>


struct KeyItemBase
{
	// timestamp of the operation
	float timeStamp;
};

/**
 * \brief Position in a keyframe (as in part of an animation "key" frame between which we will interpolate)
 */
struct KeyPosition : public KeyItemBase
{
	glm::vec3 position;
};

/**
 * \brief Rotation in the keyframe (see KeyPosition)
 */
struct KeyRotation : public KeyItemBase
{
	glm::quat orientation;
};

/**
 * \brief Scaling in the keyframe (see KeyPosition)
 * Not typically used for animating human characters but w/e
 */
struct KeyScale : public KeyItemBase
{
	glm::vec3 scale;
};


/**
 * Single bone which reads all keyframes data from aiNodeAnim. It interpolates between its keys i.e Translation,Scale & Rotation based on the current animation time.
 *
 * main ref: https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
 */
class Bone
{
public:
	Bone(const std::string& name, int boneId, const aiNodeAnim* channel);

	/**
	 * \brief Interpolates between positions, rotations and scaling keys based on the current time of the animation
	 *		  and prepares the local transformation matrix by combining all keys' transformations
	 * \param animationTime		Current time of the animation to update to
	 */
	void update(float animationTime);

	glm::mat4 getLocalTransform() const;
	const std::string& getBoneName() const;
	int getBoneId() const;

	/**
	 * \brief Get the current index on _positions to interpolate to based on the current animation time
	 * \param animationTime			Animation time at which we must get the Key Position's index
	 *
	 *	raises error when no appropriate time can be found for the timestamp
	 */
	int getPositionIndex(float animationTime) const;

	/**
	 * \brief Get the current index on _rotations to interpolate to based on the current animation time
	 * \param animationTime			Animation time at which we must get the Key Rotation's index
	 *
	 *	raises error when no appropriate time can be found for the timestamp
	 */
	int getRotationIndex(float animationTime) const;

	/**
	 * \brief Get the current index on _scales to interpolate to based on the current animation time
	 * \param animationTime			Animation time at which we must get the Key Scale's index
	 *
	 *	raises error when no appropriate time can be found for the timestamp
	 */
	int getScaleIndex(float animationTime) const;

private:
	std::vector<KeyPosition> _positions;
	std::vector<KeyRotation> _rotations;
	std::vector<KeyScale> _scales;

	int _numPositions;
	int _numRotations;
	int _numScalings;

	glm::mat4 _localTransform;
	std::string _name;
	int _id; // bone id

	/**
	 * \return Returns normalized value for Lerp & Slerp
	 */
	static float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);

	/**
	 * \brief figures out which position keys to interpolate between and performs the interpolation. Returns the translation matrix.
	 * \param animationTime			Time at which the animation is being computed
	 * \return						The translation matrix for this animation time
	 */
	glm::mat4 interpolatePosition(float animationTime) const;

	/**
	 * \brief figures out which position keys to interpolate between and performs the interpolation. Returns the rotation matrix.
	 * \param animationTime			Time at which the animation is being computed
	 * \return						The rotation matrix for this animation time
	 */
	glm::mat4 interpolateRotation(float animationTime) const;

	/**
	 * \brief figures out which position keys to interpolate between and performs the interpolation. Returns the scale matrix.
	 * \param animationTime			Time at which the animation is being computed
	 * \return						The scale matrix for this animation time
	 */
	glm::mat4 interpolateScaling(float animationTime) const;
};

#endif