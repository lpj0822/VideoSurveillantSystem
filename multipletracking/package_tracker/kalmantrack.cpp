#include "kalmantrack.h"
#include <iostream>

size_t KalmanTrack::NextTrackID = 0;
// ---------------------------------------------------------------------------
// Track constructor.
// The track begins from initial point (pt)
// ---------------------------------------------------------------------------
KalmanTrack::KalmanTrack(cv::Point2f pt):firstTime(true),dt(0.2f),Accel_noise_mag(0.5f)
{
    track_id = NextTrackID;

    NextTrackID++;
    loadConfig();
    // Every track have its own Kalman filter,
    // it user for next point position prediction.
    KF = new MyKalmanFilter(pt, dt, Accel_noise_mag);
    // Here stored points coordinates, used for next position prediction.
    prediction = pt;
    skipped_frames = 0;

    std::cout<<"KalmanTrack()"<<std::endl;
}

KalmanTrack::~KalmanTrack()
{
    // Free resources.
    if(KF)
    {
        delete KF;
        KF = NULL;
    }
    std::cout<<"~KalmanTrack()"<<std::endl;
}

void KalmanTrack::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/KalmanTrack.xml", cv::FileStorage::WRITE);

    cv::write(fs, "dt", dt);
    cv::write(fs, "Accel_noise_mag", Accel_noise_mag);

    fs.release();
}

void KalmanTrack::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/KalmanTrack.xml", cv::FileStorage::READ);

    cv::read(fs["dt"], dt,0.2f);
    cv::read(fs["Accel_noise_mag"], Accel_noise_mag, 0.5f);

    fs.release();
}

