/*
 *Y. Zhai and M. Shah. Visual attention detection in video sequences
 *   using spatiotemporal cues. In ACM Multimedia 2006.
 */
#ifndef LCSALIENCY_H
#define LCSALIENCY_H

#include <opencv2/imgproc.hpp>
#include <vector>
#include "colorConvert/rtcprgb2gray.h"
#include "isaliency.h"

class LCSaliency: public ISaliency
{
public:
    LCSaliency();
    ~LCSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

private:
    RTCPrgb2gray rtcpRgb2gray;
};

#endif // LCSALIENCY_H
