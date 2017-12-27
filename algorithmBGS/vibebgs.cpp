#include "vibebgs.h"
#include <iostream>

ViBeBGS::ViBeBGS():
    numberOfSamples(DEFAULT_NUM_SAMPLES),
    matchingThreshold(DEFAULT_MATCH_THRESH),
    matchingNumber(DEFAULT_MATCH_NUM),
    updateFactor(DEFAULT_UPDATE_FACTOR),
    model(nullptr)
{
    init();
    std::cout << "ViBeBGS()" << std::endl;
}

ViBeBGS::~ViBeBGS()
{
    libvibeModel_Sequential_Free(model);
    std::cout<<"~ViBeBGS()"<<std::endl;
}

void ViBeBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    if (img_input.empty())
        return;

    img_output = cv::Mat::zeros(img_input.size(), CV_8UC1);
    img_bgmodel = cv::Mat::zeros(img_input.size(), CV_8UC3);

    if (firstTime)
    {
        /* Initialization of the ViBe model. */
        libvibeModel_Sequential_AllocInit_8u_C3R(model, img_input.data, img_input.cols, img_input.rows);

        /* Sets default model values. */
        //libvibeModel_Sequential_SetNumberOfSamples(model, numberOfSamples);
        libvibeModel_Sequential_SetMatchingThreshold(model, matchingThreshold);
        libvibeModel_Sequential_SetMatchingNumber(model, matchingNumber);
        libvibeModel_Sequential_SetUpdateFactor(model, updateFactor);

        saveConfig();
        firstTime = false;
    }
    else
    {
        libvibeModel_Sequential_Segmentation_8u_C3R(model, img_input.data, img_output.data);
        //libvibeModel_Sequential_Update_8u_C3R(model, model_img_input.data, img_output.data);
        libvibeModel_Sequential_Update_8u_C3R(model, img_input.data, img_output.data);
        if (showOutput)
        {
            cv::imshow("ViBe", img_output);
        }
    }
}

void ViBeBGS::init()
{
    firstTime = true;
    model = libvibeModel_Sequential_New();
    loadConfig();
}

void ViBeBGS::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ViBeBGS.xml", cv::FileStorage::WRITE);

    cv::write(fs, "numberOfSamples", numberOfSamples);
    cv::write(fs, "matchingThreshold", matchingThreshold);
    cv::write(fs, "matchingNumber", matchingNumber);
    cv::write(fs, "updateFactor", updateFactor);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void ViBeBGS::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/ViBeBGS.xml", cv::FileStorage::READ);

    cv::read(fs["numberOfSamples"], numberOfSamples, DEFAULT_NUM_SAMPLES);
    cv::read(fs["matchingThreshold"], matchingThreshold, DEFAULT_MATCH_THRESH);
    cv::read(fs["matchingNumber"], matchingNumber, DEFAULT_MATCH_NUM);
    cv::read(fs["updateFactor"], updateFactor, DEFAULT_UPDATE_FACTOR);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
