#include "blobmultipletracker.h"
#include <iostream>

BlobMultipleTracker::BlobMultipleTracker()
{
    init();
    std::cout<<"BlobMultipleTracker()"<<std::endl;
}

BlobMultipleTracker::~BlobMultipleTracker()
{
    if(blobTracking)
    {
        delete blobTracking;
        blobTracking=NULL;
    }
    blobTracks.clear();
    std::cout<<"~BlobMultipleTracker()"<<std::endl;
}

//Blob Tracking Algorithm
void BlobMultipleTracker::process(const cv::Mat &inFrame,const cv::Mat &foregroundFrame)
{
    if(inFrame.empty())
    {
        return;
    }
    cv::Mat frame=inFrame.clone();
    if(isFirstRun)
    {
        saveConfig();
        isFirstRun=false;
    }
    if(blobTracking)
    {
        // Perform blob tracking
        blobTracking->process(frame, foregroundFrame, img_blob);
        blobTracks=blobTracking->getTracks();
        for(std::map<cvb::CvID,cvb::CvTrack*>::iterator it = blobTracks.begin() ; it != blobTracks.end(); it++)
        {
            //std::cout << (*it).first << " => " << (*it).second << std::endl;
            cvb::CvID id = (*it).first;
            cvb::CvTrack* track = (*it).second;

            CvPoint2D64f centerid = track->centroid;
            cv::Point2d centerPoint(centerid.x,centerid.y);
            /*
            std::cout << "---------------------------------------------------------------" << std::endl;
            std::cout << "0)id:" << id << " (" << centroid.x << "," << centroid.y << ")" << std::endl;
            std::cout << "track->active:" << track->active << std::endl;
            std::cout << "track->inactive:" << track->inactive << std::endl;
            std::cout << "track->lifetime:" << track->lifetime << std::endl;
            */

            if(points.count(id) > 0)
            {
                std::map<cvb::CvID, std::vector<cv::Point2d> >::iterator it2 = points.find(id);
                std::vector<cv::Point2d> oldCenteridPoints = it2->second;

                std::vector<cv::Point2d> tempCenteridPoints;
                if(track->inactive == 0)
                {

                    for(std::vector<cv::Point2d>::iterator it3 = oldCenteridPoints.begin() ; it3 != oldCenteridPoints.end(); it3++)
                    {
                         tempCenteridPoints.push_back(*it3);
                    }
                    tempCenteridPoints.push_back(centerPoint);

                    if(tempCenteridPoints.size()>max_trace_length)
                    {
                        tempCenteridPoints.erase(tempCenteridPoints.begin(),tempCenteridPoints.end()-max_trace_length);
                    }

                    if(showOutput)
                    {
                        if(tempCenteridPoints.size()>1)
                        {
                            for(int loop=0;loop<tempCenteridPoints.size()-1;loop++)
                            {
                                cv::line(img_blob, tempCenteridPoints[loop], tempCenteridPoints[loop+1], cv::Scalar(0,0,255), 2, CV_AA);
                            }
                        }
                    }

                    points.erase(it2);
                    points.insert(std::pair<cvb::CvID, std::vector<cv::Point2d> >(id,tempCenteridPoints));
                }
                else
                {
                    points.erase(it2);
                }
            }
            else
            {
                if(track->inactive == 0)
                {
                    std::vector<cv::Point2d> tempCenteridPoints;
                    tempCenteridPoints.push_back(centerPoint);
                    points.insert(std::pair<cvb::CvID, std::vector<cv::Point2d> >(id,tempCenteridPoints));
                }
            }
        }

        if(showOutput)
        {
            cv::imshow("VehicleTracking", img_blob);
        }

    }
}

//根据每次开始运行重新初始化
void BlobMultipleTracker::initStart(bool isStart)
{
    if(isStart)
    {
        if(blobTracking)
        {
            delete blobTracking;
            blobTracking=NULL;
        }

        blobTracking=new BlobTracking();
        blobTracks.clear();
        points.clear();//每个目标跟踪的点
        //trackerSkippedFrames.clear();//每个目标调过的帧数
        tracksPoints.clear();//每个目标的跟踪轨迹
    }
}

//初始化数据
void BlobMultipleTracker::initData()
{
    if(blobTracking)
    {
        delete blobTracking;
        blobTracking=NULL;
    }
    loadConfig();
    blobTracking=new BlobTracking(dist_thres,maximum_allowed_skipped_frames,minArea);
    blobTracks.clear();
    points.clear();//每个目标跟踪的点
    //trackerSkippedFrames.clear();//每个目标调过的帧数
    tracksPoints.clear();//每个目标的跟踪轨迹
}

//得到每个目标的跟踪轨迹
std::vector< std::vector<cv::Point2d> > BlobMultipleTracker::getTracksPoints()
{
    tracksPoints.clear();
    if(points.size()<=0)
    {
        return tracksPoints;
    }
    for(std::map<cvb::CvID, std::vector<cv::Point2d> >::iterator it = points.begin() ; it != points.end(); it++)
    {
        tracksPoints.push_back(it->second);
    }
    return tracksPoints;
}

void BlobMultipleTracker::init()
{
    isFirstRun=true;

    loadConfig();
    blobTracking=new BlobTracking(dist_thres,maximum_allowed_skipped_frames,minArea);
    blobTracks.clear();
    points.clear();//每个目标跟踪的点
    //trackerSkippedFrames.clear();//每个目标调过的帧数
    tracksPoints.clear();//每个目标的跟踪轨迹
}

void BlobMultipleTracker::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/BlobMultipleTracker.xml",cv::FileStorage::WRITE);

    cv::write(fs,"minArea",minArea);
    cv::write(fs, "dist_thres", dist_thres);
    cv::write(fs, "maximum_allowed_skipped_frames", maximum_allowed_skipped_frames);
    cv::write(fs, "max_trace_length",max_trace_length);
    cv::write(fs,"showOutput",showOutput);

    fs.release();
}

void BlobMultipleTracker::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/BlobMultipleTracker.xml",cv::FileStorage::READ);

    cv::read(fs["minArea"],minArea,300);
    cv::read(fs["dist_thres"], dist_thres,50.0);
    cv::read(fs["maximum_allowed_skipped_frames"],maximum_allowed_skipped_frames, 10);
    cv::read(fs["max_trace_length"],max_trace_length, 2);
    cv::read(fs["showOutput"],showOutput,true);

    fs.release();
}
