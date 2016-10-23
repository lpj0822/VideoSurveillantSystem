#ifndef GRADIENTSHADOWDETECTOR_H
#define GRADIENTSHADOWDETECTOR_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <vector>

class GradientShadowDetector
{
public:
    GradientShadowDetector();
    ~GradientShadowDetector();

    void removeShadows (cv::Mat& img, cv::Mat& mask, cv::Mat& bg, cv::Mat& out);

    void initData();

private:

    cv::Mat preprocessImage (cv::Mat& original);
    void computeGradients (cv::Mat& img, cv::Mat& dx, cv::Mat& dy, cv::Mat& mag);
    void computeTextureBuffers (cv::Mat& img_dx, cv::Mat& img_dy, cv::Mat& img_mag, cv::Mat& bg_dx, cv::Mat& bg_dy, cv::Mat& bg_mag, cv::Mat& ori_diff, cv::Mat& window_ori_diff);
    void computeOrientationDifferences (cv::Mat& img_dx, cv::Mat& img_dy, cv::Mat& img_mag, cv::Mat& bg_dx, cv::Mat& bg_dy, cv::Mat& bg_mag, cv::Mat& out);
    void countNonZeroInRows (cv::Mat& in, cv::Mat& out, int width);
    bool luminanceTellsItCanBeShadow (float in_img, float in_bg);
    bool magnitudeTellsItCanBeShadow (float img_mag, float bg_mag);
    bool textureTellsItCanBeShadow (float ori_diff);

private:

    // Shadow detection parameters.
    bool testLuminance;// A flag that tells if we must perform the luminance test.
    bool testMagnitudes;// A flag that tells if we must perform the gradient magnitude test.
    bool testTexture;// A flag that tells if we must perform the region texture test.
    bool textureTestFullWindow;// A flag that tells if the region texture test should consider the entire window (if false, consider only points with significant edges).
    bool textureTestGaussianWindow;// If the region texture test considers the entire window, we may use a Gaussian or a box window.

    float minMag;// Minimum gradient magnitude that we consider "significant".
    float largeMagDiff;// This is what we consider a "large" magnitude difference.
    int textureWindowWidth;// Texture tests are performed on textureWindowWidth x textureWindowWidth windows.
    float textureTestThreshold;// Higher values -> less shadows.
    float textureTestBoxWindowThreshold;// Higher values -> less shadows.
    float textureTestGaussianWindowThreshold;// Higher values -> less shadows.

    bool firstTime;

    void init();

private:
    void saveConfig();
    void loadConfig();

};

#endif // GRADIENTSHADOWDETECTOR_H
