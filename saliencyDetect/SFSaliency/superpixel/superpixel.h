/*
 * SLIC superpixels
 */
#ifndef SUPERPIXEL_H
#define SUPERPIXEL_H

#include <opencv2/core.hpp>
#include <vector>

struct SuperpixelStatistic
{
    cv::Vec3f mean_color_;
    cv::Vec3f mean_rgb_;
    cv::Vec2f mean_position_;
	int size_;
	SuperpixelStatistic();
};

class Superpixel
{
protected:
	int K_, n_iter_;
	float col_w_;
	bool geodesic_;
    cv::Mat_<int> slic( const cv::Mat_<cv::Vec3f> & im ) const;
    cv::Mat_<int> geodesicSegmentation( const cv::Mat_<cv::Vec3f> & im ) const;
public:
	Superpixel( int K, float col_w, int n_iter=5, bool geodesic=true );
    cv::Mat_<int> segment( const cv::Mat_<cv::Vec3f> & im ) const;
    std::vector<SuperpixelStatistic> stat( const cv::Mat_<cv::Vec3f> & im, const cv::Mat_<cv::Vec3b>& rgb, const cv::Mat_<int> & segmentation ) const;
    cv::Mat_<cv::Vec3f> visualizeMeanColor( const cv::Mat_<int> & segmentation, const std::vector< SuperpixelStatistic > & stat ) const;
    cv::Mat_<cv::Vec3f> visualizeRandom( const cv::Mat_<int> & segmentation ) const;
template< typename T >
    static cv::Mat_<T> assign( const std::vector<T> & val, const cv::Mat_<int> & segmentation );
    int nLabels( const cv::Mat_<int> & segmentation ) const;
};

template< typename T >
cv::Mat_<T> Superpixel::assign( const std::vector<T> & val, const cv::Mat_<int> & segmentation )
{
    cv::Mat_<T> r( segmentation.size() );
	for( int j=0; j<segmentation.rows; j++ )
		for( int i=0; i<segmentation.cols; i++ )
			r(j,i) = val[ segmentation(j,i) ];
	return r;
}

#endif // SUPERPIXEL_H
