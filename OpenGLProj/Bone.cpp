#include "Bone.h"

#include "MathConversionUtil.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

Bone::Bone(const std::string& name, const int boneId, const aiNodeAnim* channel)
:
_numPositions(channel->mNumPositionKeys),
_numRotations(channel->mNumRotationKeys),
_numScalings(channel->mNumScalingKeys),
_name(name),
_id(boneId),
_localTransforms(glm::mat4(1.0f), glm::vec3(0.0f), glm::quat(0.0f, 0.0f, 0.0f, 0.0f), glm::vec3(0.0f))
{
	for (int positionIndex = 0; positionIndex < this->_numPositions; ++positionIndex)
	{
		const aiVectorKey& data = channel->mPositionKeys[positionIndex];
		const aiVector3D& aiPosition = data.mValue;
		const float timeStamp = data.mTime;
		this->_positions.push_back({
			{.timeStamp = timeStamp },
			MathConversionUtil::convert(aiPosition)
		});
	}

	for (int rotationIndex = 0; rotationIndex < this->_numRotations; ++rotationIndex)
	{
		const aiQuatKey& data = channel->mRotationKeys[rotationIndex];
		const aiQuaternion& aiOrientation = data.mValue;
		const float timeStamp = data.mTime;
		this->_rotations.push_back({
			{ .timeStamp = timeStamp },
			MathConversionUtil::convert(aiOrientation)
		});
	}

	for (int scalingIndex = 0; scalingIndex < this->_numScalings; ++scalingIndex)
	{
		const aiVectorKey& data = channel->mScalingKeys[scalingIndex];
		const aiVector3D& aiScale = data.mValue;
		const float timeStamp = data.mTime;
		this->_scales.push_back({
			{ .timeStamp = timeStamp },
			MathConversionUtil::convert(aiScale)
		});
	}
}

void Bone::update(float animationTime)
{
	auto [translationM, translation] = this->interpolatePosition(animationTime);
	auto [rotationM, rotation] = this->interpolateRotation(animationTime);
	auto [scaleM, scale] = this->interpolateScaling(animationTime);
	const glm::mat4 combined = translationM * rotationM * scaleM;

	this->_localTransforms = {
		.combined = combined,
		.translation = translation,
		.rotation = rotation,
		.scale = scale
	};
}

glm::mat4 Bone::getLocalTransform() const
{
	return this->_localTransforms.combined;
}

const InterpolatedTransform& Bone::getLocalInterpolatedTransforms() const
{
	return this->_localTransforms;
}

const std::string& Bone::getBoneName() const
{
	return this->_name;
}

int Bone::getBoneId() const
{
	return this->_id;
}

int Bone::getPositionIndex(const float animationTime) const
{
	for (int i = 0; i < this->_numPositions - 1; ++i)
	{
		if (animationTime < this->_positions[i + 1].timeStamp)
			return i; // first index of a keyframe that happens after the specified animation time
	}
	throw std::exception("Invalid animation time state");
}

int Bone::getRotationIndex(const float animationTime) const
{
	for (int i = 0; i < this->_numRotations - 1; ++i)
	{
		if (animationTime < this->_rotations[i + 1].timeStamp)
			return i; // first index of a keyframe that happens after the specified animation time
	}
	throw std::exception("Invalid animation time state");
}

int Bone::getScaleIndex(const float animationTime) const
{
	for (int i = 0; i < this->_numScalings - 1; ++i)
	{
		if (animationTime < this->_scales[i + 1].timeStamp)
			return i; // first index of a keyframe that happens after the specified animation time
	}
	throw std::exception("Invalid animation time state");
}

glm::mat4 Bone::interpolateBetweenTwo(
	const InterpolatedTransform& first, 
	const InterpolatedTransform& second,
	float interpolationFactorFirst)
{
	// quite similar to the separate interpolations, btw!

	glm::vec3 finalPos = glm::mix(first.translation, second.translation, interpolationFactorFirst);
	glm::quat finalRotation = glm::normalize(glm::slerp(first.rotation, second.rotation, interpolationFactorFirst));
	glm::vec3 finalScale = glm::mix(first.scale, second.scale, interpolationFactorFirst);

	glm::mat4 translation = glm::translate(glm::mat4(1.0f), finalPos);
	glm::mat4 rotation = glm::toMat4(finalRotation);
	glm::mat4 scale = glm::scale(glm::mat4(1.0f), finalScale);

	return translation * rotation * scale;
}

// note from self: this entire thing is equivalent to how I'm handling movement displacement interpolation for my "Nomad" characters
// (though I am considering doing some ease-in-out interpolation for movement because that's more accurate to how objects move in real
// life for my character movement)
float Bone::getScaleFactor(const float lastTimeStamp, const float nextTimeStamp, const float animationTime)
{
	const float midWayLength = animationTime - lastTimeStamp;
	const float framesDiff = nextTimeStamp - lastTimeStamp;
	const float scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

std::tuple<glm::mat4, glm::vec3> Bone::interpolatePosition(const float animationTime) const
{
	if (this->_numPositions == 1) 
		return {glm::translate(glm::mat4(1.0f), this->_positions[0].position), this->_positions[0].position };

	const int p0Index = this->getPositionIndex(animationTime);
	const int p1Index = p0Index + 1;

	const KeyPosition& pos0 = this->_positions[p0Index];
	const KeyPosition& pos1 = this->_positions[p1Index];

	const float scaleFactor = this->getScaleFactor(pos0.timeStamp, pos1.timeStamp, animationTime);

	glm::vec3 finalPos = glm::mix(pos0.position, pos1.position, scaleFactor);
	return {glm::translate(glm::mat4(1.0f), finalPos), finalPos};
}

std::tuple<glm::mat4, glm::quat> Bone::interpolateRotation(const float animationTime) const
{
	if (this->_numRotations == 1) 
		return {glm::toMat4(glm::normalize(this->_rotations[0].orientation)), glm::normalize(this->_rotations[0].orientation) };

	const int r0Index = this->getRotationIndex(animationTime);
	const int r1Index = r0Index + 1;

	const KeyRotation& rot0 = this->_rotations[r0Index];
	const KeyRotation& rot1 = this->_rotations[r1Index];

	const float scaleFactor = this->getScaleFactor(rot0.timeStamp, rot1.timeStamp, animationTime);

	glm::quat finalRotation = glm::slerp(rot0.orientation, rot1.orientation, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return {glm::toMat4(finalRotation), finalRotation};
}

std::tuple<glm::mat4, glm::vec3> Bone::interpolateScaling(const float animationTime) const
{
	if (this->_numScalings == 1) 
		return {glm::scale(glm::mat4(1.0f), this->_scales[0].scale), this->_scales[0].scale };

	const int s0Index = this->getScaleIndex(animationTime);
	const int s1Index = s0Index + 1;

	const KeyScale& scale0 = this->_scales[s0Index];
	const KeyScale& scale1 = this->_scales[s1Index];

	const float scaleFactor = this->getScaleFactor(scale0.timeStamp, scale1.timeStamp, animationTime);

	glm::vec3 finalScale = glm::mix(scale0.scale, scale1.scale, scaleFactor);
	return {glm::scale(glm::mat4(1.0f), finalScale), finalScale};
}
