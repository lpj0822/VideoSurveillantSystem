#include "hcsaliency.h"
#include "utils/utility.h"
#include <iostream>

const int HCSaliency::DefaultNums[3] = {12, 12, 12};

HCSaliency::HCSaliency()
{
    std::cout<<"HCSaliency()"<<std::endl;
}

HCSaliency::~HCSaliency()
{
    std::cout<<"~HCSaliency()"<<std::endl;
}

cv::Mat HCSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels()==3);

    cv::Mat input;
    cv::Mat indexIC1, binColor3f, colorNums1i, colorSaliency;
    src.convertTo(input, CV_32FC3, 1.0 / 255.0);
    // Quantize colors
    quantize(input, indexIC1, binColor3f, colorNums1i);
    cv::cvtColor(binColor3f, binColor3f, cv::COLOR_BGR2Lab);

    hcSaliency(binColor3f, colorNums1i, colorSaliency);

    float* colorSal = (float*)(colorSaliency.data);
    cv::Mat saliencyMap(src.size(), CV_32FC1);
    for (int r = 0; r < src.rows; r++)
    {
        float* sal = saliencyMap.ptr<float>(r);
        int* index = indexIC1.ptr<int>(r);
        for (int c = 0; c < src.cols; c++)
            sal[c] = colorSal[index[c]];
    }
    cv::GaussianBlur(saliencyMap, saliencyMap, cv::Size(3, 3), 0);
    cv::normalize(saliencyMap, saliencyMap,1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}

// Histogram based Contrast
void HCSaliency::hcSaliency(cv::Mat &binColorFC3, cv::Mat &colorNumIC1, cv::Mat &colorSaliency)
{
    cv::Mat weight1f;
    cv::normalize(colorNumIC1, weight1f, 1.0, 0, cv::NORM_L1, CV_32FC1);

    int binN = binColorFC3.cols;
    colorSaliency = cv::Mat::zeros(1, binN, CV_32FC1);
    float* colorSal = (float*)(colorSaliency.data);
    std::vector<std::vector<CostfIdx> > similar(binN); // Similar color: how similar and their index
    cv::Vec3f* color = (cv::Vec3f*)(binColorFC3.data);
    float *w = (float*)(weight1f.data);
    for (int i = 0; i < binN; i++)
    {
        std::vector<CostfIdx> &similari = similar[i];
        similari.push_back(std::make_pair(0.f, i));
        for (int j = 0; j < binN; j++)
        {
            if (i == j)
                continue;
            float dij = vecDist<float, 3>(color[i], color[j]);
            similari.push_back(std::make_pair(dij, j));
            colorSal[i] += w[j] * dij;
        }
        std::sort(similari.begin(), similari.end());
    }

    smoothSaliency(colorSaliency, 0.25f, similar);
}

