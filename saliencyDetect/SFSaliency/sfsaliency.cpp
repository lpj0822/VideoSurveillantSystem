#include "sfsaliency.h"
#include "utility/fastmath.h"
#include "filter/filter.h"

#include <iostream>

SaliencySettings::SaliencySettings()
{
    // Superpixel settings
    n_superpixels_ = 400;
    n_iterations_= 5;
    superpixel_color_weight_ = 1;

    // Saliency filter radii
    sigma_p_ = 0.25;
    sigma_c_ = 20.0;
    k_ = 3; // The paper states K=6 (but then I think we used standard deviation and not variance, was probably a typo)

    // Upsampling parameters
    min_saliency_ = 0.1f;
    alpha_ = 1.0f / 30.0f;
    beta_ = 1.0f / 30.0f;

    // Various algorithm settings
    upsample_ = true;
    uniqueness_ = true;
    distribution_ = true;
    filter_uniqueness_ = filter_distribution_ = false;
    use_spix_color_ = false; // Disabled to get a slightly better performance
}


SFSaliency::SFSaliency( SaliencySettings settings ): superpixel( settings.n_superpixels_, settings.superpixel_color_weight_, settings.n_iterations_ )
{
    this->settings = settings;

    std::cout << "SFSaliency()" << std::endl;
}

SFSaliency::~SFSaliency()
{
    std::cout << "~SFSaliency()" << std::endl;
}

cv::Mat SFSaliency::getSaliency(const cv::Mat &src)
{
    CV_Assert(src.channels() == 3);
    // Convert the image to the lab space
    cv::Mat_<cv::Vec3f> rgbim, labim;
    src.convertTo( rgbim, CV_32F, 1.0/255. );
    cv::cvtColor( rgbim, labim, cv::COLOR_BGR2Lab );

    // Do the abstraction
    cv::Mat_<int> segmentation = superpixel.segment( labim );
    std::vector< SuperpixelStatistic > stat = superpixel.stat( labim, src, segmentation );

    // Compute the uniqueness
    std::vector<float> unique( stat.size(), 1 );
    if (settings.uniqueness_)
    {
        if (settings.filter_uniqueness_)
            unique = uniquenessFilter( stat );
        else
            unique = uniqueness( stat );
    }

    // Compute the distribution
    std::vector<float> dist( stat.size(), 0 );
    if (settings.distribution_)
    {
        if (settings.filter_distribution_)
            dist = distributionFilter( stat );
        else
            dist = distribution( stat );
    }

    // Combine the two measures
    std::vector<float> sp_saliency( stat.size() );
    for( int i=0; i<stat.size(); i++ )
        sp_saliency[i] = unique[i] * exp( - settings.k_ * dist[i] );

    // Upsampling
    cv::Mat_<float> r;
    if (settings.upsample_)
        r = assignFilter(src, segmentation, stat, sp_saliency );
    else
        r = assign( segmentation, sp_saliency );

    // Rescale the saliency to [0..1]
    double mn, mx;
    cv::minMaxLoc( r, &mn, & mx );
    r = (r - mn) / (mx - mn);

    //Increase the saliency value until we are below the minimal threshold
    double m_sal = settings.min_saliency_ * r.size().area();
    for( float sm = cv::sum( r )[0]; sm < m_sal; sm = cv::sum( r )[0] )
        r =  cv::min( r*m_sal/sm, 1.0f );

    return r;
}

// Normalize a vector of floats to the range [0..1]
void normVec( std::vector< float > &r )
{
    const int N = (int)r.size();
    float mn = r[0], mx = r[0];
    for( int i=1; i<N; i++ )
    {
        if (mn > r[i])
            mn = r[i];
        if (mx < r[i])
            mx = r[i];
    }
    for( int i=0; i<N; i++ )
        r[i] = (r[i] - mn) / (mx - mn);
}

std::vector< float > SFSaliency::uniqueness( const std::vector< SuperpixelStatistic >& stat ) const
{
    const int N = (int)stat.size();
    std::vector< float > r( N );
    const float sp = 0.5f / (settings.sigma_p_ * settings.sigma_p_);
    for( int i=0; i<N; i++ )
    {
        float u = 0.0f, norm = 1e-10f;
        cv::Vec3f c = stat[i].mean_color_;
        cv::Vec2f p = stat[i].mean_position_;

        // Evaluate the score, for now without filtering
        for( int j=0; j<N; j++ )
        {
            cv::Vec3f dc = stat[j].mean_color_ - c;
            cv::Vec2f dp = stat[j].mean_position_ - p;

            float w = fast_exp( - sp * dp.dot(dp) );
            u += w*dc.dot(dc);
            norm += w;
        }
        // Let's not normalize here, must have been a typo in the paper
// 		r[i] = u / norm;
        r[i] = u;
    }
    normVec( r );
    return r;
}

std::vector< float > SFSaliency::distribution( const std::vector< SuperpixelStatistic >& stat ) const
{
    const int N =(int)stat.size();
    std::vector< float > r( N );
    const float sc =  0.5f / (settings.sigma_c_*settings.sigma_c_);
    for( int i=0; i<N; i++ )
    {
        float u = 0.0f, norm = 1e-10f;
        cv::Vec3f c = stat[i].mean_color_;
        cv::Vec2f p(0.f, 0.f);

        // Find the mean position
        for( int j=0; j<N; j++ )
        {
            cv::Vec3f dc = stat[j].mean_color_ - c;
            float w = fast_exp( - sc * dc.dot(dc) );
            p += w*stat[j].mean_position_;
            norm += w;
        }
        p *= 1.0 / norm;

        // Compute the variance
        for( int j=0; j<N; j++ )
        {
            cv::Vec3f dc = stat[j].mean_color_ - c;
            cv::Vec2f dp = stat[j].mean_position_ - p;
            float w = fast_exp( - sc * dc.dot(dc) );
            u += w*dp.dot(dp);
        }
        r[i] = u / norm;
    }
    normVec( r );
    return r;
}

