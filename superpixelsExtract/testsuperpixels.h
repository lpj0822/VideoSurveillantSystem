#ifndef TESTSUPERPIXELS_H
#define TESTSUPERPIXELS_H

#include <opencv2/core.hpp>

void labelContourMask(const cv::Mat &labels, cv::Mat &contour, bool thick_line);

void drawImage(const cv::Mat &contour, cv::Mat &src);

#endif // TESTSUPERPIXELS_H
