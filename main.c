#include <stdio.h>
#include <stdbool.h>
#include "circ_array.h"
#include "carrays.h"

size_t num_tests_run = 0, num_tests_failed = 0;

// Test MACROs
#define QUOTE_MACRO(str) #str
#define QUOTE_VALUE(str) QUOTE_MACRO(str)

#define TASSERT2(x,fmt,...) do {                                               \
  num_tests_run++;                                                             \
  if(!(x)) {                                                                   \
    num_tests_failed++;                                                        \
    fprintf(stderr, "[%s:%i] Failed: %s\n",__FILE__,__LINE__,QUOTE_VALUE(x));  \
    if((fmt) != NULL) fprintf(stderr, fmt, ##__VA_ARGS__);                     \
  }                                                                            \
} while(0)

#define TASSERT(x) do {                                                        \
  num_tests_run++;                                                             \
  if(!(x)) {                                                                   \
    num_tests_failed++;                                                        \
    fprintf(stderr, "[%s:%i] Failed: %s\n",__FILE__,__LINE__,QUOTE_VALUE(x));  \
  }                                                                            \
} while(0)

#define die(fmt,...) do { \
  fprintf(stderr, "[%s:%i] Error: %s() "fmt"\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
  exit(EXIT_FAILURE); \
} while(0)

#define status(fmt,...) do { \
  fprintf(stdout, "[%s:%i] "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
} while(0)

void test_round()
{
  status("Testing roundup32() / roundup64()");

  uint64_t i;

  TASSERT(roundup32(0) == 0 && roundup64(0) == 0);
  TASSERT(roundup32(1) == 1 && roundup64(1) == 1);
  TASSERT(roundup32(2) == 2 && roundup64(2) == 2);
  TASSERT(roundup32(3) == 4 && roundup64(3) == 4);
  TASSERT(roundup32(4) == 4 && roundup64(4) == 4);
  TASSERT(roundup32(5) == 8 && roundup64(5) == 8);
  TASSERT(roundup32(6) == 8 && roundup64(6) == 8);
  TASSERT(roundup32(7) == 8 && roundup64(7) == 8);
  TASSERT(roundup32(8) == 8 && roundup64(8) == 8);
  TASSERT(roundup32(100) == 128 && roundup64(100) == 128);

  for(i = 2; i < 32; i++) {
    TASSERT(roundup32((1U<<i)-1) == (1U<<i));
    TASSERT(roundup32(1U<<i) == (1U<<i));
  }

  for(i = 2; i < 64; i++) {
    TASSERT(roundup64((1UL<<i)-1) == (1UL<<i));
    TASSERT(roundup64(1UL<<i) == (1UL<<i));
  }
}

void test_GCD()
{
  status("Testing calculating GCD...");

  size_t i;
  for(i = 0; i < 100; i++) {
    TASSERT(carrays_calc_GCD(i,i) == i);
    TASSERT(carrays_calc_GCD(i,0) == i);
    TASSERT(carrays_calc_GCD(0,i) == i);
    TASSERT(carrays_calc_GCD(i,1) == 1);
    TASSERT(carrays_calc_GCD(1,i) == 1);
  }

  TASSERT(carrays_calc_GCD(2,4) == 2);
  TASSERT(carrays_calc_GCD(4,2) == 2);
  TASSERT(carrays_calc_GCD(6,9) == 3);
  TASSERT(carrays_calc_GCD(9,6) == 3);
  TASSERT(carrays_calc_GCD(0,0) == 0);
  TASSERT(carrays_calc_GCD(10,0) == 10);
  TASSERT(carrays_calc_GCD(0,10) == 10);
  TASSERT(carrays_calc_GCD(2,2) == 2);
  TASSERT(carrays_calc_GCD(1,1) == 1);
  TASSERT(carrays_calc_GCD(1,2) == 1);
  TASSERT(carrays_calc_GCD(1,100) == 1);
  TASSERT(carrays_calc_GCD(2,4) == 2);
  TASSERT(carrays_calc_GCD(7,5) == 1);
  TASSERT(carrays_calc_GCD(18,6) == 6);
  TASSERT(carrays_calc_GCD(3,6) == 3);
  TASSERT(carrays_calc_GCD(100,120) == 20);
  TASSERT(carrays_calc_GCD(100,125) == 25);
}

