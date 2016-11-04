#include "MixtureOfGaussianV2BGS.h"
#include <iostream>

MixtureOfGaussianV2BGS::MixtureOfGaussianV2BGS()
{
    init();
    std::cout << "MixtureOfGaussianV2BGS()" << std::endl;
}

MixtureOfGaussianV2BGS::~MixtureOfGaussianV2BGS()
{
    std::cout << "~MixtureOfGaussianV2BGS()" << std::endl;
}

void MixtureOfGaussianV2BGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    if(img_input.empty())
        return;

    if(firstTime)
    {
        saveConfig();
        firstTime = false;
    }

    if(mog2.get())
    {
        mog2->apply(img_input, img_foreground, alpha);
  
        mog2->getBackgroundImage(img_bgmodel);

        if(enableThreshold)
        {
            cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
        }

        if(showOutput)
        {
            cv::imshow("GMM(Zivkovic&Heijden)", img_foreground);
            cv::imshow("GMM BG(Zivkovic&Heijden)", img_bgmodel);
        }

        img_foreground.copyTo(img_output);
    }
}

void MixtureOfGaussianV2BGS::init()
{
    firstTime = true;
    loadConfig();
    mog2 = cv::createBackgroundSubtractorMOG2(history, varThreshold, false);
    mog2->setDetectShadows(enableDetectShadow);
    mog2->setShadowThreshold(shadowThreshold);
}

void MixtureOfGaussianV2BGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MixtureOfGaussianV2BGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "history", history);
    cv::write(fs, "varThreshold", varThreshold);
    cv::write(fs,"enableDetectShadow", enableDetectShadow);
    cv::write(fs,"shadowThreshold", shadowThreshold);
    cv::write(fs, "alpha", alpha);
    cv::write(fs, "enableThreshold", enableThreshold);
    cv::write(fs, "threshold", threshold);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void MixtureOfGaussianV2BGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MixtureOfGaussianV2BGS.xml", cv::FileStorage::READ);
  
    cv::read(fs["history"], history, 500);
    cv::read(fs["varThreshold"], varThreshold, 16);
    cv::read(fs["enableDetectShadow"], enableDetectShadow, false);
    cv::read(fs["shadowThreshold"], shadowThreshold, 0.5);
    cv::read(fs["alpha"], alpha, -1);
    cv::read(fs["enableThreshold"], enableThreshold, true);
    cv::read(fs["threshold"], threshold, 20);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
