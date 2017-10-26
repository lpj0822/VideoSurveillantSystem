#include "rcsaliency.h"
#include "utils/utility.h"
#include "segmentation/efficientGraphBased/segment_image.h"
#include <iostream>

const int RCSaliency::DefaultNums[3] = {12, 12, 12};

RCSaliency::RCSaliency()
{
    std::cout<<"RCSaliency()"<<std::endl;
}

RCSaliency::~RCSaliency()
{
    std::cout<<"~RCSaliency()"<<std::endl;
}

cv::Mat RCSaliency::getSaliency(const cv::Mat &src)
{
    return getRCSaliency(src);
}

//基于区域对比度
cv::Mat RCSaliency::getRCSaliency(const cv::Mat &src, double sigmaDist, double segK, int segMinSize, double segSigma)
{
    CV_Assert(src.channels()==3);

    cv::Mat input;
    cv::Mat imgLab3f, regIdx1i;
    src.convertTo(input,CV_32FC3, 1.0/255);
    cv::cvtColor(input, imgLab3f, cv::COLOR_BGR2Lab);
    int regNum = segmentImage(imgLab3f, regIdx1i, segSigma, segK, segMinSize);

    cv::Mat colorIdx1i, regSal1v, tmp, color3fv;
    int quatizeNum = quantize(input, colorIdx1i, color3fv, tmp);
    if (quatizeNum == 2)
    {
        printf("QuatizeNum == 2, %d: %s\n", __LINE__, __FILE__);
        cv::Mat sal;
        cv::compare(colorIdx1i, 1, sal, cv::CMP_EQ);
        sal.convertTo(sal, CV_32F, 1.0/255);
        return sal;
    }
    if (quatizeNum <= 2) // Color quantization
        return cv::Mat::zeros(input.size(), CV_32FC1);

    cv::cvtColor(color3fv, color3fv, cv::COLOR_BGR2Lab);
    std::vector<Region> regs(regNum);
    buildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
    regionContrast(regs, color3fv, regSal1v, sigmaDist);

    cv::Mat saliencyMap = cv::Mat::zeros(input.size(), CV_32FC1);
    cv::normalize(regSal1v, regSal1v, 1.0, 0, cv::NORM_MINMAX);
    double* regSal = (double*)regSal1v.data;
    for (int r = 0; r < input.rows; r++)
    {
        const int* regIdx = regIdx1i.ptr<int>(r);
        float* sal = saliencyMap.ptr<float>(r);
        for (int c = 0; c < input.cols; c++)
            sal[c] = (float)regSal[regIdx[c]];
    }

    cv::Mat bdReg1u = getBorderRegions(regIdx1i, regNum, 0.02, 0.4);
    saliencyMap.setTo(0, bdReg1u);
    smoothByHist(input, saliencyMap, 0.1f);
    smoothByRegion(saliencyMap, regIdx1i, regNum);
    saliencyMap.setTo(0, bdReg1u);

    cv::GaussianBlur(saliencyMap, saliencyMap, cv::Size(3, 3), 0);
    cv::normalize(saliencyMap, saliencyMap,1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}

//颜色量化
int RCSaliency::quantize(const cv::Mat& srcFC3, cv::Mat &indexIC1, cv::Mat &colorFC3, cv::Mat &colorNum, double ratio,
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

void RCSaliency::buildRegions(cv::Mat& regIndexIC1, std::vector<Region> &regs, cv::Mat &colorIndexIC1, int colorNum)
{
    int rows = regIndexIC1.rows;
    int cols = regIndexIC1.cols;
    int regNum = (int)regs.size();
    double cx = cols/2.0;
    double cy = rows / 2.0;
    cv::Mat_<int> regColorFre1i = cv::Mat_<int>::zeros(regNum, colorNum); // region color frequency
    for (int y = 0; y < rows; y++)
    {
        const int *regIdx = regIndexIC1.ptr<int>(y);
        const int *colorIdx = colorIndexIC1.ptr<int>(y);
        for (int x = 0; x < cols; x++, regIdx++, colorIdx++)
        {
            Region &reg = regs[*regIdx];
            reg.pixNum ++;
            reg.centroid.x += x;
            reg.centroid.y += y;
            regColorFre1i(*regIdx, *colorIdx)++;
            reg.ad2c += cv::Point2d(abs(x - cx), abs(y - cy));
        }
    }

    for (int i = 0; i < regNum; i++)
    {
        Region &reg = regs[i];
        reg.centroid.x /= reg.pixNum * cols;
        reg.centroid.y /= reg.pixNum * rows;
        reg.ad2c.x /= reg.pixNum * cols;
        reg.ad2c.y /= reg.pixNum * rows;
        int *regColorFre = regColorFre1i.ptr<int>(i);
        for (int j = 0; j < colorNum; j++)
        {
            float fre = (float)regColorFre[j]/(float)reg.pixNum;
            if (regColorFre[j] > EPS)
                reg.freIdx.push_back(std::make_pair(fre, j));
        }
    }
}

void RCSaliency::regionContrast(const std::vector<Region> &regs, cv::Mat &colorFC3, cv::Mat& regSaliencyDC1, double sigmaDist)
{
    cv::Mat_<float> cDistCache1f = cv::Mat::zeros(colorFC3.cols, colorFC3.cols, CV_32FC1);
    {
        cv::Vec3f* pColor = (cv::Vec3f*)colorFC3.data;
        for(int i = 0; i < cDistCache1f.rows; i++)
            for(int j= i+1; j < cDistCache1f.cols; j++)
                cDistCache1f[i][j] = cDistCache1f[j][i] = vecDist<float, 3>(pColor[i], pColor[j]);
    }

    int regNum = (int)regs.size();
    cv::Mat_<double> rDistCache1d = cv::Mat::zeros(regNum, regNum, CV_64FC1);
    regSaliencyDC1 = cv::Mat::zeros(1, regNum, CV_64FC1);
    double* regSal = (double*)regSaliencyDC1.data;
    for (int i = 0; i < regNum; i++)
    {
        const cv::Point2d &rc = regs[i].centroid;
        for (int j = 0; j < regNum; j++)
        {
            if(i<j)
            {
                double dd = 0;
                const std::vector<CostfIdx> &c1 = regs[i].freIdx, &c2 = regs[j].freIdx;
                for (size_t m = 0; m < c1.size(); m++)
                    for (size_t n = 0; n < c2.size(); n++)
                        dd += cDistCache1f[c1[m].second][c2[n].second] * c1[m].first * c2[n].first;
                rDistCache1d[j][i] = rDistCache1d[i][j] = dd * exp(-pointSquareDist(rc, regs[j].centroid)/sigmaDist);
            }
            regSal[i] += regs[j].pixNum * rDistCache1d[i][j];
        }
        regSal[i] *= exp(-9.0 * (regs[i].ad2c.x*regs[i].ad2c.x + regs[i].ad2c.y*regs[i].ad2c.y));
    }
}

void RCSaliency::smoothByHist(cv::Mat &srcFC3, cv::Mat &saliencyFC1, float delta)
{
    //imshow("Before", saliencyFC1); imshow("Src", srcFC3);

    // Quantize colors
    CV_Assert(srcFC3.size() == saliencyFC1.size() && srcFC3.type() == CV_32FC3 && saliencyFC1.type() == CV_32FC1);
    cv::Mat idx1i, binColor3f, colorNums1i;
    int binN = quantize(srcFC3, idx1i, binColor3f, colorNums1i);

    // Get initial color saliency
    cv::Mat _colorSal =  cv::Mat::zeros(1, binN, CV_32FC1);
    int rows = srcFC3.rows, cols = srcFC3.cols;
    {
        float* colorSal = (float*)_colorSal.data;
        if (srcFC3.isContinuous() && saliencyFC1.isContinuous())
            cols *= srcFC3.rows, rows = 1;
        for (int y = 0; y < rows; y++)
        {
            const int* idx = idx1i.ptr<int>(y);
            const float* initialS = saliencyFC1.ptr<float>(y);
            for (int x = 0; x < cols; x++)
                colorSal[idx[x]] += initialS[x];
        }
        const int *colorNum = (int*)(colorNums1i.data);
        for (int i = 0; i < binN; i++)
            colorSal[i] /= colorNum[i];
        cv::normalize(_colorSal, _colorSal, 1.0, 0, cv::NORM_MINMAX);
    }
    // Find similar colors & Smooth saliency value for color bins
    std::vector<std::vector<CostfIdx> > similar(binN); // Similar color: how similar and their index
    cv::Vec3f* color = (cv::Vec3f*)(binColor3f.data);
    cv::cvtColor(binColor3f, binColor3f, cv::COLOR_BGR2Lab);
    for (int i = 0; i < binN; i++)
    {
        std::vector<CostfIdx> &similari = similar[i];
        similari.push_back(std::make_pair(0.f, i));
        for (int j = 0; j < binN; j++)
            if (i != j)
                similari.push_back(std::make_pair(vecDist<float, 3>(color[i], color[j]), j));
        std::sort(similari.begin(), similari.end());
    }
    cv::cvtColor(binColor3f, binColor3f, cv::COLOR_Lab2BGR);
    smoothSaliency(colorNums1i, _colorSal, delta, similar);

    // Reassign pixel saliency values
    float* colorSal = (float*)(_colorSal.data);
    for (int y = 0; y < rows; y++)
    {
        const int* idx = idx1i.ptr<int>(y);
        float* resSal = saliencyFC1.ptr<float>(y);
        for (int x = 0; x < cols; x++)
            resSal[x] = colorSal[idx[x]];
    }
    //imshow("After", saliencyFC1);
    //waitKey(0);
}

void RCSaliency::smoothByRegion(cv::Mat &saliencyFC1, cv::Mat &segIndexIC1, int regNum, bool bNormalize)
{
    std::vector<double> saliecy(regNum, 0);
    std::vector<int> counter(regNum, 0);
    for (int y = 0; y < saliencyFC1.rows; y++)
    {
        const int *idx = segIndexIC1.ptr<int>(y);
        float *sal = saliencyFC1.ptr<float>(y);
        for (int x = 0; x < saliencyFC1.cols; x++)
        {
            saliecy[idx[x]] += sal[x];
            counter[idx[x]] ++;
        }
    }

    for (size_t i = 0; i < counter.size(); i++)
        saliecy[i] /= counter[i];
    cv::Mat rSal(1, regNum, CV_64FC1, &saliecy[0]);
    if (bNormalize)
        cv::normalize(rSal, rSal, 1.0, 0, cv::NORM_MINMAX);

    for (int y = 0; y < saliencyFC1.rows; y++)
    {
        const int *idx = segIndexIC1.ptr<int>(y);
        float *sal = saliencyFC1.ptr<float>(y);
        for (int x = 0; x < saliencyFC1.cols; x++)
            sal[x] = (float)saliecy[idx[x]];
    }
}

cv::Mat RCSaliency::getBorderRegions(cv::Mat &indexIC1, int regNum, double ratio, double thr)
{
    // Variance of x and y
    std::vector<double> vX(regNum), vY(regNum);
    int w = indexIC1.cols;
    int h = indexIC1.rows;
    {
        std::vector<double> mX(regNum), mY(regNum), n(regNum); // Mean value of x and y, pixel number of region
        for (int y = 0; y < indexIC1.rows; y++)
        {
            const int *idx = indexIC1.ptr<int>(y);
            for (int x = 0; x < indexIC1.cols; x++, idx++)
                mX[*idx] += x, mY[*idx] += y, n[*idx]++;
        }
        for (int i = 0; i < regNum; i++)
            mX[i] /= n[i], mY[i] /= n[i];
        for (int y = 0; y < indexIC1.rows; y++)
        {
            const int *idx = indexIC1.ptr<int>(y);
            for (int x = 0; x < indexIC1.cols; x++, idx++)
                vX[*idx] += abs(x - mX[*idx]), vY[*idx] += abs(y - mY[*idx]);
        }
        for (int i = 0; i < regNum; i++)
            vX[i] = vX[i]/n[i] + EPS, vY[i] = vY[i]/n[i] + EPS;
    }

    // Number of border pixels in x and y border region
    std::vector<int> xbNum(regNum), ybNum(regNum);
    int wGap = cvRound(w * ratio);
    int hGap = cvRound(h * ratio);
    std::vector<cv::Point> bPnts;
    for(cv::Point pnt(0, 0); pnt.y != hGap; pnt.y++)
        for(pnt.x = 0; pnt.x != w; pnt.x++)
            ybNum[indexIC1.at<int>(pnt)]++, bPnts.push_back(pnt);
    for(cv::Point pnt(0, h - hGap); pnt.y != h; pnt.y++)
        for(pnt.x = 0; pnt.x != w; pnt.x++)
            ybNum[indexIC1.at<int>(pnt)]++, bPnts.push_back(pnt);
    for(cv::Point pnt(0, 0); pnt.y != h; pnt.y++)
        for(pnt.x = 0; pnt.x != wGap; pnt.x++)
            xbNum[indexIC1.at<int>(pnt)]++, bPnts.push_back(pnt);
    for(cv::Point pnt(0, 0); pnt.y != h; pnt.y++)
        for(pnt.x = w-wGap; pnt.x != w; pnt.x++)
            xbNum[indexIC1.at<int>(pnt)]++, bPnts.push_back(pnt);

    cv::Mat bReg1u(indexIC1.size(), CV_8UC1);
    {  // likelihood map of border region
        double xR = 1.0/(4*hGap);
        double yR = 1.0/(4*wGap);
        std::vector<uchar> regL(regNum); // likelihood of each region belongs to border background
        for (int i = 0; i < regNum; i++)
        {
            double lk = xbNum[i] * xR / vY[i] + ybNum[i] * yR / vX[i];
            regL[i] = lk/thr > 1 ? 255 : 0; //saturate_cast<uchar>(255 * lk / thr);
        }

        for (int r = 0; r < h; r++)
        {
            const int *idx = indexIC1.ptr<int>(r);
            uchar* maskData = bReg1u.ptr<uchar>(r);
            for (int c = 0; c < w; c++, idx++)
                maskData[c] = regL[*idx];
        }
    }

    for (size_t i = 0; i < bPnts.size(); i++)
        bReg1u.at<uchar>(bPnts[i]) = 255;
    return bReg1u;
}

//颜色空间平滑
void RCSaliency::smoothSaliency(cv::Mat &colorNumIC1, cv::Mat &saliencyMapFC1, float delta, const std::vector<std::vector<CostfIdx> > &similar)
{
    if (saliencyMapFC1.cols < 2)
        return;
    CV_Assert(saliencyMapFC1.rows == 1 && saliencyMapFC1.type() == CV_32FC1);
    CV_Assert(colorNumIC1.size() == saliencyMapFC1.size() && colorNumIC1.type() == CV_32SC1);

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