void _test_array_cycle(size_t *arr, size_t n)
{
  size_t shift, i;

  // initialise
  for(i = 0; i < n; i++) arr[i] = i;

  // cycle left by 0, n, 2n, ... should all return input
  array_cycle_left(arr, n, sizeof(size_t), 0);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  array_cycle_left(arr, n, sizeof(size_t), n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  array_cycle_left(arr, n, sizeof(size_t), 2*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  array_cycle_left(arr, n, sizeof(size_t), 3*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);

  // cycle right by 0, n, 2n, ... should all return input
  array_cycle_right(arr, n, sizeof(size_t), 0);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  array_cycle_right(arr, n, sizeof(size_t), n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  array_cycle_right(arr, n, sizeof(size_t), 2*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  array_cycle_right(arr, n, sizeof(size_t), 3*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);

  for(shift = 0; shift < n; shift++) {
    for(i = 0; i < n; i++) arr[i] = i;
    array_cycle_left(arr, n, sizeof(size_t), shift);
    for(i = 0; i < n; i++) TASSERT(arr[i] == ((i+shift) % n));
    // shift back
    array_cycle_right(arr, n, sizeof(size_t), shift);
    for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  }
}

void test_array_cycle()
{
  status("Testing array cycle left/right...");
  size_t n, arr[100];

  // Test all array lengths up to 100
  for(n = 0; n < 100; n++)
    _test_array_cycle(arr, n);
}

void test_reverse()
{
  status("Testing array reverse...");

  #define N 100
  size_t i, n, tmp[N];
  for(n = 0; n <= N; n++) {
    for(i = 0; i < n; i++) tmp[i] = i;
    array_reverse(tmp, n, sizeof(tmp[0]));
    for(i = 0; i < n && tmp[i] == n-i-1; i++) {}
    TASSERT(i == n);
  }
  #undef N
}

void test_bsearch()
{
  status("Testing binary search...");

  #define N 100
  int i, n, find, arr[N];
  void *ptr;
  for(i = 0; i < N; i++) arr[i] = i;

  find = 20;
  ptr = sarray_bsearch(arr, N, sizeof(arr[0]), array_search_int, &find);
  TASSERT(ptr == &arr[find]);

  for(n = 0; n <= N; n++) {
    for(find = -2; find <= n+2; find++) {
      ptr = sarray_bsearch(arr, n, sizeof(arr[0]), array_search_int, &find);
      TASSERT(ptr == (find < 0 || find >= n ? NULL : &arr[find]));
    }
  }
  #undef N
}

void test_quicksort()
{
  status("Testing quicksort...");

  #define N 100
  size_t i, j, n, arr[N];

  // Try different array lengths
  for(n = 0; n <= N; n++)
  {
    // start with reverse sorted array
    for(i = 0; i < n; i++) arr[i] = n-1-i;
    array_qsort(arr, n, sizeof(arr[0]), array_cmp2_size, NULL);
    TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_size, NULL));

    // start with sorted array
    for(i = 0; i < n; i++) arr[i] = i;
    array_qsort(arr, n, sizeof(arr[0]), array_cmp2_size, NULL);
    TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_size, NULL));

    // shuffle the array first
    for(j = 0; j < 100; j++) {
      array_shuffle(arr, n, sizeof(arr[0]));
      array_qsort(arr, n, sizeof(arr[0]), array_cmp2_size, NULL);
      TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_size, NULL));
    }
  }

  #undef N
}

// check partition around pivot
bool check_qpart(size_t *arr, size_t n, size_t pidx)
{
  size_t i = 0;
  for(i = 0; i < pidx; i++)
    if(arr[i] > arr[pidx]) return false;
  for(i = pidx+1; i < n; i++)
    if(arr[i] < arr[pidx]) return false;
  return true;
}

void test_quickpartition()
{
  status("Testing quickpartition...");

  #define N 100
  size_t i, n, arr[N], p;
  for(i = 0; i < N; i++) arr[i] = i;

  for(n = 0; n <= N; n++) {
    array_shuffle(arr, n, sizeof(arr[0]));
    for(i = 0; i < n; i++) {
      p = arr[i];
      carrays_swapm(&arr[0], &arr[i], sizeof(arr[0]));
      array_qpart(arr, n, sizeof(arr[0]), array_cmp2_size, NULL);
      TASSERT(arr[p] == p);
      TASSERT(check_qpart(arr, n, p));
    }
  }
  #undef N
}

