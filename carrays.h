#ifndef CARRAYS_H_
#define CARRAYS_H_

#include <stdlib.h>

//
// Arrays
//

#ifndef roundup32
  #define roundup32(x) roundup32(x)
  static inline uint32_t roundup32(uint32_t x) {
    return (--x, x|=x>>1, x|=x>>2, x|=x>>4, x|=x>>8, x|=x>>16, ++x);
  }
#endif

#ifndef roundup64
  #define roundup64(x) roundup64(x)
  static inline uint64_t roundup64(uint64_t x) {
    return (--x, x|=x>>1, x|=x>>2, x|=x>>4, x|=x>>8, x|=x>>16, x|=x>>32, ++x);
  }
#endif

#ifndef SWAP
  #define SWAP(x,y) do { __typeof(x) _tmp = (x); (x) = (y); (y) = _tmp; } while(0)
#endif

// comparison returns:
//   negative iff a < b
//          0 iff a == b
//   positive iff a > b

#define cmpfunc(fname,type_t)                                                  \
static inline int fname(const void *a, const void *b) __attribute__((unused)); \
static inline int fname(const void *a, const void *b) {                        \
  const type_t a2 = *(const type_t *)a;                                        \
  const type_t b2 = *(const type_t *)b;                                        \
  return (a2 < b2 ? -1 : (a2 > b2));                                           \
}

cmpfunc(array_cmp_int, int);
cmpfunc(array_cmp_long, long);
cmpfunc(array_cmp_float, float);
cmpfunc(array_cmp_double, double);
cmpfunc(array_cmp_uint32, uint32_t);
cmpfunc(array_cmp_uint64, uint64_t);
cmpfunc(array_cmp_size, size_t);
cmpfunc(array_cmp_ptr, void *const);
#undef cmpfunc

#define searchfunc(fname,type_t)                                               \
static inline int fname(const void *a, void *b) __attribute__((unused));       \
static inline int fname(const void *a, void *b) {                              \
  const type_t a2 = *(const type_t *)a;                                        \
  const type_t b2 = *(type_t *)b;                                              \
  return (a2 < b2 ? -1 : (a2 > b2));                                           \
}

searchfunc(array_search_int, int);
searchfunc(array_search_long, long);
searchfunc(array_search_float, float);
searchfunc(array_search_double, double);
searchfunc(array_search_uint32, uint32_t);
searchfunc(array_search_uint64, uint64_t);
searchfunc(array_search_size, size_t);
searchfunc(array_search_ptr, void *const);
#undef searchfunc

// Get Greatest Common Divisor using binary GCD algorithm
// http://en.wikipedia.org/wiki/Binary_GCD_algorithm
uint32_t carrays_calc_GCD(uint32_t a, uint32_t b);

static inline void carrays_swapm(char *a, char *b, size_t es)
{
  char *end, tmp;
  for(end = a + es; a < end; a++, b++) { tmp = *a; *a = *b; *b = tmp; }
}

// cyclic-shift an array by `shift` elements
// cycle left shifts towards zero
void array_cycle_left(void *_ptr, size_t n, size_t es, size_t shift);
// cycle right shifts away from zero
void array_cycle_right(void *_ptr, size_t n, size_t es, size_t shift);

// Reverse order of elements in an array
void array_reverse(void *_ptr, size_t n, size_t es);

// Fisher-Yates shuffle. Initiate srand() before calling.
void array_shuffle(void *_ptr, size_t n, size_t es);

// Merge two sorted arrays to create a merged sorted array
void sarrays_merge(void *_dst, size_t ndst, size_t nsrc, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg);

// binary search
// compar is a function that compares a given value with the value we are
// searching for. It returns <0 if _val is < target, >0 if _val is > target,
// 0 otherwise.
void* sarray_bsearch(void *_ptr, size_t n, size_t es,
                     int (*compar)(const void *_val, void *_arg),
                     void *arg);

// Quick sort
// Note: quicksort is not stable, equivalent values may be swapped
void array_qsort_r(void *base, size_t nel, size_t w,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg);

/**
 * Insertion sort, sorted elements first, then unsorted
 * @param ptr points to sorted array
 * @param n is number of sorted elements
 * @param m is number of unsorted elements AFTER ptr+n sorted to merge in
 * @param el is element size
 * @param compar is comparison function
 * @param arg is pointer to pass to comparison function
 */
static inline void arrays_isortr(void *_ptr, size_t n, size_t m, size_t el,
                                 int (*compar)(const void *_a, const void *_b,
                                               void *_arg),
                                 void *arg)
{
  char *b = (char*)_ptr, *pi, *pj, *end = b+el*(n+m);
  for(pi = b+el*n; pi < end; pi += el)
    for(pj = pi; pj > b && compar(pj-el,pj,arg) > 0; pj -= el)
      carrays_swapm(pj-el, pj, el);
}

/**
 * Insertion sort unsorted elements first, then sorted
 * @param ptr points to sorted array
 * @param n is number of sorted elements
 * @param m is number of unsorted elements BEFORE ptr
 * @param el is element size
 * @param compar is comparison function
 * @param arg is pointer to pass to comparison function
 */
static inline void arrays_isortf(void *_ptr, size_t n, size_t m, size_t el,
                                 int (*compar)(const void *_a, const void *_b,
                                               void *_arg),
                                 void *arg)
{
  char *b = (char*)_ptr, *pi, *pj, *start = b-el*m, *end = b+el*n;
  for(pi = b; pi > start; pi -= el)
    for(pj = pi; pj < end && compar(pj-el, pj, arg) > 0; pj += el)
      carrays_swapm(pj-el, pj, el);
}

/**
 * Insertion sort merging between two adjacent sorted arrays
 * @param ptr points to array
 * @param n is number of sorted elements
 * @param m is number of sorted elements
 * @param el is element size
 * @param compar is comparison function
 * @param arg is pointer to pass to comparison function
 */
static inline void sarrays_imerge(void *_ptr, size_t n, size_t m, size_t el,
                                  int (*compar)(const void *_a, const void *_b,
                                                void *_arg),
                                  void *arg)
{
  char *b = (char*)_ptr, *pi, *pj, *end = b+el*(n+m);
  for(pi = b+el*n; pi < end; pi += el) {
    for(pj = pi; pj > b && compar(pj-el,pj,arg) > 0; pj -= el)
      carrays_swapm(pj-el, pj, el);
    if(pj == pi) break;
  }
}

#endif /* CARRAYS_H_ */
