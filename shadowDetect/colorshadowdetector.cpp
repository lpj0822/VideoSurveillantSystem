#include "colorshadowdetector.h"
#include <iostream>
#include <opencv2/core/core_c.h>

ColorShadowDetector::ColorShadowDetector()
{
    init();
    std::cout << "ColorShadowDetector()" << std::endl;
}

ColorShadowDetector::~ColorShadowDetector()
{
    std::cout << "ColorShadowDetector()" << std::endl;
}

//初始化参数
void ColorShadowDetector::initData()
{
    loadConfig();
}

void ColorShadowDetector::removeShadows(const cv::Mat& frame, const cv::Mat& fgMask, const cv::Mat& bg, cv::Mat& srMask)
{
    if(frame.empty()||fgMask.empty()||bg.empty())
        return;

    if(firstTime)
    {
        saveConfig();
        firstTime=false;
    }

    ConnCompGroup fg(fgMask);
    fgMask.copyTo(srMask);

    ConnCompGroup darkPixels;
    ConnCompGroup shadows;
    cv::Mat hsvFrame, hsvBg;
    cv::cvtColor(frame, hsvFrame, cv::COLOR_BGR2HSV);
    cv::cvtColor(bg, hsvBg, cv::COLOR_BGR2HSV);

    extractDarkPixels(hsvFrame, fg, hsvBg, darkPixels);
    extractShadows(hsvFrame, darkPixels, hsvBg, shadows);

    srMask.setTo(0, shadows.mask);

    if (cleanSrMask)
    {
        ConnCompGroup ccg;
        ccg.update(srMask, true, true);
        ccg.mask.copyTo(srMask);
    }
}

void ColorShadowDetector::extractDarkPixels(const cv::Mat& hsvFrame, const ConnCompGroup& fg, const cv::Mat& hsvBg,ConnCompGroup& darkPixels)
{
    cv::Mat mask(hsvFrame.size(), CV_8U, cv::Scalar(0));

    for (int cc = 0; cc < (int) fg.comps.size(); ++cc)
    {
        const ConnComp& object = fg.comps[cc];

        for (int p = 0; p < (int) object.pixels.size(); ++p)
        {
            int x = object.pixels[p].x;
            int y = object.pixels[p].y;

            const uchar* hsvFramePtr = hsvFrame.ptr(y) + x * 3;
            const uchar* hsvBgPtr = hsvBg.ptr(y) + x * 3;

            float vRatio = (float) hsvFramePtr[2] / hsvBgPtr[2];
            if (vRatio > vThreshLower && vRatio < vThreshUpper)
            {
                uchar* maskPtr = mask.ptr(y);
                maskPtr[x] = 255;
            }
        }
    }

    darkPixels.update(mask);
}

void ColorShadowDetector::extractShadows(const cv::Mat& hsvFrame, const ConnCompGroup& darkPixels,
        const cv::Mat& hsvBg, ConnCompGroup& shadows)
{
    cv::Mat mask(hsvFrame.size(), CV_8U, cv::Scalar(0));

    for (int cc = 0; cc < (int) darkPixels.comps.size(); ++cc)
    {
        const ConnComp& object = darkPixels.comps[cc];

        for (int p = 0; p < (int) object.pixels.size(); ++p)
        {
            int x = object.pixels[p].x;
            int y = object.pixels[p].y;

            int hDiffSum = 0;
            int sDiffSum = 0;
            int winArea = 0;
            int minY = std::max(y - winSize, 0);
            int maxY = std::min(y + winSize, hsvFrame.rows - 1);
            int minX = std::max(x - winSize, 0);
            int maxX = std::min(x + winSize, hsvFrame.cols - 1);
            for (int i = minY; i <= maxY; ++i)
            {
                const uchar* hsvFramePtr = hsvFrame.ptr(i);
                const uchar* hsvBgPtr = hsvBg.ptr(i);

                for (int j = minX; j <= maxX; ++j)
                {
                    int hDiff = CV_IABS(hsvFramePtr[j * 3] - hsvBgPtr[j * 3]);
                    if (hDiff > 90)
                    {
                        hDiff = 180 - hDiff;
                    }
                    hDiffSum += hDiff;

                    int sDiff = hsvFramePtr[j * 3 + 1] - hsvBgPtr[j * 3 + 1];
                    sDiffSum += sDiff;

                    ++winArea;
                }
            }

            bool hIsShadow = (hDiffSum / winArea < hThresh);
            bool sIsShadow = (sDiffSum / winArea < sThresh);

            if (hIsShadow && sIsShadow)
            {
                uchar* maskPtr = mask.ptr(y);
                maskPtr[x] = 255;
            }
        }
    }

    shadows.update(mask);
}

void ColorShadowDetector::init()
{
    firstTime=true;
    loadConfig();
}

void ColorShadowDetector::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ColorShadowDetector.xml",cv::FileStorage::WRITE);

    cv::write(fs, "winSize", winSize);
    cv::write(fs, "cleanSrMask", cleanSrMask);
    cv::write(fs, "hThresh", hThresh);
    cv::write(fs,"sThresh",sThresh);
    cv::write(fs,"vThreshUpper",vThreshUpper);
    cv::write(fs, "vThreshLower", vThreshLower);

    fs.release();
}

void ColorShadowDetector::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ColorShadowDetector.xml",cv::FileStorage::READ);

    cv::read(fs["winSize"], winSize,1);
    cv::read(fs["cleanSrMask"],cleanSrMask, false);
    cv::read(fs["hThresh"], hThresh,48);
    cv::read(fs["sThresh"],sThresh,40);
    cv::read(fs["vThreshUpper"],vThreshUpper,1.0f);
    cv::read(fs["vThreshLower"], vThreshLower,0.3f);

    fs.release();
}
