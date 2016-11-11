#include "blobtracker.h"
#include <iostream>

BlobTracker::BlobTracker() : firstTime(true), minArea(300), maxArea(20000),
    debugTrack(false), debugBlob(false), showOutput(true)
{
    loadConfig();
    std::cout << "BlobTracker()" << std::endl;
}

BlobTracker::BlobTracker(double distance,int inactive,int minArea)
{
    loadConfig();
    this->distance = distance;
    this->inactive = inactive;
    this->minArea = minArea;
    std::cout << "~BlobTracker()" << std::endl;
}

BlobTracker::~BlobTracker()
{
    tracks.clear();
    std::cout << "~BlobTracking()" << std::endl;
}

void BlobTracker::process(const cv::Mat& img_input, const cv::Mat &img_mask, cv::Mat &img_output)
{
    if(img_input.empty() || img_mask.empty())
        return;

    if(firstTime)
    {
        saveConfig();
        firstTime = false;
    }

    IplImage* frame = new IplImage(img_input);
    cvConvertScale(frame, frame, 1, 0);

    IplImage* segmentated = new IplImage(img_mask);

    IplImage* labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);

    cvb::CvBlobs blobs;
    unsigned int result = cvb::cvLabel(segmentated, labelImg, blobs);

    //cvb::cvFilterByArea(blobs, 500, 1000000);
    cvb::cvFilterByArea(blobs, minArea, maxArea);

    //cvb::cvRenderBlobs(labelImg, blobs, copyFrame, copyFrame, CV_BLOB_RENDER_BOUNDING_BOX);
    if(debugBlob)
        cvb::cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_ANGLE|CV_BLOB_RENDER_TO_STD);
    else
        cvb::cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_ANGLE);

    cvb::cvUpdateTracks(blobs, tracks, distance,inactive);

    if(debugTrack)
        cvb::cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX|CV_TRACK_RENDER_TO_STD);
    else
        cvb::cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);

    //std::map<CvID, CvTrack *> CvTracks

    if(showOutput)
        cvShowImage("Blob Tracking", frame);

    cv::Mat img_result = cv::cvarrToMat(frame);
    img_result.copyTo(img_output);

    //cvReleaseImage(&frame);
    //cvReleaseImage(&segmentated);
    cvReleaseImage(&labelImg);
    delete frame;
    delete segmentated;
    cvReleaseBlobs(blobs);
}

cvb::CvTracks BlobTracker::getTracks() const
{
    return tracks;
}

void BlobTracker::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/BlobTracking.xml", cv::FileStorage::WRITE);

    cv::write(fs, "minArea", minArea);
    cv::write(fs, "maxArea", maxArea);
    cv::write(fs, "distance", distance);
    cv::write(fs, "inactive", inactive);

    cv::write(fs, "debugTrack", debugTrack);
    cv::write(fs, "debugBlob", debugBlob);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void BlobTracker::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/BlobTracking.xml", cv::FileStorage::READ);

    cv::read(fs["minArea"], minArea, 300);
    cv::read(fs["maxArea"], maxArea, 20000);
    cv::read(fs["distance"], distance, 50.0);
    cv::read(fs["inactive"], inactive, 10);

    cv::read(fs["debugTrack"], debugTrack, false);
    cv::read(fs["debugBlob"], debugBlob, false);
    cv::read(fs["showOutput"], showOutput, false);

    fs.release();
}
