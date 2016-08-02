/*
 * $Id: arraylist.c,v 1.4 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include "config.h"

#include <limits.h>

#ifdef STDC_HEADERS
# include <stdlib.h>
# include <string.h>
#endif /* STDC_HEADERS */

#if defined(HAVE_STRINGS_H) && !defined(_STRING_H) && !defined(__USE_BSD)
# include <strings.h>
#endif /* HAVE_STRINGS_H */

#include "arraylist.h"

struct array_list*
array_list_new(array_list_free_fn *free_fn)
{
  struct array_list *arr;

  arr = (struct array_list*)calloc(1, sizeof(struct array_list));
  if(!arr) return NULL;
  arr->size = ARRAY_LIST_DEFAULT_SIZE;
  arr->length = 0;
  arr->free_fn = free_fn;
  if(!(arr->array = (void**)calloc(sizeof(void*), arr->size))) {
    free(arr);
    return NULL;
  }
  return arr;
}

extern void
array_list_free(struct array_list *arr)
{
  int i;
  for(i = 0; i < arr->length; i++)
    if(arr->array[i]) arr->free_fn(arr->array[i]);
  free(arr->array);
  free(arr);
}

void*
array_list_get_idx(struct array_list *arr, int i)
{
  if(i >= arr->length) return NULL;
  return arr->array[i];
}

static int array_list_expand_internal(struct array_list *arr, int max)
{
  void *t;
  int new_size;

  if(max < arr->size) return 0;
  /* Avoid undefined behaviour on int32 overflow */
  if( arr->size >= INT_MAX / 2 )
    new_size = max;
  else
  {
    new_size = arr->size << 1;
    if (new_size < max)
      new_size = max;
  }
  if((size_t)new_size > (~((size_t)0)) / sizeof(void*)) return -1;
  if(!(t = realloc(arr->array, ((size_t)new_size)*sizeof(void*)))) return -1;
  arr->array = (void**)t;
  (void)memset(arr->array + arr->size, 0, (new_size-arr->size)*sizeof(void*));
  arr->size = new_size;
  return 0;
}

int
array_list_put_idx(struct array_list *arr, int idx, void *data)
{
  if( idx < 0 || idx > INT_MAX - 1 ) return -1;
  if(array_list_expand_internal(arr, idx+1)) return -1;
  if(arr->array[idx]) arr->free_fn(arr->array[idx]);
  arr->array[idx] = data;
  if(arr->length <= idx) arr->length = idx + 1;
  return 0;
}

int
array_list_add(struct array_list *arr, void *data)
{
  return array_list_put_idx(arr, arr->length, data);
}

void
array_list_sort(struct array_list *arr, int(*sort_fn)(const void *, const void *))
{
  qsort(arr->array, arr->length, sizeof(arr->array[0]), sort_fn);
}

void* array_list_bsearch(const void **key, struct array_list *arr,
		int (*sort_fn)(const void *, const void *))
{
	return bsearch(key, arr->array, arr->length, sizeof(arr->array[0]),
			sort_fn);
}

int
array_list_length(struct array_list *arr)
{
  return arr->length;
}

int
array_list_del_idx( struct array_list *arr, int idx, int count )
{
	int i, stop;

	stop = idx + count;
	if ( idx >= arr->length || stop > arr->length ) return -1;
	for ( i = idx; i < stop; ++i ) {
		if ( arr->array[i] ) arr->free_fn( arr->array[i] );
	}
	memmove( arr->array + idx, arr->array + stop, (arr->length - stop) * sizeof(void*) );
	arr->length -= count;
	return 0;
}
