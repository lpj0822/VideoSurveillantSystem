/*
 * 静态显著图检测
*/
#ifndef ISALIENCY_H
#define ISALIENCY_H

#include <opencv2/core.hpp>

class ISaliency
{
public:
    virtual ~ISaliency();

    virtual cv::Mat getSaliency(const cv::Mat &src) = 0;
};

#endif // ISALIENCY_H
