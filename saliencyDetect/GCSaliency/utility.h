#ifndef UTILITY
#define UTILITY

#include <opencv2/core.hpp>
#include <cmath>

template<class T, int D>
inline T vecSquareDist(const cv::Vec<T, D> &v1, const cv::Vec<T, D> &v2)
{
    T s = 0;
    for (int i=0; i<D; i++)
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

#endif // UTILITY

