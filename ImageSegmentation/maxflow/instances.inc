#include "graph.h"

#ifdef _MSC_VER
#pragma warning(disable: 4661)
#endif

// Instantiations: <captype, tcaptype, flowtype>
// IMPORTANT: 
//    flowtype should be 'larger' than tcaptype 
//    tcaptype should be 'larger' than captype

template class MaxFlowGraph<int,int,int>;
template class MaxFlowGraph<short,int,int>;
template class MaxFlowGraph<float,float,float>;
template class MaxFlowGraph<double,double,double>;

