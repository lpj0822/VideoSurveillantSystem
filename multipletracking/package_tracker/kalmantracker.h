#ifndef KALMANTRACKER_H
#define KALMANTRACKER_H

#include "kalmanFilter/mykalmanfilter.h"
#include <opencv2/core.hpp>
#include <vector>

class KalmanTracker
{
public:
    KalmanTracker();
    ~KalmanTracker();

private:
    MyKalmanFilter* KF;
};

#endif // KALMANTRACKER_H
