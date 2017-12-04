#ifndef APCLUSTER_H
#define APCLUSTER_H

#include <opencv2/core.hpp>
#include <cmath>
#include <vector>
#include <map>
#include <algorithm>
#include <cassert>

struct Edge {
    int src;      // index of source
    int dst;      // index of destination
    double s;     // similarity s(src, dst)
    double r;     // responsibility r(src, dst)
    double a;     // availability a(src, dst)

    Edge(int src, int dst, double s): src(src), dst(dst), s(s), r(0), a(0) {}
    bool operator<(const Edge& rhs) const { return s < rhs.s; }
};

typedef std::vector<Edge*> Edges;

struct Graph {
    int n;                // the number of vertices
    Edges* outEdges;      // array of out edges of corresponding vertices
    Edges* inEdges;       // array of in edges of corresponding vertices
    std::vector<Edge> edges;   // all edges
};

class APCluster
{
public:
    APCluster(int N,int prefType=1, double lambda=0.9, int maximum_iterations=2000, int converge_iterations=200);
    ~APCluster();

    std::vector<int> affinityPropagation(cv::Mat src);
    int reMapIdx(std::vector<int> &mapIdx);

private:
    Graph* buildGraph(cv::Mat);
    void destroyGraph(Graph* graph);

    void updateResponsibilities(Graph* graph);
    void updateAvailabilities(Graph* graph);
    bool updateExamplars(Graph* graph, std::vector<int>& examplar);

    void update(double& variable, double newValue)
    {
        variable = lambda * variable + (1.0 - lambda) * newValue;
    }

private:
    int prefType;
    double lambda;
    int maximum_iterations;
    int converge_iterations;
    int N;

    void init();
};

#endif // APCLUSTER_H
