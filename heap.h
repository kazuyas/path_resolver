/*
 * Author: Kazuya Suzuki
 *
 * Copyright (C) 2012 NEC Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef HEAP_H
#define HEAP_H


#include <sys/types.h>
#include "bool.h"


typedef int ( *compare_heap_function )( const void *x, const void *y );


typedef struct {
  void **elements;
  size_t size;
  size_t element_num;
  compare_heap_function compare;
} heap_t;


heap_t *create_heap( const compare_heap_function compare, const size_t size );
void destroy_heap( heap_t *heap );
bool push_to_heap( heap_t *heap, void *value );
void *pop_from_heap( heap_t *heap );
void *remove_from_heap( heap_t *heap, int index );
bool check_heap( heap_t *heap );


#endif // HEAP_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
