#include "colorquantize.h"
#include "../utility.h"

ColorQuantize::ColorQuantize()
{

}

ColorQuantize::~ColorQuantize()
{

}

const int ColorQuantize::binNum[] = {6*6*6, 256, 5*7*7, 256};
const S_QUANTIZE_FUNC ColorQuantize::sqFuns[] = {ColorQuantize::SQ_BGR, ColorQuantize::SQ_HSV, ColorQuantize::SQ_Lab};
const S_RECOVER_FUNC  ColorQuantize::srFuns[] = {ColorQuantize::SR_BGR, ColorQuantize::SR_HSV, ColorQuantize::SR_Lab};

// img3f is the input BGR image
void ColorQuantize::S_Quantize(cv::Mat& _img3f, cv::Mat &idx1i, int method)
{
    CV_Assert(method >= 0 && method < S_Q_NUM && _img3f.data != NULL && _img3f.type() == CV_32FC3);
    S_QUANTIZE_FUNC SQ_Function = sqFuns[method];

    cv::Mat img;
    switch (method)
    {
    case S_BGR:
        img = _img3f;
        break;
    case S_HSV:
        cv::cvtColor(_img3f, img, cv::COLOR_BGR2HSV);
        break;
    case S_LAB:
        cv::cvtColor(_img3f, img, cv::COLOR_BGR2Lab);
        break;
    }

    idx1i.create(img.size(), CV_32SC1);
    for (int r = 0; r < img.rows; r++)
    {
        const cv::Vec3f * imgD = img.ptr<cv::Vec3f>(r);
        int *idx = idx1i.ptr<int>(r);
        for (int c = 0; c < img.cols; c++)
            idx[c] = (*SQ_Function)(imgD[c]);
    }
}

// src3f are BGR, color3f are 1xBinDim matrix represent color fore each histogram bin
int ColorQuantize::S_BinInf(cv::Mat& idx1i, cv::Mat &color3f, std::vector<int> &colorNum, int method, cv::Mat &src3f)
{
    int totalBinNum = 0;
    CV_Assert(idx1i.data != NULL && idx1i.type() == CV_32S && method >= 0 && method < S_Q_NUM);

    // Find colors for each bin
    color3f = cv::Mat::zeros(1, binNum[method], CV_32FC3);
    cv::Vec3f* color = (cv::Vec3f*)(color3f.data);

    std::vector<cv::Vec3d> colorD(color3f.cols, 0);
    colorNum.resize(color3f.cols, 0);
    if (src3f.size() != cv::Size() && src3f.data != NULL)
    {
        for (int r = 0; r < idx1i.rows; r++)
        {
            const int *idx = idx1i.ptr<int>(r);
            const cv::Vec3f *src = src3f.ptr<cv::Vec3f>(r);
            for (int c = 0; c < idx1i.cols; c++)
            {
                colorD[idx[c]] += src[c];
                colorNum[idx[c]] ++;
            }
        }
    }
    S_RECOVER_FUNC SR_Function = srFuns[method];
    for (int i = 0; i < color3f.cols; i++)
    {
        if (colorNum[i] == 0)
            (*SR_Function)(i, color[i]);
        else
            totalBinNum += colorNum[i];
    }
    if (method == 1)
        cv::cvtColor(color3f, color3f, cv::COLOR_HSV2BGR);
    else if (method == 2)
        cv::cvtColor(color3f, color3f, cv::COLOR_Lab2BGR);

    for (int i = 0; i < color3f.cols; i++)
        if (colorNum[i] > 0)
            color[i] = cv::Vec3f((float)(colorD[i][0]/colorNum[i]), (float)(colorD[i][1]/colorNum[i]), (float)(colorD[i][2]/colorNum[i]));

    return totalBinNum;
}

