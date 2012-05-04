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

#include <sys/types.h>
#include <stdlib.h>
#include "trema.h"
#include "heap.h"


static void move_up( heap_t *heap, int index );
static void move_down( heap_t *heap, int index );
static void set_index( heap_t *heap, void *value, int index );
static int parent_of( int c_index );
static int min_child_of( heap_t *heap, int p_index );
static void swap_in_heap( heap_t *heap, int i, int j );


heap_t *
create_heap( const compare_function compare, const set_index_function set_index, const size_t capacity ) {
  heap_t *heap = ( heap_t * )malloc( sizeof( heap_t ) );
  if ( heap == NULL ) {
    return NULL;
  }

  heap->capacity = capacity;
  heap->size = 0;
  heap->elements = ( void ** )malloc( sizeof( void * ) * capacity );
  heap->compare = compare;
  heap->set_index = set_index;

  if ( heap->elements == NULL ) {
    free( heap );
    return NULL;
  }

  return heap;
}


void 
destroy_heap( heap_t *heap ) {
  if ( heap == NULL ) {
    return;
  }

  free( heap->elements );
  free( heap );
  return;
}


bool
push_to_heap( heap_t *heap, void *value ) {
  if ( heap == NULL || value == NULL ) {
    return false;
  }

  size_t size = heap->size;
  if ( size == heap->capacity ) {
    return false;
  }

  heap->elements[ size ] = value;
  heap->size++;

  ( *heap->set_index )( value, ( int )size );
  move_up( heap, ( int )size );
  
  return true;  
}


void *
pop_from_heap( heap_t *heap ) {
  return remove_from_heap( heap, 0 );
}


void *
remove_from_heap( heap_t *heap, int index ) {
  if ( index < 0 || heap->size <= ( size_t )index ) {
    return NULL;
  }

  void *value = heap->elements[ index ];
  heap->size--;

  if ( ( size_t )index != heap->size ) {
    heap->elements[ index ] = heap->elements[ heap->size ];
    set_index( heap, heap->elements[ index ], index );
    move_down( heap, index );    
  }

  heap->elements[ heap->size ] = NULL;
  set_index( heap, value, -1 );

  return value;
}


bool
check_heap( heap_t *heap ) {
  int c = ( int )heap->size - 1;

  for ( ; c > 0; c-- ) {
    int p = ( c - 1 ) / 2;

    void *parent = heap->elements[ p ];
    void *child = heap->elements[ c ];

    if ( ( *heap->compare )( parent, child ) ) {
      return false;
    }
  }

  return true;
}


static void 
move_up( heap_t *heap, int index ) {
  int c_index = index;

  for (;;) {
    int p_index = parent_of( c_index );
    if ( p_index < 0 ) return;

    void *child = heap->elements[ c_index ];
    void *parent = heap->elements[ p_index ];

    if ( ( *heap->compare )( parent, child ) == false ) {
      break;
    }

    swap_in_heap( heap, p_index, c_index );
    c_index = p_index;
  }
}


static void 
move_down( heap_t *heap, int index ) {
  int p_index = index;
  
  for (;;) {
    int c_index = min_child_of( heap, p_index );
    if ( c_index < 0 ) return;

    void *child = heap->elements[ c_index ];
    void *parent = heap->elements[ p_index ];
    
    if ( ( *heap->compare )( parent, child ) <= 0 ) {
      break;
    }

    swap_in_heap( heap, p_index, c_index );
    c_index = p_index;
  }
}


static void 
set_index( heap_t *heap, void *value, int index ) {
  if ( heap->set_index != NULL ) {
    ( *heap->set_index )( value, index );
  }
}


static int 
parent_of( int c_index ) {
  if ( c_index <= 0 ) {
    return -1;
  } 
  
  return ( c_index - 1 ) / 2;
}


static int 
min_child_of( heap_t *heap, int p_index ) {
  if ( p_index < 0 ) {
    return -1;
  }
 
  int l_index = p_index * 2 + 1;
  int r_index = l_index + 1;
  if ( l_index < 0 ) {
    return -1;
  }

  if ( ( size_t )l_index >= heap->size ) {
    return -1;
  } 

  if ( ( size_t )r_index >= heap->size ) {
    return l_index;
  }

  void *left = heap->elements[ l_index ];
  void *right = heap->elements[ r_index ];

  if ( ( *heap->compare )( left, right ) <= 0 ) {
    return l_index;
  } else {
    return r_index;
  }
}


static void
swap_in_heap( heap_t *heap, int i, int j ) {
  void *temp = heap->elements[ i ];
  heap->elements[ i ] = heap->elements[ j ];
  heap->elements[ j ] = temp;

  set_index( heap, heap->elements[ i ], i );
  set_index( heap, heap->elements[ j ], j );
}

