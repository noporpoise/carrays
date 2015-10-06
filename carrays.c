#include <string.h>
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

// Fisher-Yates shuffle. Initiate srand() before calling.
void array_shuffle(void *_ptr, size_t n, size_t es)
{
  if(n <= 1 || !es) return;
  char *ptr = (char*)_ptr;
  size_t i, j;
  for(i = n-1; i != SIZE_MAX; i--) {
    j = ((size_t)rand()*(i+1))/RAND_MAX; // rand between 0,i inclusive
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
  else if(ndst < nsrc && ndst < CARRAYS_ISORT_CUTOFF) {
    // insertion sort of dst into src
    sarrays_isortf(dst+es*ndst, nsrc, ndst, es, compar, arg);
  } else if(nsrc < CARRAYS_ISORT_CUTOFF) {
    // insertion sort of src into dst
    sarrays_isortr(dst, ndst, nsrc, es, compar, arg);
  }
  else {
    array_qsort_r(dst, ndst+nsrc, es, compar, arg);
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

// Quick sort
// Note: quicksort is not stable, equivalent values may be swapped
void array_qsort_r(void *base, size_t nel, size_t w,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)
{
  char *b = base, *end = b + nel*w;
  if(nel < CARRAYS_ISORT_CUTOFF) {
    /* Insertion sort for small inputs */
    sarrays_isortr(base, 0, nel, w, compar, arg);
  }
  else
  {
    /* nel >= 6; Quicksort */

    /* Use median of first, middle and last items as pivot */
    char *last = b+w*(nel-1);
    char *l[3] = {b, b+w*(nel/2), last};
    if(compar(l[0],l[1],arg) > 0) { SWAP(l[0], l[1]); }
    if(compar(l[1],l[2],arg) > 0) {
      SWAP(l[1], l[2]);
      if(compar(l[0],l[1],arg) > 0) { SWAP(l[0], l[1]); }
    }

    // swap l[id], l[2] to put pivot as last element
    carrays_swapm(l[1], last, w);

    char *pl = b, *pr = last;

    while(pl < pr) {
      for(; pl < pr; pl += w) {
        if(compar(pl, pr, arg) > 0) {
          carrays_swapm(pl, pr, w);
          pr -= w; /* pivot now at pl */
          break;
        }
      }
      for(; pl < pr; pr -= w) {
        if(compar(pl, pr, arg) > 0) {
          carrays_swapm(pl, pr, w);
          pl += w; /* pivot now at pr */
          break;
        }
      }
    }

    array_qsort_r(b, (pl-b)/w, w, compar, arg);
    array_qsort_r(pl+w, (end-(pl+w))/w, w, compar, arg);
  }
}
