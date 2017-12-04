/*
 * This is only a implementation from article:
 * "Global Contrast Based Salient Region Detection,"
 * M. M. Cheng, N. J. Mitra, X. Huang, P. H. S. Torr and S. M. Hu,
 * in IEEE Transactions on Pattern Analysis and Machine Intelligence, vol. 37, no. 3, pp. 569-582, March 1 2015.
 */
#ifndef GCSALIENCY_H
#define GCSALIENCY_H

#include "cluster/CmAPCluster.h"
#include "cluster/colorquantize.h"
#include "cluster/apcluster.h"
#include "cluster/gmm.h"

#include "../isaliency.h"

#define DEFAULT_GMM_NUM 15

typedef std::pair<double, int> CostIdx;
typedef std::pair<float, int> CostfIdx;
typedef std::pair<int, int> CostiIdx;
typedef std::vector<CostIdx> CostIdxV;
typedef std::vector<CostfIdx> CostfIdxV;
typedef std::vector<CostiIdx> CostiIdxV;

class GCSaliency : public ISaliency
{
public:
    GCSaliency(bool isCSD = true);
    ~GCSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

    static double dummyD;

private:

    void histgramGMMs(const cv::Mat &src);

    void getCSD(std::vector<double> &csd, std::vector<double> &cD);
    void getGU(std::vector<double>& gc, std::vector<double> &cD, double sigmaDist, double dominate = 30);
    cv::Mat getSaliencyFromGMMs(std::vector<double> &val, bool isNormlize = true);
    void reScale(std::vector<double>& saliencyVals, double minRatio = 0.01);
    void mergeGMMs();
    void MergeGMMs();
    // return the spatial variance. Pixel locations are normalized to [0, 1]
    double spatialVar(cv::Mat& map1f, double &cD=dummyD);


private: // Data values
    CmGMM gmm;
    cv::Mat imgFC3;

    // L0: pixel layer, each Mat variable has the same size as image
    cv::Mat histBinIndexIC1; // The histogram bin index of each pixel, associating L0 and L1
    std::vector<cv::Mat> pixelSaliencyComponentFC1; // Pixel saliency of component i, type CV_32FC1 (denoted 1f)

    // L1: histogram layer, each Mat variable has the same size as histogram
    std::vector<cv::Mat> histBinSaliencyComponentFC1; // Histogram bin saliency of component i

    // L2: GMM layer, each vector has the size of GMM numbers
    int NUM;
    std::vector<cv::Vec3f> gmmColors; // Colors of GMM components
    std::vector<double> gmmWeight; // Weight of each GMM component
    std::vector<int> clusteredIndex; // Bridge between L2 and L3
    std::vector<double> colorSpatialDistribution; // color spatial distribution
    std::vector<double> globalContrast; // global contrast
    std::vector<double> finalSaliency; // Final saliency

    // L3: clustered layer
    int NUM_Merged;
    std::vector<cv::Mat> probabilityGMMComponentsFC1; // Probability of GMM components, after merged

    bool isGetCSD;
};

#endif // GCSALIENCY_H
