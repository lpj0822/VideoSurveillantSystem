/*****************************************************************************
*	Implemetation of the saliency detction method described in paper
*	"Minimum Barrier Salient Object Detection at 80 FPS", Jianming Zhang,
*	Stan Sclaroff, Zhe Lin, Xiaohui Shen, Brian Price, Radomir Mech, ICCV,
*       2015
*
*	Copyright (C) 2015 Jianming Zhang
*******************************************************************************/

#ifndef MBSALIENCY_H
#define MBSALIENCY_H

#include <vector>

#include "isaliency.h"

class MBSaliency : public ISaliency
{
public:
    MBSaliency();
    ~MBSaliency();

    cv::Mat getSaliency(const cv::Mat& src);

private:

    cv::Mat fastMBS(const std::vector<cv::Mat> featureMaps);
    cv::Mat fastGeodesic(const std::vector<cv::Mat> featureMaps);

    void rasterScan(const cv::Mat& featMap, cv::Mat& map, cv::Mat& lb, cv::Mat& ub);
    void invRasterScan(const cv::Mat& featMap, cv::Mat& map, cv::Mat& lb, cv::Mat& ub);
    float getThreshForGeo(const cv::Mat& src);
    void rasterScanGeo(const cv::Mat& featMap, cv::Mat& map, float thresh);
    void invRasterScanGeo(const cv::Mat& featMap, cv::Mat& map, float thresh);

private:
    std::vector<cv::Mat> mFeatureMaps;
};

#endif // MBSALIENCY_H
