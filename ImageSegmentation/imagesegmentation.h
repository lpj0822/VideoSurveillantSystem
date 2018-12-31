/*
 * image segmentation
 */
#ifndef IMAGESEGMENTATION_H
#define IMAGESEGMENTATION_H

#include <opencv2/core.hpp>

class ImageSegmentation
{
public:
    virtual ~ImageSegmentation();
    virtual cv::Mat getSegmentMap(const cv::Mat &src, const cv::Mat &weightMap) = 0;
};

#endif // IMAGESEGMENTATION_H
