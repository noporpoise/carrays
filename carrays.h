#ifndef CARRAYS_H_
#define CARRAYS_H_

//
// Generic C Arrays
//
// All functions have the gca_ prefix
//
// 2015-10-24 Isaac Turner (turner.isaac@gmail.com)
//

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>


static inline uint32_t gca_roundup32(uint32_t x) {
  return (--x, x|=x>>1, x|=x>>2, x|=x>>4, x|=x>>8, x|=x>>16, ++x);
}

static inline uint64_t gca_roundup64(uint64_t x) {
  return (--x, x|=x>>1, x|=x>>2, x|=x>>4, x|=x>>8, x|=x>>16, x|=x>>32, ++x);
}

#ifndef SWAP
  #define SWAP(x,y) do { __typeof(x) _tmp = (x); (x) = (y); (y) = _tmp; } while(0)
#endif

// Resize an array only if needed
// If resized new number of elements is power of two
// New elements are initialised to zero
static inline void* gca_capacity(void *ptr, size_t *size, size_t es,
                                 size_t new_size)
{
  if(new_size > *size) {
    new_size = gca_roundup64(new_size);
    if((ptr = realloc(ptr, new_size * es)) == NULL) return NULL;
    memset((char*)ptr+es*(*size), 0, es*(new_size - *size));
    *size = new_size;
  }
  return ptr;
}

#define gca_resize(b, n, new_n) \
  do { (b) = gca_capacity(b, &(n), sizeof((b)[0]), (new_n)); } while(0)

// comparison returns:
//   negative iff a < b
//          0 iff a == b
//   positive iff a > b

#define gca_cmp(a,b) (((a) > (b)) - ((b) > (a)))

#define cmpfunc(fname,fname2,type_t,cmp)                                       \
static inline int fname(const void *aa, const void *bb) __attribute__((unused));\
static inline int fname(const void *aa, const void *bb) {                      \
  const type_t a = *(const type_t *)aa;                                        \
  const type_t b = *(const type_t *)bb;                                        \
  return cmp(a, b);                                                            \
}                                                                              \
static inline int fname2(const void *aa, const void *bb, void *p)              \
  __attribute__((unused));                                                     \
static inline int fname2(const void *aa, const void *bb, void *p) {            \
  (void)p;                                                                     \
  const type_t a = *(const type_t *)aa;                                        \
  const type_t b = *(const type_t *)bb;                                        \
  return cmp(a, b);                                                            \
}
cmpfunc(gca_cmp_char,    gca_cmp2_char,    char,        gca_cmp);
cmpfunc(gca_cmp_int,     gca_cmp2_int,     int,         gca_cmp);
cmpfunc(gca_cmp_long,    gca_cmp2_long,    long,        gca_cmp);
cmpfunc(gca_cmp_float,   gca_cmp2_float,   float,       gca_cmp);
cmpfunc(gca_cmp_double,  gca_cmp2_double,  double,      gca_cmp);
cmpfunc(gca_cmp_int8,    gca_cmp2_int8,    int8_t,      gca_cmp);
cmpfunc(gca_cmp_int16,   gca_cmp2_int16,   int16_t,     gca_cmp);
cmpfunc(gca_cmp_int32,   gca_cmp2_int32,   int32_t,     gca_cmp);
cmpfunc(gca_cmp_int64,   gca_cmp2_int64,   int64_t,     gca_cmp);
cmpfunc(gca_cmp_uint8,   gca_cmp2_uint8,   uint8_t,     gca_cmp);
cmpfunc(gca_cmp_uint16,  gca_cmp2_uint16,  uint16_t,    gca_cmp);
cmpfunc(gca_cmp_uint32,  gca_cmp2_uint32,  uint32_t,    gca_cmp);
cmpfunc(gca_cmp_uint64,  gca_cmp2_uint64,  uint64_t,    gca_cmp);
cmpfunc(gca_cmp_size,    gca_cmp2_size,    size_t,      gca_cmp);
cmpfunc(gca_cmp_ptr,     gca_cmp2_ptr,     void *const, gca_cmp);
cmpfunc(gca_cmp_charptr, gca_cmp2_charptr, char *const, strcmp);
#undef cmpfunc


