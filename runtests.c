#include <stdio.h>
#include "circ_array.h"
#include "carrays.h"

// seeding random
#include <sys/time.h> // for seeding random
#include <unistd.h> // getpid()

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

  TASSERT(gca_roundup32(0) == 0 && gca_roundup64(0) == 0);
  TASSERT(gca_roundup32(1) == 1 && gca_roundup64(1) == 1);
  TASSERT(gca_roundup32(2) == 2 && gca_roundup64(2) == 2);
  TASSERT(gca_roundup32(3) == 4 && gca_roundup64(3) == 4);
  TASSERT(gca_roundup32(4) == 4 && gca_roundup64(4) == 4);
  TASSERT(gca_roundup32(5) == 8 && gca_roundup64(5) == 8);
  TASSERT(gca_roundup32(6) == 8 && gca_roundup64(6) == 8);
  TASSERT(gca_roundup32(7) == 8 && gca_roundup64(7) == 8);
  TASSERT(gca_roundup32(8) == 8 && gca_roundup64(8) == 8);
  TASSERT(gca_roundup32(100) == 128 && gca_roundup64(100) == 128);

  for(i = 2; i < 32; i++) {
    TASSERT(gca_roundup32((1U<<i)-1) == (1U<<i));
    TASSERT(gca_roundup32(1U<<i) == (1U<<i));
  }

  for(i = 2; i < 64; i++) {
    TASSERT(gca_roundup64((1UL<<i)-1) == (1UL<<i));
    TASSERT(gca_roundup64(1UL<<i) == (1UL<<i));
  }
}

void test_GCD()
{
  status("Testing calculating GCD...");

  size_t i;
  for(i = 0; i < 100; i++) {
    TASSERT(gca_calc_GCD(i,i) == i);
    TASSERT(gca_calc_GCD(i,0) == i);
    TASSERT(gca_calc_GCD(0,i) == i);
    TASSERT(gca_calc_GCD(i,1) == 1);
    TASSERT(gca_calc_GCD(1,i) == 1);
  }

  TASSERT(gca_calc_GCD(2,4) == 2);
  TASSERT(gca_calc_GCD(4,2) == 2);
  TASSERT(gca_calc_GCD(6,9) == 3);
  TASSERT(gca_calc_GCD(9,6) == 3);
  TASSERT(gca_calc_GCD(0,0) == 0);
  TASSERT(gca_calc_GCD(10,0) == 10);
  TASSERT(gca_calc_GCD(0,10) == 10);
  TASSERT(gca_calc_GCD(2,2) == 2);
  TASSERT(gca_calc_GCD(1,1) == 1);
  TASSERT(gca_calc_GCD(1,2) == 1);
  TASSERT(gca_calc_GCD(1,100) == 1);
  TASSERT(gca_calc_GCD(2,4) == 2);
  TASSERT(gca_calc_GCD(7,5) == 1);
  TASSERT(gca_calc_GCD(18,6) == 6);
  TASSERT(gca_calc_GCD(3,6) == 3);
  TASSERT(gca_calc_GCD(100,120) == 20);
  TASSERT(gca_calc_GCD(100,125) == 25);
}

