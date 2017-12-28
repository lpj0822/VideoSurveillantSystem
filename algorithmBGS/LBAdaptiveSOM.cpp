#include "LBAdaptiveSOM.h"
#include <iostream>

LBAdaptiveSOM::LBAdaptiveSOM() :
  sensitivity(75), trainingSensitivity(245), learningRate(62), trainingLearningRate(255), trainingSteps(55)
{
    init();
    std::cout << "LBAdaptiveSOM()" << std::endl;
}

LBAdaptiveSOM::~LBAdaptiveSOM()
{
    delete m_pBGModel;
    std::cout << "~LBAdaptiveSOM()" << std::endl;
}

void LBAdaptiveSOM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    IplImage *frame = new IplImage(img_input);

    img_output = cv::Mat::zeros(img_input.size(), CV_8UC1);
    img_bgmodel = cv::Mat::zeros(img_input.size(), CV_8UC3);

    if (firstTime)
    {
        int w = cvGetSize(frame).width;
        int h = cvGetSize(frame).height;

        m_pBGModel = new BGModelSom(w, h);
        m_pBGModel->InitModel(frame);
        firstTime = false;
    }

    m_pBGModel->setBGModelParameter(0, sensitivity);
    m_pBGModel->setBGModelParameter(1, trainingSensitivity);
    m_pBGModel->setBGModelParameter(2, learningRate);
    m_pBGModel->setBGModelParameter(3, trainingLearningRate);
    m_pBGModel->setBGModelParameter(5, trainingSteps);

    m_pBGModel->UpdateModel(frame);

    img_foreground = cv::cvarrToMat(m_pBGModel->GetFG());
    img_background = cv::cvarrToMat(m_pBGModel->GetBG());

    if (showOutput)
    {
        cv::imshow("SOM Mask", img_foreground);
        cv::imshow("SOM Model", img_background);
    }

    img_foreground.copyTo(img_output);
    img_background.copyTo(img_bgmodel);

    delete frame;
}

void LBAdaptiveSOM::init()
{
    m_pBGModel = NULL;
    firstTime = true;
    loadConfig();
}

void LBAdaptiveSOM::saveConfig()
{
    cv::FileStorage fs;
    fs.open("./config/LBAdaptiveSOM.xml", cv::FileStorage::WRITE);

    cv::write(fs, "sensitivity", sensitivity);
    cv::write(fs, "trainingSensitivity", trainingSensitivity);
    cv::write(fs, "learningRate", learningRate);
    cv::write(fs, "trainingLearningRate", trainingLearningRate);
    cv::write(fs, "trainingSteps", trainingSteps);
    cv::write(fs, "showOutput", showOutput);

    fs.release();
}

void LBAdaptiveSOM::loadConfig()
{
    cv::FileStorage fs;
    fs.open("./config/LBAdaptiveSOM.xml", cv::FileStorage::READ);

    cv::read(fs["sensitivity"], sensitivity, 75);
    cv::read(fs["trainingSensitivity"], trainingSensitivity, 245);
    cv::read(fs["learningRate"], learningRate, 62);
    cv::read(fs["trainingLearningRate"], trainingLearningRate, 255);
    cv::read(fs["trainingSteps"], trainingSteps, 55);
    cv::read(fs["showOutput"], showOutput, true);

    fs.release();
}