void test_quickselect()
{
  status("Testing quickselect...");

  #define N 10
  size_t i, n, arr[N], pos[N];
  for(i = 0; i < N; i++) pos[i] = i;

  for(n = 0; n <= N; n++)
  {
    // Create shuffled array of positions
    array_shuffle(pos, n, sizeof(pos[0]));
    // check all positions are <n (this is a test of array_shuffle())
    for(i = 0; i < n && pos[i] < n; i++) {}
    TASSERT(i == n);

    // Initialise array to 0..n-1
    for(i = 0; i < n; i++) arr[i] = i;

    // Check we can select each index without changing the array
    for(i = 0; i < n; i++) {
      array_qselect(arr, n, sizeof(arr[0]), i, array_cmp2_size, NULL);
      TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_size, NULL));
    }

    // shuffle and check
    for(i = 0; i < n; i++) {
      array_qselect(arr, n, sizeof(arr[0]), pos[i], array_cmp2_size, NULL);
      TASSERT(check_qpart(arr, n, pos[i]));
    }
    // Should now be sorted after pivoting on each index
    TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_size, NULL));
  }

  #undef N
}

void test_heapsort()
{
  status("Testing heapsort...");

  #define N 200
  int i, j, n, arr[N];

  // Try different array lengths
  for(n = 0; n <= N; n++)
  {
    // start with sorted array
    for(j = 0; j < n; j++) arr[j] = j;
    array_heap_make(arr, n, sizeof(arr[0]), array_cmp2_int, NULL);
    array_heap_sort(arr, n, sizeof(arr[0]), array_cmp2_int, NULL);
    TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_int, NULL));

    // start with reverse sorted array
    for(j = 0; j < n; j++) arr[j] = n-1-j;
    array_heap_make(arr, n, sizeof(arr[0]), array_cmp2_int, NULL);
    array_heap_sort(arr, n, sizeof(arr[0]), array_cmp2_int, NULL);
    TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_int, NULL));

    // start with a shuffled array
    for(i = 0; i < 10; i++) {
      array_shuffle(arr, n, sizeof(arr[0]));
      array_heap_make(arr, n, sizeof(arr[0]), array_cmp2_int, NULL);
      array_heap_sort(arr, n, sizeof(arr[0]), array_cmp2_int, NULL);
      TASSERT(array_is_sorted(arr, n, sizeof(arr[0]), array_cmp2_int, NULL));
    }
  }
  #undef N
}


#define arrset5(x,a,b,c,d,e) do { x[0]=(a);x[1]=(b);x[2]=(c);x[3]=(d);x[4]=(e); }while(0)

static inline void check_median(size_t *arr, size_t ans)
{
  size_t i, tmp;
  char *ptr = array_median5(&arr[0], &arr[1], &arr[2], &arr[3], &arr[4],
                            array_cmp2_size, NULL);
  memcpy(&tmp, ptr, sizeof(tmp));
  TASSERT(tmp == ans);
  if(tmp != ans) {
    for(i = 0; i < 5; i++) printf(" %zu", arr[i]);
    printf("\n");
  }
}


void test_median5()
{
  size_t arr[5];
  size_t i, m = 2, *itr = NULL;

  // 5*4*3*2*1 = 120 perumations
  for(i = 0; carray_itr_next(&itr, 5); i++)
    check_median(itr, m);

  TASSERT(i == 5*4*3*2*1);
  free(itr);

  for(i = 0; i < 10; i++) {
    arrset5(arr, i, i, i, i, i);
    check_median(arr, i);
  }

  arrset5(arr, 10, 0, 10,  0, 10); check_median(arr, 10);
  arrset5(arr, 1,  9,  9, 10, 10); check_median(arr, 9);
  arrset5(arr, 10, 10, 9,  8,  7); check_median(arr, 9);
}

void check_permutation5(size_t **pp, size_t a, size_t b, size_t c, size_t d, size_t e)
{
  size_t *p = carray_itr_next(pp, 5);
  TASSERT(p != NULL);
  TASSERT(p[0] == a && p[1] == b && p[2] == c && p[3] == d && p[4] == e);
}

