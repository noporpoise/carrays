#define _XOPEN_SOURCE /* drand48() */
#include <stdlib.h> // drand48()
#include <string.h>
#include <assert.h>
#include "carrays.h"

//
// Generic C Arrays
//
// All functions have the gca_ prefix
//
// 2015-10-24 Isaac Turner (turner.isaac@gmail.com)
//

//
// TODO:
// 1. quickselect using median of medians
//

// Get Greatest Common Divisor using binary GCD algorithm
// http://en.wikipedia.org/wiki/Binary_GCD_algorithm
uint32_t gca_calc_GCD(uint32_t a, uint32_t b)
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
void gca_cycle_left(void *_ptr, size_t n, size_t es, size_t shift)
{
  char *ptr = (char*)_ptr;
  if(n <= 1 || !shift) return; // cannot mod by zero
  shift = shift % n; // shift cannot be greater than n

  // Using GCD
  size_t i, j, k, gcd = gca_calc_GCD(n, shift);
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
void gca_cycle_right(void *_ptr, size_t n, size_t es, size_t shift)
{
  if(!n || !shift) return; // cannot mod by zero
  shift = shift % n; // shift cannot be greater than n
  // cycle right by `s` is equivalent to cycle left by `n - s`
  gca_cycle_left(_ptr, n, es, n - shift);
}

void gca_reverse(void *_ptr, size_t n, size_t es)
{
  if(n <= 1 || !es) return;
  char *a = (char*)_ptr, *b = a + es*(n-1);
  for(; a < b; a += es, b -= es) gca_swapm(a, b, es);
}

// Shuffle entire array
// Fisher-Yates shuffle. Initiate srand48() before calling.
void gca_shuffle(void *base, size_t n, size_t es)
{
  gca_sample(base, n, es, n);
}

// Sample m elements by moving them to the front of the array
// Fisher-Yates shuffle. Initiate srand48() before calling.
void gca_sample(void *base, size_t n, size_t es, size_t m)
{
  char *b = (char*)base;
  size_t i, j;
  for(i = 0; i < m; i++) {
    j = i + (n-i)*drand48(); // i + rand where: 0 <= rand < n-i
    gca_swapm(b+es*i, b+es*j, es);
  }
}


// Merge two sorted arrays to create a merged sorted array
void gca_merge(void *_dst, size_t ndst, size_t nsrc, size_t es,
               int (*compar)(const void *_a, const void *_b, void *_arg),
               void *arg)
{
  char *dst = (char*)_dst, *src = dst+es*ndst, *end = dst+es*(ndst+nsrc);
  if(!nsrc || !ndst) {}
  else if(compar(src-es, src, arg) <= 0) {}
  else if(compar(dst, end-es, arg) >= 0) {
    gca_cycle_left(dst, ndst, es, ndst);
  }
  else if(ndst+nsrc < 6) {
    // insertion sort merge of dst and src
    gca_imerge(dst, ndst, nsrc, es, compar, arg);
  }
  else {
    gca_qsort(dst, ndst+nsrc, es, compar, arg);
  }
}

// binary search
// searchf is a function that compares a given value with the value we are
// searching for. It returns <0 if _val is < target, >0 if _val is > target,
// 0 otherwise.
void* gca_bsearch(void *_ptr, size_t n, size_t es,
                  int (*searchf)(const void *_val, void *_arg),
                  void *arg)
{
  if(n == 0) return NULL;

  char *ptr = (char*)_ptr, *mptr;
  size_t l = 0, r = n-1, mid;
  int cmp;

  while(1)
  {
    // find mid without overflow
    mid = l + (r - l)/2;
    mptr = ptr+es*mid;
    cmp = searchf(mptr, arg);
    if(cmp == 0) return mptr;
    else if(cmp > 0) {
      if(!mid) return NULL;
      r = mid-1;
    }
    else if(cmp < 0) {
      if(mid+1 == n) return NULL;
      l = mid+1;
    }
  }

  return NULL;
}

// Linear search
void* gca_lsearch(void *base, size_t n, size_t es,
                  int (*searchf)(const void *_val, void *_arg),
                  void *arg)
{
  char *b = (char*)base, *end = b+es*n;
  for(; b < end; b++)
    if(searchf(b,arg) == 0)
      return b;
  return NULL;
}

//
// Quick sort
//

// Quicksort Partition
// Pivot is in first index
// returns index of pivot after partitioning
size_t gca_qpart(void *base, size_t nel, size_t es,
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

// Note: quicksort is not stable, equivalent values may be swapped
void gca_qsort(void *base, size_t nel, size_t es,
               int (*compar)(const void *_a, const void *_b, void *_arg),
               void *arg)
{
  char *b = (char*)base;

  if(nel < 6) {
    /* Insertion sort for small inputs */
    gca_isortr(base, 0, nel, es, compar, arg);
  }
  else
  {
    /* Use median of first, middle and last items as pivot */
    char *pivot = gca_median3(b, b+es*(nel/2), b+es*(nel-1), compar, arg);

    // swap pivot into first element and partition
    gca_swapm(b, pivot, es);
    size_t pidx = gca_qpart(b, nel, es, compar, arg);

    gca_qsort(b, pidx, es, compar, arg);
    gca_qsort(b+es*(pidx+1), nel-(pidx+1), es, compar, arg);
  }
}

//
// Quickselect
//

// Get the k-th smallest element from unsorted array, using quickselect
void* gca_qselect(void *base, size_t nel, size_t es, size_t kidx,
                  int (*compar)(const void *_a, const void *_b, void *_arg),
                  void *arg)
{
  char *b = (char*)base;
  size_t pidx, l = 0, r = nel-1;

  assert(kidx < nel);
  if(nel <= 1) return b;

  while(1)
  {
    /* Use median of first, middle and last items as pivot */
    char *pivot = gca_median3(b+es*l, b+es*(l+(r-l+1)/2), b+es*r, compar, arg);

    // swap pivot into first element and partition
    gca_swapm(b+es*l, pivot, es);
    pidx = l + gca_qpart(b+es*l, r-l+1, es, compar, arg);

    if(pidx > kidx) r = pidx-1;
    else if(pidx < kidx) l = pidx+1;
    else break;
  }

  return b+es*kidx;
}

// Get k-th element from unsorted array, using quickselect and median of medians
// void gca_qselect_mmed(void *base, size_t nel, size_t es, size_t kidx,
//                       int (*compar)(const void *_a, const void *_b, void *_arg),
//                       void *arg)
// {

// }

//
// Heapsort
//

// New element at index nel-1, to be pushed up the heap
void gca_heap_pushup(void *heap, size_t nel, size_t es,
                       int (*compar)(const void *_a, const void *_b, void *_arg),
                       void *arg)
{
  size_t chi, pi; // child idx, parent idx
  char tmp[es], *b = (char*)heap;
  memcpy(tmp, b+es*(nel-1), es);
  for(chi = nel-1; chi > 0; chi = pi) {
    pi = gca_heap_parent(chi);
    if(compar(b+es*pi, tmp, arg) >= 0) break;
    memcpy(b+es*chi, b+es*pi, es);
  }
  memcpy(b+es*chi, tmp, es);
}

// New element at index 0, to be pushed down the heap
void gca_heap_pushdwn(void *heap, size_t nel, size_t es,
                      int (*compar)(const void *_a, const void *_b, void *_arg),
                      void *arg)
{
  char tmp[es], *b = (char*)heap, *last = b+es*(nel-1), *p, *ch;
  memcpy(tmp, last, es);
  for(p = b, ch = b+es; ch < last; p = ch, ch = b + 2*(ch-b) + es) {
    ch = (ch+es < last && compar(ch,ch+es,arg) < 0 ? ch+es : ch); // biggest child
    if(compar(tmp, ch, arg) >= 0) break;
    memcpy(p, ch, es);
  }
  memcpy(p, tmp, es);
}

void gca_heap_make(void *heap, size_t nel, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)
{
  size_t n;
  // add elements one-at-a-time to the end
  for(n = 2; n <= nel; n++)
    gca_heap_pushup(heap, n, es, compar, arg);
}

// ar[idx]: child1 => arr[2*idx+1], child2 => arr[2*idx+2]
void gca_heap_sort(void *heap, size_t nel, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)
{
  if(nel <= 1) return;
  char *b = (char*)heap, *end;
  size_t n;
  // take elements off the top one at a time, by swapping first and end
  for(n = nel-1, end = b+es*n; n > 1; n--, end -= es)
  {
    gca_swapm(b, end, es);
    gca_heap_pushdwn(heap, n, es, compar, arg);
  }
  gca_swapm(b, b+es, es);
}

//
// Median
//

// Get pointer to median of three elements
void* gca_median3(void *p0, void *p1, void *p2,
                  int (*compar)(const void *_a, const void *_b, void *_arg),
                  void *arg)
{
  if(compar(p0, p1, arg) > 0) SWAP(p0, p1);
  if(compar(p1, p2, arg) > 0) {
    SWAP(p1, p2);
    if(compar(p0, p1, arg) > 0) SWAP(p0, p1);
  }
  return p1;
}

// Get pointer to median of five elements
void* gca_median5(void *p0, void *p1, void *p2, void *p3, void *p4,
                  int (*compar)(const void *_a, const void *_b, void *_arg),
                  void *arg)
{
  // make p0<p1
  if(compar(p0, p1, arg) > 0) SWAP(p0, p1);
  // make p2<p3
  if(compar(p2, p3, arg) > 0) SWAP(p2, p3);

  // get rid of min(min(p0,p1),min(p2,p3)),
  // overwrite with p4
  // make p0<p1, p2<p3 again
  if(compar(p0, p2, arg) < 0) {
    p0 = p4;
    if(compar(p0, p1, arg) > 0) SWAP(p0, p1);
  } else {
    p2 = p4;
    if(compar(p2, p3, arg) > 0) SWAP(p2, p3);
  }

  if(compar(p0, p2, arg) < 0) {
    return (compar(p1, p2, arg) < 0 ? p1 : p2);
  } else {
    return (compar(p3, p0, arg) < 0 ? p3 : p0);
  }
}


//
// Permutation
//

size_t* gca_itr_reset(size_t *p, size_t n)
{
  if(!p) p = malloc(n * sizeof(size_t));
  if(p && n) p[0] = SIZE_MAX;
  return p;
}

size_t* gca_itr_next(size_t **pp, size_t n, size_t *init)
{
  size_t i, j, *p = *pp;

  if(!n) return NULL;
  if(!*pp) {
    p = *pp = malloc(n * sizeof(size_t));
    p[0] = SIZE_MAX;
  }
  if(p[0] == SIZE_MAX) {
    if(init) memcpy(p, init, n * sizeof(size_t));
    else for(i = 0; i < n; i++) p[i] = i;
    return p;
  }

  // find i such that p[i-1] < p[i]
  i = n-1;
  while(i > 0 && p[i-1] >= p[i]) i--;
  if(!i) return NULL; // end; hit max e.g 4,3,2,1
  // find last j following i-1 where that p[i-1] < p[j]
  for(j = i; j+1 < n && p[i-1] < p[j+1]; j++) {}

  // printf(" i:%zu j:%zu\n", i-1, j);
  SWAP(p[i-1], p[j]);
  gca_reverse(p+i, n-i, sizeof(p[0]));

  return p;
}
