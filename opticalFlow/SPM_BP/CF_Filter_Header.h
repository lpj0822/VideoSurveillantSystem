/*
 * This is an implementation：
 * "Cross-based local multipoint filtering,"
 * J. Lu, K. Shi, D. Min, L. Lin, and M. N. Do,
 * in Proc. IEEE Int. Conf. Computer Vision and Pattern Recognition (CVPR), Providence, Rhode Island, 2012.
 */
#ifndef CF_FILTER_HEADER
#define CF_FILTER_HEADER

#include <opencv2/core.hpp>
#include <cmath>
#include <omp.h>
#include <cstdio>
#include <algorithm>

class CFFilter
{
public:
	cv::Mat_<cv::Vec3b> img;
	cv::Mat_<cv::Vec4b> crossMapN;
	cv::Mat_<cv::Vec4b> crossMapSW;

	cv::Mat_<cv::Vec4b> crossGrad;

    CFFilter()
    {

    }

    ~CFFilter()
    {

    }

	void GetCrossUsingSlidingWindow(const cv::Mat_<cv::Vec3b> &img, cv::Mat_<cv::Vec4b> &crMap, int maxL, int tau);
	void FastCLMF0FloatFilterPointer( const cv::Mat_<cv::Vec4b> &crMap, const cv::Mat_<float> &src, cv::Mat_<float> &dst );
};

#endif
