#include "acsaliency.h"
#include <iostream>

ACSaliency::ACSaliency()
{
    std::cout<<"ACSaliency()"<<std::endl;
}

ACSaliency::~ACSaliency()
{
    std::cout<<"~ACSaliency()"<<std::endl;
}

cv::Mat ACSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3);

    cv::Mat input;
    cv::Mat tempLab;
    cv::Mat sumLab;
    float mean10 = 0.0f;
    float mean11 = 0.0f;
    float mean12 = 0.0f;
    float mean20 = 0.0f;
    float mean21 = 0.0f;
    float mean22 = 0.0f;
    float mean30 = 0.0f;
    float mean31 = 0.0f;
    float mean32 = 0.0f;
    float sal1 = 0.0f;
    float sal2 = 0.0f;
    float sal3 = 0.0f;
    cv::Mat saliencyMap(src.size(), CV_32FC1);
    src.convertTo(input,CV_32FC3, 1.0 / 255.0);
    cv::cvtColor(input, tempLab, cv::COLOR_BGR2Lab);
    cv::GaussianBlur(tempLab, tempLab, cv::Size(3, 3), 0, 0);
    cv::integral(tempLab, sumLab, CV_32F);
    int height = sumLab.rows;
    int width = sumLab.cols;
    int mindim = std::min(tempLab.rows, tempLab.cols);
    int off1 = mindim / 2;
    int off2 = mindim / 4;
    int off3 = mindim / 8;
    for (int r = 1; r < height; r++)
    {
        float *dataS = saliencyMap.ptr<float>(r - 1);
        float *lab = tempLab.ptr<float>(r - 1);

        int y11 = std::max(r - off1, 1);
        int y12 = std::min(r + off1, height - 1);
        int y21 = std::max(r - off2, 1);
        int y22 = std::min(r + off2, height - 1);
        int y31 = std::max(r - off3, 1);
        int y32 = std::min(r + off3, height - 1);

        float* data11 = sumLab.ptr<float>(y11 - 1);
        float* data12 = sumLab.ptr<float>(y12);
        float* data21 = sumLab.ptr<float>(y21 - 1);
        float* data22 = sumLab.ptr<float>(y22);
        float* data31 = sumLab.ptr<float>(y31 - 1);
        float* data32 = sumLab.ptr<float>(y32);

        for( int c = 1; c < width; c++, lab += 3)
        {
            int x11 = std::max(c - off1, 1);
            int x12 = std::min(c + off1, width - 1);
            int x21 = std::max(c - off2, 1);
            int x22 = std::min(c + off2, width - 1);
            int x31 = std::max(c - off3, 1);
            int x32 = std::min(c + off3, width - 1);

            int area1 = (x12 - x11 + 1) * (y12 - y11 + 1);
            int area2 = (x22 - x21 + 1) * (y22 - y21 + 1);
            int area3 = (x32 - x31 + 1) * (y32 - y31 + 1);

            float* p10 = data11 + (x11 - 1) * sumLab.channels();
            float* p11 = data11 + x12 * sumLab.channels();
            float* p12 = data12 + (x11 - 1) * sumLab.channels();
            float* p13 = data12 + x12 *sumLab.channels();

            float* p20 = data21 + (x21 - 1) * sumLab.channels();
            float* p21 = data21 + x22 * sumLab.channels();
            float* p22 = data22 + (x21 - 1) * sumLab.channels();
            float* p23 = data22 + x22 *sumLab.channels();

            float* p30 = data31 + (x31 - 1) * sumLab.channels();
            float* p31 = data31 + x32 * sumLab.channels();
            float* p32 = data32 + (x31 - 1) * sumLab.channels();
            float* p33 = data32 + x32 *sumLab.channels();

            mean10 = (*p13++ - *p12++ - *p11++ + *p10++) / area1;
            mean11 = (*p13++ - *p12++ - *p11++ + *p10++) / area1;
            mean12 = (*p13 - *p12 - *p11 + *p10) / area1;

            mean20 = (*p23++ - *p22++ - *p21++ + *p20++) / area2;
            mean21 = (*p23++ - *p22++ - *p21++ + *p20++) / area2;
            mean22 = (*p23 - *p22 - *p21 + *p20) / area2;

            mean30 = (*p33++ - *p32++ - *p31++ + *p30++) / area3;
            mean31 = (*p33++ - *p32++ - *p31++ + *p30++) / area3;
            mean32 = (*p33 - *p32 - *p31 + *p30) / area3;

            sal1= (mean10 - lab[0])*(mean10 - lab[0]) +
                    (mean11 - lab[1])*(mean11 - lab[1]) +
                    (mean12 - lab[2])*(mean12 - lab[2]);
            sal2= (mean20 - lab[0])*(mean20 - lab[0]) +
                    (mean21 - lab[1])*(mean21 - lab[1]) +
                    (mean22 - lab[2])*(mean22 - lab[2]);
            sal3= (mean30 - lab[0])*(mean30 - lab[0]) +
                    (mean31 - lab[1])*(mean31 - lab[1]) +
                    (mean32 - lab[2])*(mean32 - lab[2]);

            dataS[c-1] = sal1 + sal2 + sal3;
        }
    }
    cv::normalize(saliencyMap, saliencyMap, 1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}
