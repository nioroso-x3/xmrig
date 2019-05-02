#ifndef SSE2ALTIVEC_H
#define SSE2ALTIVEC_H

#include <altivec.h>
#undef bool
#undef pixel
typedef __vector float __m128;
typedef __vector double __m128d;
typedef __vector unsigned char __m128i;
typedef __vector unsigned long long __m128ll;
typedef __vector unsigned int __m128l;
#define _MM_SHUFFLE(w,x,y,z) (((w) << 6) | ((x) << 4) | ((y) << 2) | (z))

static inline __m128i vec_zero1q (void)
{
  return (__m128i) (__vector signed int) vec_splats (0);
}
/* Splat 8-bit char to 16 8-bit chars */
static inline __m128i vec_splat16sb (char scalar)
{ return (__m128i) vec_splats ((signed char) scalar); }

/* Splat 16-bit short to 8 16-bit shorts */
static inline __m128i vec_splat8sh (short scalar)
{ return (__m128i) vec_splats (scalar); }

/* Splat 32-bit int to 4 32-bit ints */
static inline __m128i vec_splat4sw (int scalar)
{ return (__m128i) vec_splats (scalar); }

/* Splat 64-bit long long to 2 64-bit long longs */
static inline __m128i vec_splat2sd (long long scalar)
{ return (__m128i) vec_splats (scalar); }


static inline __m128d _mm_set1_pd (double __F)
{
  return  (__m128d){ __F, __F };
}

static inline __m128i _mm_load_si128(__m128i const *a)
{
  return *a;
}

static inline void _mm_store_si128(__m128i *a, __m128i b)
{
  vec_st(b, 0, a);
}

static inline __m128i _mm_set_epi64x(long long __q1, long long __q0)
{
  return (__m128i)(__vector long long){ __q0, __q1 };
}

static inline __m128i _mm_cvtsi64_si128 (long long __A)
{
  return (__m128i)(__vector long long){ __A, 0LL };
}

static inline __m128i _mm_set_epi32(unsigned int q3, unsigned int q2, unsigned int q1, unsigned int q0)
{
   return (__m128l){q0,q1,q2,q3};
}

static inline __m128i _mm_add_epi64 (__m128i __A, __m128i __B)
{
  return (__m128i) ((__vector unsigned long long)__A + (__vector unsigned long long)__B);
}

static inline __m128i _mm_shuffle_epi32(__m128i __A, const long element_selectors)
{
  unsigned long element_selector_10 =  element_selectors       & 0x03;
  unsigned long element_selector_32 = (element_selectors >> 2) & 0x03;
  unsigned long element_selector_54 = (element_selectors >> 4) & 0x03;
  unsigned long element_selector_76 = (element_selectors >> 6) & 0x03;
  static const unsigned int permute_selectors [4] = {
    0x03020100, 0x07060504, 0x0B0A0908, 0x0F0E0D0C
  };
 __m128i t = (__m128l){permute_selectors[element_selector_10],permute_selectors[element_selector_32],permute_selectors[element_selector_54],permute_selectors[element_selector_76]};
 return vec_perm (__A, __A, t);
}

static inline __m128i _mm_slli_si128(__m128i __A, const unsigned long bytecount)
{
  if ((unsigned long) bytecount >= 16)
  {
    /* SSE2 shifts >= element_size or < 0 produce 0; Altivec/MMX shifts by bytecount%element_size. */
     return (__m128i) vec_splats (0);
  } else if (bytecount == 0) {
    return __A;
  } else {
     /* The PowerPC byte shift count must be multiplied by 8. */
     /* It need not but can be replicated, which handles both LE and BE shift count positioning. */
     __m128i r = vec_splats ((unsigned char)(bytecount << 3));
     return (__m128i) vec_slo (__A, r);
  }
}
static inline __m128i _mm_xor_si128(__m128i __A, __m128i __B)
{
  return vec_xor(__A,__B);
}

static inline __m128i _mm_or_si128(__m128i __A, __m128i __B)
{
  return vec_or(__A,__B);
}

static inline __m128d _mm_castsi128_pd(__m128i __A)
{
  return (__m128d) __A;
}

static inline __m128 _mm_castsi128_ps(__m128i __A)
{
  return (__m128) __A;
}

static inline __m128 _mm_cvtsi64_ss (__m128 __A, long long __B)
{
  float temp = __B;
  __A[0] = temp;

  return __A;
}

