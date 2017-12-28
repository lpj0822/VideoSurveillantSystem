#include "saliencybgs.h"
#include "../utility/tool.h"
#include <iostream>
#include <ctime>

// Initial background estimation.
#define INITIAL_BG_SAMPLES 20 // Number of frames to use.
#define INITIAL_BG_INTERVAL 25 // Number of frames between two samples.

SaliencyBGS::SaliencyBGS()
{
    init();
    std::cout<<"SaliencyBGS()"<<std::endl;
}

SaliencyBGS::~SaliencyBGS()
{
    if(imageFilter)
    {
        delete imageFilter;
        imageFilter = NULL;
    }
    if(saliency)
    {
        delete saliency;
        saliency = NULL;
    }
    if(vibe)
    {
        delete vibe;
        vibe = NULL;
    }
    std::cout<<"~SaliencyBGS()"<<std::endl;
}

void SaliencyBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    cv::Mat myForeground;
    cv::Mat bgEstimated;
    if (img_input.empty())
    {
        return;
    }
    img_bgmodel = cv::Mat::zeros(img_input.size(), CV_8UC3);
    if (firstRun)
    {
        if (bgNumberFrame < (INITIAL_BG_SAMPLES-1) * INITIAL_BG_INTERVAL)
        {
                if (bgNumberFrame%INITIAL_BG_INTERVAL == 0)
                {
                    bgSamples.push_back(img_input.clone());
                }
        }
        else if (bgNumberFrame == (INITIAL_BG_SAMPLES-1) * INITIAL_BG_INTERVAL)
        {
            bgSamples.push_back(img_input.clone());
            // Now, compute the median of the stored frames.
            bgEstimated=cv::Mat(img_input.size(), CV_8UC3);
            roughBGEstimation(bgEstimated);
            cv::imwrite("./test.png", bgEstimated);
            vibe->initModel(bgEstimated);
            firstRun = false;
            bgSamples.clear();
        }
        bgNumberFrame++;
    }
    else
    {
        vibe->detectAndUpdate(img_input, img_foreground);
        saliencyMap=saliency->getSaliency(img_input);
        cv::medianBlur(img_foreground, img_foreground, 7);
        //imageFilter->guidedFilter(saliencyMap, img_foreground, saliencyMap, 8, 0.1*0.1);
        calculateForeground(myForeground);
        std::cout << "threshold:" << threshold << std::endl;
        cv::threshold(myForeground, myForeground, threshold, 255, cv::THRESH_BINARY);
        //vibe->getEstimatedBG(img_bgmodel);
        if (showOutput)
        {
            cv::imshow("Vibe", img_foreground);
            cv::imshow("saliency", saliencyMap);
            cv::imshow("myForeground", myForeground);
            //cv::imshow("Vibe BG", img_bgmodel);
        }
        myForeground.copyTo(img_output);
    }

}

//背景估计
void SaliencyBGS::roughBGEstimation(cv::Mat &bg)
{
    // Now, compute the median of the stored frames.
    std::vector<uchar> pixel_samples;
    for (int row = 0; row < bg.rows; row++)
    {
        uchar* ptr_out = bg.ptr<uchar>(row);
        uchar* ptr_in[INITIAL_BG_SAMPLES];
        for (int i = 0; i < INITIAL_BG_SAMPLES; i++)
            ptr_in[i] = bgSamples[i].ptr<uchar>(row);

        for (int col = 0; col < bg.cols; col++)
        {
            for (int channel = 0; channel < bg.channels (); channel++)
            {
                pixel_samples.clear ();
                for (int i = 0; i < INITIAL_BG_SAMPLES; i++)
                    pixel_samples.push_back (ptr_in[i][channel]);

                ptr_out [channel] = medianValue <unsigned char> (pixel_samples);
            }

            for (int i = 0; i < INITIAL_BG_SAMPLES; i++)
                ptr_in [i] += bgSamples[i].channels ();
            ptr_out += bg.channels ();
        }
    }
}

//计算前景
void SaliencyBGS::calculateForeground(cv::Mat &myForeground)
{
    int sumMean = 0;
    double max = 0;
    double min = 0;
    int num = 0;
    cv::Mat tempRoi;
    std::vector< std::vector<cv::Point> > contours; //存储轮廓
    std::vector<cv::Vec4i> hierarchy;//轮廓索引编号
    std::vector<cv::Point> contours_hull;

    if (img_foreground.empty() || saliencyMap.empty())
    {
        return;
    }
    myForeground = cv::Mat::zeros(img_foreground.size(), CV_32FC1);
    //寻找轮廓
    cv::findContours(img_foreground.clone(), contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    //得到轮廓的
    for (int i = 0; i < contours.size(); i++)
    {
        cv::convexHull(cv::Mat(contours[i]), contours_hull ,true);
        if (cv::contourArea(contours_hull) > minArea)
        {
            cv::Rect box = cv::boundingRect(contours_hull);
            cv::Mat roi = saliencyMap(box);
            cv::Scalar mean = cv::mean(roi);
            std::cout << "mean:" << mean[0] << std::endl;
            if(mean[0] > salienctThreshold)
            {
                cv::Mat roi1 = myForeground(box);
                cv::Mat roi2 = img_foreground(box);
                roi2.convertTo(tempRoi, CV_32FC1, 1.0 / 255.0);
                cv::add(tempRoi, roi, tempRoi);
                cv::normalize(tempRoi, roi1, 255, 0, cv::NORM_MINMAX);
                imageFilter->guidedFilter(roi1, roi1, 16, 0.1  *0.1);
                cv::minMaxIdx(roi1, &min, &max);
                sumMean += (int)(max/alpha);
                num++;
            }
        }
    }
    if (num > 0)
    {
        threshold = sumMean / num;
    }
    myForeground.convertTo(myForeground, CV_8UC1);
    cv::GaussianBlur(myForeground, myForeground, cv::Size(9,9), 0);
    //imageFilter->guidedFilter(myForeground, myForeground, 16, 0.1*0.1);
}

void SaliencyBGS::init()
{
    bgNumberFrame = 0;
    bgSamples.clear();

    vibe = new IViBeBGS();
    saliency = new HCSaliency();
    imageFilter = new ImageFilter();
    firstRun=true;

    showOutput=true;
    salienctThreshold = 0.2f;
    minArea = 350;
    threshold = 160;
    alpha = 1.5f;
}
void SaliencyBGS::saveConfig()
{

}

void SaliencyBGS::loadConfig()
{

}
