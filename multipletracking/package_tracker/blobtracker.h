#ifndef BLOBTRACKER_H
#define BLOBTRACKER_H

#include <cv.h>
#include <highgui.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "cvblob/cvblob.h"

class BlobTracker
{
public:
    BlobTracker();
    BlobTracker(double distance,int inactive,int minArea);
    ~BlobTracker();

    void process(const cv::Mat& img_input, const cv::Mat &img_mask, cv::Mat &img_output);
    cvb::CvTracks getTracks() const;

private:
    bool firstTime;
    cvb::CvTracks tracks;

    int minArea;
    int maxArea;
    double distance;
    int inactive;
    bool debugTrack;
    bool debugBlob;
    bool showOutput;

private:
    void saveConfig();
    void loadConfig();
};

#endif // BLOBTRACKER_H
