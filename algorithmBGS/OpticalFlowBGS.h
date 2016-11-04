#ifndef OPTICALFLOWBGS_H
#define OPTICALFLOWBGS_H

#include <opencv2/video.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include "IBGS.h"

class OpticalFlowBGS: public IBGS
{
public:
    OpticalFlowBGS();
    ~OpticalFlowBGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    inline bool isFlowCorrect(cv::Point2f u);
    cv::Vec3b computeColor(float fx, float fy);
    void drawOpticalFlow(const cv::Mat_<cv::Point2f>& flow, cv::Mat& output, float maxmotion = -1);

private:
    bool firstTime;
    cv::Ptr<cv::DenseOpticalFlow> opticalFlow;
    cv::Mat preFrame;
    cv::Mat img_foreground;

    bool enableThreshold;
    int threshold;
    bool showOutput;

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // OPTICALFLOWBGS_H
