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


typedef struct {
  void ( *set_index )( void *value, int index );
  int ( *compare )( void *value1, void *value2 );
} heap_ops_t;


typedef struct {
  void **elements;
  size_t capacity;
  size_t size;
  heap_ops_t *ops;
} heap_t;


heap_t *create_heap( size_t capacity, heap_ops_t *ops );
void destroy_heap( heap_t *heap );
bool push_to_heap( heap_t *heap, void *value );
void *pop_from_heap( heap_t *heap );
void *remove_from_heap( heap_t *heap, int index );


#endif // HEAP_H


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
