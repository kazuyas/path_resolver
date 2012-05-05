#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include "trema.h"
#include "pathresolver.h"
#include "heap.h"


static size_t default_heap_size = 1000;

void
test1() {
  heap_t *heap = create_heap( compare_heap_node, default_heap_size );

  if ( heap != NULL ) {
    return;
  } else {
    return;
  }
}

void
test2() {
  heap_t *heap = create_heap( compare_heap_uint8, default_heap_size );
  uint8_t val1 = 100;
  uint8_t val2 = 22;

  push_to_heap( heap, &val1 );
  push_to_heap( heap, &val2 );

  uint8_t *ptr = pop_from_heap( heap );
  if ( *ptr != val2 ) {
    printf( "Failed. (%d != %d) \n", *ptr, val2 );
    abort();
  }

  ptr = pop_from_heap( heap );
  if ( *ptr != val1 ) {
    printf( "Failed. (%d != %d) \n", *ptr, val1 );
    abort();
  }

  if ( check_heap( heap ) ) {
    printf( "%s : Success.\n", __func__ );
  } else {
    printf( "%s : Failed.\n", __func__ );
    abort();
  }
}


void
test3() {
  heap_t *heap = create_heap( compare_heap_uint8, default_heap_size );

  srandom(100);

  for ( unsigned int i = 0; i < default_heap_size; i++ ) {
    uint8_t *val = ( uint8_t * )malloc( sizeof( uint8_t ) );

    *val = ( uint8_t )( random() % UCHAR_MAX );
    push_to_heap( heap, val );
  }

#if 0
  if ( check_heap( heap ) ) {
    printf( "%s : Success.\n", __func__ );
  } else {
    printf( "%s : Failed.\n", __func__ );
    abort();
  }
#endif

  uint8_t prev = 0;
  for ( unsigned int i = 0; i < default_heap_size; i++ ) {
    uint8_t *val = pop_from_heap( heap );
    printf( "%d\n", *val );
    if ( prev > *val ) {
      printf( "Failed.\n" );
      abort();
    }
    prev = *val;
    free( val );
  }

  return;
}

void
test4() {
  heap_t *heap = create_heap( compare_heap_node, default_heap_size );

  srandom(100);

  for ( int i = 0; i < 1000; i++ ) {
    node_t *node = ( node_t * )malloc( sizeof( node_t ) );

    node->datapath_id = ( uint64_t )random();
    push_to_heap( heap, node );
  }

#if 0
  if ( check_heap( heap ) ) {
    printf( "Success.\n" );
  } else {
    printf( "Failed.\n" );
    abort();
  }
#endif

  uint64_t prev = 0;
  for ( int i = 0; i < 1000; i++ ) {
    node_t *node = pop_from_heap( heap );
    printf( "%ld\n", node->datapath_id );
    if ( prev > node->datapath_id ) {
      printf( "Failed.\n" );
      abort();
    }
    prev = node->datapath_id;
    free( node );
  }

  return;
}

int
main() {
  test1();
  test2();
  test3();
  test4();
}
