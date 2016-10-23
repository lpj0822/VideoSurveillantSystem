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

/** Returns the median of the values inside the array. Use very simple methods
 * for arrays with size 1 to 3, and quickSelect for other cases.
 * would be too much.
 *
 * Parameters: vector <Type>& v: input array.
 *
 * Return value: the median value inside the array.
**/
template <typename Type>
inline Type medianValue (std::vector <Type>& v)
{
    if (v.size () == 0)
        throw(std::exception ("Trying to get the median of an empty vector."));

    if (v.size () == 1)
        return (v [0]); // Special case.

    if (v.size () == 3) // 3 values. Very simple.
    {
        if (v [0] >= v [1] && v [0] <= v [2])
            return (v [0]);
        if (v [1] >= v [0] && v [1] <= v [2])
            return (v [1]);
        return (v [2]);
    }

    return (quickSelect <Type> (v));
}

/** Simple implementation of quickSelect.
 *
 * Parameters: vector <Type>& v: input array. The data will be moved inside it.
 *
 * Return value: the median value inside the array.
**/
template <typename Type>
inline int quickSelectPartition (std::vector <Type>& v, int lo, int hi)
{
    // Select pivot.
    Type pivot = v [hi];

    int pos = lo; // Position to place the next value < pivot.

    for (int i = lo; i < hi; i++)
    {
        if (v [i] < pivot)
        {
            if (i != pos)
            {
                // Flip.
                Type aux = v [i];
                v [i] = v [pos];
                v [pos] = aux;
            }
            pos++;
        }
    }

    // Save the pivot.
    v [hi] = v[pos];
    v [pos] = pivot;

    return (pos); // Return the pivot position.
}

template <typename Type>
inline Type quickSelect (std::vector <Type>& v)
{
    int target1, target2;
    if (v.size () % 2)
    {
        target1 = target2 = (int)v.size ()/2;
    }
    else
    {
        target1 = (int)v.size ()/2-1;
        target2 = target1+1;
    }

    int pos = 0;
    int lo = 0;
    int hi = (int)v.size ()-1;

    while (pos != target1 && pos != target2)
    {
        pos = quickSelectPartition <Type>(v, lo, hi);
        if (pos < target1)
            lo = pos+1;
        else
            hi = pos-1;
    }

    return (v[pos]);
}

/**
 * 1D Gaussian, for a given x and sigma.
**/
float gaussian1d (float x, float sigma);

#endif // UTILITY