// img3f and src3f are BGR
void ColorQuantize::S_Recover(cv::Mat& idx1i,cv::Mat& img3f, int method, cv::Mat &src3f)
{
    cv::Mat color3f;
    std::vector<int> colorNum;
    S_BinInf(idx1i, color3f, colorNum, method, src3f);
    cv::Vec3f* color = (cv::Vec3f*)(color3f.data);

    img3f.create(idx1i.size(), CV_32FC3);
    for (int r = 0; r < idx1i.rows; r++)
    {
        const int *idx = idx1i.ptr<int>(r);
        cv::Vec3f *img = img3f.ptr<cv::Vec3f>(r);
        for (int c = 0; c < idx1i.cols; c++)
            img[c] = color[idx[c]];
    }
}

int ColorQuantize::SQ_BGR(const cv::Vec3f &c)
{
    return (int)(c[0]*5.9999f) * 36 + (int)(c[1]*5.9999f) * 6 + (int)(c[2]*5.9999f);
}

void ColorQuantize::SR_BGR(int idx, cv::Vec3f &c)
{
    c[0] = ((float)(idx / 36) + 0.5f)/6.f;
    idx %= 36;
    c[1] = ((float)(idx / 6) + 0.5f)/6.f;
    c[2] = ((float)(idx % 6) + 0.5f)/6.f;
}

int ColorQuantize::SQ_HSV(const cv::Vec3f &c)
{
    const float S_MIN_HSV = 0.1f;
    float h(c[0]/360.001f), s(c[1]/1.001f), v(c[2]/1.001f);

    int result;
    if (s < S_MIN_HSV) // 240 ... 255
        result = 240 + (int)(v * 16.f);
    else
    {
        int ih, is, iv;
        ih = (int)(h * 15.f); //0..14
        is = (int)((s - S_MIN_HSV)/(1 - S_MIN_HSV) * 4.f); //0..3
        iv = (int)(v * 4.f); //0..3
        result = ih * 16 + is * 4 + iv; // 0..239

        CV_Assert(ih < 15 && is < 4 && iv < 4);
    }

    return result;
}

void ColorQuantize::SR_HSV(int idx, cv::Vec3f &c)
{
    const float S_MIN_HSV = 0.1f;
    if (idx >= 240)
    {
        c[0] = c[1] = 0;
        c[2] = (idx - 239.5f) / 16.f;
    }
    else
    {
        c[0] = (float(idx / 16) + 0.5f) / 15.f;
        idx %= 16;
        c[1] = (float(idx / 4) + 0.5f) * (1 - S_MIN_HSV) / 4.f + S_MIN_HSV;
        c[2] = (float(idx % 4) + 0.5f) / 4.f;
    }
    CV_Assert(c[0] < 1.f && c[1] < 1.f && c[2] < 1.f);
    c[0] *= 360.f;
}

int ColorQuantize::SQ_Lab(const cv::Vec3f &c)
{
    float L(c[0] / 100.0001f), a((c[1] + 127) / 254.0001f), b((c[2] + 127) / 254.0001f);
    int iL = (int)(L * 5), ia = (int)(a * 7), ib = (int)(b * 7);
    CV_Assert(iL >= 0 && ia >= 0 && ib >= 0 && iL < 5 && ia < 7 && ib < 7);
    return iL + ia * 5 + ib * 35;
}

void ColorQuantize::SR_Lab(int idx, cv::Vec3f &c)
{
    c[2] = (float(idx / 35) + 0.5f) * (254.f / 7.f) - 127.f;
    idx %= 35;
    c[1] = (float(idx / 5) + 0.5f) * (254.f / 7.f) - 127.f;
    c[0] = (float(idx % 5) + 0.5f) * (100.f / 5.f);

    CV_Assert(c[0] < 100.f && c[0] >= 0.f && c[1] < 127.f  && c[1] > -127.f && c[2] < 127.f && c[2] > -127.f);
}

const int ColorQuantize::DefaultNums[3] = {12, 12, 12};

