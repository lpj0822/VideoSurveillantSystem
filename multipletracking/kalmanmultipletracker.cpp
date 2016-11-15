#include "kalmanmultipletracker.h"
#include "utils/assignmentproblemsolver.h"
#include <iostream>

cv::Scalar Colors[] = { cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255), cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 255),
        cv::Scalar(255, 0, 255), cv::Scalar(255, 127, 255), cv::Scalar(127, 0, 255), cv::Scalar(127, 0, 127) };

KalmanMultipleTracker::KalmanMultipleTracker()
{
    init();
    std::cout << "KalmanMultipleTracker()" <<std::endl;
}

KalmanMultipleTracker::~KalmanMultipleTracker()
{
    for (int i = 0; i<listTrackers.size(); i++)
    {
        if (listTrackers[i])
        {
            delete listTrackers[i];
            listTrackers[i] = NULL;
        }
    }
    listTrackers.clear();
    std::cout << "~KalmanMultipleTracker()" <<std::endl;
}

void KalmanMultipleTracker::mutilpleTracking(const cv::Mat& inFrame, const std::vector<cv::Point2f>& detections)
{
    if (inFrame.empty())
    {
        return;
    }
    if (isFirstRun)
    {
        saveConfig();
        isFirstRun = false;
    }
    if (detections.size() <= 0)
    {
        for (int i=0; i<listTrackers.size(); i++)
        {
            listTrackers[i]->addSkippedFrame(1);
            if (listTrackers[i]->getSkippedFrame() > maximum_allowed_skipped_frames)
            {
                delete listTrackers[i];
                listTrackers[i] = NULL;
                listTrackers.erase(listTrackers.begin() + i);
                i--;
            }
        }
        for (int i=0; i<listTrackers.size(); i++)
        {
            listTrackers[i]->kalmanPrediction();
            listTrackers[i]->kalmanUpdate(cv::Point2f(0, 0), false);
            if (listTrackers[i]->getTraceSize() > max_trace_length)
            {
                listTrackers[i]->eraseTrace(max_trace_length);
            }
        }
        return;
    }

    // If there is no tracks yet, then every point begins its own track.
    if (listTrackers.size() == 0)
    {
        // If no tracks yet,every point setup tracker
        for (int i = 0; i<detections.size(); i++)
        {
            KalmanTracker* tracker = new KalmanTracker(detections[i]);
            listTrackers.push_back(tracker);
        }
    }

    int N = (int)listTrackers.size();
    int M = (int)detections.size();
    std::vector< std::vector<double> > Cost(N, std::vector<double>(M));
    std::vector<int> assignment;

    float dist;
    for (int i = 0; i<listTrackers.size(); i++)
    {
        for (int j = 0; j<detections.size(); j++)
        {
            cv::Point2f diff = (listTrackers[i]->getPredictPoint() - detections[j]);
            dist = std::sqrtf(diff.x*diff.x + diff.y*diff.y);
            Cost[i][j] = static_cast<double>(dist);
        }
    }

    // Solving assignment problem (listTrackers and predictions of Kalman filter)
    AssignmentProblemSolver APS;
    APS.Solve(Cost, assignment);

    // clean assignment from pairs with large distance
    // Not assigned trackers
    std::vector<int> not_assigned_tracks;

    for (int i = 0; i<assignment.size(); i++)
    {
        if (assignment[i] != -1)
        {
            if (Cost[i][assignment[i]] > dist_thres)
            {
                assignment[i] = -1;
                // Mark unassigned trackers, and increment skipped frames counter,
                // when skipped frames counter will be larger than threshold, track will be deleted.
                not_assigned_tracks.push_back(i);
                listTrackers[i]->addSkippedFrame(1);
            }
        }
        else
        {
            // If tracker have no assigned detect, then increment skipped frames counter.
            listTrackers[i]->addSkippedFrame(1);
        }

    }

    // If tracker didn't get detects long time, remove it.
    for (int i = 0; i<listTrackers.size(); i++)
    {
        if (listTrackers[i]->getSkippedFrame() > maximum_allowed_skipped_frames)
        {
            delete listTrackers[i];
            listTrackers[i] = NULL;
            listTrackers.erase(listTrackers.begin() + i);
            assignment.erase(assignment.begin() + i);
            i--;
        }
    }

    // Search for unassigned detects
    std::vector<int> not_assigned_detections;
    std::vector<int>::iterator it;
    for (int i = 0; i<detections.size(); i++)
    {
        it = std::find(assignment.begin(), assignment.end(), i);
        if (it == assignment.end())
        {
            not_assigned_detections.push_back(i);
        }
    }

    // and start new trackers for them.
    if (not_assigned_detections.size() != 0)
    {
        for (int i = 0; i<not_assigned_detections.size(); i++)
        {
            KalmanTracker* tracker = new KalmanTracker(detections[not_assigned_detections[i]]);
            listTrackers.push_back(tracker);
        }
    }

    // Update Kalman Filters state
    for (int i = 0; i<assignment.size(); i++)
    {
        // If tracker updated less than one time, than filter state is not correct.
        listTrackers[i]->kalmanPrediction();

        if (assignment[i] != -1) // If we have assigned detect, then update using its coordinates,
        {
            listTrackers[i]->setSkippedFrame(0);
            listTrackers[i]->kalmanUpdate(detections[assignment[i]], true);//估计值
        }
        else				  // if not continue using predictions
        {
            listTrackers[i]->kalmanUpdate(cv::Point2f(0, 0), false);
        }

        if (listTrackers[i]->getTraceSize() > max_trace_length)
        {
            listTrackers[i]->eraseTrace(max_trace_length);
        }
    }

    if(showOutput)
    {
        drawTrack(const_cast<cv::Mat&>(inFrame));
    }

}

std::vector<KalmanTracker*>& KalmanMultipleTracker::getListTrackers()
{
    return this->listTrackers;
}

int KalmanMultipleTracker::getTrackersCount()
{
    return static_cast<int>(this->listTrackers.size());
}

//根据每次开始运行重新初始化
void KalmanMultipleTracker::initStart(bool isStart)
{
    if(isStart)
    {
        for (int i = 0; i<listTrackers.size(); i++)
        {
            if(listTrackers[i])
            {
                delete listTrackers[i];
                listTrackers[i] = NULL;
            }
        }
        listTrackers.clear();
    }
}

//初始化数据
void KalmanMultipleTracker::initData()
{
    initStart(true);
    loadConfig();
}

//绘制每个跟踪目标的轨迹
void KalmanMultipleTracker::drawTrack(cv::Mat& inFrame)
{
    int count = (int)listTrackers.size();
    std::cout << "tracker number:"<< count << std::endl;

    for (int i = 0; i<count; i++)
    {
        if (listTrackers[i]->getTraceSize() > 1)
        {
            std::vector<cv::Point2f> trace = listTrackers[i]->getTrace();
            for (int j = 0; j<trace.size() - 1; j++)
            {
                cv::line(inFrame, trace[j], trace[j + 1], cv::Scalar(0,0,255), 2, CV_AA);
            }
        }
    }
}

void KalmanMultipleTracker::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MultipleTracker.xml", cv::FileStorage::WRITE);

    cv::write(fs, "dist_thres", dist_thres);
    cv::write(fs, "maximum_allowed_skipped_frames", maximum_allowed_skipped_frames);
    cv::write(fs, "max_trace_length", max_trace_length);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void KalmanMultipleTracker::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MultipleTracker.xml", cv::FileStorage::READ);

    cv::read(fs["dist_thres"], dist_thres, 60.0);
    cv::read(fs["maximum_allowed_skipped_frames"], maximum_allowed_skipped_frames, 12);
    cv::read(fs["max_trace_length"], max_trace_length, 12);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}

void KalmanMultipleTracker::init()
{
    isFirstRun=true;
    listTrackers.clear();
    loadConfig();
}
