#ifndef MYKALMANFILTER_H
#define MYKALMANFILTER_H
#pragma once
#include <opencv2/core.hpp>
#include <opencv2/video.hpp>

class MyKalmanFilter
{
public:
    cv::KalmanFilter* kalman;
    double deltatime;
    cv::Point2f LastResult;
    MyKalmanFilter(cv::Point2f p, float dt = 0.2, float Accel_noise_mag = 0.5);
    ~MyKalmanFilter();
    cv::Point2f GetPrediction();
    cv::Point2f Update(cv::Point2f p, bool DataCorrect);
};

#endif // MYKALMANFILTER_H