#define searchfunc(fname,type_t,cmp)                                           \
static inline int fname(const void *aa, void *bb) __attribute__((unused));     \
static inline int fname(const void *aa, void *bb) {                            \
  const type_t a = *(const type_t *)aa;                                        \
  const type_t b = *(type_t *)bb;                                              \
  return cmp(a, b);                                                            \
}
searchfunc(gca_search_char,    char,        gca_cmp);
searchfunc(gca_search_int,     int,         gca_cmp);
searchfunc(gca_search_long,    long,        gca_cmp);
searchfunc(gca_search_float,   float,       gca_cmp);
searchfunc(gca_search_double,  double,      gca_cmp);
searchfunc(gca_search_int8,    int8_t,      gca_cmp);
searchfunc(gca_search_int16,   int16_t,     gca_cmp);
searchfunc(gca_search_int32,   int32_t,     gca_cmp);
searchfunc(gca_search_int64,   int64_t,     gca_cmp);
searchfunc(gca_search_uint8,   uint8_t,     gca_cmp);
searchfunc(gca_search_uint16,  uint16_t,    gca_cmp);
searchfunc(gca_search_uint32,  uint32_t,    gca_cmp);
searchfunc(gca_search_uint64,  uint64_t,    gca_cmp);
searchfunc(gca_search_size,    size_t,      gca_cmp);
searchfunc(gca_search_ptr,     void *const, gca_cmp);
searchfunc(gca_search_charptr, char *const, strcmp);
#undef searchfunc

// Get Greatest Common Divisor using binary GCD algorithm
// http://en.wikipedia.org/wiki/Binary_GCD_algorithm
uint32_t gca_calc_GCD(uint32_t a, uint32_t b);

static inline void gca_swapm(void *aa, void *bb, size_t es)
{
  char *end, *a = (char*)aa, *b = (char*)bb, tmp;
  for(end = a + es; a < end; a++, b++) { tmp = *a; *a = *b; *b = tmp; }
}

// cyclic-shift an array by `shift` elements
// cycle left shifts towards zero
void gca_cycle_left(void *_ptr, size_t n, size_t es, size_t shift);
// cycle right shifts away from zero
void gca_cycle_right(void *_ptr, size_t n, size_t es, size_t shift);

// Reverse order of elements in an array
void gca_reverse(void *_ptr, size_t n, size_t es);

// Shuffle entire array
// Fisher-Yates shuffle. Initiate srand() before calling.
void gca_shuffle(void *_ptr, size_t n, size_t es);

// Sample m elements by moving them to the front of the array
// Fisher-Yates shuffle. Initiate srand() before calling.
void gca_sample(void *base, size_t n, size_t es, size_t m);

//
// Permutations
//

/*
 * Permutation example:
 *

  size_t n = 5;
  int *d = ...;
  size_t *p = NULL;

  while(gca_itr_next(&p, n))
    printf("%i %i %i %i %i", d[p[0]], d[p[1]], d[p[2]], d[p[3]], d[p[4]]);

  // Reset and loop over all permutations again
  p = gca_itr_reset(p, n);
  while(gca_itr_next(&p, n, NULL))
    printf("%i %i %i %i %i", d[p[0]], d[p[1]], d[p[2]], d[p[3]], d[p[4]]);

  // release iterator memory
  free(p);

*/

size_t* gca_itr_reset(size_t *p, size_t n);
size_t* gca_itr_next(size_t **pp, size_t n, size_t *init);

//
// binary search
//

// searchf is a function that compares a given value with the value we are
// searching for. It returns <0 if _val is < target, >0 if _val is > target,
// 0 otherwise.
void* gca_bsearch(void *_ptr, size_t n, size_t es,
                  int (*searchf)(const void *_val, void *_arg),
                  void *arg);

// Linear search
void* gca_lsearch(void *_ptr, size_t n, size_t es,
                  int (*searchf)(const void *_val, void *_arg),
                  void *arg);

//
// Quick sort
//