void _test_cycle(size_t *arr, size_t n)
{
  size_t shift, i;

  // initialise
  for(i = 0; i < n; i++) arr[i] = i;

  // cycle left by 0, n, 2n, ... should all return input
  gca_cycle_left(arr, n, sizeof(size_t), 0);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  gca_cycle_left(arr, n, sizeof(size_t), n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  gca_cycle_left(arr, n, sizeof(size_t), 2*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  gca_cycle_left(arr, n, sizeof(size_t), 3*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);

  // cycle right by 0, n, 2n, ... should all return input
  gca_cycle_right(arr, n, sizeof(size_t), 0);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  gca_cycle_right(arr, n, sizeof(size_t), n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  gca_cycle_right(arr, n, sizeof(size_t), 2*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  gca_cycle_right(arr, n, sizeof(size_t), 3*n);
  for(i = 0; i < n; i++) TASSERT(arr[i] == i);

  for(shift = 0; shift < n; shift++) {
    for(i = 0; i < n; i++) arr[i] = i;
    gca_cycle_left(arr, n, sizeof(size_t), shift);
    for(i = 0; i < n; i++) TASSERT(arr[i] == ((i+shift) % n));
    // shift back
    gca_cycle_right(arr, n, sizeof(size_t), shift);
    for(i = 0; i < n; i++) TASSERT(arr[i] == i);
  }
}

void test_cycle()
{
  status("Testing array cycle left/right...");
  size_t n, arr[100];

  // Test all array lengths up to 100
  for(n = 0; n < 100; n++)
    _test_cycle(arr, n);
}

void test_reverse()
{
  status("Testing array reverse...");

  #define N 100
  size_t i, n, tmp[N];
  for(n = 0; n <= N; n++) {
    for(i = 0; i < n; i++) tmp[i] = i;
    gca_reverse(tmp, n, sizeof(tmp[0]));
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
  ptr = gca_bsearch(arr, N, sizeof(arr[0]), gca_search_int, &find);
  TASSERT(ptr == &arr[find]);

  for(n = 0; n <= N; n++) {
    for(find = -2; find <= n+2; find++) {
      ptr = gca_bsearch(arr, n, sizeof(arr[0]), gca_search_int, &find);
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
    gca_qsort(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL);
    TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL));

    // start with sorted array
    for(i = 0; i < n; i++) arr[i] = i;
    gca_qsort(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL);
    TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL));

    // shuffle the array first
    for(j = 0; j < 100; j++) {
      gca_shuffle(arr, n, sizeof(arr[0]));
      gca_qsort(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL);
      TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL));
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
    gca_shuffle(arr, n, sizeof(arr[0]));
    for(i = 0; i < n; i++) {
      p = arr[i];
      gca_swapm(&arr[0], &arr[i], sizeof(arr[0]));
      gca_qpart(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL);
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
    gca_shuffle(pos, n, sizeof(pos[0]));
    // check all positions are <n (this is a test of gca_shuffle())
    for(i = 0; i < n && pos[i] < n; i++) {}
    TASSERT(i == n);

    // Initialise array to 0..n-1
    for(i = 0; i < n; i++) arr[i] = i;

    // Check we can select each index without changing the array
    for(i = 0; i < n; i++) {
      gca_qselect(arr, n, sizeof(arr[0]), i, gca_cmp2_size, NULL);
      TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL));
    }

    // shuffle and check
    for(i = 0; i < n; i++) {
      gca_qselect(arr, n, sizeof(arr[0]), pos[i], gca_cmp2_size, NULL);
      TASSERT(check_qpart(arr, n, pos[i]));
    }
    // Should now be sorted after pivoting on each index
    TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_size, NULL));
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
    gca_heap_make(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL);
    gca_heap_sort(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL);
    TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL));

    // start with reverse sorted array
    for(j = 0; j < n; j++) arr[j] = n-1-j;
    gca_heap_make(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL);
    gca_heap_sort(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL);
    TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL));

    // start with a shuffled array
    for(i = 0; i < 10; i++) {
      gca_shuffle(arr, n, sizeof(arr[0]));
      gca_heap_make(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL);
      gca_heap_sort(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL);
      TASSERT(gca_is_sorted(arr, n, sizeof(arr[0]), gca_cmp2_int, NULL));
    }
  }
  #undef N
}


#define arrset5(x,a,b,c,d,e) do { x[0]=(a);x[1]=(b);x[2]=(c);x[3]=(d);x[4]=(e); }while(0)

