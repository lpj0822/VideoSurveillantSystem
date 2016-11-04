#include "opticalflowbgs.h"
#include <iostream>
#include <cmath>

OpticalFlowBGS::OpticalFlowBGS()
{
    init();
    std::cout << "OpticalFlowBGS()" << std::endl;
}

OpticalFlowBGS::~OpticalFlowBGS()
{
    std::cout << "~OpticalFlowBGS()" << std::endl;
}

void OpticalFlowBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    cv::Mat grayFrame;
    cv::Mat tempOutput;
    cv::Mat_<cv::Point2f> flow;
    if (img_input.empty())
        return;

    if (firstTime)
    {
        saveConfig();
        firstTime = false;
    }

    if (opticalFlow.get())
    {
        if (img_input.channels() == 3)
            cv::cvtColor(img_input, grayFrame, CV_BGR2GRAY);

        if (preFrame.empty())
        {
            grayFrame.copyTo(preFrame);
            return;
        }
        opticalFlow->calc(preFrame, grayFrame, flow);
        drawOpticalFlow(flow, tempOutput);
        cv::cvtColor(tempOutput, img_foreground, CV_BGR2GRAY);

        if (enableThreshold)
        {
            cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY_INV);
        }

        if (showOutput)
        {
            cv::imshow("flow", tempOutput);
            cv::imshow("OpticalFlowBGS", img_foreground);
        }

        img_foreground.copyTo(img_output);
        grayFrame.copyTo(preFrame);
    }
}

bool OpticalFlowBGS::isFlowCorrect(cv::Point2f u)
{
    return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
}

cv::Vec3b OpticalFlowBGS::computeColor(float fx, float fy)
{
    static bool first = true;

    // relative lengths of color transitions:
    // these are chosen based on perceptual similarity
    // (e.g. one can distinguish more shades between red and yellow
    //  than between yellow and green)
    const int RY = 15;
    const int YG = 6;
    const int GC = 4;
    const int CB = 11;
    const int BM = 13;
    const int MR = 6;
    const int NCOLS = RY + YG + GC + CB + BM + MR;
    static cv::Vec3i colorWheel[NCOLS];

    if (first)
    {
        int k = 0;

        for (int i = 0; i < RY; ++i, ++k)
            colorWheel[k] = cv::Vec3i(255, 255 * i / RY, 0);

        for (int i = 0; i < YG; ++i, ++k)
            colorWheel[k] = cv::Vec3i(255 - 255 * i / YG, 255, 0);

        for (int i = 0; i < GC; ++i, ++k)
            colorWheel[k] = cv::Vec3i(0, 255, 255 * i / GC);

        for (int i = 0; i < CB; ++i, ++k)
            colorWheel[k] = cv::Vec3i(0, 255 - 255 * i / CB, 255);

        for (int i = 0; i < BM; ++i, ++k)
            colorWheel[k] = cv::Vec3i(255 * i / BM, 0, 255);

        for (int i = 0; i < MR; ++i, ++k)
            colorWheel[k] = cv::Vec3i(255, 0, 255 - 255 * i / MR);

        first = false;
    }

    const float rad = sqrt(fx * fx + fy * fy);
    const float a = atan2(-fy, -fx) / (float)CV_PI;

    const float fk = (a + 1.0f) / 2.0f * (NCOLS - 1);
    const int k0 = static_cast<int>(fk);
    const int k1 = (k0 + 1) % NCOLS;
    const float f = fk - k0;

    cv::Vec3b pix;

    for (int b = 0; b < 3; b++)
    {
        const float col0 = colorWheel[k0][b] / 255.f;
        const float col1 = colorWheel[k1][b] / 255.f;

        float col = (1 - f) * col0 + f * col1;

        if (rad <= 1)
            col = 1 - rad * (1 - col); // increase saturation with radius
        else
            col *= .75; // out of range

        pix[2 - b] = static_cast<uchar>(255.f * col);
    }

    return pix;
}

void OpticalFlowBGS::drawOpticalFlow(const cv::Mat_<cv::Point2f>& flow, cv::Mat& output, float maxmotion)
{
    output.create(flow.size(), CV_8UC3);
    output.setTo(cv::Scalar::all(0));

    // determine motion range:
    float maxrad = maxmotion;

    if (maxmotion <= 0)
    {
        maxrad = 1;
        for (int y = 0; y < flow.rows; ++y)
        {
            for (int x = 0; x < flow.cols; ++x)
            {
                cv::Point2f u = flow(y, x);

                if (!isFlowCorrect(u))
                    continue;

                maxrad = std::max(maxrad, sqrt(u.x * u.x + u.y * u.y));
            }
        }
    }

    for (int y = 0; y < flow.rows; ++y)
    {
        for (int x = 0; x < flow.cols; ++x)
        {
            cv::Point2f u = flow(y, x);

            if (isFlowCorrect(u))
                output.at<cv::Vec3b>(y, x) = computeColor(u.x / maxrad, u.y / maxrad);
        }
    }
}

void OpticalFlowBGS::init()
{
    opticalFlow = cv::createOptFlow_DualTVL1();
    firstTime = true;
    loadConfig();
}

void OpticalFlowBGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/OpticalFlowBGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "enableThreshold", enableThreshold);
    cv::write(fs, "threshold", threshold);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void OpticalFlowBGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/OpticalFlowBGS.xml", cv::FileStorage::READ);

    cv::read(fs["enableThreshold"], enableThreshold, true);
    cv::read(fs["threshold"], threshold, 100);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