std::vector< float > SFSaliency::uniquenessFilter( const std::vector< SuperpixelStatistic >& stat ) const
{
    const int N = (int)stat.size();

    // Setup the data and features
    std::vector< cv::Vec2f > features( stat.size() );
    cv::Mat_<float> data( (int)stat.size(), 5 );
    for( int i=0; i<N; i++ )
    {
        features[i] = stat[i].mean_position_ / settings.sigma_p_;
        cv::Vec3f c = stat[i].mean_color_;
        data(i,0) = 1;
        data(i,1) = c[0];
        data(i,2) = c[1];
        data(i,3) = c[2];
        data(i,4) = c.dot(c);
    }
    // Filter
    Filter filter( (const float*)features.data(), N, 2 );
    filter.filter( data.ptr<float>(), data.ptr<float>(), 5 );

    // Compute the uniqueness
    std::vector< float > r( N );
    for( int i=0; i<N; i++ )
    {
        cv::Vec3f c = stat[i].mean_color_;
        float u = 0.0f, norm = 1e-10f;
        cv::Vec2f p = stat[i].mean_position_;

        r[i] = data(i,0)*c.dot(c) + data(i,4) - 2*( c[0]*data(i,1) + c[1]*data(i,2) + c[2]*data(i,3) );
    }
    normVec( r );
    return r;
}

std::vector< float > SFSaliency::distributionFilter( const std::vector< SuperpixelStatistic >& stat ) const
{
    const int N = (int)stat.size();

    // Setup the data and features
    std::vector< cv::Vec3f > features( stat.size() );
    cv::Mat_<float> data( (int)stat.size(), 4 );
    for( int i=0; i<N; i++ )
    {
        features[i] = stat[i].mean_color_ / settings.sigma_c_;
        cv::Vec2f p = stat[i].mean_position_;
        data(i,0) = 1;
        data(i,1) = p[0];
        data(i,2) = p[1];
        data(i,3) = p.dot(p);
    }
    // Filter
    Filter filter( (const float*)features.data(), N, 3 );
    filter.filter( data.ptr<float>(), data.ptr<float>(), 4 );

    // Compute the uniqueness
    std::vector< float > r( N );
    for( int i=0; i<N; i++ )
        r[i] = data(i,3) / data(i,0) - ( data(i,1) * data(i,1) + data(i,2) * data(i,2) ) / ( data(i,0) * data(i,0) );

    normVec( r );
    return r;
}

cv::Mat_< float > SFSaliency::assign( const cv::Mat_< int >& seg, const std::vector< float >& sal ) const
{
    return Superpixel::assign( sal, seg );
}

cv::Mat_< float > SFSaliency::assignFilter( const cv::Mat_< cv::Vec3b >& im, const cv::Mat_< int >& seg, const std::vector< SuperpixelStatistic >& stat, const std::vector< float >& sal ) const
{
    std::vector< float > source_features( seg.size().area()*5 ), target_features( im.size().area()*5 );
    cv::Mat_< cv::Vec2f > data( seg.size() );
    // There is a type on the paper: alpha and beta are actually squared, or directly applied to the values
    const float a = settings.alpha_, b = settings.beta_;

    const int D = 5;
    // Create the source features
    for( int j=0,k=0; j<seg.rows; j++ )
    {
        for( int i=0; i<seg.cols; i++, k++ )
        {
            int id = seg(j,i);
            data(j,i) = cv::Vec2f( sal[id], 1 );

            source_features[D*k+0] = a * i;
            source_features[D*k+1] = a * j;
            if (D == 5)
            {
                source_features[D*k+2] = b * stat[id].mean_rgb_[0];
                source_features[D*k+3] = b * stat[id].mean_rgb_[1];
                source_features[D*k+4] = b * stat[id].mean_rgb_[2];
            }
        }
    }
    // Create the source features
    for( int j=0,k=0; j<im.rows; j++ )
    {
        for( int i=0; i<im.cols; i++, k++ )
        {
            target_features[D*k+0] = a * i;
            target_features[D*k+1] = a * j;
            if (D == 5)
            {
                target_features[D*k+2] = b * im(j,i)[0];
                target_features[D*k+3] = b * im(j,i)[1];
                target_features[D*k+4] = b * im(j,i)[2];
            }
        }
    }

    // Do the filtering [Filtering using the target features twice works slightly better, as the method described in our paper]
    if (settings.use_spix_color_)
    {
        Filter filter( source_features.data(), seg.cols*seg.rows, target_features.data(), im.cols*im.rows, D );
        filter.filter( data.ptr<float>(), data.ptr<float>(), 2 );
    }
    else
    {
        Filter filter( target_features.data(), im.cols*im.rows, D );
        filter.filter( data.ptr<float>(), data.ptr<float>(), 2 );
    }

    cv::Mat_<float> r( im.size() );
    for( int j=0; j<im.rows; j++ )
        for( int i=0; i<im.cols; i++ )
            r(j,i) = data(j,i)[0] / (data(j,i)[1] + 1e-10);
    return r;
}


