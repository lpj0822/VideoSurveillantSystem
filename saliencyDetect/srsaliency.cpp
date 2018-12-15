#include "srsaliency.h"
#include <iostream>

SRSaliency::SRSaliency()
{
    std::cout<<"SRSaliency()"<<std::endl;
}

SRSaliency::~SRSaliency()
{
    std::cout<<"~SRSaliency()"<<std::endl;
}

cv::Mat SRSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3||src.channels() == 1);

    int mindim = std::min(src.rows,src.cols);
    int n = mindim/64;
    cv::Size saliencySize(64*n, 64*n);
    cv::Mat imgFC1[2];
    cv::Mat smooth;
    cv::Mat complexSrc2f;
    cv::Mat complexDst2f;
    cv::Mat saliencyMap;
    if(src.channels() == 3)
    {
        cv::cvtColor(src, imgFC1[0], cv::COLOR_BGR2GRAY);
    }
    else
    {
        imgFC1[0] = src.clone();
    }
    imgFC1[0].convertTo(imgFC1[0],CV_32FC1,1.0/255);
    cv::resize(imgFC1[0], imgFC1[0], saliencySize, 0, 0, cv::INTER_AREA);

    imgFC1[1] = cv::Mat::zeros(saliencySize, CV_32FC1);
    cv::merge(imgFC1, 2, complexSrc2f);
    cv::dft(complexSrc2f, complexDst2f);
    absAngle(complexDst2f, imgFC1[0], imgFC1[1]);

    cv::log(imgFC1[0], imgFC1[0]);
    cv::blur(imgFC1[0], smooth, cv::Size(3, 3));
    imgFC1[0] = imgFC1[0] - smooth;

    cv::exp(imgFC1[0], imgFC1[0]);
    getComplex(imgFC1[0], imgFC1[1], complexDst2f);
    cv::dft(complexDst2f, complexSrc2f, cv::DFT_INVERSE | cv::DFT_SCALE);
    cv::split(complexSrc2f, imgFC1);

    cv::pow(imgFC1[0], 2, imgFC1[0]);
    cv::pow(imgFC1[1], 2, imgFC1[1]);
    saliencyMap=imgFC1[0]+imgFC1[1];

    cv::GaussianBlur(saliencyMap, saliencyMap, cv::Size(9, 9), 0, 0);
    cv::normalize(saliencyMap, saliencyMap, 0, 1, cv::NORM_MINMAX);
    cv::resize(saliencyMap, saliencyMap, src.size(), 0, 0, cv::INTER_CUBIC);

    return saliencyMap;
}

cv::Mat SRSaliency::getSRSaliency1(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3||src.channels() == 1);

    cv::Mat saliencyMap;
    cv::Mat img;
    //int mindim = std::min(img.rows,img.cols);
    //int n = mindim/64;
    //cv::Size saliencySize(64*n, 64*n);
    if(src.channels() == 3)
    {
        cv::cvtColor(src, img, cv::COLOR_BGR2GRAY);
    }
    else
    {
        img = src.clone();
    }
    //cv::resize(img, img, saliencySize, 0, 0, CV_INTER_AREA);

    cv::Mat planes[] = {cv::Mat_<float>(img), cv::Mat::zeros(img.size(), CV_32F)};
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);
    cv::dft(complexImg, complexImg);
    cv::split(complexImg, planes);

    cv::Mat mag, logmag, smooth, spectralResidual;
    cv::magnitude(planes[0], planes[1], mag);//求幅度谱
    cv::log(mag, logmag);
    cv::blur(logmag, smooth, cv::Size(3, 3));
    cv::subtract(logmag, smooth, spectralResidual);
    cv::exp(spectralResidual, spectralResidual);

    planes[0] = planes[0].mul(spectralResidual) / mag;
    planes[1] = planes[1].mul(spectralResidual) / mag;

    cv::merge(planes, 2, complexImg);
    cv::dft(complexImg, complexImg, cv::DFT_INVERSE | cv::DFT_SCALE);
    cv::split(complexImg, planes);
    cv::magnitude(planes[0], planes[1], mag);
    cv::multiply(mag, mag, saliencyMap);
    cv::GaussianBlur(saliencyMap, saliencyMap, cv::Size(9, 9), 2.5, 2.5);
    cv::normalize(saliencyMap, saliencyMap, 0, 1, cv::NORM_MINMAX);
    //cv::resize(saliencyMap, saliencyMap, src.size(), 0, 0, cv::INTER_CUBIC);

    return saliencyMap;
}

// absAngle: Calculate magnitude and angle of vectors.
void SRSaliency::absAngle(cv::Mat& complex32FC2, cv::Mat& magnitude32FC1, cv::Mat& angle32FC1)
{
    CV_Assert(complex32FC2.type() == CV_32FC2);

    magnitude32FC1.create(complex32FC2.size(), CV_32FC1);
    angle32FC1.create(complex32FC2.size(), CV_32FC1);

    for (int y = 0; y < complex32FC2.rows; y++)
    {
        const float* cmpD = complex32FC2.ptr<float>(y);
        float* dataM = magnitude32FC1.ptr<float>(y);
        float* dataA = angle32FC1.ptr<float>(y);
        for (int x = 0; x < complex32FC2.cols; x++, cmpD += 2)
        {
            dataA[x] = atan2(cmpD[1], cmpD[0]);
            dataM[x] = sqrt(cmpD[0] * cmpD[0] + cmpD[1] * cmpD[1]);
        }
    }
}

// getComplex: Get a complex value image from it's magnitude and angle.
void SRSaliency::getComplex(cv::Mat& magnitude32F, cv::Mat& angle32F, cv::Mat& complexFC2)
{
    CV_Assert(magnitude32F.type() == CV_32FC1 && angle32F.type() == CV_32FC1 && magnitude32F.size() == angle32F.size());

    complexFC2.create(magnitude32F.size(), CV_32FC2);

    for (int y = 0; y < magnitude32F.rows; y++)
    {
        float* cmpD = complexFC2.ptr<float>(y);
        const float* dataM = magnitude32F.ptr<float>(y);
        const float* dataA = angle32F.ptr<float>(y);
        for (int x = 0; x < magnitude32F.cols; x++, cmpD += 2)
        {
            cmpD[0] = dataM[x] * cos(dataA[x]);
            cmpD[1] = dataM[x] * sin(dataA[x]);
        }
    }
}
