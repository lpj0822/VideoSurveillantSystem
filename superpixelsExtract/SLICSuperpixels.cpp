#include "SLICSuperpixels.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <algorithm>
#include <iostream>

#define MAX_ITERATION 5

SLICSuperpixels::SLICSuperpixels(): K(225), compactness(20)
{
    std::cout << "SLICSuperpixels()" << std::endl;
}

SLICSuperpixels::~SLICSuperpixels()
{
    std::cout << "~SLICSuperpixels()" << std::endl;
}

void SLICSuperpixels::run(const cv::Mat &src)
{
    this->width = src.cols;
    this->height = src.rows;
    int N = (int)sqrt(K);
    float dx = width / float(N);
    float dy = height / float(N);

    this->S = (int)((dx + dy + 1) / 2);

    // Normalize original image between 0 and 1.0
    cv::Mat normalized_image;
    src.convertTo(normalized_image, CV_32F, 1.0/255.0);

    // Convert normalized image from BGR to CIELAB
    cv::Mat lab_image;
    cv::cvtColor(normalized_image, lab_image, cv::COLOR_BGR2Lab);
    this->image = normalized_image;

    // TODO: Use dinamic grid instead of regular one
    // int N_ = height*width;
    // int K_ = 194;
    // int sp_size = N_/K_;
    // int S_ = sqrt(sp_size); // grid interval
    // this->S = S_;
    // cout << height << " " << width << " " << sp_size << " " << S_ << " " << K_ << endl;

    // vector<Point2i> centroids;
    // for (int i=0; i<height-S_; i+=S_) {
    // 	for (int j=0; j<width-S_; j+=S_) {
    // 		float ci = (i + (i+S_))/2;
    // 		float cj = (j + (j+S_))/2;
    // 		//cout << i << " " << j << " " << ci << " " << cj << endl;
    // 		centroids.push_back(Point2f(ci, cj));
    // 	}
    // }

    // Initialize centroids on a regular grid
    for (size_t i = 0, max = N; i < max; ++i)
    {
        for (size_t j = 0, max = N; j < max; ++j)
        {
            this->centroids.push_back(cv::Point2f( dx*i + dx/2, dy*j + dy/2 ));
        }
    }

    // Initialize labels width ones and distances with "infinity"
    cv::Mat distances = cv::Mat(lab_image.size(),CV_32F, cv::Scalar(std::numeric_limits<float>::max()));
    this->labels = cv::Mat(lab_image.size(), CV_32S, cv::Scalar(1));

    float actual_distance, last_distance;
    cv::Point2i centroid_coords, neighbor_coords;
    cv::Vec3f centroid_lab, neighbor_lab;

    int count = (int)centroids.size();
    // Iterate many times
    for (int i = 0, max = MAX_ITERATION; i < max; ++i)
    {
        // Iterate over all centroids (one label per centroid)
        for (int centroid = 0, max = count; centroid < max; ++centroid)
        {
            // Get centroid's coordiantes and CIELAB color
            centroid_coords = centroids[centroid];
            centroid_lab = lab_image.at<cv::Vec3f>(centroid_coords);

            // Find centroid's neighbors
            find_neighbors_of(centroid_coords);

            // Update neighbors to their closest centroid (like K-means)
            for (int loop = 0, max = this->neighbors.rows; loop < max; ++loop)
            {
                for (int loop1 = 0, max = this->neighbors.cols; loop1 < max; ++loop1)
                {
                    // Get centroid's neighbors coordinates and CIELAB color
                    neighbor_coords = cv::Point2i(this->min_x + loop1, this->min_y + loop);
                    neighbor_lab = lab_image.at<cv::Vec3f>(neighbor_coords);

                    actual_distance = distance_between_points(centroid_coords,neighbor_coords,centroid_lab,neighbor_lab);

                    last_distance = distances.at<float>(neighbor_coords);

                    // Update if the actual distance between centroid and pixel is less
                    // than the last time it was calculated
                    if (actual_distance < last_distance)
                    {
                        distances.at<float>(neighbor_coords) = actual_distance;
                        labels.at<int>(neighbor_coords) = centroid;
                    }
                }
            }
        }
    }
}

void SLICSuperpixels::find_neighbors_of(cv::Point2i point)
{
    int max_x, max_y;

    // Take care of image limits while finding the window we will search on
    this->min_x = std::max(point.x - this->S, 0);
    max_x = std::min(point.x + this->S, this->width - 1);

    this->min_y = std::max(point.y - this->S, 0);
    max_y = std::min(point.y + this->S, this->height - 1);

    this->neighbors = image(cv::Range(this->min_y, max_y),
                              cv::Range(this->min_x, max_x));
}

float SLICSuperpixels::distance_between_points(cv::Point2i coords1, cv::Point2i coords2, cv::Vec3f lab1, cv::Vec3f lab2)
{
    float color_distance = (float)sqrt( (lab2[0] - lab1[0]) * (lab2[0] - lab1[0]) +
                                   (lab2[1] - lab1[1]) * (lab2[1] - lab1[1]) +
                                 (lab2[2] - lab1[2]) * (lab2[2] - lab1[2]) );

    float spatial_distance = (float)sqrt( (coords2.x - coords1.x) *
                                     (coords2.x - coords1.x) +
                                     (coords2.y - coords1.y) *
                                     (coords2.y - coords1.y) );

    return color_distance +
           float(this->compactness) / float(this->S) *
           spatial_distance;
}

cv::Mat SLICSuperpixels::getLabels()
{
    //writeLabelsToFile("li.png");
    //writeCentroidsToFile("li1.png");
    return this->labels;
}

void SLICSuperpixels::writeLabelsToFile(std::string file_path)
{
    cv::imwrite(file_path, this->labels);
}

void SLICSuperpixels::writeCentroidsToFile(std::string file_path)
{
    cv::Mat bgr_image;
    this->labels.convertTo(bgr_image, cv::COLOR_Lab2BGR);

    for (size_t i=0, max = centroids.size(); i < max; ++i)
    {
        cv::circle(bgr_image, cv::Point(this->centroids[i].x, this->centroids[i].y), 2, cv::Scalar(255, 0, 0), -1);
    }

    cv::imwrite(file_path, bgr_image);
}
