#ifndef KALMANTRACK_H
#define KALMANTRACK_H
#pragma once
#include "kalmanFilter/mykalmanfilter.h"
#include <opencv2/core.hpp>
#include <vector>

class KalmanTrack
{
public:
    std::vector<cv::Point2f> trace;
    static size_t NextTrackID;

    size_t skipped_frames;
    cv::Point2f prediction;
    MyKalmanFilter* KF;

    KalmanTrack(cv::Point2f p);
    ~KalmanTrack();

private:
    bool firstTime;
    size_t track_id;
    float dt;
    float Accel_noise_mag;

private:
    void saveConfig();
    void loadConfig();
};

#endif // KALMANTRACK_H
