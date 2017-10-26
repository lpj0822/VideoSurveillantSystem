#ifndef SALIENCYBGS_H
#define SALIENCYBGS_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include "ivibebgs.h"
#include "saliencyDetect/isaliency.h"
#include "saliencyDetect/hcsaliency.h"
#include "imageFiltering/imagefilter.h"
#include "../IBGS.h"


class SaliencyBGS : public IBGS
{
public:
    SaliencyBGS();
    ~SaliencyBGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    void init();
    void roughBGEstimation(cv::Mat &bg);//背景估计
    void calculateForeground(cv::Mat &myForeground);//计算前景

private:
    int bgNumberFrame;//估计背景帧数计数
    std::vector<cv::Mat> bgSamples;//统计背景

    IViBeBGS *vibe;//vibe提取背景
    cv::Mat elementBGS;//自定义核，用于背景检测形态学操作
    cv::Mat img_foreground;//前景

    ISaliency* saliency;//显著性检测
    cv::Mat saliencyMap;//显著图

    ImageFilter *imageFilter;//图像滤波

    bool firstRun;//第一次运行

    bool showOutput;
    float salienctThreshold;//显著性阈值
    int minArea;//最小面积
    int threshold;//前景分割阈值
    float alpha;//自适应阈值缩放比例

private:
    void saveConfig();
    void loadConfig();
};

#endif // SALIENCYBGS_H
