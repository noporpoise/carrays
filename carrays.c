#include <string.h>
#include <assert.h>
#include "carrays.h"

// Get Greatest Common Divisor using binary GCD algorithm
// http://en.wikipedia.org/wiki/Binary_GCD_algorithm
uint32_t carrays_calc_GCD(uint32_t a, uint32_t b)
{
  uint32_t shift;

  if(a == 0) return b;
  if(b == 0) return a;

  // Find power of two divisor
  for(shift = 0; ((a | b) & 1) == 0; shift++) { a >>= 1; b >>= 1; }

  // Remove remaining factors of two from a - they are not common
  while((a & 1) == 0) a >>= 1;

  do
  {
    // Remove remaining factors of two from b - they are not common
    while((b & 1) == 0) b >>= 1;

    if(a > b) { SWAP(a,b); }
    b = b - a;
  }
  while(b != 0);

  return a << shift;
}

// cyclic-shift an array by `shift` elements
// cycle left shifts towards zero
void array_cycle_left(void *_ptr, size_t n, size_t es, size_t shift)
{
  char *ptr = (char*)_ptr;
  if(n <= 1 || !shift) return; // cannot mod by zero
  shift = shift % n; // shift cannot be greater than n

  // Using GCD
  size_t i, j, k, gcd = carrays_calc_GCD(n, shift);
  char tmp[es];

  // i is initial starting position
  // Copy from k -> j, stop if k == i, since arr[i] already overwritten
  for(i = 0; i < gcd; i++) {
    memcpy(tmp, ptr+es*i, es); // tmp = arr[i]
    for(j = i; 1; j = k) {
      k = j+shift;
      if(k >= n) k -= n;
      if(k == i) break;
      memcpy(ptr+es*j, ptr+es*k, es); // arr[j] = arr[k];
    }
    memcpy(ptr+es*j, tmp, es); // arr[j] = tmp;
  }
}

// cycle right shifts away from zero
void array_cycle_right(void *_ptr, size_t n, size_t es, size_t shift)
{
  if(!n || !shift) return; // cannot mod by zero
  shift = shift % n; // shift cannot be greater than n
  // cycle right by `s` is equivalent to cycle left by `n - s`
  array_cycle_left(_ptr, n, es, n - shift);
}

void array_reverse(void *_ptr, size_t n, size_t es)
{
  if(n <= 1 || !es) return;
  char *a = (char*)_ptr, *b = a + es*(n-1);
  for(; a < b; a += es, b -= es) carrays_swapm(a, b, es);
}

// Fisher-Yates shuffle. Initiate srand48() before calling.
void array_shuffle(void *_ptr, size_t n, size_t es)
{
  if(n <= 1 || !es) return;
  char *ptr = (char*)_ptr;
  size_t i, j;
  for(i = n-1; i > 0; i--) {
    j = drand48() * (i+1); // rand between 0,i inclusive
    carrays_swapm(ptr+es*i, ptr+es*j, es);
  }
}

#define CARRAYS_ISORT_CUTOFF 6

// Merge two sorted arrays to create a merged sorted array
void sarrays_merge(void *_dst, size_t ndst, size_t nsrc, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)
{
  char *dst = (char*)_dst, *src = dst+es*ndst, *end = dst+es*(ndst+nsrc);
  if(!nsrc || !ndst) {}
  else if(compar(src-es, src, arg) <= 0) {}
  else if(compar(dst, end-es, arg) >= 0) {
    array_cycle_left(dst, ndst, es, ndst);
  }
  else if(ndst+nsrc < CARRAYS_ISORT_CUTOFF) {
    // insertion sort merge of dst and src
    sarrays_imerge(dst, ndst, nsrc, es, compar, arg);
  }
  else {
    array_qsort(dst, ndst+nsrc, es, compar, arg);
  }
}

// binary search
// compar is a function that compares a given value with the value we are
// searching for. It returns <0 if _val is < target, >0 if _val is > target,
// 0 otherwise.
void* sarray_bsearch(void *_ptr, size_t n, size_t es,
                     int (*compar)(const void *_val, void *_arg),
                     void *arg)
{
  if(n == 0) return NULL;

  char *ptr = (char*)_ptr;
  char *a = ptr, *b = ptr + es*(n-1), *mid;
  int cmp;

  while(a <= b)
  {
    mid = ptr + es * (((a-ptr) + (b-ptr)) / (2*es));
    cmp = compar(mid, arg);
    if(cmp > 0) b = mid - es;
    else if(cmp < 0) a = mid + es;
    else return mid;
  }

  return NULL;
}

//
// Quick sort
//

