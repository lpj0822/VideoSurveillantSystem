/*
 * Implemetation of the saliency detction method described in paper
 * "Saliency Detection via Graph-based Manifold Ranking"
 * by Chuan Yang, Lihe Zhang, Huchuan Lu, Xiang Ruan, and Ming-Hsuan Yang
 * in CVPR13.
*/
#ifndef _GMRSALIENCY_H
#define _GMRSALIENCY_H

#include <opencv2/imgproc.hpp>

#include "isaliency.h"

class GMRsaliency : public ISaliency
{
public:
	GMRsaliency();
	~GMRsaliency();

	//Get saliency map of an input image
    cv::Mat getSaliency(const cv::Mat &src);

private://parameters

	float alpha;//balance the fittness and smoothness
	float delta;//contral the edge weight
	int spcounta;//actual superpixel number

private:
	//Get the superpixels of the image
    cv::Mat getSuperpixels(const cv::Mat &img);

	//Get the adjacent matrix
    cv::Mat getAdjLoop(const cv::Mat &supLab);

	//Get the affinity matrix of edges 
    cv::Mat getWeight(const cv::Mat &img,const cv::Mat &supLab,const cv::Mat &adj);

	//Get the optimal affinity matrix learned by minifold ranking (e.q. 3 in paper)
    cv::Mat getOptAff(const cv::Mat &W);

	//Get the indicator vector based on boundary prior
    cv::Mat getBdQuery(const cv::Mat &supLab, int type);

	//Remove the obvious frame of the image
    cv::Mat removeFrame(const cv::Mat &img, int *wcut);

};

#endif
