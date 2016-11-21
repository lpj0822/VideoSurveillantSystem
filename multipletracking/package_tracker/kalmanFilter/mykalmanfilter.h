#ifndef MYKALMANFILTER_H
#define MYKALMANFILTER_H

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <memory>

class MyKalmanFilter
{
public:
    MyKalmanFilter(cv::Point2f p, float dt = 0.2f, float acceleration_noise_mag = 0.5f);
    ~MyKalmanFilter();

    cv::Point2f prediction();//预测
    cv::Point2f update(cv::Point2f point, bool DataCorrect);//更新

private:
    void initMaxtrix();

private:
    std::unique_ptr<cv::KalmanFilter> kalman;
    float deltatime;
    float acceleration_noise_mag;

    cv::Point2f lastResult;//最后估计值
};

#endif // MYKALMANFILTER_H