// Quicksort Partition 
// Pivot is in first index
// returns index of pivot after partitioning
size_t gca_qpart(void *base, size_t nel, size_t es,
                 int (*compar)(const void *_a, const void *_b, void *_arg),
                 void *arg);

// Note: quicksort is not stable, equivalent values may be swapped
void gca_qsort(void *base, size_t nel, size_t es,
               int (*compar)(const void *_a, const void *_b, void *_arg),
               void *arg);

// Get the k-th smallest element from unsorted array, using quickselect
void* gca_qselect(void *base, size_t nel, size_t es, size_t kidx,
                  int (*compar)(const void *_a, const void *_b, void *_arg),
                  void *arg);

/*
 Get median of array of size_t:

    #define avgfunc(a,b) ((a)+(b)+1.0)/2.0)
    size_t arr[], n = 10;
    size_t median = gca_median(arr, n, gca_cmp2_size, NULL, size_t, avgfunc)
*/

// Undefined if nel == 0
#define gca_median(base,nel,compar,arg,type,avgfunc) \
  (gca_qselect(base,nel,sizeof(type),(nel)/2,compar,arg), \
   ((nel)&1 ? *((type*)(base) + (nel)/2) \
            : (type)avgfunc(*(type*)gca_max((base),(nel)/2,sizeof(type),compar,arg), \
                            *((type*)(base) + (nel)/2))))

// Returns (type)(zero) if nel == 0
#define gca_median2(base,nel,compar,arg,type,avgfunc,zero) \
        (nel ? gca_median(base,nel,compar,arg,type,avgfunc) : (type)(zero))

// Mean function for integers of any size
// Can overflow, would be better to use: a/2 + b/2 + ((a|b)&1), but if macro
// uses parameters multiple times, they will be re-calculated
#define gca_ab_mean_int(a,b) (((a)+(b)+1)/2)
#define gca_ab_mean_real(a,b) ((a)/2+(b)/2)

#define gca_median_uint32(base,nel) gca_median2(base,nel,gca_cmp2_uint32,NULL,uint32_t,gca_ab_mean_int,0)
#define gca_median_uint64(base,nel) gca_median2(base,nel,gca_cmp2_uint64,NULL,uint64_t,gca_ab_mean_int,0)
#define gca_median_int(base,nel)    gca_median2(base,nel,gca_cmp2_int,   NULL,int,     gca_ab_mean_int,0)
#define gca_median_long(base,nel)   gca_median2(base,nel,gca_cmp2_long,  NULL,long,    gca_ab_mean_int,0)
#define gca_median_size(base,nel)   gca_median2(base,nel,gca_cmp2_size,  NULL,size_t,  gca_ab_mean_int,0)
#define gca_median_float(base,nel)  gca_median2(base,nel,gca_cmp2_float, NULL,float,   gca_ab_mean_real,0.0)
#define gca_median_double(base,nel) gca_median2(base,nel,gca_cmp2_double,NULL,double,  gca_ab_mean_real,0.0)

//
// Heapsort
//

#define gca_heap_parent(idx) (((idx)-1)/2)
#define gca_heap_child1(idx) (2*(idx)+1)
#define gca_heap_child2(idx) (2*(idx)+2)

// New element at index nel-1, to be pushed up the heap
void gca_heap_pushup(void *heap, size_t nel, size_t es,
                     int (*compar)(const void *_a, const void *_b, void *_arg),
                     void *arg);

// New element at index 0, to be pushed down the heap
void gca_heap_pushdwn(void *heap, size_t nel, size_t es,
                      int (*compar)(const void *_a, const void *_b, void *_arg),
                      void *arg);

void gca_heap_make(void *base, size_t nel, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg);

void gca_heap_sort(void *heap, size_t nel, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg);

//
// To heapsort an array:
//   gca_heap_make(...)
//   gca_heap_sort(...)
//

//
// Median
//

// Get pointer to median of three elements, using three comparisons
void* gca_median3(void *p0, void *p1, void *p2,
                  int (*compar)(const void *_a, const void *_b, void *_arg),
                  void *arg);

