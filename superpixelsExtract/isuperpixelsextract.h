/*
 * Superpixels Extract
 */
#ifndef ISUPERPIXELSEXTRACT_H
#define ISUPERPIXELSEXTRACT_H

#include <opencv2/core.hpp>

class ISuperpixelsExtract
{
public:
    virtual ~ISuperpixelsExtract();

    virtual void run(const cv::Mat &src) = 0;

    virtual cv::Mat getLabels() = 0;
};

#endif // ISUPERPIXELSEXTRACT_H