//颜色量化
int HCSaliency::quantize(const cv::Mat& srcFC3, cv::Mat &indexIC1, cv::Mat &colorFC3, cv::Mat &colorNum, double ratio,
                         const int colorNums[3])
{
    CV_Assert(srcFC3.data != NULL);

    int w[3] = {colorNums[1] * colorNums[2], colorNums[2], 1};

    indexIC1 = cv::Mat::zeros(srcFC3.size(), CV_32SC1);
    int rows = srcFC3.rows;
    int cols = srcFC3.cols;
    if (srcFC3.isContinuous() && indexIC1.isContinuous())
    {
        cols *= rows;
        rows = 1;
    }

    // Build color pallet
    std::map<int, int> pallet;
    for (int y = 0; y < rows; y++)
    {
        const float* imgData = srcFC3.ptr<float>(y);
        int* index = indexIC1.ptr<int>(y);
        for (int x = 0; x < cols; x++, imgData += 3)
        {
            index[x] = (int)(imgData[0]*colorNums[0])*w[0] + (int)(imgData[1]*colorNums[1])*w[1] + (int)(imgData[2]*colorNums[2]);
            pallet[index[x]] ++;
        }
    }

    // Find significant colors
    int maxNum = 0;
    std::vector<std::pair<int, int> > num; // (num, color) pairs in num
    num.reserve(pallet.size());
    for(std::map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++)
        num.push_back(std::pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
    std::sort(num.begin(), num.end(), std::greater<std::pair<int, int>>());

    maxNum = (int)num.size();
    int maxDropNum = cvRound(rows * cols * (1-ratio));
    for (int crnt = num[maxNum-1].first; crnt < maxDropNum && maxNum > 1; maxNum--)
        crnt += num[maxNum - 2].first;
    maxNum = std::min(maxNum, 256); // To avoid very rarely case
    if (maxNum <= 10)
        maxNum = std::min(10, (int)num.size());

    pallet.clear();
    for (int i = 0; i < maxNum; i++)
        pallet[num[i].second] = i;

    std::vector<cv::Vec3i> color3i(num.size());
    for (unsigned int i = 0; i < num.size(); i++)
    {
        color3i[i][0] = num[i].second / w[0];
        color3i[i][1] = num[i].second % w[0] / w[1];
        color3i[i][2] = num[i].second % w[1];
    }

    for (unsigned int i = maxNum; i < num.size(); i++)
    {
        int simIdx = 0;
        int simVal = INT_MAX;
        for (int j = 0; j < maxNum; j++)
        {
            int d_ij = vecSquareDist<int, 3>(color3i[i], color3i[j]);
            if (d_ij < simVal)
            {
                simVal = d_ij;
                simIdx = j;
            }
        }
        pallet[num[i].second] = pallet[num[simIdx].second];
    }

    colorFC3 = cv::Mat::zeros(1, maxNum, CV_32FC3);
    colorNum = cv::Mat::zeros(colorFC3.size(), CV_32SC1);

    cv::Vec3f* color = (cv::Vec3f*)(colorFC3.data);
    int* cNum = (int*)(colorNum.data);
    for (int y = 0; y < rows; y++)
    {
        const cv::Vec3f* imgData = srcFC3.ptr<cv::Vec3f>(y);
        int* index = indexIC1.ptr<int>(y);
        for (int x = 0; x < cols; x++)
        {
            index[x] = pallet[index[x]];
            color[index[x]] += imgData[x];
            cNum[index[x]] ++;
        }
    }
    for (int i = 0; i < colorFC3.cols; i++)
        color[i] /= (float)cNum[i];

    return colorFC3.cols;
}

//颜色空间平滑
void HCSaliency::smoothSaliency(cv::Mat &saliencyMapFC1, float delta, const std::vector<std::vector<CostfIdx> > &similar)
{
    if (saliencyMapFC1.cols < 2)
        return;
    CV_Assert(saliencyMapFC1.rows == 1 && saliencyMapFC1.type() == CV_32FC1);

    cv::Mat colorNumIC1 = cv::Mat::ones(saliencyMapFC1.size(), CV_32SC1);
    int binN = saliencyMapFC1.cols;
    cv::Mat newSal1d= cv::Mat::zeros(1, binN, CV_64FC1);
    float *sal = (float*)(saliencyMapFC1.data);
    double *newSal = (double*)(newSal1d.data);
    int *pW = (int*)(colorNumIC1.data);

    // Distance based smooth
    int n = std::max(cvRound(binN * delta), 2);
    std::vector<double> dist(n, 0), val(n), w(n);
    for (int i = 0; i < binN; i++)
    {
        const std::vector<CostfIdx> &similari = similar[i];
        double totalDist = 0, totoalWeight = 0;
        for (int j = 0; j < n; j++)
        {
            int ithIdx =similari[j].second;
            dist[j] = similari[j].first;
            val[j] = sal[ithIdx];
            w[j] = pW[ithIdx];
            totalDist += dist[j];
            totoalWeight += w[j];
        }
        double valCrnt = 0;
        for (int j = 0; j < n; j++)
            valCrnt += val[j] * (totalDist - dist[j]) * w[j];

        newSal[i] =  valCrnt / (totalDist * totoalWeight);
    }
    cv::normalize(newSal1d, saliencyMapFC1, 1.0, 0, cv::NORM_MINMAX, CV_32FC1);
}
