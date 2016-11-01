/*
 *M.-M. Cheng, N. J. Mitra, X. Huang, P.H.S. Torr S.-M. Hu. Global
 *   Contrast based Salient Region Detection. IEEE PAMI, 2014.
 */
#ifndef RCSALIENCY_H
#define RCSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <map>
#include <vector>
#include <functional>
#include <algorithm>
#include <limits>
#include <cmath>
#include <string>

#include "isaliency.h"

#define EPS 1e-200

typedef std::pair<float, int> CostfIdx;

struct Region{
    Region() { pixNum = 0; ad2c = cv::Point2d(0, 0);}
    int pixNum;  // Number of pixels
    std::vector<CostfIdx> freIdx;  // Frequency of each color and its index
    cv::Point2d centroid;
    cv::Point2d ad2c; // Average distance to image center
};

class RCSaliency: public ISaliency
{
public:
    RCSaliency();
    ~RCSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

    //基于区域对比度
    cv::Mat getRCSaliency(const cv::Mat &src, double sigmaDist=0.4,
                                      double segK=50, int segMinSize=200, double segSigma=0.5);

private:

    //颜色空间量化
    int quantize(const cv::Mat& srcFC3, cv::Mat &indexIC1, cv::Mat &colorFC3, cv::Mat &colorNum, double ratio=0.95, const int colorNums[3]=DefaultNums);
    void buildRegions(cv::Mat& regIndexIC1, std::vector<Region> &regs, cv::Mat &colorIndexIC1, int colorNum);
    void regionContrast(const std::vector<Region> &regs, cv::Mat &colorFC3, cv::Mat& regSaliencyDC1, double sigmaDist);
    void smoothByHist(cv::Mat &srcFC3, cv::Mat &saliencyFC1, float delta);
    void smoothByRegion(cv::Mat &saliencyFC1, cv::Mat &segIndexIC1, int regNum, bool bNormalize = true);
    // Get border regions, which typically corresponds to background region
    cv::Mat getBorderRegions(cv::Mat &indexIC1, int regNum, double ratio = 0.02, double thr = 0.3);
    //颜色空间平滑
    void RCSaliency::smoothSaliency(cv::Mat &colorNumIC1, cv::Mat &saliencyMapFC1, float delta, const std::vector<std::vector<CostfIdx> > &similar);

private:
    static const int DefaultNums[3];

};

#endif // RCSALIENCY_H
