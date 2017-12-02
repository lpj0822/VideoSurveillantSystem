/*
 * Copyright (c) 2012, Philipp
 * All rights reserved.
 * Implement:
 * Perazzi, Federico, et al. "Saliency filters: Contrast based filtering for salient region detection."
 * Computer Vision and Pattern Recognition (CVPR), 2012 IEEE Conference on. IEEE, 2012.
*/
#ifndef SFSALIENCY_H
#define SFSALIENCY_H

#include <opencv2/imgproc.hpp>
#include "superpixel/superpixel.h"
#include "../isaliency.h"

struct SaliencySettings
{
    SaliencySettings();

    // Superpixel settings
    int n_superpixels_, n_iterations_;
    float superpixel_color_weight_;

    // Saliency filter radii
    float sigma_p_; // Radius for the uniqueness operator [eq 1]
    float sigma_c_; // Color radius for the distribution operator [eq 3]
    float k_; // The sharpness parameter of the exponential in the merging operation [eq 5]

    // Upsampling parameters
    float min_saliency_; // Minimum number of salient pixels for final rescaling
    float alpha_, beta_;

    // Various algorithm settings
    // Enable or disable parts of the algorithm
    bool upsample_, uniqueness_, distribution_, filter_uniqueness_, filter_distribution_;
    // Should we use the image color or superpixel color as a feature for upsampling
    bool use_spix_color_;
};

class SFSaliency : public ISaliency
{
public:
    SFSaliency(SaliencySettings settings = SaliencySettings());
    ~SFSaliency();

    cv::Mat getSaliency(const cv::Mat &src);

protected:
    SaliencySettings settings;
    Superpixel superpixel;
protected:
    std::vector< float > uniqueness( const std::vector< SuperpixelStatistic > & stat ) const;
    std::vector< float > uniquenessFilter( const std::vector< SuperpixelStatistic > & stat ) const;
    std::vector< float > distribution( const std::vector< SuperpixelStatistic > & stat ) const;
    std::vector< float > distributionFilter( const std::vector< SuperpixelStatistic > & stat ) const;
    cv::Mat_<float> assign( const cv::Mat_<int> & seg, const std::vector< float > & sal ) const;

    cv::Mat_<float> assignFilter( const cv::Mat_<cv::Vec3b> & im, const cv::Mat_<int> & seg, const std::vector< SuperpixelStatistic > & stat, const std::vector< float > & sal ) const ;
};

#endif // SFSALIENCY_H
