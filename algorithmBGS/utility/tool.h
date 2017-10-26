#ifndef TOOL_H
#define TOOL_H
#include <vector>

/** Returns the median of the values inside the array. Use very simple methods
 * for arrays with size 1 to 3, and quickSelect for other cases.
 * would be too much.
 *
 * Parameters: vector <Type>& v: input array.
 *
 * Return value: the median value inside the array.
**/
template <typename Type>
inline Type medianValue (std::vector<Type>& v)
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
inline int quickSelectPartition (std::vector<Type>& v, int lo, int hi)
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
inline Type quickSelect (std::vector<Type>& v)
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

#endif // TOOL_H
