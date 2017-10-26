/** ViBe is a background subtraction algorithm, described in:
 *
 * Olivier Barnich and Marc Van Droogenbroeck,
 * ViBe: A Universal Background Subtraction Algorithm for Video Sequences,
 * IEEE Transactions on Image Processing, Vol. 20, No. 6, June 2011.
**/
#ifndef IVIBEBGS_H
#define IVIBEBGS_H

#include <opencv2/core.hpp>
#include <vector>

class IViBeBGS
{
public:
    IViBeBGS();
    ~IViBeBGS();

    void initModel(cv::Mat &bg);
    void detectAndUpdate(const cv::Mat &img, cv::Mat& mask);

    void getEstimatedBG(cv::Mat &bg)
    {
       estimated_bg.copyTo(bg);
    }

private:
    cv::RNG rng;//随机数生成器
    cv::Size img_size; // Frame size.
    int img_type; // Frame type. Must be CV_8UC1 or CV_8UC3.

    std::vector<cv::Mat> bg_samples; // Background samples. Is an array containing samples_per_pixel images.
    cv::Mat estimated_bg; // Estimated background. This is not part of the original method, but is important for other purposes.

    //samples_per_pixel: number of samples kept for each pixel,identified as N in the paper.
    //The default value is 20.
    int samples_per_pixel;
    //min_neighbors_for_bg: minimum number of similar pixels in the model for a pixel to be classified as background.
    //This is the #_min parameter from the paper, and it has thedefault value of 2.
    int min_neighbors_for_bg;
    //max_distance_for_bg: maximum 3D distance for two pixels to be considered similar.
    //This is the R parameter from the paper,with default value of 20.
    float max_distance_for_bg;
    //update_probability: the chance of using one pixel to update the model is 1/update_probability.
    //his is the phi parameter from the paper, with default value of 16.
    int update_probability;

private:

    void initBGModel (cv::Mat& bg);
    inline int pixelDistance (const uchar* p1, const uchar* p2, int n_channels);

    void init();
};

#endif // IVIBEBGS_H
