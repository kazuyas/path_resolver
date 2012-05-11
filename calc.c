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


#include <assert.h>
#include "trema.h"
#include "pathresolver.h"


static void calculate( tree_t *tree, const topology_cache_t *cache, const hash_table *costmap );
static void add_node_to_tree( tree_t *tree, node_t *treenode, node_t *node );
static void add_link_to_tree( tree_t *tree, link_t *treelink, link_t *link );


tree_t *
create_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap ) {
  die_if_NULL( cache );

  tree_t *tree = ( tree_t * )malloc( sizeof( tree_t ) );
  if ( tree == NULL ) {
    return NULL;
  }

  tree->root_dpid = root;
  tree->node_table = create_hash( compare_hash_node, hash_node );
  tree->link_table = create_hash( compare_hash_link, hash_link );
  tree->node = ( node_t * )malloc( sizeof( node_t ) * cache->node_num );
  tree->link = ( link_t * )malloc( sizeof( link_t ) * cache->node_num - 1 );
  tree->node_num = 0;
  tree->link_num = 0;

  calculate( tree, cache, costmap );

  return tree;
}


void
destroy_tree( tree_t *tree ) {
  die_if_NULL( tree );

  for ( unsigned int i; i < tree->node_num; i++ ) {
    delete_dlist( tree->node[ i ].in_links );
    delete_dlist( tree->node[ i ].out_links );
  }

  delete_hash( tree->node_table );
  delete_hash( tree->link_table );

  memset( tree->node, 0, sizeof( node_t ) * tree->node_num );
  free( tree->node );

  memset( tree->link, 0, sizeof( tree->link ) * tree->link_num );
  free( tree->link );

  memset( tree, 0, sizeof( tree_t ) );
  free( tree );
}


list_element *
resolve_path_from_tree( const tree_t *tree, const uint64_t to ) {
  list_element *path;
  create_list( &path );

  uint64_t current;
  for ( current = to; current != tree->root_dpid; ) {
    node_t *node = lookup_hash_entry( tree->node_table, &current );
    if ( node == NULL ) {
      return NULL;
    }

    dlist_element *dl_element = get_first_element( node->in_links );
    link_t *link = dl_element->data;

    hop_t *hop = ( hop_t * )malloc( sizeof( hop_t ) );
    hop->datapath_id = link->from;
    hop->out_port = link->from_port;

    append_to_tail( &path, hop );

    current = link->from;
  }

  return path;
}


void
destroy_path( list_element *path ) {
  list_element *curr;
  for ( curr = path; curr != NULL; curr = curr->next ) {
    free( curr->data );
  }
  delete_list( path );
}


static void
calculate( tree_t *tree, const topology_cache_t *cache, const hash_table *costmap ) {
  UNUSED( costmap );
  heap_t *heap = create_heap( compare_heap_link, cache->link_num );
  die_if_NULL( heap );

  node_t *treenode = tree->node;
  link_t *treelink = tree->link;
  unsigned int ncount = 0;
  unsigned int lcount = 0;

  node_t *from_node = lookup_hash_entry( cache->node_table, &tree->root_dpid );
  die_if_NULL( from_node );
  add_node_to_tree( tree, &treenode[ ncount++ ], from_node );
  uint16_t cost = 0;

  for ( ; tree->node_num < cache->node_num; ) {
    // Update phase
    dlist_element *element;
    for ( element = get_first_element( from_node->out_links );
          element != NULL && element->next != NULL; // REVISIT
          element = element->next ) {
      link_t *link = ( link_t * )element->data;
      push_to_heap( heap, link );
      link->total_cost = ( uint16_t )( cost + 1 );
    }

    // Selection phase
    link_t *candidate_link;
    node_t *candidate_node;
    for ( ;; ) {
      candidate_link = pop_from_heap( heap );
      if ( candidate_link == NULL ) { // REVISIT
        error( "Not found." );
        destroy_heap( heap );
        return;
      }
      candidate_node = lookup_hash_entry( tree->node_table,
                                          &candidate_link->to );
      if ( candidate_node == NULL ) {
        break;
      }
    }

    // Add the candidate node into tree.
    assert( ncount < cache->node_num );
    assert( lcount < cache->node_num - 1 );
    node_t *node = lookup_hash_entry( cache->node_table,
                                      &candidate_link->to );
    add_node_to_tree( tree, &treenode[ ncount++ ], node );
    add_link_to_tree( tree, &treelink[ lcount++ ], candidate_link );

    // Prepare for next routine
    cost = candidate_link->total_cost;
    from_node = node;
  }

  destroy_heap( heap );
}


static void
add_node_to_tree( tree_t *tree, node_t *treenode, node_t *node ) {
  die_if_NULL( tree );
  die_if_NULL( node );
  die_if_NULL( treenode );

  treenode->datapath_id = node->datapath_id;
  treenode->data = node->data;
  treenode->in_links = create_dlist();
  treenode->out_links = create_dlist();

  if ( lookup_hash_entry( tree->node_table, &treenode->datapath_id ) != NULL ) {
    die( "%s : Node (datapath_id=0x%lx) exists.", __func__ );
  }
  insert_hash_entry( tree->node_table, &treenode->datapath_id, treenode );
  tree->node_num++;

  return;
}


static void
add_link_to_tree( tree_t *tree, link_t *treelink, link_t *link ) {
  die_if_NULL( tree );
  die_if_NULL( link );
  die_if_NULL( treelink );
  memcpy( treelink, link, sizeof( link_t ) );

  if ( lookup_hash_entry( tree->link_table, &treelink->id ) != NULL ) {
    die( "%s : Link (id=0x%lx) exists.", __func__ );
  }
  insert_hash_entry( tree->link_table, &treelink->id, treelink );

  node_t *from_node = lookup_hash_entry( tree->node_table, &treelink->from );
  die_if_NULL( from_node );
  insert_before_dlist( from_node->out_links, treelink );

  node_t *to_node = lookup_hash_entry( tree->node_table, &treelink->to );
  die_if_NULL( to_node );
  insert_before_dlist( to_node->in_links, treelink );

  tree->link_num++;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */





