#include "multipletracker.h"
#include <iostream>

cv::Scalar Colors[] = { cv::Scalar(255, 0, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 0, 255), cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 255),
        cv::Scalar(255, 0, 255), cv::Scalar(255, 127, 255), cv::Scalar(127, 0, 255), cv::Scalar(127, 0, 127) };

MultipleTracker::MultipleTracker()
{
    init();
    std::cout<<"MultipleTracker()"<<std::endl;
}

MultipleTracker::~MultipleTracker()
{
    for (int i = 0; i<tracks.size(); i++)
    {
        if(tracks[i])
        {
            delete tracks[i];
            tracks[i] = NULL;
        }
    }
    tracks.clear();
    std::cout<<"~MultipleTracker()"<<std::endl;
}

void MultipleTracker::Update(cv::Mat& inFrame, std::vector<cv::Point2f>& detections)
{
    if(inFrame.empty())
    {
        return;
    }
    if(isFirstRun)
    {
        saveConfig();
        isFirstRun = false;
    }
    if(detections.size() <= 0)
    {
        for (int i=0; i<tracks.size(); i++)
        {
            tracks[i]->skipped_frames++;
            if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames)
            {
                delete tracks[i];
                tracks.erase(tracks.begin() + i);
                i--;
            }
        }
        for (int i=0; i<tracks.size(); i++)
        {
            tracks[i]->KF->GetPrediction();
            tracks[i]->prediction = tracks[i]->KF->Update(cv::Point2f(0, 0), false);
            if (tracks[i]->trace.size() > max_trace_length)
            {
                tracks[i]->trace.erase(tracks[i]->trace.begin(), tracks[i]->trace.end() - max_trace_length);
            }

            tracks[i]->trace.push_back(tracks[i]->prediction);
            tracks[i]->KF->LastResult = tracks[i]->prediction;
        }
        return;
    }
    // -----------------------------------
    // If there is no tracks yet, then every point begins its own track.
    // -----------------------------------
    if (tracks.size() == 0)
    {
        // If no tracks yet,every point setup track
        for (int i = 0; i<detections.size(); i++)
        {
            KalmanTrack* tr = new KalmanTrack(detections[i]);
            tracks.push_back(tr);
        }
    }

    // -----------------------------------
    int N = (int)tracks.size();
    int M = (int)detections.size();
    std::vector< std::vector<double> > Cost(N, std::vector<double>(M));
    std::vector<int> assignment;

    // -----------------------------------
    double dist;
    for (int i = 0; i<tracks.size(); i++)
    {
        // Point2d prediction = tracks[i]->prediction;
        // cout << prediction << endl;
        for (int j = 0; j<detections.size(); j++)
        {
            cv::Point2d diff = (tracks[i]->prediction - detections[j]);
            dist = std::sqrtf(diff.x*diff.x + diff.y*diff.y);
            Cost[i][j] = dist;
        }
    }

    // Solving assignment problem (tracks and predictions of Kalman filter)
    AssignmentProblemSolver APS;
    APS.Solve(Cost, assignment, AssignmentProblemSolver::optimal);

    // -----------------------------------
    // clean assignment from pairs with large distance
    // -----------------------------------
    // Not assigned tracks
    std::vector<int> not_assigned_tracks;

    for (int i = 0; i<assignment.size(); i++)
    {
        if (assignment[i] != -1)
        {
            if (Cost[i][assignment[i]]>dist_thres)
            {
                assignment[i] = -1;
                // Mark unassigned tracks, and increment skipped frames counter,
                // when skipped frames counter will be larger than threshold, track will be deleted.
                not_assigned_tracks.push_back(i);
                tracks[i]->skipped_frames++;
            }
        }
        else
        {
            // If track have no assigned detect, then increment skipped frames counter.
            tracks[i]->skipped_frames++;
        }

    }

    // -----------------------------------
    // If track didn't get detects long time, remove it.
    // -----------------------------------
    for (int i = 0; i<tracks.size(); i++)
    {
        if (tracks[i]->skipped_frames > maximum_allowed_skipped_frames)
        {
            delete tracks[i];
            tracks.erase(tracks.begin() + i);
            assignment.erase(assignment.begin() + i);
            i--;
        }
    }
    // -----------------------------------
    // Search for unassigned detects
    // -----------------------------------
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

    // -----------------------------------
    // and start new tracks for them.
    // -----------------------------------
    if (not_assigned_detections.size() != 0)
    {
        for (int i = 0; i<not_assigned_detections.size(); i++)
        {
            KalmanTrack* tr = new KalmanTrack(detections[not_assigned_detections[i]]);
            tracks.push_back(tr);
        }
    }

    // Update Kalman Filters state

    for (int i = 0; i<assignment.size(); i++)
    {
        // If track updated less than one time, than filter state is not correct.

        tracks[i]->KF->GetPrediction();

        if (assignment[i] != -1) // If we have assigned detect, then update using its coordinates,
        {
            tracks[i]->skipped_frames = 0;
            tracks[i]->prediction = tracks[i]->KF->Update(detections[assignment[i]], true);//估计值
        }
        else				  // if not continue using predictions
        {
            tracks[i]->prediction = tracks[i]->KF->Update(cv::Point2f(0, 0), false);
        }

        if (tracks[i]->trace.size()>max_trace_length)
        {
            tracks[i]->trace.erase(tracks[i]->trace.begin(), tracks[i]->trace.end() - max_trace_length);
        }

        tracks[i]->trace.push_back(tracks[i]->prediction);
        tracks[i]->KF->LastResult = tracks[i]->prediction;
    }

    if(showOutput)
    {
        drawTrack(inFrame);
    }

}

//根据每次开始运行重新初始化
void MultipleTracker::initStart(bool isStart)
{
    if(isStart)
    {
        for (int i = 0; i<tracks.size(); i++)
        {
            if(tracks[i])
            {
                delete tracks[i];
                tracks[i]=NULL;
            }
        }
        tracks.clear();
    }
}

//初始化数据
void MultipleTracker::initData()
{
    initStart(true);
    loadConfig();
}

//绘制每个跟踪目标的轨迹
void MultipleTracker::drawTrack(cv::Mat& inFrame)
{
    int count=(int)tracks.size();
    std::cout << "tracker number:"<<count << std::endl;

    for (int i = 0; i<count; i++)
    {
        if (tracks[i]->trace.size()>1)
        {
            for (int j = 0; j<tracks[i]->trace.size() - 1; j++)
            {
                cv::line(inFrame, tracks[i]->trace[j], tracks[i]->trace[j + 1], cv::Scalar(0,0,255), 2, CV_AA);
            }
        }
    }
}

void MultipleTracker::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MultipleTracker.xml",cv::FileStorage::WRITE);

    cv::write(fs, "dist_thres", dist_thres);
    cv::write(fs, "maximum_allowed_skipped_frames", maximum_allowed_skipped_frames);
    cv::write(fs, "max_trace_length",max_trace_length);
    cv::write(fs,"showOutput",showOutput);

    fs.release();
}

void MultipleTracker::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MultipleTracker.xml",cv::FileStorage::READ);

    cv::read(fs["dist_thres"], dist_thres,60.0);
    cv::read(fs["maximum_allowed_skipped_frames"],maximum_allowed_skipped_frames, 15);
    cv::read(fs["max_trace_length"],max_trace_length, 10);
    cv::read(fs["showOutput"],showOutput,true);

    fs.release();
}

void MultipleTracker::init()
{
    isFirstRun=true;
    tracks.clear();
    loadConfig();
}

