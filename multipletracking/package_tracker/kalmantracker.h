#ifndef KALMANTRACKER_H
#define KALMANTRACKER_H

#include "kalmanFilter/mykalmanfilter.h"
#include <opencv2/core.hpp>
#include <memory>
#include <vector>

class KalmanTracker
{
public:
    KalmanTracker(cv::Point2f initPoint);
    ~KalmanTracker();

    void kalmanPrediction();//kalman预测
    void kalmanUpdate(cv::Point2f point, bool isCorrect);//kalman更新
    void setSkippedFrame(size_t num);
    void addSkippedFrame(size_t num);
    size_t getSkippedFrame();
    void eraseTrace(int keepSize);
    int getTraceSize();
    std::vector<cv::Point2f> getTrace();
    cv::Point2f getPredictPoint();


private:
    std::unique_ptr<MyKalmanFilter> kalmanFilter;
    bool firstTime;
    size_t tracker_id;
    static size_t NextTrackerID;
    size_t skipped_frames;

    std::vector<cv::Point2f> trace;
    cv::Point2f predictPoint;

    float dt;
    float Accel_noise_mag;

private:
    void init();
    void saveConfig();
    void loadConfig();
};

#endif // KALMANTRACKER_H
