#include "vibebackgroundsegmenter.h"
#include <iostream>

VibeBackgroundSegmenter::VibeBackgroundSegmenter()
{
    init();
    std::cout << "VibeBackgroundSegmenter()" << std::endl;
}

VibeBackgroundSegmenter::~VibeBackgroundSegmenter()
{
    libvibeModelFree(model);
    std::cout << "~VibeBackgroundSegmenter()" << std::endl;
}

void VibeBackgroundSegmenter::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    cv::Mat gray;
    if (img_input.empty())
        return;
    if (img_input.channels() == 3)
    {
        cv::cvtColor(img_input, gray, cv::COLOR_BGR2GRAY);
    }

    if(firstTime)
    {
        int32_t width = gray.size().width;
        int32_t height = gray.size().height;
        int32_t stride = gray.channels()*gray.size().width;
        uint8_t *image_data = (uint8_t*)gray.data;
        libvibeModelInit(model, image_data, width, height, stride);

        saveConfig();
        firstTime = false;
        img_foreground.create(height, width, CV_8UC1);
    }
    else
    {
        uint8_t *image_data1 = (uint8_t*)gray.data;
        uint8_t *segmentation_map = (uint8_t*)img_foreground.data;

        libvibeModelUpdate(model, image_data1, segmentation_map);

        if(showOutput)
        {
            cv::imshow("VIBE", img_foreground);
        }
        img_foreground.copyTo(img_output);
    }

}

void VibeBackgroundSegmenter::init()
{
    loadConfig();
    model = libvibeModelNew(numSamples, radius, minMatchNum, updateFactor);
}

void VibeBackgroundSegmenter::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/VibeBackgroundSegmenter.xml", cv::FileStorage::WRITE);

    cv::write(fs, "showOutput", showOutput);
    cv::write(fs, "numSamples", numSamples);
    cv::write(fs, "radius", radius);
    cv::write(fs, "minMatchNum", minMatchNum);
    cv::write(fs, "updateFactor", updateFactor);

    fs.release();
}

void VibeBackgroundSegmenter::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/VibeBackgroundSegmenter.xml", cv::FileStorage::READ);

    cv::read(fs["showOutput"], showOutput, true);
    cv::read(fs["numSamples"], numSamples, 20);
    cv::read(fs["radius"], radius, 20);
    cv::read(fs["minMatchNum"], minMatchNum, 2);
    cv::read(fs["updateFactor"], updateFactor, 16);

    fs.release();
}
