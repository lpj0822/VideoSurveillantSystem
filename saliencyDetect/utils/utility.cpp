#include "utility.h"

/**
 * 1D Gaussian, for a given x and sigma.
**/
float gaussian1d (float x, float sigma)
{
    return (1.0f / (sigma * sqrtf (2.0f * (3.14159265358979323846f))) * exp (-(x * x) / (2.0f * sigma * sigma)));
}
