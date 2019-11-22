/*
    This file is a part of SORT(Simple Open Ray Tracing), an open-source cross
    platform physically based renderer.

    Copyright (c) 2011-2019 by Jiayin Cao - All rights reserved.

    SORT is a free software written for educational purpose. Anyone can distribute
    or modify it under the the terms of the GNU General Public License Version 3 as
    published by the Free Software Foundation. However, there is NO warranty that
    all components are functional in a perfect manner. Without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License along with
    this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

#pragma once

#include "core/define.h"

#ifdef SSE_ENABLED

#include <nmmintrin.h>

static const __m128 zeros       = _mm_set_ps1( 0.0f );
static const __m128 infinites   = _mm_set_ps1( FLT_MAX );

static SORT_FORCEINLINE float	sse_data(__m128 simd , int index) {
#ifdef SORT_IN_WINDOWS
	return simd.m128_f32[index];
#else
	return simd[index];
#endif
}

static SORT_FORCEINLINE int __bsf(int v) {
#ifdef SORT_IN_WINDOWS
	unsigned long r = 0;
	_BitScanForward(&r, v);
	return r;
#else
	return __builtin_ctz(v);
#endif
}

static SORT_FORCEINLINE __m128 _mm_sqr_ps( const __m128& m ){
    return _mm_mul_ps( m , m );
}

static SORT_FORCEINLINE __m128 _mm_rcpa_ps( const __m128& m ){
	static const __m128 ones		= _mm_set_ps1(1.0f);
    return _mm_div_ps(ones, m);
}

static SORT_FORCEINLINE __m128 _mm_mad_ps( const __m128& a , const __m128& b , const __m128& c ){
    return _mm_add_ps( _mm_mul_ps( a , b ) , c );
}

static SORT_FORCEINLINE __m128 _mm_pick_ps( const __m128& mask , const __m128& a , const __m128& b ){
    return _mm_or_ps( _mm_and_ps( mask , a ) , _mm_andnot_ps( mask , b ) );
}
#endif
