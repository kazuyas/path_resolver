#include <sys/time.h>
#include "trema.h"
#include "pathresolver.h"

void
test1() {
  printf( "%s starts.\n", __func__ );

  topology_cache_t *cache = create_topology_cache();

  printf( "Adding 3 nodes into topology cache.\n" );
  add_node_to_cache( cache, 0x1, NULL );
  add_node_to_cache( cache, 0x2, NULL );
  add_node_to_cache( cache, 0x3, NULL );
  printf( "A number of nodes : %zu, %u\n", cache->node_num, 
	  cache->node_table->length );

  printf( "Adding 4 links into topology cache.\n" );
  add_link_to_cache( cache, 0x1, 0x1, 0x1, 0x2, 0x1, NULL );
  add_link_to_cache( cache, 0x2, 0x2, 0x2, 0x1, 0x2, NULL );
  add_link_to_cache( cache, 0x3, 0x2, 0x3, 0x3, 0x1, NULL );
  add_link_to_cache( cache, 0x4, 0x3, 0x2, 0x2, 0x4, NULL );
  printf( "A number of links : %zu, %u, %u\n", cache->link_num, 
	  cache->link_table_by_id->length, cache->link_table_by_ends->length );

  printf( "Deleting all links from topology cache.\n" );
  del_link_from_cache( cache, 0x1 );
  del_link_from_cache( cache, 0x2 );
  del_link_from_cache( cache, 0x3 );
  del_link_from_cache( cache, 0x4 );
  printf( "A number of links : %zu, %u, %u\n", cache->link_num, 
	  cache->link_table_by_id->length, cache->link_table_by_ends->length );

  printf( "Deleting all nodes from topology cache.\n" );
  del_node_from_cache( cache, 0x1 );
  del_node_from_cache( cache, 0x2 );
  del_node_from_cache( cache, 0x3 );
  printf( "A number of nodes : %zu, %u\n", cache->node_num, 
	  cache->node_table->length );

  destroy_topology_cache( cache );
  printf( "%s ends.\n", __func__ );
}

void
test2() {
  printf( "%s starts.\n", __func__ );

  topology_cache_t *cache = create_topology_cache();

  add_node_to_cache( cache, 0x1, NULL );
  add_node_to_cache( cache, 0x2, NULL );
  add_node_to_cache( cache, 0x3, NULL );

  if ( add_link_to_cache( cache, get_link_id( cache ), 0x1, 0x1, 0x2, 0x1, NULL ) == NULL ) {
      printf( "1: NULL\n" );
  } else {
      printf( "1: not NULL\n" );
  }
  if ( add_link_to_cache( cache, get_link_id( cache ), 0x1, 0x1, 0x2, 0x1, NULL ) == NULL ) {
      printf( "2: NULL\n" );
  } else {
      printf( "2: not NULL\n" );
  }

  destroy_topology_cache( cache );
  printf( "%s ends.\n", __func__ );
}

#if 0
void
test0() {
  printf( "%s starts.\n", __func__ );
  printf( "%s ends.\n", __func__ );
}
#endif

int
main() {
  test1();
  test2();
}
