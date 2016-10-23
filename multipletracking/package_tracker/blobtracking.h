#pragma once

#include <cv.h>
#include <highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "cvblob/cvblob.h"

class BlobTracking
{
public:
    BlobTracking();
    BlobTracking(double distance,int inactive,int minArea);
    ~BlobTracking();

    void process(const cv::Mat img_input, const cv::Mat &img_mask, cv::Mat &img_output);
    const cvb::CvTracks getTracks();

private:
    bool firstTime;
    int minArea;
    int maxArea;

    double distance;
    int inactive;

    bool debugTrack;
    bool debugBlob;
    bool showOutput;

    cvb::CvTracks tracks;

    void saveConfig();
    void loadConfig();
};

