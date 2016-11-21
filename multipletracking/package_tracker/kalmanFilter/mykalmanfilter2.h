#ifndef MYKALMANFILTER2_H
#define MYKALMANFILTER2_H

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <memory>

class MyKalmanFilter2
{
public:
    MyKalmanFilter2(cv::Rect2f rect, float dt = 0.2f);
    ~MyKalmanFilter2();

    cv::Rect2f prediction();//预测
    cv::Rect2f update(cv::Rect2f rect, bool DataCorrect);//更新

private:
    void initMaxtrix();

private:
    std::unique_ptr<cv::KalmanFilter> kalman;
    float deltatime;

    cv::Rect2f lastResult;//最后估计值
};

#endif // MYKALMANFILTER2_H
