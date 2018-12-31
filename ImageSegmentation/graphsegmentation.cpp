#include "graphsegmentation.h"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>

GraphSegmentation::GraphSegmentation()
{
    init();
    std::cout << "GraphSegmentation()" << std::endl;
}

GraphSegmentation::~GraphSegmentation()
{
    if(graph != NULL)
    {
        delete graph;
        graph = NULL;
    }
    std::cout << "GraphSegmentation()" << std::endl;
}

cv::Mat GraphSegmentation::getSegmentMap(const cv::Mat &src, const cv::Mat &weightMap)
{
    CV_Assert(weightMap.type() == CV_32FC1);
    cv::Mat segmentMap;
    cv::Mat saliencyMap;
    cv::Mat imgLab;
    std::vector<cv::Mat> vectorMat;
    if(src.empty())
    {
        return segmentMap;
    }
    graph = new GraphType(src.cols*src.rows, src.cols*src.rows*2);
    cv::cvtColor(src, imgLab, cv::COLOR_BGR2Lab);
    cv::split(imgLab, vectorMat);
    std::vector<cv::Mat> vectorMat1(vectorMat.size());
    cv::normalize(vectorMat[0], vectorMat1[0], 0, 100, cv::NORM_MINMAX, CV_16SC1);
    cv::normalize(vectorMat[1], vectorMat1[1], -127, 127, cv::NORM_MINMAX, CV_16SC1);
    cv::normalize(vectorMat[2], vectorMat1[2], -127, 127, cv::NORM_MINMAX, CV_16SC1);
    cv::merge(vectorMat1, imgLab);
    conGraph(weightMap, imgLab);// 构建图
    int energy = (int)graph->maxflow();// 最大流算法
    std::cout << "Energy: " << energy << std::endl;

    int height = imgLab.rows;
    int width = imgLab.cols;

    segmentMap = cv::Mat::zeros(height, width, CV_8UC1);

    int indexPt = 0;
    for(int h=0; h < height; h++)
    {
        uchar* p=segmentMap.ptr<uchar>(h);
        for(int w=0; w < width; w++)
        {
            if(graph->what_segment(indexPt) == GraphType::SOURCE)
            {
                p[w] = 255;
            }
            indexPt++;
        }
    }
    return segmentMap;
}

float GraphSegmentation::nWeight(cv::Vec3i c1, cv::Vec3i c2)
{
    double delta = 1.0;
    double dist = (c1.val[0] - c2.val[0]) * (c1.val[0] - c2.val[0]) +
            (c1.val[1] - c2.val[1]) * (c1.val[1] - c2.val[1]) +
            (c1.val[2] - c2.val[2]) * (c1.val[2] - c2.val[2]);
    const double EPSILON = 0.01;

    float w = exp(-dist/delta)/(EPSILON + dist);

    return w;
}

void GraphSegmentation::conGraph(cv::Mat map,cv::Mat imgLab)
{
    int indexPt = 0;
    float EPSILON = 0.0000001f;
    for(int h=0; h < map.rows; h++)
    {
        float* dataMap = map.ptr<float>(h);
        for(int w=0; w < map.cols; w++)
        {
            float e0 = dataMap[w];
            float e1 = 1 - e0;

            //cout << e0 << " " << e1 << endl;
            e0 += EPSILON;
            e1 += EPSILON;
            graph->add_node();
            graph->add_tweights(indexPt, e0, e1);

            if(w > 0)
            {
                cv::Vec3i c1 = imgLab.at<cv::Vec3s>(h, w-1);
                cv::Vec3i c2 = imgLab.at<cv::Vec3s>(h, w);
                float e2 = nWeight(c1,c2);
                graph->add_edge(indexPt, indexPt-1, e2, e2);
            }
            if(h > 0)
            {
                cv::Vec3i c1 = imgLab.at<cv::Vec3s>(h-1, w);
                cv::Vec3i c2 = imgLab.at<cv::Vec3s>(h, w);
                float e2 = nWeight(c1, c2);
                graph->add_edge(indexPt, indexPt-map.cols, e2, e2);
            }
            indexPt++;
        }
    }
}

void GraphSegmentation::init()
{
    graph = NULL;
}

