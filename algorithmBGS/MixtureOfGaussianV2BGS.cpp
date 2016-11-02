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

     //------------------------------------------------------------------
     // BackgroundSubtractorMOG2
     // http://opencv.itseez.com/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractormog2
     //
     // Gaussian Mixture-based Backbround/Foreground Segmentation Algorithm.
     //
     // The class implements the Gaussian mixture model background subtraction described in:
     //  (1) Z.Zivkovic, Improved adaptive Gausian mixture model for background subtraction, International Conference Pattern Recognition, UK, August, 2004,
     //  The code is very fast and performs also shadow detection. Number of Gausssian components is adapted per pixel.
     //
     //  (2) Z.Zivkovic, F. van der Heijden, Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction,
     //  Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006.
     //  The algorithm similar to the standard Stauffer&Grimson algorithm with additional selection of the number of the Gaussian components based on:
     //    Z.Zivkovic, F.van der Heijden, Recursive unsupervised learning of finite mixture models, IEEE Trans. on Pattern Analysis and Machine Intelligence,
     //    vol.26, no.5, pages 651-656, 2004.
     //------------------------------------------------------------------
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

//初始化混合高斯模型
void MixtureOfGaussianV2BGS::initMOG2(int history, double varThreshold)
{
    mog2 = cv::createBackgroundSubtractorMOG2(history, varThreshold);
    mog2->setDetectShadows(enableDetectShadow);
    mog2->setShadowThreshold(shadowThreshold);
}

void MixtureOfGaussianV2BGS::init()
{
    firstTime = true;
    loadConfig();
    mog2 = cv::createBackgroundSubtractorMOG2(500, 16, false);
    mog2->setDetectShadows(enableDetectShadow);
    mog2->setShadowThreshold(shadowThreshold);
}

void MixtureOfGaussianV2BGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MixtureOfGaussianV2BGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "alpha", alpha);
    cv::write(fs, "enableThreshold", enableThreshold);
    cv::write(fs, "threshold", threshold);
    cv::write(fs,"enableDetectShadow", enableDetectShadow);
    cv::write(fs,"shadowThreshold", shadowThreshold);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void MixtureOfGaussianV2BGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/MixtureOfGaussianV2BGS.xml", cv::FileStorage::READ);
  
    cv::read(fs["alpha"], alpha, -1);
    cv::read(fs["enableThreshold"], enableThreshold, true);
    cv::read(fs["threshold"], threshold, 20);
    cv::read(fs["enableDetectShadow"], enableDetectShadow, false);
    cv::read(fs["shadowThreshold"], shadowThreshold, 0.5);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
