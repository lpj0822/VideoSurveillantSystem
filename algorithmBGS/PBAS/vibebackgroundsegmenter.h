#ifndef VIBEBACKGROUNDSEGMENTER_H
#define VIBEBACKGROUNDSEGMENTER_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "../IBGS.h"
#include "vibe.h"

class VibeBackgroundSegmenter: public IBGS
{
public:
    VibeBackgroundSegmenter();
    ~VibeBackgroundSegmenter();

    void process(const cv::Mat &img_input, cv::Mat &img_output,cv::Mat &img_bgmodel);

private:
    vibeModel_t *model;
    cv::Mat img_foreground;

    bool firstTime;

    int numSamples;//每个像素点的样本数目
    int radius;//检测半径
    int minMatchNum;//给定的阈值#min
    int updateFactor;//更新因子
    bool showOutput;

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // VIBEBACKGROUNDSEGMENTER_H
