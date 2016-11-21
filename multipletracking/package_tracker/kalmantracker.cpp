#include "kalmantracker.h"
#include <iostream>

size_t KalmanTracker::NextTrackerID = 0;

KalmanTracker::KalmanTracker(cv::Point2f initPoint): firstTime(true)
{
    NextTrackerID++;
    // Here stored points coordinates, used for next position prediction.
    predictPoint = initPoint;
    init();
    std::cout << "KalmanTracker()" << std::endl;
}

KalmanTracker::~KalmanTracker()
{
    std::cout << "~KalmanTracker()" << std::endl;
}

//kalman预测
void KalmanTracker::kalmanPrediction()
{
    kalmanFilter->prediction();
}

//kalman更新
void KalmanTracker::kalmanUpdate(cv::Point2f point, bool isCorrect)
{
    this->predictPoint = kalmanFilter->update(point, isCorrect);
    this->trace.push_back(this->predictPoint);
}

void KalmanTracker::setSkippedFrame(size_t num)
{
    this->skipped_frames = num;
}

void KalmanTracker::addSkippedFrame(size_t num)
{
    this->skipped_frames += num;
}

size_t KalmanTracker::getSkippedFrame()
{
    return this->skipped_frames;
}

void KalmanTracker::eraseTrace(int keepSize)
{
    if (keepSize <= this->getTraceSize())
        trace.erase(trace.begin(), trace.end() - keepSize);
}

int KalmanTracker::getTraceSize()
{
    return static_cast<int>(this->trace.size());
}

std::vector<cv::Point2f> KalmanTracker::getTrace()
{
    return this->trace;
}

cv::Point2f KalmanTracker::getPredictPoint()
{
    return this->predictPoint;
}

void KalmanTracker::init()
{
    tracker_id = NextTrackerID;
    loadConfig();
    // Every track have its own Kalman filter,
    // it user for next point position prediction.
    kalmanFilter = std::make_unique<MyKalmanFilter>(predictPoint, dt, Accel_noise_mag);
    skipped_frames = 0;
    trace.clear();
}

void KalmanTracker::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/KalmanTrack.xml", cv::FileStorage::WRITE);

    cv::write(fs, "dt", dt);
    cv::write(fs, "Accel_noise_mag", Accel_noise_mag);

    fs.release();
}

void KalmanTracker::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/KalmanTrack.xml", cv::FileStorage::READ);

    cv::read(fs["dt"], dt, 0.2f);
    cv::read(fs["Accel_noise_mag"], Accel_noise_mag, 0.5f);

    fs.release();
}