void test_next_permutation()
{
  status("Testing next permutation...");

  size_t i, n;
  size_t *p = NULL;

  check_permutation5(&p,0,1,2,3,4);
  check_permutation5(&p,0,1,2,4,3);
  check_permutation5(&p,0,1,3,2,4);
  check_permutation5(&p,0,1,3,4,2);
  check_permutation5(&p,0,1,4,2,3);
  check_permutation5(&p,0,1,4,3,2);

  check_permutation5(&p,0,2,1,3,4);
  check_permutation5(&p,0,2,1,4,3);
  check_permutation5(&p,0,2,3,1,4);
  check_permutation5(&p,0,2,3,4,1);
  check_permutation5(&p,0,2,4,1,3);
  check_permutation5(&p,0,2,4,3,1);

  check_permutation5(&p,0,3,1,2,4);
  check_permutation5(&p,0,3,1,4,2);
  check_permutation5(&p,0,3,2,1,4);
  check_permutation5(&p,0,3,2,4,1);
  check_permutation5(&p,0,3,4,1,2);
  check_permutation5(&p,0,3,4,2,1);

  check_permutation5(&p,0,4,1,2,3);
  check_permutation5(&p,0,4,1,3,2);
  check_permutation5(&p,0,4,2,1,3);
  check_permutation5(&p,0,4,2,3,1);
  check_permutation5(&p,0,4,3,1,2);
  check_permutation5(&p,0,4,3,2,1);

  //
  check_permutation5(&p,1,0,2,3,4);
  check_permutation5(&p,1,0,2,4,3);
  check_permutation5(&p,1,0,3,2,4);
  check_permutation5(&p,1,0,3,4,2);
  check_permutation5(&p,1,0,4,2,3);
  check_permutation5(&p,1,0,4,3,2);

  check_permutation5(&p,1,2,0,3,4);
  check_permutation5(&p,1,2,0,4,3);
  check_permutation5(&p,1,2,3,0,4);
  check_permutation5(&p,1,2,3,4,0);
  check_permutation5(&p,1,2,4,0,3);
  check_permutation5(&p,1,2,4,3,0);

  check_permutation5(&p,1,3,0,2,4);
  check_permutation5(&p,1,3,0,4,2);
  check_permutation5(&p,1,3,2,0,4);
  check_permutation5(&p,1,3,2,4,0);
  check_permutation5(&p,1,3,4,0,2);
  check_permutation5(&p,1,3,4,2,0);

  check_permutation5(&p,1,4,0,2,3);
  check_permutation5(&p,1,4,0,3,2);
  check_permutation5(&p,1,4,2,0,3);
  check_permutation5(&p,1,4,2,3,0);
  check_permutation5(&p,1,4,3,0,2);
  check_permutation5(&p,1,4,3,2,0);

  //
  check_permutation5(&p,2,0,1,3,4);
  check_permutation5(&p,2,0,1,4,3);
  check_permutation5(&p,2,0,3,1,4);
  check_permutation5(&p,2,0,3,4,1);
  check_permutation5(&p,2,0,4,1,3);
  check_permutation5(&p,2,0,4,3,1);

  check_permutation5(&p,2,1,0,3,4);
  check_permutation5(&p,2,1,0,4,3);
  check_permutation5(&p,2,1,3,0,4);
  check_permutation5(&p,2,1,3,4,0);
  check_permutation5(&p,2,1,4,0,3);
  check_permutation5(&p,2,1,4,3,0);

  check_permutation5(&p,2,3,0,1,4);
  check_permutation5(&p,2,3,0,4,1);
  check_permutation5(&p,2,3,1,0,4);
  check_permutation5(&p,2,3,1,4,0);
  check_permutation5(&p,2,3,4,0,1);
  check_permutation5(&p,2,3,4,1,0);

  check_permutation5(&p,2,4,0,1,3);
  check_permutation5(&p,2,4,0,3,1);
  check_permutation5(&p,2,4,1,0,3);
  check_permutation5(&p,2,4,1,3,0);
  check_permutation5(&p,2,4,3,0,1);
  check_permutation5(&p,2,4,3,1,0);

  //
  check_permutation5(&p,3,0,1,2,4);
  check_permutation5(&p,3,0,1,4,2);
  check_permutation5(&p,3,0,2,1,4);
  check_permutation5(&p,3,0,2,4,1);
  check_permutation5(&p,3,0,4,1,2);
  check_permutation5(&p,3,0,4,2,1);

  check_permutation5(&p,3,1,0,2,4);
  check_permutation5(&p,3,1,0,4,2);
  check_permutation5(&p,3,1,2,0,4);
  check_permutation5(&p,3,1,2,4,0);
  check_permutation5(&p,3,1,4,0,2);
  check_permutation5(&p,3,1,4,2,0);

  check_permutation5(&p,3,2,0,1,4);
  check_permutation5(&p,3,2,0,4,1);
  check_permutation5(&p,3,2,1,0,4);
  check_permutation5(&p,3,2,1,4,0);
  check_permutation5(&p,3,2,4,0,1);
  check_permutation5(&p,3,2,4,1,0);

  check_permutation5(&p,3,4,0,1,2);
  check_permutation5(&p,3,4,0,2,1);
  check_permutation5(&p,3,4,1,0,2);
  check_permutation5(&p,3,4,1,2,0);
  check_permutation5(&p,3,4,2,0,1);
  check_permutation5(&p,3,4,2,1,0);

  //
  check_permutation5(&p,4,0,1,2,3);
  check_permutation5(&p,4,0,1,3,2);
  check_permutation5(&p,4,0,2,1,3);
  check_permutation5(&p,4,0,2,3,1);
  check_permutation5(&p,4,0,3,1,2);
  check_permutation5(&p,4,0,3,2,1);

  check_permutation5(&p,4,1,0,2,3);
  check_permutation5(&p,4,1,0,3,2);
  check_permutation5(&p,4,1,2,0,3);
  check_permutation5(&p,4,1,2,3,0);
  check_permutation5(&p,4,1,3,0,2);
  check_permutation5(&p,4,1,3,2,0);

  check_permutation5(&p,4,2,0,1,3);
  check_permutation5(&p,4,2,0,3,1);
  check_permutation5(&p,4,2,1,0,3);
  check_permutation5(&p,4,2,1,3,0);
  check_permutation5(&p,4,2,3,0,1);
  check_permutation5(&p,4,2,3,1,0);

  check_permutation5(&p,4,3,0,1,2);
  check_permutation5(&p,4,3,0,2,1);
  check_permutation5(&p,4,3,1,0,2);
  check_permutation5(&p,4,3,1,2,0);
  check_permutation5(&p,4,3,2,0,1);
  check_permutation5(&p,4,3,2,1,0);

  TASSERT(carray_itr_next(&p, 5) == NULL);
  TASSERT(carray_itr_next(&p, 5) == NULL);
  TASSERT(carray_itr_next(&p, 5) == NULL);

  TASSERT(carray_itr_reset(p, 5) == p && p != NULL);
  check_permutation5(&p,0,1,2,3,4);
  check_permutation5(&p,0,1,2,4,3);

  free(p);

  p = NULL;
  TASSERT(carray_itr_next(&p, 2) == p);
  TASSERT(p[0] == 0 && p[1] == 1);
  TASSERT(carray_itr_next(&p, 2) == p);
  TASSERT(p[0] == 1 && p[1] == 0);
  TASSERT(carray_itr_next(&p, 2) == NULL);
  TASSERT(carray_itr_next(&p, 2) == NULL);

  p = carray_itr_reset(p, 2);
  TASSERT(carray_itr_next(&p, 2) == p);
  TASSERT(p[0] == 0 && p[1] == 1);
  TASSERT(carray_itr_next(&p, 2) == p);
  TASSERT(p[0] == 1 && p[1] == 0);
  free(p);

  // Test init various lengths
  for(n = 0; n < 5; n++)
  {
    p = carray_itr_reset(NULL, n);
    TASSERT(carray_itr_next(&p, n) == (n ? p : NULL));
    for(i = 0; i < n; i++) TASSERT(p[i] == i);
    // reuse
    p = carray_itr_reset(p, n);
    TASSERT(carray_itr_next(&p, n) == (n ? p : NULL));
    for(i = 0; i < n; i++) TASSERT(p[i] == i);
    free(p);

    // initialise without carray_itr_reset()
    p = NULL;
    TASSERT(carray_itr_next(&p, n) == (n ? p : NULL));
    for(i = 0; i < n; i++) TASSERT(p[i] == i);
    free(p);
  }
}

int main()
{
  status("Running tests...");
  test_round();
  test_GCD();
  test_array_cycle();
  test_reverse();
  test_bsearch();
  test_quicksort();
  test_quickpartition();
  test_quickselect();
  test_heapsort();
  test_median5();
  test_next_permutation();
  status("Passed: %zu / %zu (%s)", num_tests_run-num_tests_failed, num_tests_run,
         !num_tests_failed ? "all" : (num_tests_failed<num_tests_run ? "some" : "none"));
  status("Done.");

  return num_tests_failed ? -1 : 0;
}
