#ifndef BLOBMULTIPLETRACKER_H
#define BLOBMULTIPLETRACKER_H

#include "package_tracker/blobtracker.h"
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

class BlobMultipleTracker
{
public:
    BlobMultipleTracker();
    ~BlobMultipleTracker();

    void initStart(bool isStart);//根据每次开始运行重新初始化
    void initData();//初始化数据

    //Blob Tracking Algorithm
    void multipleTracking(const cv::Mat &inFrame,const cv::Mat &foregroundFrame);
    std::vector< std::vector<cv::Point2d> > getTracksPoints();//得到每个目标的跟踪轨迹

private:

    int minArea;//最小目标的面积
    double dist_thres;//两帧之间目标最大的移动距离
    int maximum_allowed_skipped_frames;//允许目标消失的最大帧数
    int max_trace_length;//跟踪轨迹的最大长度
    bool showOutput;//显示跟踪过程

    /* Blob Tracking Algorithm */
    cv::Mat img_blob;
    BlobTracker* blobTracking;
    cvb::CvTracks blobTracks;
    std::map<cvb::CvID, std::vector<cv::Point2d> > points;//每个目标跟踪的点
    //std::vector<int> trackerSkippedFrames;//每个目标调过的帧数

    std::vector< std::vector<cv::Point2d> > tracksPoints;//每个目标的跟踪轨迹

    bool isFirstRun;//第一次运行

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // BLOBMULTIPLETRACKER_H
