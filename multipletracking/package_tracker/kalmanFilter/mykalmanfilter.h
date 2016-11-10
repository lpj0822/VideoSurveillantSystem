#ifndef MYKALMANFILTER_H
#define MYKALMANFILTER_H

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

class MyKalmanFilter
{
public:
    MyKalmanFilter(cv::Point2f p, float dt = 0.2f, float acceleration_noise_mag = 0.5f);
    ~MyKalmanFilter();

    cv::Point2f prediction();//预测
    cv::Point2f update(cv::Point2f p, bool DataCorrect);//更新

private:
    cv::KalmanFilter* kalman;
    float deltatime;
    float acceleration_noise_mag;

    cv::Point2f lastResult;//最后估计值

private:
    void initMaxtrix();
};

#endif // MYKALMANFILTER_H
