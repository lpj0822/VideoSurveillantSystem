#ifndef KALMANMULTIPLETRACKER_H
#define KALMANMULTIPLETRACKER_H

#include "package_tracker/kalmantracker.h"

class KalmanMultipleTracker
{
public:
    KalmanMultipleTracker();
    ~KalmanMultipleTracker();

    void mutilpleTracking(const cv::Mat& inFrame, const std::vector<cv::Point2f>& detections);
    std::vector<KalmanTracker*>& getListTrackers();
    int getTrackersCount();
    void initStart(bool isStart);//根据每次开始运行重新初始化
    void initData();//初始化数据

private:
    bool isFirstRun;//第一次运行
    std::vector<KalmanTracker*> listTrackers;//卡尔曼滤波跟踪算法

    double dist_thres;//两帧之间目标最大的移动距离
    int maximum_allowed_skipped_frames;//允许目标消失的最大帧数
    int max_trace_length;//跟踪轨迹的最大长度
    bool showOutput;//显示跟踪过程

private:

    void init();
    void drawTrack(cv::Mat& inFrame);//绘制每个跟踪目标的轨迹

    void saveConfig();
    void loadConfig();
};

#endif // KALMANMULTIPLETRACKER_H