static inline __m128 _mm_castsi128(__m128i __A)
{
  return (__m128) __A;
}
static inline __m128 _mm_setzero_ps(void)
{
  return  (__m128){ 0.0f, 0.0f, 0.0f, 0.0f };
}

static inline __m128d _mm_setzero_pd(void)
{
  return (__m128d) vec_splats(0);
}

static inline __m128d _mm_setr_pd (double __W, double __X)
{
  return (__m128d){ __W, __X };
}

static inline __m128d _mm_sqrt_sd (__m128d __A, __m128d __B)
{
  __vector double c;
  c = vec_sqrt ((__vector double ) _mm_set1_pd (__B[0]));
  return (__m128d ) _mm_setr_pd (c[0], __A[1]);
}

static inline __m128i _mm_castpd_si128(__m128d __A)
{
  return (__m128i) __A;
}

static inline __m128i _mm_castps_si128(__m128 __A)
{
  return (__m128i) __A;
}

static inline long long _mm_cvtsi128_si64 (__m128i __A)
{
  return ((__vector long long)__A)[0];
}

static inline int _mm_cvtsi128_si32 (__m128i __A)
{
  return ((__vector int)__A)[0];
}

static inline __m128d _mm_cvtsi64_sd (__m128d __A, long long __B)
{
  __vector double result = (__vector double)__A;
  double db = __B;
  result [0] = db;
  return (__m128d)result;
}

static inline __m128i _mm_srli_si128 (__m128i __A, int __N)
{
  __vector unsigned char result;
  const __vector unsigned char zeros =
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  if (__N < 16)
    if (__builtin_constant_p(__N))
      {
	/* Would like to use Vector Shift Left Double by Octet
	   Immediate here to use the immediate form and avoid
	   load of __N * 8 value into a separate VR.  */
	result = vec_sld (zeros, (__vector unsigned char) __A, (16 - __N));
      }
    else
      {
	__vector unsigned char shift = vec_splats((unsigned char)(__N*8));
	result = vec_sro ((__vector unsigned char)__A, shift);
      }
    else
    result = zeros;

  return (__m128i) result;
}

static inline __m128i _mm_slli_si128 (__m128i v, int bytecount)
{
  if ((unsigned long) bytecount >= 16)
  {
    /* SSE2 shifts >= element_size or < 0 produce 0; Altivec/MMX shifts by bytecount%element_size. */
    return (__m128i) vec_splats (0);
  } else if (bytecount == 0) {
    return v;
  } else {
    /* The PowerPC byte shift count must be multiplied by 8. */
    /* It need not but can be replicated, which handles both LE and BE shift count positioning. */
    __m128i replicated_count;
    replicated_count = vec_splat16sb (bytecount << 3);
    /* AT gcc v7.1 may miscompile vec_sro as vec_slo? */
    return (__m128i) vec_sro (v, replicated_count);
  }
}

static inline __m128i _mm_setzero_si128 (void)
{
  return (__m128i)(__vector int){ 0, 0, 0, 0 };
}
static inline __m128 _mm_movehl_ps (__m128 a, __m128 b){
return vec_perm(a,b,(__m128i){0x18,0x19,0x1A,0x1B,
                              0x1C,0x1D,0x1E,0x1F,
                              0x8,0x9,0xA,0xB,
                              0xC,0xD,0xE,0xF});
}

static inline __m128l _mm_set1_epi32 ( uint32_t v){
  return (__m128l){v};
}
static inline __m128 _mm_set1_ps ( float v){
  return (__m128){v};
}

static inline __m128l _mm_cvttps_epi32(__m128 __A)
{
  return (__m128l) __A;
}

static inline __m128 _mm_cvtepi32_ps(__m128l __A){
  return (__m128)__A;
}

static inline __m128 _mm_and_ps(__m128 __A,__m128 __B){
  return (__m128)vec_and(__A,__B);
}

static inline __m128 _mm_or_ps(__m128 __A,__m128 __B){
  return (__m128)vec_or(__A,__B);
}

static inline __m128 _mm_add_ps(__m128 __A,__m128 __B){
  return (__m128)vec_add(__A,__B);
}

static inline __m128 _mm_mul_ps(__m128 __A,__m128 __B){
  return (__m128)vec_mul(__A,__B);
}

static inline __m128 _mm_sub_ps(__m128 __A,__m128 __B){
  return (__m128)vec_sub(__A,__B);
}

static inline __m128 _mm_div_ps(__m128 __A,__m128 __B){
  return (__m128)vec_div(__A,__B);
}


#endif
