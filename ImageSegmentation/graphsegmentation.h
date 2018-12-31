#ifndef GRAPHSEGMENTATION_H
#define GRAPHSEGMENTATION_H

#include "imagesegmentation.h"
#include "maxflow/graph.h"

class GraphSegmentation : public ImageSegmentation
{
public:

    typedef MaxFlowGraph<float, float, float> GraphType;

    GraphSegmentation();
    ~GraphSegmentation();

    cv::Mat getSegmentMap(const cv::Mat &src, const cv::Mat &weightMap);

private:
    void conGraph(cv::Mat map,cv::Mat imgLab);
    float nWeight(cv::Vec3i c1, cv::Vec3i c2);

private:
    GraphType *graph;

    void init();
};

#endif // GRAPHSEGMENTATION_H
