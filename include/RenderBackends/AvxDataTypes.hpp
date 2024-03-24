#ifndef AVX_DATA_TYPES_HPP_
#define AVX_DATA_TYPES_HPP_

#include <cstddef>
#include <cstdint>
#include <immintrin.h>

static const size_t OPTIMIZATION_RATE = 16;

union avxu_t;

union avxf_t {
    __m512 data;
    float array [OPTIMIZATION_RATE];

    avxf_t (__m512 value);
    avxf_t (float  value);
    avxf_t (float  values [OPTIMIZATION_RATE]);
    avxf_t (__mmask16 mask);

    avxf_t    operator+  (const avxf_t &value);
    avxf_t    operator-  (const avxf_t &value);
    avxf_t    operator*  (const avxf_t &value);
    avxf_t    operator/  (const avxf_t &value);
    __mmask16 operator<= (const avxf_t &value);
    
    operator avxu_t () const;
};

union avxu_t {
    __m512i  data;
    uint32_t array [OPTIMIZATION_RATE];
    
    avxu_t (__m512i    value);
    avxu_t (uint32_t  value);
    avxu_t (uint32_t  values [OPTIMIZATION_RATE]);
    avxu_t (__mmask16 mask);

    avxu_t operator+ (const avxu_t &value);
    avxu_t operator- (const avxu_t &value);
    avxu_t operator* (const avxu_t &value);
};

inline avxf_t::avxf_t (__m512 value)                     :data (value) {}
inline avxf_t::avxf_t (float  value)                     :data (_mm512_set1_ps (value)) {}
inline avxf_t::avxf_t (float  values [OPTIMIZATION_RATE]):data (_mm512_set_ps  (values [0],  values [1],  values [2],  values [3],
                                                                                values [4],  values [5],  values [6],  values [7],
                                                                                values [8],  values [9],  values [10], values [11],
                                                                                values [12], values [13], values [14], values [15])) {}
inline avxf_t::avxf_t (__mmask16 mask)                   :data (_mm512_mask_blend_ps (mask, _mm512_set1_ps (0), _mm512_set1_ps (1))) {}

inline avxf_t    avxf_t::operator+  (const avxf_t &value) {return _mm512_add_ps        (data, value.data);}
inline avxf_t    avxf_t::operator-  (const avxf_t &value) {return _mm512_sub_ps        (data, value.data);}
inline avxf_t    avxf_t::operator*  (const avxf_t &value) {return _mm512_mul_ps        (data, value.data);}
inline avxf_t    avxf_t::operator/  (const avxf_t &value) {return _mm512_div_ps        (data, value.data);}
inline __mmask16 avxf_t::operator<= (const avxf_t &value) {return _mm512_cmple_ps_mask (data, value.data);}

inline avxf_t::operator avxu_t () const {return avxu_t (_mm512_castps_si512 (data));}

inline avxu_t::avxu_t (__m512i  value)                     :data (value) {}
inline avxu_t::avxu_t (uint32_t value)                     :data (_mm512_set1_epi32 (value)) {}
inline avxu_t::avxu_t (uint32_t values [OPTIMIZATION_RATE]):data (_mm512_set_epi32  (values [0],  values [1],  values [2],  values [3],
                                                                                     values [4],  values [5],  values [6],  values [7],
                                                                                     values [8],  values [9],  values [10], values [11],
                                                                                     values [12], values [13], values [14], values [15])) {}
inline avxu_t::avxu_t (__mmask16 mask)                     :data (_mm512_mask_blend_epi32 (mask, _mm512_set1_epi32 (0), _mm512_set1_epi32 (1))) {}

inline avxu_t avxu_t::operator+ (const avxu_t &value) {return _mm512_add_epi32 (data, value.data);}
inline avxu_t avxu_t::operator- (const avxu_t &value) {return _mm512_sub_epi32 (data, value.data);}
inline avxu_t avxu_t::operator* (const avxu_t &value) {return _mm512_mul_epi32 (data, value.data);}

#endif
