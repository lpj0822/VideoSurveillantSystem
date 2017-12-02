#ifndef SSE_DEFS_H
#define SSE_DEFS_H

#ifdef _MSC_VER

#include <mmintrin.h>

__m128 operator*( __m128 a, __m128 b ){
	return _mm_mul_ps( a, b );
}
__m128 operator/( __m128 a, __m128 b ){
	return _mm_div_ps( a, b );
}
__m128 operator+( __m128 a, __m128 b ){
	return _mm_add_ps( a, b );
}
__m128 operator-( __m128 a, __m128 b ){
	return _mm_sub_ps( a, b );
}
__m128 operator*=( __m128 &a, __m128 b ){
	return a=_mm_mul_ps( a, b );
}
__m128 operator/=( __m128 &a, __m128 b ){
	return a=_mm_div_ps( a, b );
}
__m128 operator+=( __m128 &a, __m128 b ){
	return a=_mm_add_ps( a, b );
}
__m128 operator-=( __m128 &a, __m128 b ){
	return a=_mm_sub_ps( a, b );
}
__m128 operator-( __m128 a ){
	return _mm_sub_ps( _mm_set1_ps(0), a );
}

#endif //_MSC_VER

#endif SSE_DEFS_H
