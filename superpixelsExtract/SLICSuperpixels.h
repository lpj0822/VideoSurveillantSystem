/*
 * This code implements the superpixel method described in:
 * Radhakrishna Achanta, Appu Shaji, Kevin Smith, Aurelien Lucchi, Pascal Fua, and Sabine Susstrunk,
 * "SLIC Superpixels",
 * EPFL Technical Report no. 149300, June 2010.
*/
#ifndef SLICSUPERPIXELS_H
#define SLICSUPERPIXELS_H

#include "isuperpixelsextract.h"
#include <vector>

class SLICSuperpixels : public ISuperpixelsExtract
{
public:
    SLICSuperpixels();
    ~SLICSuperpixels();

    void run(const cv::Mat &src);
    cv::Mat getLabels();

private:

    cv::Mat labels;
    int countSuperpixels;

    int compactness;
    int S;
    int K;
    int width;
    int height;
    int min_x;
    int min_y;
    std::vector<cv::Point2i> centroids;
    cv::Mat image;
    cv::Mat neighbors;

private:

    float distance_between_points(cv::Point2i coords1, cv::Point2i coords2, cv::Vec3f lab1, cv::Vec3f lab2);
    void find_neighbors_of(cv::Point2i point);

    //Write cluster labels to an image file
    void writeLabelsToFile(std::string file_path);

    //Write cluster centroids to an image file
    void writeCentroidsToFile(std::string file_path);
};

#endif // SLICSUPERPIXELS_H
