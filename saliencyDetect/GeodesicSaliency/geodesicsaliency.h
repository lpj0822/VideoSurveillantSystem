/*
 * Y.Wei, F.Wen,W. Zhu, and J. Sun,
 * Geodesic saliency using background priors,
 * In ECCV, 2012.
 */
#ifndef GEODESICSALIENCY_H
#define GEODESICSALIENCY_H

#include "BGMapExtractor_Grid.h"
#include "../isaliency.h"

class GeodesicSaliency : public ISaliency
{
public:
    GeodesicSaliency();
    ~GeodesicSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

private:

    BGMapExtractor_Grid bgExtractor;
};

#endif // GEODESICSALIENCY_H
