#include "trema.h"
#include "pathresolver.h"

void
test1() {
  printf( "%s starts.\n", __func__ );

  topology_cache_t *cache = create_topology_cache();

  add_node_to_cache( cache, 0x1, NULL );
  add_node_to_cache( cache, 0x2, NULL );
  add_node_to_cache( cache, 0x3, NULL );

  add_link_to_cache( cache, 0x1, 0x1, 0x1, 0x2, 0x1, NULL );
  add_link_to_cache( cache, 0x2, 0x2, 0x2, 0x1, 0x2, NULL );
  add_link_to_cache( cache, 0x3, 0x2, 0x3, 0x3, 0x1, NULL );
  add_link_to_cache( cache, 0x4, 0x3, 0x2, 0x2, 0x4, NULL );

  tree_t *tree = create_tree( cache, 0x1, NULL );
  list_element *path = resolve_path_from_tree( tree, 0x3 );

  list_element *current;
  for ( current = path; current != NULL; current = current->next ) {
    hop_t *hop = current->data;
    printf( "dpid = 0x%lx, port = 0x%x\n", hop->datapath_id, hop->out_port );
  }

  destroy_path( path );
  destroy_tree( tree );
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
  add_node_to_cache( cache, 0x4, NULL );
  add_node_to_cache( cache, 0x5, NULL );
  add_node_to_cache( cache, 0x6, NULL );

  add_link_to_cache( cache, 0x1, 0x1, 0x1, 0x2, 0x1, NULL );
  add_link_to_cache( cache, 0x2, 0x1, 0x2, 0x3, 0x1, NULL );
  add_link_to_cache( cache, 0x3, 0x2, 0x2, 0x4, 0x1, NULL );
  add_link_to_cache( cache, 0x4, 0x2, 0x3, 0x5, 0x1, NULL );
  add_link_to_cache( cache, 0x5, 0x3, 0x2, 0x4, 0x2, NULL );
  add_link_to_cache( cache, 0x6, 0x3, 0x3, 0x5, 0x2, NULL );
  add_link_to_cache( cache, 0x7, 0x4, 0x3, 0x6, 0x1, NULL );
  add_link_to_cache( cache, 0x8, 0x5, 0x3, 0x6, 0x2, NULL );

  tree_t *tree = create_tree( cache, 0x1, NULL );
  list_element *path = resolve_path_from_tree( tree, 0x6 );

  list_element *current;
  for ( current = path; current != NULL; current = current->next ) {
    hop_t *hop = current->data;
    printf( "dpid = 0x%lx, port = 0x%x\n", hop->datapath_id, hop->out_port );
  }

  destroy_path( path );
  destroy_tree( tree );
  destroy_topology_cache( cache );

  printf( "%s ends.\n", __func__ );
}

int
main() {
  test1();
  test2();
}
