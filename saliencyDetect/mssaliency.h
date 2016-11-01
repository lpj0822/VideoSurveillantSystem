/*
 * Radhakrishna Achanta and Sabine Susstrunk,
 * Saliency Detection using Maximum Symmetric Surround,
 * International Conference on Image Processing (ICIP),Hong Kong, September 2010.
**/
#ifndef MSSALIENCY_H
#define MSSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <vector>

#include "isaliency.h"

class MSSaliency: public ISaliency
{
public:
    MSSaliency();
    ~MSSaliency();

    cv::Mat getSaliency(const cv::Mat &src);
};

#endif // MSSALIENCY_H