// Get pointer to median of five elements, using six comparisons
void* gca_median5(void *p0, void *p1, void *p2, void *p3, void *p4,
                  int (*compar)(const void *_a, const void *_b, void *_arg),
                  void *arg);

//
// Insertion sort and merging
//

/**
 * Insertion sort, sorted elements first, then unsorted
 * @param ptr points to sorted array
 * @param n is number of sorted elements
 * @param m is number of unsorted elements AFTER ptr+n sorted to merge in
 * @param el is element size
 * @param compar is comparison function
 * @param arg is pointer to pass to comparison function
 */
static inline void gca_isortr(void *_ptr, size_t n, size_t m, size_t el,
                              int (*compar)(const void *_a, const void *_b,
                                            void *_arg),
                              void *arg)
{
  char *b = (char*)_ptr, *pi, *pj, *end = b+el*(n+m);
  for(pi = b+el*n; pi < end; pi += el)
    for(pj = pi; pj > b && compar(pj-el,pj,arg) > 0; pj -= el)
      gca_swapm(pj-el, pj, el);
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
static inline void gca_isortf(void *_ptr, size_t n, size_t m, size_t el,
                              int (*compar)(const void *_a, const void *_b,
                                            void *_arg),
                              void *arg)
{
  char *b = (char*)_ptr, *pi, *pj, *start = b-el*m, *end = b+el*n;
  for(pi = b; pi > start; pi -= el)
    for(pj = pi; pj < end && compar(pj-el, pj, arg) > 0; pj += el)
      gca_swapm(pj-el, pj, el);
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
static inline void gca_imerge(void *_ptr, size_t n, size_t m, size_t el,
                              int (*compar)(const void *_a, const void *_b,
                                            void *_arg),
                              void *arg)
{
  char *b = (char*)_ptr, *pi, *pj, *end = b+el*(n+m);
  for(pi = b+el*n; pi < end; pi += el) {
    for(pj = pi; pj > b && compar(pj-el,pj,arg) > 0; pj -= el)
      gca_swapm(pj-el, pj, el);
    if(pj == pi) break;
  }
}

// Merge two sorted arrays to create a merged sorted array
void gca_merge(void *_dst, size_t ndst, size_t nsrc, size_t es,
               int (*compar)(const void *_a, const void *_b, void *_arg),
               void *arg);

//
// Check if an array is sorted
//
static inline bool gca_is_sorted(void *base, size_t nel, size_t es,
                                 int (*compar)(const void *_a, const void *_b,
                                               void *_arg),
                                 void *arg)
{
  char *b = (char*)base, *end = b+es*nel, *ptr;
  for(ptr = b; ptr+es < end; ptr += es)
    if(compar(ptr, ptr+es, arg) > 0)
      return false;
  return true;
}

static inline bool gca_is_rsorted(void *base, size_t nel, size_t es,
                                  int (*compar)(const void *_a, const void *_b,
                                                void *_arg),
                                  void *arg)
{
  char *b = (char*)base, *end = b+es*nel, *ptr;
  for(ptr = b; ptr+es < end; ptr += es)
    if(compar(ptr, ptr+es, arg) < 0)
      return false;
  return true;
}

// Get pointer to max entry using comparison function
static inline void* gca_max(void *base, size_t nel, size_t es,
                            int (*compar)(const void *_a, const void *_b,
                                          void *_arg),
                            void *arg)
{
  char *b = (char*)base, *end = b+es*nel, *ptr, *max = base;
  for(ptr = b+es; ptr < end; ptr += es)
    if(compar(max, ptr, arg) < 0)
      max = ptr;
  return max;
}

// Get pointer to min entry using comparison function
static inline void* gca_min(void *base, size_t nel, size_t es,
                            int (*compar)(const void *_a, const void *_b,
                                          void *_arg),
                            void *arg)
{
  char *b = (char*)base, *end = b+es*nel, *ptr, *min = base;
  for(ptr = b+es; ptr < end; ptr += es)
    if(compar(min, ptr, arg) > 0)
      min = ptr;
  return min;
}

#endif /* CARRAYS_H_ */
