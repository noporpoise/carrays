#ifndef CIRC_ARRAY_H_
#define CIRC_ARRAY_H_

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "carrays.h"

//
// Generic circular array
//

typedef struct
{
  const size_t el; // element size in bytes
  size_t start, n, size, mask; // n<=size, mask=size-1
  void *b;
} CircArray;

static inline void circa_alloc(CircArray *l, size_t el, size_t size) __attribute__((unused));
static inline void circa_dealloc(CircArray *l) __attribute__((unused));
static inline void circa_capacity(CircArray *l, size_t s) __attribute__((unused));
static inline void* circa_push(CircArray *l) __attribute__((unused));
static inline void* circa_pop(CircArray *l) __attribute__((unused));
static inline void* circa_unshift(CircArray *l) __attribute__((unused));
static inline void* circa_shift(CircArray *l) __attribute__((unused));

static inline void circa_alloc(CircArray *l, size_t el, size_t size)
{
  size = roundup64(size);
  CircArray tmp = {.el = el, .start = 0, .n = 0,
                .size = size, .mask = size-1,
                .b = malloc(size * el)};
  memcpy(l, &tmp, sizeof(CircArray));
}

static inline void circa_dealloc(CircArray *l)
{
  free(l->b);
}

static inline void circa_resize(CircArray *l, size_t size)
{
  // resize is only for growing array and new size must be a power of two
  assert(size > l->size && (size & (size-1)) == 0);

  l->b = realloc(l->b, l->el*size);
  if(l->start + l->n > l->size) {
    // nend is the num items at the end of the b, nbeg is at the beginning
    size_t nend = l->size-l->start, nbeg = (l->start+l->n) & l->mask;
    if(nend < nbeg) memmove(l->b+size-nend, l->b+l->size-nend, l->el*nend);
    else            memmove(l->b+l->size, l->b, l->el*nbeg);
  }
  l->size = size;
  l->mask = size-1;
}

static inline void circa_capacity(CircArray *l, size_t size)
{
  if(size > l->size) circa_resize(l, roundup64(size));
}

#define circa_get(l,idx) ((l)->b[(l)->el * (((l)->start + (idx)) & (l)->mask)])

// Add to start
// Returns a pointer to the item added (zero'd)
static inline void* circa_push(CircArray *l)
{
  if(l->n == l->size) circa_resize(l, l->size*2);
  l->start = l->start ? l->start-1 : l->size-1;
  l->n++;
  void *ptr = &circa_get(l, 0);
  memset(ptr, 0, l->el);
  return ptr;
}

// Remove from start
// Returns a pointer to the item removed
static inline void* circa_pop(CircArray *l)
{
  assert(l->n > 0);
  size_t old = l->start;
  l->start = (l->start+1) & l->mask;
  l->n--;
  return l->b + l->el*old;
}

// Add to end
// Returns a pointer to the item added (zero'd)
static inline void* circa_unshift(CircArray *l)
{
  if(l->n == l->size) circa_resize(l, l->size*2);
  void *ptr = &circa_get(l, l->n);
  memset(ptr, 0, l->el);
  l->n++;
  return ptr;
}

// Remove from end
// Returns a pointer to the item removed
static inline void* circa_shift(CircArray *l)
{
  assert(l->n > 0);
  void *ptr = &circa_get(l, l->n);
  l->n--;
  return ptr;
}

#endif /* CIRC_ARRAY_H_ */
