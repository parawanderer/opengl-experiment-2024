#ifndef INTERPOLATIONMATHUTIL_MINE_H
#define INTERPOLATIONMATHUTIL_MINE_H

namespace InterpolationMathUtil
{
	/**
	 * \brief	map a linearly interpolated t to a ease-in-out interpolated output (via cosine function)
	 * \param t			must be in range [0, 1] 
	 * \return			translation into the cosine value
	 */
	float easeInOutCosine(float t);
}

#endif