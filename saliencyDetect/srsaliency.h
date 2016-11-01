/*
 * X. Hou and L. Zhang.
 * Saliency detection: A spectral residual approach.
 * In IEEE CVPR 2007, 2007.
 */
#ifndef SRSALIENCY_H
#define SRSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <vector>

#include "isaliency.h"

class SRSaliency: public ISaliency
{
public:
    SRSaliency();
    ~SRSaliency();

    cv::Mat getSaliency(const cv::Mat &src);
    cv::Mat getSRSaliency1(const cv::Mat &src);

private:
    // absAngle: Calculate magnitude and angle of vectors.
    void absAngle(cv::Mat& complex32FC2, cv::Mat& magnitude32FC1, cv::Mat& angle32FC1);
    // GetCmplx: Get a complex value image from it's magnitude and angle.
    void getComplex(cv::Mat& magnitude32F, cv::Mat& angle32F, cv::Mat& complexFC2);
};

#endif // SRSALIENCY_H