// Quicksort Partition
// Pivot is in first index
// returns index of pivot after partitioning
size_t array_qpart(void *base, size_t nel, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)
{
  if(nel <= 1) return 0;
  char pivot[es], *b = (char*)base, *pl = b, *pr = b+es*(nel-1);
  memcpy(pivot, b, es);
  // hole at pl

  while(pl < pr) {
    for(; pl < pr; pr -= es) {
      if(compar(pr, pivot, arg) < 0) {
        memcpy(pl, pr, es);
        pl += es; /* hole now at pr */
        break;
      }
    }
    for(; pl < pr; pl += es) {
      if(compar(pl, pivot, arg) > 0) {
        memcpy(pr, pl, es);
        pr -= es; /* hole now at pl */
        break;
      }
    }
  }
  // now pl == pr
  memcpy(pl, pivot, es);
  return (pl-b)/es;
}

// Get pointer to median of first, middle and last elements
static inline char* median3(char *b, size_t nel, size_t es,
                            int (*compar)(const void *_a, const void *_b,
                                          void *_arg),
                            void *arg)
{
  char *p[3] = {b, b+es*(nel/2), b+es*(nel-1)};
  if(compar(p[0],p[1],arg) > 0) { SWAP(p[0], p[1]); }
  if(compar(p[1],p[2],arg) > 0) {
    SWAP(p[1], p[2]);
    if(compar(p[0],p[1],arg) > 0) { SWAP(p[0], p[1]); }
  }
  return p[1];
}

// Note: quicksort is not stable, equivalent values may be swapped
void array_qsort(void *base, size_t nel, size_t es,
                 int (*compar)(const void *_a, const void *_b, void *_arg),
                 void *arg)
{
  char *b = (char*)base;

  if(nel < CARRAYS_ISORT_CUTOFF) {
    /* Insertion sort for small inputs */
    arrays_isortr(base, 0, nel, es, compar, arg);
  }
  else
  {
    /* nel >= 6; Quicksort */

    /* Use median of first, middle and last items as pivot */
    char *pivot = median3(b, nel, es, compar, arg);

    // swap pivot into first element and partition
    carrays_swapm(b, pivot, es);
    size_t pidx = array_qpart(b, nel, es, compar, arg);

    array_qsort(b, pidx, es, compar, arg);
    array_qsort(b+es*(pidx+1), nel-(pidx+1), es, compar, arg);
  }
}

//
// Quickselect
//

// Get k-th element from unsorted array, uising quickselect
void array_qselect(void *base, size_t nel, size_t es, size_t kidx,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)
{
  char *b = (char*)base;
  size_t pidx, l = 0, r = nel-1;

  assert(kidx < nel);
  if(nel <= 1) return;

  while(1)
  {
    /* Use median of first, middle and last items as pivot */
    char *pivot = median3(b+es*l, r-l+1, es, compar, arg);

    // swap pivot into first element and partition
    carrays_swapm(b+es*l, pivot, es);
    pidx = l + array_qpart(b+es*l, r-l+1, es, compar, arg);

    if(pidx > kidx) r = pidx-1;
    else if(pidx < kidx) l = pidx+1;
    else break;
  }
}

// Get k-th element from unsorted array, using quickselect and median of medians
// void array_qselect_mmed(void *base, size_t nel, size_t es, size_t kidx,
//                         int (*compar)(const void *_a, const void *_b, void *_arg),
//                         void *arg)
// {
  
// }

//
// Heapsort
//

void array_heap_make(void *base, size_t nel, size_t es,
                     int (*compar)(const void *_a, const void *_b, void *_arg),
                     void *arg)
{
  char *b = (char*)base;
  size_t chi, pi, n;
  char tmp[es];
  // add elements one-at-a-time to the end
  for(n = 1; n < nel; n++)
  {
    memcpy(tmp, b+es*n, es);
    // push up the tree
    for(chi = n; chi > 0; chi = pi) {
      pi = array_heap_parent(chi);
      if(compar(b+es*pi, tmp, arg) >= 0) break;
      memcpy(b+es*chi, b+es*pi, es);
    }
    memcpy(b+es*chi, tmp, es);
  }
}

// ar[idx]: child1 => arr[2*idx+1], child2 => arr[2*idx+2]
void array_heap_sort(void *heap, size_t nel, size_t es,
                     int (*compar)(const void *_a, const void *_b, void *_arg),
                     void *arg)
{
  if(nel <= 1) return;
  char *b = (char*)heap, *last, *p, *ch;
  char tmp[es];
  // take elements off the top one at a time, by swapping first and last
  for(last = b+es*(nel-1); last > b; last -= es)
  {
    memcpy(tmp, last, es);
    memcpy(last, b, es);
    // push down the tree
    for(p = b, ch = b+es; ch < last; p = ch, ch = b + 2*(ch-b) + es) {
      ch = (ch+es < last && compar(ch,ch+es,arg) < 0 ? ch+es : ch); // biggest child
      if(compar(tmp, ch, arg) >= 0) break;
      memcpy(p, ch, es);
    }
    memcpy(p, tmp, es);
  }
}
