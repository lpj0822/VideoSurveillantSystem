/*
 * This is an implementation：
 * Fast Global Image Smoothing Based on Weighted Least Squares,
 * D. Min, S. Choi, J. Lu, B. Ham, K. Sohn, and M. N. Do,
 * IEEE Trans. on Image Processing (TIP), 23(12), 5638-5653, 2014.
 */
#ifndef FGS_HEADER
#define FGS_HEADER

#include <opencv2/core.hpp>

// x = FGS(input_image, guidance_image = NULL, sigma, lambda, fgs_iteration = 3, fgs_attenuation = 4);
void FGS(const cv::Mat_<float> &in, const cv::Mat_<cv::Vec3b> &color, cv::Mat_<float> &out, double sigma, double lambda, int fgs_iteration = 3, int fgs_attenuation = 4);

#endif
