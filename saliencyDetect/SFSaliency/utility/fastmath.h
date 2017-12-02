#ifndef FASTMATH_H
#define FASTMATH_H

inline float fast_log2 (float val) {
   int * const  exp_ptr = reinterpret_cast <int *> (&val);
   int          x = *exp_ptr;
   const int    log_2 = ((x >> 23) & 255) - 128;
   x &= ~(255 << 23);
   x += 127 << 23;
   *exp_ptr = x;

   return (val + log_2);
}
inline float fast_log (const float &val) {
   return (fast_log2 (val) * 0.69314718f);
}
inline float very_fast_exp(float x) {
	// err <= 3e-3
// 	return 1
// 		   -x*(0.9664
// 		   -x*(0.3536));

	return 1
	-x*(0.9999999995f
	-x*(0.4999999206f

	-x*(0.1666653019f
	-x*(0.0416573475f
	-x*(0.0083013598f

	-x*(0.0013298820f
	-x*(0.0001413161f)))))));
}
inline float fast_exp(float x) {
	bool lessZero = true;
	if (x < 0) {

		lessZero = false;
		x = -x;

	}
	// This diry little trick only works because of the normalization and the fact that one element in the normalization is 1
	if (x > 20)
		return 0;
	int mult = 0;

	while (x > 0.69*2*2*2) {

		mult+=3;
		x /= 8.0f;
	}

	while (x > 0.69*2*2) {
		mult+=2;

		x /= 4.0f;
	}
	while (x > 0.69) {

		mult++;
		x /= 2.0f;
	}

	x = very_fast_exp(x);
	while (mult) {

		mult--;
		x = x*x;
	}

	if (lessZero) {
		return 1 / x;

	} else {
		return x;
	}
}

#ifdef __SSE__
#include <xmmintrin.h>
#include "sse_defs.h"
inline __m128 very_fast_exp(__m128 x) {
	return _mm_set1_ps(1)
	-x*(_mm_set1_ps(0.9999999995)
	-x*(_mm_set1_ps(0.4999999206)

	-x*(_mm_set1_ps(0.1666653019)
	-x*(_mm_set1_ps(0.0416573475)
	-x*(_mm_set1_ps(0.0083013598)

	-x*(_mm_set1_ps(0.0013298820)
	-x*(_mm_set1_ps(0.0001413161))))))));
}
inline __m128 fast_exp(__m128 x) {
	const __m128 zero = _mm_set1_ps( 0.0f );
	__m128 lessZero = _mm_cmplt_ps( zero, x );
	__m128 one      = _mm_set1_ps(1.0);
	// Invert x of less than 0
	x = _mm_add_ps( _mm_and_ps(lessZero,x), _mm_andnot_ps(lessZero,-x) );
	
	// This diry little trick only works because of the normalization and then fact that one element in the normalization is 1
	__m128 set_zero = _mm_cmple_ps( x, _mm_set1_ps(20.0) );
	
	// Bring it into range
	__m128 mult = zero;
	for( int i=4, j=2; i>0; i/=2, j-- ){
		__m128 cmp = _mm_cmpgt_ps( x, _mm_set1_ps( 0.69*i ) );
		mult += _mm_and_ps( cmp, _mm_set1_ps( j+1 ) );
		x = x / (_mm_and_ps( cmp, _mm_set1_ps( 2*i-1 )) + one);
	}
	
	x = very_fast_exp(x);
	
	while(1){
		__m128 cmp = _mm_cmpgt_ps( mult, _mm_set1_ps(0.0) );
		if (!_mm_movemask_ps(cmp)) break;
		mult -= _mm_and_ps( cmp, one );
		x *= (_mm_and_ps( cmp, x-one ) + one);
	}
	
	x = _mm_add_ps( _mm_and_ps(lessZero,one/x), _mm_andnot_ps(lessZero,x) );
	return _mm_and_ps( set_zero, x );
}
#endif

#endif FASTMATH_H
