#include <stdio.h>
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
  status("Testing carrays_calc_GCD()");

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
  status("Testing array_cycle_left() / array_cycle_right()");
  size_t n, arr[100];

  // Test all array lengths up to 100
  for(n = 0; n < 100; n++)
    _test_array_cycle(arr, n);
}

void test_reverse()
{
  status("Testing array_reverse()");
  size_t i, n, tmp[100];
  for(n = 0; n < 100; n++) {
    for(i = 0; i < n; i++) tmp[i] = i;
    array_reverse(tmp, n, sizeof(tmp[0]));
    for(i = 0; i < n && tmp[i] == n-i-1; i++) {}
    TASSERT(i == n);
  }
}


int _search_int(const void *aa, void *bb)
{
  int a = *(const int*)aa, b = *(int*)bb;
  return a < b ? -1 : (a > b);
}

void test_bsearch()
{
  status("Testing bsearch()");
  int i, n, find, arr[100];
  void *ptr;
  for(i = 0; i < 100; i++) arr[i] = i;

  find = 20;
  ptr = sarray_bsearch(arr, 100, sizeof(arr[0]), _search_int, &find);
  TASSERT(ptr == &arr[find]);

  for(n = 0; n < 100; n++) {
    for(find = -2; find <= n+2; find++) {
      ptr = sarray_bsearch(arr, n, sizeof(arr[0]), _search_int, &find);
      TASSERT(ptr == (find < 0 || find >= n ? NULL : &arr[find]));
    }
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
  status("Done.");

  return num_tests_failed ? -1 : 0;
}
