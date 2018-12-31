#ifndef MEANSHIFTSEGMENTATION_H
#define MEANSHIFTSEGMENTATION_H

#include <opencv2/imgproc.hpp>
#include "imagesegmentation.h"

class MeanShiftSegmentation : public ImageSegmentation
{
public:
    MeanShiftSegmentation();
    ~MeanShiftSegmentation();

    cv::Mat getSegmentMap(const cv::Mat &src, const cv::Mat &weightMap);
};

#endif // MEANSHIFTSEGMENTATION_H
