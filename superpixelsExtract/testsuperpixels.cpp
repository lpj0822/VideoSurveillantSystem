#include "testsuperpixels.h"
#include <iostream>

void labelContourMask(const cv::Mat &labels, cv::Mat &contour, bool thick_line)
{
    if (labels.empty())
        std::cout << "image is empty" << std::endl;
    if (labels.type() != CV_32SC1)
        std::cout <<  "labels mush have CV_32SC1 type" << std::endl;

    int width = labels.cols;
    int height = labels.rows;

    contour.create(height, width, CV_8UC1);
    contour.setTo(cv::Scalar(0));

    const int dx8[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
    const int dy8[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };

    for (int j = 0; j < height; j++)
    {
        for (int k = 0; k < width; k++)
        {
            int neighbors = 0;
            for (int i = 0; i < 8; i++)
            {
                int x = k + dx8[i];
                int y = j + dy8[i];

                if( (x >= 0 && x < width) && (y >= 0 && y < height) )
                {
                    if( labels.at<int>(j, k) != labels.at<int>(y, x) )
                    {
                        if( thick_line || !contour.at<uchar>(y, x) )
                            neighbors++;
                    }
                }
            }
            if( neighbors > 1 )
                contour.at<uchar>(j, k) = (uchar)-1;
        }
    }
}

void drawImage(const cv::Mat &contour, cv::Mat &src)
{
    src.setTo(cv::Scalar(255, 255, 255), contour);
}