int ColorQuantize::D_Quantize(cv::Mat& img3f, cv::Mat &idx1i, cv::Mat &_color3f, cv::Mat &_colorNum, double ratio, const int clrNums[3])
{
    float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
    int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};

    CV_Assert(img3f.data != NULL);
    idx1i = cv::Mat::zeros(img3f.size(), CV_32SC1);
    int rows = img3f.rows, cols = img3f.cols;
    if (img3f.isContinuous() && idx1i.isContinuous())
    {
        cols *= rows;
        rows = 1;
    }

    // Build color pallet
    for (int y = 0; y < rows; y++)
    {
        const float* imgDataP = img3f.ptr<float>(y);
        int* idx = idx1i.ptr<int>(y);
        for (int x = 0; x < cols; x++)
        {
            const float* imgData = imgDataP + 3*x;
            idx[x] = (int)(imgData[0]*clrTmp[0])*w[0] + (int)(imgData[1]*clrTmp[1])*w[1] + (int)(imgData[2]*clrTmp[2]);
        }
    }
    std::map<int, int> pallet;
    for (int y = 0; y < rows; y++)
    {
        int* idx = idx1i.ptr<int>(y);
        for (int x = 0; x < cols; x++)
            pallet[idx[x]] ++;
    }

    // Find significant colors
    int maxNum = 0;
    {
        std::vector<std::pair<int, int>> num; // (num, color) pairs in num
        num.reserve(pallet.size());
        for (std::map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++)
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

        int numSZ = (int)num.size();
        std::vector<cv::Vec3i> color3i(numSZ);
#pragma omp parallel for
        for (int i = 0; i < numSZ; i++)
        {
            color3i[i][0] = num[i].second / w[0];
            color3i[i][1] = num[i].second % w[0] / w[1];
            color3i[i][2] = num[i].second % w[1];
        }

        for (unsigned int i = maxNum; i < num.size(); i++)
        {
            int simIdx = 0, simVal = INT_MAX;
#pragma omp parallel for
            for (int j = 0; j < maxNum; j++)
            {
                int d_ij = vecSquareDist(color3i[i], color3i[j]);
                if (d_ij < simVal)
                    simVal = d_ij, simIdx = j;
            }
            pallet[num[i].second] = pallet[num[simIdx].second];
        }
    }

    _color3f = cv::Mat::zeros(1, maxNum, CV_32FC3);
    _colorNum = cv::Mat::zeros(_color3f.size(), CV_32SC1);

    cv::Vec3f* color = (cv::Vec3f*)(_color3f.data);
    int* colorNum = (int*)(_colorNum.data);
    for (int y = 0; y < rows; y++)
    {
        const cv::Vec3f* imgData = img3f.ptr<cv::Vec3f>(y);
        int* idx = idx1i.ptr<int>(y);
#pragma omp parallel for
        for (int x = 0; x < cols; x++)
            idx[x] = pallet[idx[x]];
        for (int x = 0; x < cols; x++)
        {
            color[idx[x]] += imgData[x];
            colorNum[idx[x]] ++;
        }
    }
    for (int i = 0; i < _color3f.cols; i++)
        color[i] /= colorNum[i];

    return _color3f.cols;
}

void ColorQuantize::D_Recover(cv::Mat& idx1i, cv::Mat &img3f, cv::Mat &color3f)
{
    CV_Assert(idx1i.data != NULL);
    img3f.create(idx1i.size(), CV_32FC3);

    cv::Vec3f* color = (cv::Vec3f*)(color3f.data);
    for (int y = 0; y < idx1i.rows; y++)
    {
        cv::Vec3f* imgData = img3f.ptr<cv::Vec3f>(y);
        const int* idx = idx1i.ptr<int>(y);
        for (int x = 0; x < idx1i.cols; x++)
        {
            imgData[x] = color[idx[x]];
            CV_Assert(idx[x] < color3f.cols);
        }
    }
}
