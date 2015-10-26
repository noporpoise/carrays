# CArrays: Generic C arrays

Fast algorithms to work on generic C arrays. Public domain license (no waranty, do what you like).

Compile and run tests:

    make all
    make test

To use in other projects, add include to the top of your `.c` files:

    #include "pathto/carrays/carrays.h"

And add the .c file to your compile path:

    gcc -o myprog ... pathto/carrays/carrays.c main.c

Alternatively just copy the `carrays.c` and `carrays.h` in with your `.c` and `.h` files.

## Functions

Many functions take a comparison function and an argument to pass to this function.
Comparison functions take the form:

    int (*compar)(const void *_a, const void *_b, void *_arg)

And should return:
- < 0 if a < b
- 0 if a == b
- > 0 if a > b

All functions start with `gca_`, short for Generic C Array.

### General

Reverse the order of elements in an array

    void gca_reverse(void *_ptr, size_t n, size_t es)

Sample m elements by moving them to the front of the array.
Fisher-Yates shuffle. Initiate srand() before calling.

    void gca_sample(void *base, size_t n, size_t es, size_t m)

Shuffle entire array.
Fisher-Yates shuffle. Initiate srand() before calling.

    void gca_shuffle(void *base, size_t n, size_t es)

Get Greatest Common Divisor using binary GCD algorithm. This is used in the cycle
shift functions.  http://en.wikipedia.org/wiki/Binary_GCD_algorithm

    uint32_t gca_calc_GCD(uint32_t a, uint32_t b)

### Cycle shift

    void gca_cycle_left(void *_ptr, size_t n, size_t es, size_t shift)
    void gca_cycle_right(void *_ptr, size_t n, size_t es, size_t shift)

### Permutation Iteration

Iterating over all permutations of an array in order

    size_t* gca_itr_reset(size_t *p, size_t n)
    size_t* gca_itr_next(size_t **pp, size_t n)

Iteration example:

    size_t n = 5;
    int *d = ...;
    size_t *p = NULL;

    while(gca_itr_next(&p, n))
      printf("%i %i %i %i %i", d[p[0]], d[p[1]], d[p[2]], d[p[3]], d[p[4]]);

    // Reset and loop over all permutations again
    // after each call to gca_itr_next, p={0,1,2,3}, then {0,1,3,2} ...
    // in numerical order
    p = gca_itr_reset(p, n);
    while(gca_itr_next(&p, n))
      printf("%i %i %i %i %i", d[p[0]], d[p[1]], d[p[2]], d[p[3]], d[p[4]]);

    // release iterator memory
    free(p);

### Binary search

searchf is a function that compares a given value with the value we are
searching for. It returns <0 if _val is < target, >0 if _val is > target,
0 otherwise.

    void* gca_bsearch(void *_ptr, size_t n, size_t es,
                      int (*searchf)(const void *_val, void *_arg),
                      void *arg)

### Quicksort

Quicksort Partition. Pivot is in first index. Returns index of pivot after partitioning.
Note: quicksort is not stable, equivalent values may be swapped.

    size_t gca_qpart(void *base, size_t nel, size_t es,
                     int (*compar)(const void *_a, const void *_b, void *_arg),
                     void *arg)

Quick sort:

    void gca_qsort(void *base, size_t nel, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)

Get the k-th smallest element from unsorted array, using quickselect:

    void gca_qselect(void *base, size_t nel, size_t es, size_t kidx,
                     int (*compar)(const void *_a, const void *_b, void *_arg),
                     void *arg)

### Heaps

Get index of parent / child of a given element:

    gca_heap_parent(idx)
    gca_heap_child1(idx)
    gca_heap_child2(idx)

New element at index nel-1, to be pushed up the heap:

    void gca_heap_pushup(void *heap, size_t nel, size_t es,
                         int (*compar)(const void *_a, const void *_b, void *_arg),
                         void *arg)

