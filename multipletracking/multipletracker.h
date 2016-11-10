#ifndef MULTIPLETRACKER_H
#define MULTIPLETRACKER_H
#include "utils/assignmentproblemsolver.h"
#include "package_tracker/kalmantrack.h"

class MultipleTracker
{

public:
    MultipleTracker();
    ~MultipleTracker();

    //卡尔曼滤波跟踪算法
    std::vector<KalmanTrack*> tracks;
    void initStart(bool isStart);//根据每次开始运行重新初始化
    void Update(cv::Mat& inFrame, std::vector<cv::Point2f>& detections);

    void initData();//初始化数据


private:
    double dist_thres;//两帧之间目标最大的移动距离
    int maximum_allowed_skipped_frames;//允许目标消失的最大帧数
    int max_trace_length;//跟踪轨迹的最大长度

    bool showOutput;//显示跟踪过程

    bool isFirstRun;//第一次运行

    void init();

private:

    void drawTrack(cv::Mat& inFrame);//绘制每个跟踪目标的轨迹

    void saveConfig();
    void loadConfig();
};

#endif // MULTIPLETRACKER_H
