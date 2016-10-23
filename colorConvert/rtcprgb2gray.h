/*
 * An implementation of "Real-time Contrast Preserving Decolorization" Cewu Lu, Li Xu, Jiaya Jia, ACM SIGGRAPH ASIA 2012 Technical Berief, 2012.
 * License:
   MIT License.
 */
#ifndef RTCPRGB2GRAY_H
#define RTCPRGB2GRAY_H

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <algorithm>
#include <cmath>

class RTCPrgb2gray
{
public:
    RTCPrgb2gray();
    ~RTCPrgb2gray();

    cv::Mat BGR2Gray(const cv::Mat &src);

private:

    inline std::vector<cv::Vec3f> weight();
    inline void add(const cv::Vec3b &c0, const cv::Vec3b &c1, std::vector<cv::Vec3f> &P, std::vector<float> &det);
};

#endif // RTCPRGB2GRAY_H
