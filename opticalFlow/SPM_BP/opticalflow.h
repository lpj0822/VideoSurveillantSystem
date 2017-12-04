#ifndef OPTICALFLOW_H
#define OPTICALFLOW_H

#include <opencv2/core.hpp>
#include "Common_Datastructure_Header.h"

class OpticalFlow
{
public:
    OpticalFlow();
    ~OpticalFlow();

    cv::Mat runOpticalFlowEstimator(const cv::Mat &frame1, const cv::Mat frame2);

private:

    void opticalFlowEst(cv::Mat& im1, cv::Mat& im2, cv::Mat_<cv::Vec2f> &flow12, spm_bp_params* params);
    void occMapEst(cv::Mat_<cv::Vec2f> &flow12, cv::Mat_<cv::Vec2f> &flow21, cv::Mat_<uchar>&occMap);
    void opticalFlowRefine(cv::Mat_<cv::Vec2f> &flow12, cv::Mat_<uchar> &occMap,const cv::Mat_<cv::Vec3b> &weightColorImg, cv::Mat_<cv::Vec2f> &flow_refined);

private:

    spm_bp_params params;
    int height1;
    int width1;
    int height2;
    int width2;
    float m_lambda;
    float m_tau;

    cv::Mat_<cv::Vec2f> flow12, flow21, flow_refined;
    cv::Mat_<uchar> occMap;

    cv::Mat_<cv::Vec3b> currentFlowColorLeft;
    cv::Mat_<cv::Vec3b> currentFlowColorRight;
};

#endif // OPTICALFLOW_H
