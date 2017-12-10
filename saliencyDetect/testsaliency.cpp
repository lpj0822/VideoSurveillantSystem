#include "isaliency.h"
#include "ftsaliency.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


void testSaliency(const cv::Mat &src)
{
    cv::Mat saliencyMap;
    ISaliency *saliency = NULL;
    saliency = new FTSaliency();
    saliencyMap = saliency->getSaliency(src);
    cv::normalize(saliencyMap, saliencyMap, 255.0, 0.0, cv::NORM_MINMAX);
    saliencyMap.convertTo(saliencyMap, CV_8UC1);
    cv::imshow( "saliency Image", saliencyMap);
    cv::imshow("src", src);
    cv::waitKey(0);
    delete saliency;
    saliency = NULL;
}