New element at index 0, to be pushed down the heap

    void gca_heap_pushdwn(void *heap, size_t nel, size_t es,
                          int (*compar)(const void *_a, const void *_b, void *_arg),
                          void *arg)

Build a heap from an unsorted array:

    void gca_heap_make(void *base, size_t nel, size_t es,
                       int (*compar)(const void *_a, const void *_b, void *_arg),
                       void *arg)

Convert a heap into a sorted array:

    void gca_heap_sort(void *heap, size_t nel, size_t es,
                       int (*compar)(const void *_a, const void *_b, void *_arg),
                       void *arg)

To heapsort an array:

    gca_heap_make(...)
    gca_heap_sort(...)

### Insertion sort

Insertion sort, sorted elements first, then unsorted. Parameters:
- ptr points to sorted array
- n is number of sorted elements
- m is number of unsorted elements AFTER ptr+n sorted to merge in
- el is element size
- compar is comparison function
- arg is pointer to pass to comparison function

    void gca_isortr(void *_ptr, size_t n, size_t m, size_t el,
                    int (*compar)(const void *_a, const void *_b, void *_arg),
                    void *arg)

Insertion sort unsorted elements first, then sorted.
- ptr points to sorted array
- n is number of sorted elements
- m is number of unsorted elements BEFORE ptr
- el is element size
- compar is comparison function
- arg is pointer to pass to comparison function

    void gca_isortf(void *_ptr, size_t n, size_t m, size_t el,
                    int (*compar)(const void *_a, const void *_b, void *_arg),
                    void *arg)


Insertion sort merging between two adjacent sorted arrays
- ptr points to array
- n is number of sorted elements
- m is number of sorted elements
- el is element size
- compar is comparison function
- arg is pointer to pass to comparison function

    void gca_imerge(void *_ptr, size_t n, size_t m, size_t el,
                    int (*compar)(const void *_a, const void *_b, void *_arg),
                    void *arg)

Merge two sorted arrays to create a merged sorted array:

    void gca_merge(void *_dst, size_t ndst, size_t nsrc, size_t es,
                   int (*compar)(const void *_a, const void *_b, void *_arg),
                   void *arg)

### Median

Get pointer to median of three elements, using three comparisons:

    void* gca_median3(void *p0, void *p1, void *p2,
                      int (*compar)(const void *_a, const void *_b, void *_arg),
                      void *arg)

Get pointer to median of five elements, using six comparisons:

    void* gca_median5(void *p0, void *p1, void *p2, void *p3, void *p4,
                      int (*compar)(const void *_a, const void *_b, void *_arg),
                      void *arg)

### Testing if sorted

Test if an array is sorted, given a comparison function:

    bool gca_is_sorted(void *base, size_t nel, size_t es,
                       int (*compar)(const void *_a, const void *_b, void *_arg),
                       void *arg)

Test if an array is reverse sorted, given a comparison function:

    bool gca_is_rsorted(void *base, size_t nel, size_t es,
                        int (*compar)(const void *_a, const void *_b,
                                      void *_arg),
                        void *arg)

## Development

Please submit suggestions, requests and bug reports through Github or email me:
- Isaac Turner (turner.isaac@gmail.com)
- https://github.com/noporpoise/carrays

Pull requests and bug reports very welcome.

TODO:
[ ] Quickselect using median of medians: gca_qselect_mmed(...)

License
=======

This software is in the *Public Domain*. That means you can do whatever you like
with it. That includes being used in proprietary products without attribution or
restrictions. There are no warranties and there may be bugs.

Formally we are using CC0 - a Creative Commons license to place this work in the
public domain. A copy of CC0 is in the LICENSE file.

    "CC0 is a public domain dedication from Creative Commons. A work released
    under CC0 is dedicated to the public domain to the fullest extent permitted
    by law. If that is not possible for any reason, CC0 also provides a lax,
    permissive license as a fallback. Both public domain works and the lax
    license provided by CC0 are compatible with the GNU GPL."
      - http://www.gnu.org/licenses/license-list.html#CC0

