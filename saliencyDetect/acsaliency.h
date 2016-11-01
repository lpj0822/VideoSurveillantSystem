/*
 * R. Achanta, F. Estrada, P. Wils and S. Susstrunk,
 * Salient Region Detectionand Segmentation,
 * International Conference on Computer Vision Systems(ICVS '08), Vol. 5008, Springer Lecture Notes in Computer Science,
    pp. 66-75, 2008.
 */
#ifndef ACSALIENCY_H
#define ACSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <vector>

#include "isaliency.h"

class ACSaliency: public ISaliency
{
public:
    ACSaliency();
    ~ACSaliency();

    cv::Mat getSaliency(const cv::Mat &src);
};

#endif // ACSALIENCY_H