static inline void check_median(size_t *arr, size_t ans)
{
  size_t i, tmp;
  char *ptr = gca_median5(&arr[0], &arr[1], &arr[2], &arr[3], &arr[4],
                          gca_cmp2_size, NULL);
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
  for(i = 0; gca_itr_next(&itr, 5, NULL); i++)
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

// size_t avgfunc(size_t a, size_t b) {
//   status("a=%zu b=%zu", a, b);
//   return (a+b+1.0)/2.0;
// }

void test_median()
{
  #define avgfunc(a,b) (((a)+(b)+1.0)/2.0)
  #define N 10
  #define NTESTS 4
  size_t arr[N], i, t, n;

  arr[0] = 1; arr[1] = 5; arr[2] = 4;
  TASSERT(gca_median2(arr, 0, gca_cmp2_size, NULL, size_t, avgfunc, 0) == 0);
  TASSERT(gca_median2(arr, 1, gca_cmp2_size, NULL, size_t, avgfunc, 0) == 1);
  TASSERT(gca_median2(arr, 2, gca_cmp2_size, NULL, size_t, avgfunc, 0) == 3);
  TASSERT(gca_median2(arr, 3, gca_cmp2_size, NULL, size_t, avgfunc, 0) == 4);

  // 7, 7, ... 7, 7
  for(i = 0; i < N; i++) arr[i] = 7;
  for(n = 1; n <= N; n++)
    TASSERT(gca_median(arr, n, gca_cmp2_size, NULL, size_t, avgfunc) == 7);

  // 0, 1, ... 8, 9
  for(i = 0; i < N; i++) arr[i] = i;
  TASSERT(gca_median2(arr, 0, gca_cmp2_size, NULL, size_t, avgfunc, SIZE_MAX) == SIZE_MAX);
  TASSERT(gca_median2(arr, 0, gca_cmp2_size, NULL, size_t, avgfunc, 0) == 0);
  TASSERT(gca_median(arr, 1, gca_cmp2_size, NULL, size_t, avgfunc) == 0);
  TASSERT(gca_median(arr, 2, gca_cmp2_size, NULL, size_t, avgfunc) == 1);
  TASSERT(gca_median(arr, 3, gca_cmp2_size, NULL, size_t, avgfunc) == 1);
  TASSERT(gca_median(arr, 4, gca_cmp2_size, NULL, size_t, avgfunc) == 2);

  for(t = 0; t < NTESTS; t++)
  {
    // (4 + 5) / 2 = 9/2 = 4.5 => 5
    for(i = 0; i < N; i++) arr[i] = i;
    gca_shuffle(arr, N, sizeof(arr[0]));
    TASSERT(gca_median(arr, N, gca_cmp2_size, NULL, size_t, avgfunc) == 5);

    // 0, 2, ... 16, 18
    // (2*4 + 2*5) / 2 = (8+10) / 2 = 18/2 = 9
    for(i = 0; i < N; i++) arr[i] = 2*i;
    gca_shuffle(arr, N, sizeof(arr[0]));
    TASSERT(gca_median(arr, N, gca_cmp2_size, NULL, size_t, avgfunc) == 9);

    // 0, 3, ... 24, 27
    // (3*4 + 3*5) / 2 = (12+15) / 2 = 27/2 = 13.5 => 14
    for(i = 0; i < N; i++) arr[i] = 3*i;
    gca_shuffle(arr, N, sizeof(arr[0]));
    TASSERT(gca_median(arr, N, gca_cmp2_size, NULL, size_t, avgfunc) == 14);
  }
  #undef N
}


void check_permutation5(size_t **pp, size_t a, size_t b, size_t c, size_t d, size_t e,
                        size_t *init)
{
  size_t *p = gca_itr_next(pp, 5, init);
  TASSERT(p != NULL);
  bool nxtpermut = p[0] == a && p[1] == b && p[2] == c && p[3] == d && p[4] == e;
  TASSERT2(nxtpermut, "%zu %zu %zu %zu %zu / %zu %zu %zu %zu %zu\n",
           p[0], p[1], p[2], p[3], p[4], a, b, c, d, e);
}

void test_next_perm_with_dupes()
{
  status("Testing next permutation with duplicates...");

  size_t *p = NULL, init[] = {0,1,2,2,2};

  check_permutation5(&p,0,1,2,2,2, init);
  check_permutation5(&p,0,2,1,2,2, init);
  check_permutation5(&p,0,2,2,1,2, init);
  check_permutation5(&p,0,2,2,2,1, init);

  check_permutation5(&p,1,0,2,2,2, init);
  check_permutation5(&p,1,2,0,2,2, init);
  check_permutation5(&p,1,2,2,0,2, init);
  check_permutation5(&p,1,2,2,2,0, init);

  check_permutation5(&p,2,0,1,2,2, init);
  check_permutation5(&p,2,0,2,1,2, init);
  check_permutation5(&p,2,0,2,2,1, init);
  check_permutation5(&p,2,1,0,2,2, init);
  check_permutation5(&p,2,1,2,0,2, init);
  check_permutation5(&p,2,1,2,2,0, init);

  check_permutation5(&p,2,2,0,1,2, init);
  check_permutation5(&p,2,2,0,2,1, init);
  check_permutation5(&p,2,2,1,0,2, init);
  check_permutation5(&p,2,2,1,2,0, init);

  check_permutation5(&p,2,2,2,0,1, init);
  check_permutation5(&p,2,2,2,1,0, init);

  free(p);
}

void test_next_permutation()
{
  status("Testing next permutation...");

  size_t i, n;
  size_t *p = NULL;

  check_permutation5(&p,0,1,2,3,4,NULL);
  check_permutation5(&p,0,1,2,4,3,NULL);
  check_permutation5(&p,0,1,3,2,4,NULL);
  check_permutation5(&p,0,1,3,4,2,NULL);
  check_permutation5(&p,0,1,4,2,3,NULL);
  check_permutation5(&p,0,1,4,3,2,NULL);

  check_permutation5(&p,0,2,1,3,4,NULL);
  check_permutation5(&p,0,2,1,4,3,NULL);
  check_permutation5(&p,0,2,3,1,4,NULL);
  check_permutation5(&p,0,2,3,4,1,NULL);
  check_permutation5(&p,0,2,4,1,3,NULL);
  check_permutation5(&p,0,2,4,3,1,NULL);

  check_permutation5(&p,0,3,1,2,4,NULL);
  check_permutation5(&p,0,3,1,4,2,NULL);
  check_permutation5(&p,0,3,2,1,4,NULL);
  check_permutation5(&p,0,3,2,4,1,NULL);
  check_permutation5(&p,0,3,4,1,2,NULL);
  check_permutation5(&p,0,3,4,2,1,NULL);

  check_permutation5(&p,0,4,1,2,3,NULL);
  check_permutation5(&p,0,4,1,3,2,NULL);
  check_permutation5(&p,0,4,2,1,3,NULL);
  check_permutation5(&p,0,4,2,3,1,NULL);
  check_permutation5(&p,0,4,3,1,2,NULL);
  check_permutation5(&p,0,4,3,2,1,NULL);

  //
  check_permutation5(&p,1,0,2,3,4,NULL);
  check_permutation5(&p,1,0,2,4,3,NULL);
  check_permutation5(&p,1,0,3,2,4,NULL);
  check_permutation5(&p,1,0,3,4,2,NULL);
  check_permutation5(&p,1,0,4,2,3,NULL);
  check_permutation5(&p,1,0,4,3,2,NULL);

  check_permutation5(&p,1,2,0,3,4,NULL);
  check_permutation5(&p,1,2,0,4,3,NULL);
  check_permutation5(&p,1,2,3,0,4,NULL);
  check_permutation5(&p,1,2,3,4,0,NULL);
  check_permutation5(&p,1,2,4,0,3,NULL);
  check_permutation5(&p,1,2,4,3,0,NULL);

  check_permutation5(&p,1,3,0,2,4,NULL);
  check_permutation5(&p,1,3,0,4,2,NULL);
  check_permutation5(&p,1,3,2,0,4,NULL);
  check_permutation5(&p,1,3,2,4,0,NULL);
  check_permutation5(&p,1,3,4,0,2,NULL);
  check_permutation5(&p,1,3,4,2,0,NULL);

  check_permutation5(&p,1,4,0,2,3,NULL);
  check_permutation5(&p,1,4,0,3,2,NULL);
  check_permutation5(&p,1,4,2,0,3,NULL);
  check_permutation5(&p,1,4,2,3,0,NULL);
  check_permutation5(&p,1,4,3,0,2,NULL);
  check_permutation5(&p,1,4,3,2,0,NULL);

  //
  check_permutation5(&p,2,0,1,3,4,NULL);
  check_permutation5(&p,2,0,1,4,3,NULL);
  check_permutation5(&p,2,0,3,1,4,NULL);
  check_permutation5(&p,2,0,3,4,1,NULL);
  check_permutation5(&p,2,0,4,1,3,NULL);
  check_permutation5(&p,2,0,4,3,1,NULL);

  check_permutation5(&p,2,1,0,3,4,NULL);
  check_permutation5(&p,2,1,0,4,3,NULL);
  check_permutation5(&p,2,1,3,0,4,NULL);
  check_permutation5(&p,2,1,3,4,0,NULL);
  check_permutation5(&p,2,1,4,0,3,NULL);
  check_permutation5(&p,2,1,4,3,0,NULL);

  check_permutation5(&p,2,3,0,1,4,NULL);
  check_permutation5(&p,2,3,0,4,1,NULL);
  check_permutation5(&p,2,3,1,0,4,NULL);
  check_permutation5(&p,2,3,1,4,0,NULL);
  check_permutation5(&p,2,3,4,0,1,NULL);
  check_permutation5(&p,2,3,4,1,0,NULL);

  check_permutation5(&p,2,4,0,1,3,NULL);
  check_permutation5(&p,2,4,0,3,1,NULL);
  check_permutation5(&p,2,4,1,0,3,NULL);
  check_permutation5(&p,2,4,1,3,0,NULL);
  check_permutation5(&p,2,4,3,0,1,NULL);
  check_permutation5(&p,2,4,3,1,0,NULL);

  //
  check_permutation5(&p,3,0,1,2,4,NULL);
  check_permutation5(&p,3,0,1,4,2,NULL);
  check_permutation5(&p,3,0,2,1,4,NULL);
  check_permutation5(&p,3,0,2,4,1,NULL);
  check_permutation5(&p,3,0,4,1,2,NULL);
  check_permutation5(&p,3,0,4,2,1,NULL);

  check_permutation5(&p,3,1,0,2,4,NULL);
  check_permutation5(&p,3,1,0,4,2,NULL);
  check_permutation5(&p,3,1,2,0,4,NULL);
  check_permutation5(&p,3,1,2,4,0,NULL);
  check_permutation5(&p,3,1,4,0,2,NULL);
  check_permutation5(&p,3,1,4,2,0,NULL);

  check_permutation5(&p,3,2,0,1,4,NULL);
  check_permutation5(&p,3,2,0,4,1,NULL);
  check_permutation5(&p,3,2,1,0,4,NULL);
  check_permutation5(&p,3,2,1,4,0,NULL);
  check_permutation5(&p,3,2,4,0,1,NULL);
  check_permutation5(&p,3,2,4,1,0,NULL);

  check_permutation5(&p,3,4,0,1,2,NULL);
  check_permutation5(&p,3,4,0,2,1,NULL);
  check_permutation5(&p,3,4,1,0,2,NULL);
  check_permutation5(&p,3,4,1,2,0,NULL);
  check_permutation5(&p,3,4,2,0,1,NULL);
  check_permutation5(&p,3,4,2,1,0,NULL);

  //
  check_permutation5(&p,4,0,1,2,3,NULL);
  check_permutation5(&p,4,0,1,3,2,NULL);
  check_permutation5(&p,4,0,2,1,3,NULL);
  check_permutation5(&p,4,0,2,3,1,NULL);
  check_permutation5(&p,4,0,3,1,2,NULL);
  check_permutation5(&p,4,0,3,2,1,NULL);

  check_permutation5(&p,4,1,0,2,3,NULL);
  check_permutation5(&p,4,1,0,3,2,NULL);
  check_permutation5(&p,4,1,2,0,3,NULL);
  check_permutation5(&p,4,1,2,3,0,NULL);
  check_permutation5(&p,4,1,3,0,2,NULL);
  check_permutation5(&p,4,1,3,2,0,NULL);

  check_permutation5(&p,4,2,0,1,3,NULL);
  check_permutation5(&p,4,2,0,3,1,NULL);
  check_permutation5(&p,4,2,1,0,3,NULL);
  check_permutation5(&p,4,2,1,3,0,NULL);
  check_permutation5(&p,4,2,3,0,1,NULL);
  check_permutation5(&p,4,2,3,1,0,NULL);

  check_permutation5(&p,4,3,0,1,2,NULL);
  check_permutation5(&p,4,3,0,2,1,NULL);
  check_permutation5(&p,4,3,1,0,2,NULL);
  check_permutation5(&p,4,3,1,2,0,NULL);
  check_permutation5(&p,4,3,2,0,1,NULL);
  check_permutation5(&p,4,3,2,1,0,NULL);

  TASSERT(gca_itr_next(&p, 5, NULL) == NULL);
  TASSERT(gca_itr_next(&p, 5, NULL) == NULL);
  TASSERT(gca_itr_next(&p, 5, NULL) == NULL);

  TASSERT(gca_itr_reset(p, 5) == p && p != NULL);
  check_permutation5(&p,0,1,2,3,4,NULL);
  check_permutation5(&p,0,1,2,4,3,NULL);

  free(p);

  p = NULL;
  TASSERT(gca_itr_next(&p, 2, NULL) == p);
  TASSERT(p[0] == 0 && p[1] == 1);
  TASSERT(gca_itr_next(&p, 2, NULL) == p);
  TASSERT(p[0] == 1 && p[1] == 0);
  TASSERT(gca_itr_next(&p, 2, NULL) == NULL);
  TASSERT(gca_itr_next(&p, 2, NULL) == NULL);

  p = gca_itr_reset(p, 2);
  TASSERT(gca_itr_next(&p, 2, NULL) == p);
  TASSERT(p[0] == 0 && p[1] == 1);
  TASSERT(gca_itr_next(&p, 2, NULL) == p);
  TASSERT(p[0] == 1 && p[1] == 0);
  free(p);

  // Test init various lengths
  for(n = 0; n < 5; n++)
  {
    p = gca_itr_reset(NULL, n);
    TASSERT(gca_itr_next(&p, n, NULL) == (n ? p : NULL));
    for(i = 0; i < n; i++) TASSERT(p[i] == i);
    // reuse
    p = gca_itr_reset(p, n);
    TASSERT(gca_itr_next(&p, n, NULL) == (n ? p : NULL));
    for(i = 0; i < n; i++) TASSERT(p[i] == i);
    free(p);

    // initialise without gca_itr_reset()
    p = NULL;
    TASSERT(gca_itr_next(&p, n, NULL) == (n ? p : NULL));
    for(i = 0; i < n; i++) TASSERT(p[i] == i);
    free(p);
  }
}

#define strhash_fast_mix(h,x) ((h) * 37 + (x))
#define rotl32(h,r) ((h)<<(r)|(h)>>(32-(r)))

static inline uint32_t get_rand_seed()
{
  struct timeval now;
  gettimeofday(&now, NULL);

  uint32_t h = rand();
  h = strhash_fast_mix(h, rotl32((uint32_t)now.tv_sec,  h & 31));
  h = strhash_fast_mix(h, rotl32((uint32_t)now.tv_usec, h & 31));
  h = strhash_fast_mix(h, (uint32_t)getpid());
  return h;
}

int main()
{
  uint32_t seed32 = get_rand_seed();
  srand48(seed32);
  srand(seed32);
  status("Random seed = %u", seed32);

  status("Running tests...");
  test_round();
  test_GCD();
  test_cycle();
  test_reverse();
  test_bsearch();
  test_quicksort();
  test_quickpartition();
  test_quickselect();
  test_heapsort();
  test_median5();
  test_median();
  test_next_permutation();
  test_next_perm_with_dupes();
  status("Passed: %zu / %zu (%s)", num_tests_run-num_tests_failed, num_tests_run,
         !num_tests_failed ? "All" : (num_tests_failed<num_tests_run ? "Some" : "None"));
  status("Done.");

  return num_tests_failed ? -1 : 0;
}
