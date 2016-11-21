#include "mykalmanfilter.h"
#include <iostream>

MyKalmanFilter::MyKalmanFilter(cv::Point2f pt, float dt, float acceleration_noise_mag):
    lastResult(pt), deltatime(dt), acceleration_noise_mag(acceleration_noise_mag)
{
    // deltatime:time increment (lower values makes target more "massive")
    //acceleration_noise_mag:
    // We don't know acceleration, so, assume it to process noise.
    // But we can guess, the range of acceleration values thich can be achieved by tracked object.
    // Process noise. (standard deviation of acceleration: )
    // shows, woh much target can accelerate.

    //4 state variables, 2 measurements, 0 control variables
    kalman = std::make_unique<cv::KalmanFilter>(4, 2, 0, CV_32F);
    initMaxtrix();

    std::cout << "MyKalmanFilter()" << std::endl;

}

MyKalmanFilter::~MyKalmanFilter()
{
    std::cout << "~MyKalmanFilter()" << std::endl;
}

cv::Point2f MyKalmanFilter::prediction()
{
    cv::Mat prediction = kalman->predict();
    lastResult = cv::Point2f(prediction.at<float>(0), prediction.at<float>(1));
    return lastResult;
}

cv::Point2f MyKalmanFilter::update(cv::Point2f point, bool DataCorrect)
{
    //观测值
    cv::Mat measure(2, 1, CV_32FC1);
    if (!DataCorrect)
    {
        //update using prediction
        measure.at<float>(0) = lastResult.x;
        measure.at<float>(1) = lastResult.y;
    }
    else
    {
        //update using measurements
        measure.at<float>(0) = point.x;
        measure.at<float>(1) = point.y;
    }
    // Correction矫正
    cv::Mat estimated = kalman->correct(measure);
    lastResult.x = estimated.at<float>(0);
    lastResult.y = estimated.at<float>(1);
    return lastResult;
}

void MyKalmanFilter::initMaxtrix()
{
    // Transition matrix A
    // [ 1 0 dT 0 ]
    // [ 0 1 0  dT]
    // [ 0 0 1  0 ]
    // [ 0 0 0  1 ]
    kalman->transitionMatrix = (cv::Mat_<float>(4, 4) << 1, 0, deltatime, 0, 0, 1, 0, deltatime, 0, 0, 1, 0, 0, 0, 0, 1);
    //init X vector
    kalman->statePre.at<float>(0) = lastResult.x; // x
    kalman->statePre.at<float>(1) = lastResult.y; // y
    kalman->statePre.at<float>(2) = 0;
    kalman->statePre.at<float>(3) = 0;
    //init Z vector
    kalman->statePost.at<float>(0) = lastResult.x;
    kalman->statePost.at<float>(1) = lastResult.y;
    //Measure matrix H
    // [ 1 0 0 0]
    // [ 0 1 0 0]
    cv::setIdentity(kalman->measurementMatrix);
    //Wk~(0,Q) Process Noise Covariance Matrix Q
    // [ Ex   0   0     0     ]
    // [ 0    Ey  0     0     ]
    // [ 0    0   Ev_x  0     ]
    // [ 0    0   0     Ev_y  ]
    kalman->processNoiseCov = (cv::Mat_<float>(4, 4) <<
        pow(deltatime, 4.0) / 4.0, 0, pow(deltatime, 3.0) / 2.0, 0,
        0, pow(deltatime, 4.0) / 4.0, 0, pow(deltatime, 3.0) / 2.0,
        pow(deltatime, 3.0) / 2.0, 0, pow(deltatime, 2.0), 0,
        0, pow(deltatime, 3.0) / 2.0, 0, pow(deltatime, 2.0));

    kalman->processNoiseCov *= acceleration_noise_mag;
    //Vk~(0,R) Measures Noise Covariance Maxtrix R
    // [ Ex   0  ]
    // [ 0    Ey ]
    cv::setIdentity(kalman->measurementNoiseCov, cv::Scalar::all(0.1));
    //posteriori error estimate covariance matrix P
    // [ 1   0  0  0]
    // [ 0   1  0  0]
    // [ 0   0  1  0]
    // [ 0   0  0  1]
    cv::setIdentity(kalman->errorCovPost, cv::Scalar::all(.1));
}

