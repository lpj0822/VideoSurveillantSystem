/*
 *R. Achanta, S. Hemami, F. Estrada and S. Susstrunk,
 * Frequency-tuned Salient Region Detection, IEEE CVPR, 2009.
 */
#ifndef FTSALIENCY_H
#define FTSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <vector>

#include "isaliency.h"

class FTSaliency: public ISaliency
{
public:
    FTSaliency();
    ~FTSaliency();

    cv::Mat getSaliency(const cv::Mat &src);
};

#endif // FTSALIENCY_H
