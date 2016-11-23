#ifndef UTILITY
#define UTILITY

#include <opencv2/core.hpp>
#include <cmath>
#include <vector>

template<class T,int D>
inline cv::Vec<T, D> vecSubtract(const cv::Vec<T, D> &v1, const cv::Vec<T, D> &v2)
{
    cv::Vec<T, D> result;
    for(int i = 0; i < D; i++)
        result[i]=v1[i]-v2[i];
    return result;
}

template<class T,int D>
inline cv::Vec<T, D> vecDivide(const cv::Vec<T, D> &v, T num)
{
    cv::Vec<T, D> result;
    for(int i = 0; i < D; i++)
        result[i]=v[i]/num;
    return result;
}

template<class T, int D>
inline T vecSquareDist(const cv::Vec<T, D> &v1, const cv::Vec<T, D> &v2)
{
    T s = 0;
    for(int i=0; i<D; i++)
        s += (v1[i] - v2[i])*(v1[i] - v2[i]);
    return s;
} // out of range risk for T = byte, ...

template<class T, int D>
inline T vecDist(const cv::Vec<T, D> &v1, const cv::Vec<T, D> &v2)
{
    return sqrt(vecSquareDist(v1, v2));
} // out of range risk for T = byte, ...

template<class T>
inline T pointSquareDist(const cv::Point_<T> &p1, const cv::Point_<T> &p2)
{
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
} // out of range risk for T = byte, ...

/**
 * 1D Gaussian, for a given x and sigma.
**/
float gaussian1d (float x, float sigma);

#endif // UTILITY

