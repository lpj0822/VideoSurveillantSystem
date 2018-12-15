#include "bmsaliency.h"

#include <iostream>

#define COV_MAT_REG 50.0f
#define DILATION_WIDTH 2
#define NORMALIZE 0
#define HANDLE_BORDER 1
#define COLOR_SPACE 2
#define WHITENING 1
#define STEP 6

static cv::RNG BMS_RNG;

static const int CL_RGB = 1;
static const int CL_Lab = 2;
static const int CL_Luv = 4;

BMSaliency::BMSaliency()
{
     std::cout<<"BMSaliency()"<<std::endl;
}

BMSaliency::~BMSaliency()
{
    std::cout << "~BMSaliency()" << std::endl;
}

cv::Mat BMSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3);

    cv::Mat saliencyMap = cv::Mat::zeros(src.size(), CV_32FC1);

    if (CL_RGB & COLOR_SPACE)
        whitenFeatMap(src, COV_MAT_REG);
    if (CL_Lab & COLOR_SPACE)
    {
        cv::Mat lab;
        cv::cvtColor(src, lab, cv::COLOR_RGB2Lab);
        whitenFeatMap(lab, COV_MAT_REG);
    }
    if (CL_Luv & COLOR_SPACE)
    {
        cv::Mat luv;
        cv::cvtColor(src, luv, cv::COLOR_RGB2Luv);
        whitenFeatMap(luv, COV_MAT_REG);
    }

    for (int i = 0; i < mFeatureMaps.size(); ++i)
    {
        cv::Mat bm;
        double maxPixel , minPixel;
        cv::minMaxLoc(mFeatureMaps[i], &minPixel, &maxPixel);
        for (double thresh = minPixel; thresh < maxPixel; thresh += STEP)
        {
            bm = mFeatureMaps[i] > thresh;
            cv::Mat am = getAttentionMap(bm, DILATION_WIDTH, NORMALIZE, HANDLE_BORDER);
            saliencyMap += am;
        }
    }
    cv::normalize(saliencyMap, saliencyMap, 1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}

cv::Mat BMSaliency::getAttentionMap(const cv::Mat& bm, int dilation_width_1, bool toNormalize, bool handle_border)
{
    cv::Mat ret = bm.clone();
    int jump;
    if (handle_border)
    {
        for (int i=0;i<bm.rows;i++)
        {
            jump= BMS_RNG.uniform(0.0,1.0)>0.99 ? BMS_RNG.uniform(5,25):0;
            if (ret.at<uchar>(i,0+jump)!=1)
                cv::floodFill(ret,cv::Point(0+jump,i), cv::Scalar(1),0, cv::Scalar(0), cv::Scalar(0),8);
            jump = BMS_RNG.uniform(0.0,1.0)>0.99 ?BMS_RNG.uniform(5,25):0;
            if (ret.at<uchar>(i,bm.cols-1-jump)!=1)
                cv::floodFill(ret, cv::Point(bm.cols-1-jump,i), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0),8);
        }
        for (int j=0;j<bm.cols;j++)
        {
            jump= BMS_RNG.uniform(0.0,1.0)>0.99 ? BMS_RNG.uniform(5,25):0;
            if (ret.at<uchar>(0+jump,j)!=1)
                cv::floodFill(ret, cv::Point(j,0+jump), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0), 8);
            jump= BMS_RNG.uniform(0.0,1.0)>0.99 ? BMS_RNG.uniform(5,25):0;
            if (ret.at<uchar>(bm.rows-1-jump,j)!=1)
                cv::floodFill(ret, cv::Point(j,bm.rows-1-jump), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0), 8);
        }
    }
    else
    {
        for (int i=0;i<bm.rows;i++)
        {
            if (ret.at<uchar>(i,0)!=1)
                cv::floodFill(ret, cv::Point(0,i), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0), 8);
            if (ret.at<uchar>(i,bm.cols-1)!=1)
                cv::floodFill(ret, cv::Point(bm.cols-1,i), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0), 8);
        }
        for (int j=0;j<bm.cols;j++)
        {
            if (ret.at<uchar>(0,j)!=1)
                cv::floodFill(ret, cv::Point(j,0), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0), 8);
            if (ret.at<uchar>(bm.rows-1,j)!=1)
                cv::floodFill(ret, cv::Point(j,bm.rows-1), cv::Scalar(1), 0, cv::Scalar(0), cv::Scalar(0), 8);
        }
    }

    ret = ret != 1;

    cv::Mat map1, map2;
    map1 = ret & bm;
    map2 = ret & (~bm);

    if (dilation_width_1 > 0)
    {
        cv::dilate(map1, map1, cv::Mat(), cv::Point(-1, -1), dilation_width_1);
        cv::dilate(map2, map2, cv::Mat(), cv::Point(-1, -1), dilation_width_1);
    }

    map1.convertTo(map1,CV_32FC1);
    map2.convertTo(map2,CV_32FC1);

    if (toNormalize)
    {
        cv::normalize(map1, map1, 1.0, 0.0, cv::NORM_L2);
        cv::normalize(map2, map2, 1.0, 0.0, cv::NORM_L2);
    }
    else
        cv::normalize(ret,ret, 0.0, 1.0, cv::NORM_MINMAX);
    return map1 + map2;
}

void BMSaliency::whitenFeatMap(const cv::Mat& img, float reg)
{
    assert(img.channels() == 3 && img.type() == CV_8UC3);

    std::vector<cv::Mat> featureMaps;

    if (!WHITENING)
    {
        split(img, featureMaps);
        for (int i = 0; i < featureMaps.size(); i++)
        {
            cv::normalize(featureMaps[i], featureMaps[i], 255.0, 0.0, cv::NORM_MINMAX);
            cv::medianBlur(featureMaps[i], featureMaps[i], 3);
            mFeatureMaps.push_back(featureMaps[i]);
        }
        return;
    }

    cv::Mat srcF,meanF,covF;
    img.convertTo(srcF, CV_32FC3);
    cv::Mat samples = srcF.reshape(1, img.rows*img.cols);
    cv::calcCovarMatrix(samples, covF, meanF, cv::COVAR_NORMAL | cv::COVAR_ROWS | cv::COVAR_SCALE, CV_32F);

    covF += cv::Mat::eye(covF.rows, covF.cols, CV_32FC1)*reg;
    cv::SVD svd(covF);
    cv::Mat sqrtW;
    cv::sqrt(svd.w,sqrtW);
    cv::Mat sqrtInvCovF = svd.u * cv::Mat::diag(1.0 / sqrtW);

    cv::Mat whitenedSrc = srcF.reshape(1, img.rows*img.cols)*sqrtInvCovF;
    whitenedSrc = whitenedSrc.reshape(3, img.rows);

    cv::split(whitenedSrc, featureMaps);

    for (int i = 0; i < featureMaps.size(); i++)
    {
        cv::normalize(featureMaps[i], featureMaps[i], 255.0, 0.0, cv::NORM_MINMAX);
        featureMaps[i].convertTo(featureMaps[i], CV_8U);
        medianBlur(featureMaps[i], featureMaps[i], 3);
        mFeatureMaps.push_back(featureMaps[i]);
    }
}
