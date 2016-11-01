#include "mssaliency.h"
#include <iostream>

MSSaliency::MSSaliency()
{
    std::cout<<"MSSaliency()"<<std::endl;
}

MSSaliency::~MSSaliency()
{
    std::cout<<"~MSSaliency()"<<std::endl;
}

cv::Mat MSSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels()==3);

    cv::Mat input;
    cv::Mat tempLab;
    cv::Mat sumLab;
    float mean0 = 0.0f;
    float mean1 = 0.0f;
    float mean2 = 0.0f;
    cv::Mat saliencyMap(src.size(), CV_32FC1);
    src.convertTo(input,CV_32FC3, 1.0/255.0);
    cv::cvtColor(input, tempLab, cv::COLOR_BGR2Lab);
    cv::GaussianBlur(tempLab, tempLab, cv::Size(3, 3), 0, 0);
    cv::integral(tempLab, sumLab, CV_32F);
    int height = sumLab.rows;
    int width = sumLab.cols;
    for( int r = 1; r < height; r++ )
    {
        float *dataS = saliencyMap.ptr<float>(r - 1);
        float *lab = tempLab.ptr<float>(r - 1);
        int yoff = std::min(r, height - r);
        int row1 = std::max(r - yoff, 1);
        int row2 = std::min(r + yoff, height - 1);
        float* data1 = sumLab.ptr<float>(row1 - 1);
        float* data2 = sumLab.ptr<float>(row2);

        for( int c = 1; c < width; c++, lab += 3)
        {
            int xoff = std::min(c, width - c);
            int col1 = std::max(c - xoff, 1);
            int col2 = std::min(c + xoff, width - 1);
            int area = (col2 - col1 + 1) * (row2 - row1 + 1);
//            cv::Vec3f p0 = data0[col1-1];
//            cv::Vec3f p1 = data0[col2];
//            cv::Vec3f p2 = data1[col1-1];
//            cv::Vec3f p3 = data1[col2];
            float* p0 = data1 + (col1 - 1) * sumLab.channels();
            float* p1 = data1 + col2 * sumLab.channels();
            float* p2 = data2 + (col1 - 1) * sumLab.channels();
            float* p3 = data2 + col2 *sumLab.channels();

            mean0 = (*p3++ - *p2++ - *p1++ + *p0++) / area;
            mean1 = (*p3++ - *p2++ - *p1++ + *p0++) / area;
            mean2 = (*p3 - *p2 - *p1 + *p0) / area;

            dataS[c-1] = (float)((mean0 - lab[0])*(mean0 - lab[0]) +
                    (mean1 - lab[1])*(mean1 - lab[1]) +
                    (mean2 - lab[2])*(mean2 - lab[2]));
        }
    }
    cv::normalize(saliencyMap, saliencyMap, 1.0, 0, cv::NORM_MINMAX);
    return saliencyMap;
}
