/*
 * BackgroundSubtractorMOG2
 *  http://opencv.itseez.com/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractormog2
 * Gaussian Mixture-based Backbround/Foreground Segmentation Algorithm.
 * The class implements the Gaussian mixture model background subtraction described in:
    (1) Z.Zivkovic, Improved adaptive Gausian mixture model for background subtraction, International Conference Pattern Recognition, UK, August, 2004,
    The code is very fast and performs also shadow detection. Number of Gausssian components is adapted per pixel.

    (2) Z.Zivkovic, F. van der Heijden, Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction,
    Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006.
 *The algorithm similar to the standard Stauffer&Grimson algorithm with additional selection of the number of the Gaussian components based on:
     Z.Zivkovic, F.van der Heijden, Recursive unsupervised learning of finite mixture models, IEEE Trans. on Pattern Analysis and Machine Intelligence,
     vol.26, no.5, pages 651-656, 2004.
*/
#ifndef MIXTUREOFGAUSSIANV2BGS_H
#define MIXTUREOFGAUSSIANV2BGS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>

#include "IBGS.h"

class MixtureOfGaussianV2BGS : public IBGS
{

public:
    MixtureOfGaussianV2BGS();
    ~MixtureOfGaussianV2BGS();

    void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
    bool firstTime;
    cv::Ptr<cv::BackgroundSubtractorMOG2> mog2;
    cv::Mat img_foreground;

    int history;
    double varThreshold;
    double alpha;
    bool enableThreshold;
    int threshold;
    bool enableDetectShadow;
    double shadowThreshold;
    bool showOutput;

private:
    void init();
    void saveConfig();
    void loadConfig();
};
#endif //MIXTUREOFGAUSSIANV2BGS_H
