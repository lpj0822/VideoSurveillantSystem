#ifndef FILTER_H
#define FILTER_H
// This function defines a simplified interface to the permutohedral lattice
// We assume a filter standard deviation of 1
class Permutohedral;

class Filter
{
protected:
	int n1_, o1_, n2_, o2_;
	Permutohedral * permutohedral_;
	// Don't copy
    Filter( const Filter& filter )
    {

    }
public:
	// Use different source and target features
	Filter( const float * source_features, int N_source, const float * target_features, int N_target, int feature_dim );
	// Use the same source and target features
	Filter( const float * features, int N, int feature_dim );
	//
	~Filter();
	// Filter a bunch of values
	void filter( const float * source, float * target, int value_size );
	// Reverse filter (swap source and target features)
	void reverseFilter( const float * source, float * target, int value_size );
};

#endif FILTER_H
