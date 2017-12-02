/*
*	Implemetation of the saliency detction method described in paper
*	"Exploit Surroundedness for Saliency Detection: A Boolean Map Approach",
*   Jianming Zhang, Stan Sclaroff, submitted to PAMI, 2014
*
*	Copyright (C) 2014 Jianming Zhang
*
*/
#ifndef BMSALIENCY_H
#define BMSALIENCY_H

#include <vector>
#include <opencv2/imgproc.hpp>

#include "isaliency.h"

class BMSaliency
{
public:
    BMSaliency();
    ~BMSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

private:

    std::vector<cv::Mat> mFeatureMaps;

    cv::Mat getAttentionMap(const cv::Mat& bm, int dilation_width_1, bool toNormalize, bool handle_border);
    void whitenFeatMap(const cv::Mat& img, float reg);
};

#endif // BMSALIENCY_H
