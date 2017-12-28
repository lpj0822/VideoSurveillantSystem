#include "ThreeFrameDifferenceBGS.h"
#include <iostream>

ThreeFrameDifferenceBGS::ThreeFrameDifferenceBGS()
{
    init();
    std::cout << "ThreeFrameDifferenceBGS()" << std::endl;
}

ThreeFrameDifferenceBGS::~ThreeFrameDifferenceBGS()
{
    std::cout << "~ThreeFrameDifferenceBGS()" << std::endl;
}

void ThreeFrameDifferenceBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    cv::Mat gray_diff1, gray_diff2;//存储2次相减的图片
    cv::Mat gray_diff11, gray_diff12;
    cv::Mat gray_diff21, gray_diff22;

    if (img_input.empty())
        return;

    img_bgmodel = cv::Mat::zeros(img_input.size(), CV_8UC3);
    if (firstTime)
    {
        saveConfig();
        firstTime=false;
    }

    if (img_input_prev.empty())
    {
        img_input.copyTo(img_input_prev);
        return;
    }

    if (img_input_prev1.empty())
    {
        img_input_prev.copyTo(img_input_prev1);
        img_input.copyTo(img_input_prev);
        return;
    }

    cv::subtract(img_input_prev, img_input_prev1, gray_diff11);//第二帧减第一帧
    cv::subtract(img_input_prev1, img_input_prev, gray_diff12);
    cv::add(gray_diff11, gray_diff12, gray_diff1);
    cv::subtract(img_input, img_input_prev, gray_diff21);//第三帧减第二帧
    cv::subtract(img_input_prev, img_input, gray_diff22);
    cv::add(gray_diff21, gray_diff22, gray_diff2);

    if (gray_diff1.channels() == 3)
    {
        cv::cvtColor(gray_diff1, gray_diff1, cv::COLOR_BGR2GRAY);
    }
    if (gray_diff2.channels() == 3)
    {
        cv::cvtColor(gray_diff2, gray_diff2, cv::COLOR_BGR2GRAY);
    }

    for (int i = 0; i < gray_diff1.rows; i++)
    {
        uchar* data1 = gray_diff1.ptr<uchar>(i);
        uchar* data2 = gray_diff2.ptr<uchar>(i);
        for (int j=0; j<gray_diff1.cols; j++)
        {
            if (abs(data1[j]) >= threshold_diff1)//这里模板参数一定要用unsigned char，否则就一直报错
                data1[j] = 255;            //第一次相减阈值处理
            else
                data1[j] = 0;

            if (abs(data2[j]) >= threshold_diff2)//第二次相减阈值处理
                data2[j] = 255;
            else
                data2[j] = 0;
        }
    }

    cv::bitwise_and(gray_diff1, gray_diff2, img_foreground);

    if (showOutput)
      cv::imshow("ThreeFrameDifferenceBGS", img_foreground);

    img_foreground.copyTo(img_output);

    img_input_prev.copyTo(img_input_prev1);
    img_input.copyTo(img_input_prev);
}

void ThreeFrameDifferenceBGS::init()
{
    firstTime=true;
    loadConfig();
}

void ThreeFrameDifferenceBGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ThreeFrameDifferenceBGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "threshold_diff1", threshold_diff1);
    cv::write(fs, "threshold_diff2", threshold_diff2);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void ThreeFrameDifferenceBGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ThreeFrameDifferenceBGS.xml", cv::FileStorage::READ);

    cv::read(fs["threshold_diff1"], threshold_diff1, 10);
    cv::read(fs["threshold_diff2"], threshold_diff2, 10);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}

