#include "mykalmanfilter2.h"
#include <iostream>

MyKalmanFilter2::MyKalmanFilter2(cv::Rect2f rect, float dt):
    lastResult(rect), deltatime(dt)
{
    // deltatime:time increment (lower values makes target more "massive")
    //4 state variables, 2 measurements, 0 control variables
    kalman = std::make_unique<cv::KalmanFilter>(6, 4, 0, CV_32F);
    initMaxtrix();

    std::cout << "MyKalmanFilter2()" << std::endl;

}

MyKalmanFilter2::~MyKalmanFilter2()
{
    std::cout << "~MyKalmanFilter2()" << std::endl;
}

cv::Rect2f MyKalmanFilter2::prediction()
{
    cv::Mat prediction = kalman->predict();
    lastResult.width = prediction.at<float>(4);
    lastResult.height = prediction.at<float>(5);
    lastResult.x = prediction.at<float>(0) - lastResult.width / 2;
    lastResult.y = prediction.at<float>(1) - lastResult.height / 2;
    return lastResult;
}

cv::Rect2f MyKalmanFilter2::update(cv::Rect2f rect, bool DataCorrect)
{
    //观测值
    cv::Mat measure(4, 1, CV_32FC1);
    if (!DataCorrect)
    {
        //update using prediction
        measure.at<float>(0) = lastResult.x + lastResult.width / 2;
        measure.at<float>(1) = lastResult.y + lastResult.height / 2;
        measure.at<float>(2) = lastResult.width;
        measure.at<float>(3) = lastResult.height;
    }
    else
    {
        //update using measurements
        measure.at<float>(0) = rect.x + rect.width / 2;
        measure.at<float>(1) = rect.y + rect.height / 2;
        measure.at<float>(2) = rect.width;
        measure.at<float>(3) = rect.height;
    }
    // Correction矫正
    cv::Mat estimated = kalman->correct(measure);
    lastResult.width = estimated.at<float>(4);
    lastResult.height = estimated.at<float>(5);
    lastResult.x = estimated.at<float>(0) - lastResult.width / 2;
    lastResult.y = estimated.at<float>(1) - lastResult.height / 2;
    return lastResult;
}

void MyKalmanFilter2::initMaxtrix()
{
    // Transition matrix A
    // [ 1 0 dT 0  0 0 ]
    // [ 0 1 0  dT 0 0 ]
    // [ 0 0 1  0  0 0 ]
    // [ 0 0 0  1  0 0 ]
    // [ 0 0 0  0  1 0 ]
    // [ 0 0 0  0  0 1 ]
    kalman->transitionMatrix = (cv::Mat_<float>(6, 6) << 1, 0, deltatime, 0, 0, 0,
                                0, 1, 0, deltatime, 0, 0,
                                0, 0, 1, 0, 0, 0,
                                0, 0, 0, 1, 0, 0,
                                0, 0, 0, 0, 1, 0,
                                0, 0, 0, 0, 0, 1);
    //init X vector [x,y,v_x,v_y,w,h]
    float centerX = lastResult.x + lastResult.width / 2;
    float centerY = lastResult.y + lastResult.height / 2;
    kalman->statePre.at<float>(0) = centerX; // x
    kalman->statePre.at<float>(1) = centerY; // y
    kalman->statePre.at<float>(2) = 0;
    kalman->statePre.at<float>(3) = 0;
    kalman->statePre.at<float>(4) = lastResult.width;
    kalman->statePre.at<float>(5) = lastResult.height;
    //init Z vector [z_x,z_y,z_w,z_h]
    kalman->statePost.at<float>(0) = centerX;
    kalman->statePost.at<float>(1) = centerY;
    kalman->statePost.at<float>(2) = lastResult.width;
    kalman->statePost.at<float>(3) = lastResult.height;
    //Measure matrix H
    // [ 1 0 0 0 0 0 ]
    // [ 0 1 0 0 0 0 ]
    // [ 0 0 0 0 1 0 ]
    // [ 0 0 0 0 0 1 ]
    kalman->measurementMatrix = cv::Mat::zeros(4, 6, CV_32F);
    kalman->measurementMatrix.at<float>(0) = 1.0f;
    kalman->measurementMatrix.at<float>(7) = 1.0f;
    kalman->measurementMatrix.at<float>(16) = 1.0f;
    kalman->measurementMatrix.at<float>(23) = 1.0f;
    //Wk~(0,Q) Process Noise Covariance Matrix Q
    // [ Ex   0   0     0     0    0  ]
    // [ 0    Ey  0     0     0    0  ]
    // [ 0    0   Ev_x  0     0    0  ]
    // [ 0    0   0     Ev_y  0    0  ]
    // [ 0    0   0     0     Ew   0  ]
    // [ 0    0   0     0     0    Eh ]
    //cv::setIdentity(kf.processNoiseCov, cv::Scalar(1e-2));
    kalman->processNoiseCov.at<float>(0) = 1e-2f;
    kalman->processNoiseCov.at<float>(7) = 1e-2f;
    kalman->processNoiseCov.at<float>(14) = 5.0f;
    kalman->processNoiseCov.at<float>(21) = 5.0f;
    kalman->processNoiseCov.at<float>(28) = 1e-2f;
    kalman->processNoiseCov.at<float>(35) = 1e-2f;

    //Vk~(0,R) Measures Noise Covariance Maxtrix R
    // [ Ex   0   0  0 ]
    // [ 0    Ey  0  0 ]
    // [ 0    0   Ew 0 ]
    // [ 0    0   0  Eh]
    cv::setIdentity(kalman->measurementNoiseCov, cv::Scalar::all(0.1));
    //posteriori error estimate covariance matrix P
    // [ 1   0  0  0  0  0]
    // [ 0   1  0  0  0  0]
    // [ 0   0  1  0  0  0]
    // [ 0   0  0  1  0  0]
    // [ 0   0  0  0  1  0]
    // [ 0   0  0  0  0  1]
    cv::setIdentity(kalman->errorCovPost, cv::Scalar::all(.1));
}
