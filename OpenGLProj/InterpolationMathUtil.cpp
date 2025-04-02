#include "InterpolationMathUtil.h"

#define _USE_MATH_DEFINES
#include <exception>

#include "math.h"


float InterpolationMathUtil::easeInOutCosine(float t)
{
	if (t < 0.0 || t > 1.0) throw std::exception("t must be in range [0, 1]");
	return 1.0f - ((cos(t * M_PI) + 1.0f) / 2.0f);
}
