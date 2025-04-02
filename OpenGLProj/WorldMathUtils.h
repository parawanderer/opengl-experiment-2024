#ifndef WORLDMATHUTILS_MINE_H
#define WORLDMATHUTILS_MINE_H
#include <vector>

#include "SphericalBoundingBoxedEntity.h"

namespace WorldMathUtils
{
	/**
	 * \brief	Checks which of the objects in listOfIntersectableObjects is the closest to the given cameraPos with the given cameraFront (where cameraFront acts as the ray in the world)
	 * 
	 * \param listOfIntersectableObjects		List of objects to check against
	 * \param cameraPos							Position of the camera/player in the world
	 * \param cameraFront						directional (normalized!!) ray being cast by the camera into the world. This is the direction that we are looking into
	 * \param maxDistance						distance after which intersections are ignored.
	 *
	 * \return									A pointer to the object that is closest within the defined limits, out of the input list. Nullptr if no such object exists.
	 *
	 * **Author's note**:
	 *
	 * I mainly referenced this article when writing this: https://antongerdelan.net/opengl/raycasting.html
	 * and realised the ray vector that we use here is just the same thing as the cameraPos of the camera view, at the very center of the screen.
	 * so the inverse computation is unnecessary.
	 */
	inline SphericalBoundingBoxedEntity* findClosestIntersection(
		const std::vector<SphericalBoundingBoxedEntity*>& listOfIntersectableObjects, 
		const glm::vec3& cameraPos, 
		const glm::vec3& cameraFront, 
		float maxDistance = FLT_MAX)
	{
		SphericalBoundingBoxedEntity* res = nullptr;
		float tClosest = FLT_MAX; // <- I'll say it's unlikely that we'd ever actually intersect something at this distance in my program. But the idea is to do tClosest = inf to reduce this by the min checks

		for (unsigned int i = 0; i < listOfIntersectableObjects.size(); ++i)
		{
			// https://antongerdelan.net/opengl/raycasting.html + https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
			// referenced for sphere intersection logic

			const float radiusSphere = listOfIntersectableObjects[i]->getRadiusSphericalBoundingBox();
			const glm::vec3 worldCenterSphere = listOfIntersectableObjects[i]->getBoundMidPoint();

			const glm::vec3 tmp = cameraPos - worldCenterSphere;
			const float b = glm::dot(cameraFront, tmp);
			const float b2 = b * b;
			const float c = glm::dot(tmp, tmp) - (radiusSphere * radiusSphere);

			const float sphereIntersectTester = b2 - c;


			float t0 = -1;
			float t1 = -1;

			//if (sphereIntersectTester < 0) // no intersect
			if (sphereIntersectTester > 0)
			{
				// 2 intersections straight through the sphere (there is a "closer" and a "farther" intersection point here)
				t0 = -b + sqrt(b2 - c);
				t1 = -b - sqrt(b2 - c);
			}
			else if (sphereIntersectTester == 0.0f)
			{
				// TODO: floating point percision loss is a thing. should we test using abs(sphereIntersectTester) <= 0.001 (<- error) ?
				// I think doing this could yield nonsensical results when we solve the polynomial below, though.
				// I think it is basically not really an issue since most intersections where a player wants to pick up some item will
				// be near the middle of the object, and the player hitting the side of the object is quite unlikely.
				
				t0 = -b + sqrt(b2 - c); // single intersection point around the outside of the sphere
				t1 = t0;
			}


			for (const float tPossibility : {t0, t1})
			{
				if (tPossibility >= 0 && tPossibility < tClosest) // found a new closest item
				{
					tClosest = tPossibility;
					res = listOfIntersectableObjects[i];
				}
			}
		}

		return tClosest <= maxDistance ? res : nullptr;
	}

	/**
	 * \brief Computes a (normalised) ray in the world by reversing all transformations that we perform when rendering to the world.
	 *
	 * References:
	 * - https://antongerdelan.net/opengl/raycasting.html
	 * - https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-sphere-intersection.html
	 *
	 * This is identical to cameraFront in our program (due to center of screen + direction + normalisation)
	 *
	 * \return a normalised vector representing the ray. (Equivalent to cameraFront with the player/noclip camera!)
	 */
	inline glm::vec3 computeRayWorld(
		const float mouseX, 
		const float mouseY, 
		const glm::mat4& projection, 
		const glm::mat4& view, 
		const int windowWidth, 
		const int windowHeight)
	{
		// ray picking
		glm::vec3 rayNDC(
			(2.0f * mouseX) / windowWidth - 1.0f,
			1.0f - (2.0f * mouseY) / windowHeight,
			1.0f
		);
		glm::vec4 rayClip = glm::vec4(rayNDC.x, rayNDC.y, -1.0f, 1.0f);
		glm::vec4 rayEye = glm::inverse(projection) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
		glm::vec3 rayWorld(glm::inverse(view) * rayEye);
		rayWorld = glm::normalize(rayWorld);

		return rayWorld;
	}

	/**
	 * \brief Compute direction vector given pitch and yaw
	 */
	inline glm::vec3 computeDirection(float pitch, float yaw)
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		return glm::normalize(direction);
	}

	/**
	 * \brief Get a basic 2D rotation matrix. See: https://en.wikipedia.org/wiki/Rotation_matrix
	 * This is probably the only time I use my own math in the project at this point, rather than using the glm functions.
	 */
	inline glm::mat2 getRotationMatrix2D(const float angleRadians)
	{
		return glm::mat2(
			cos(angleRadians), -sin(angleRadians),
			sin(angleRadians), cos(angleRadians)
		);
	}
}

#endif 