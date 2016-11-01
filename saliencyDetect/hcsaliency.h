/*
 *M.-M. Cheng, N. J. Mitra, X. Huang, P.H.S. Torr S.-M. Hu. Global
 *   Contrast based Salient Region Detection. IEEE PAMI, 2014.
 */
#ifndef HCSALIENCY_H
#define HCSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>
#include <string>

#include "isaliency.h"

typedef std::pair<float, int> CostfIdx;

class HCSaliency: public ISaliency
{
public:
    HCSaliency();
    ~HCSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

private:
    // Histogram based Contrast
    void hcSaliency(cv::Mat &binColorFC3, cv::Mat &colorNumIC1, cv::Mat &colorSaliency);
    //颜色量化
    int quantize(const cv::Mat& srcFC3, cv::Mat &indexIC1, cv::Mat &colorFC3, cv::Mat &colorNum, double ratio=0.95,
                 const int colorNums[3]= DefaultNums);
    //颜色空间平滑
    void smoothSaliency(cv::Mat &saliencyMapFC1, float delta, const std::vector<std::vector<CostfIdx> > &similar);

private:
    static const int DefaultNums[3];
};

#endif // HCSALIENCY_H
