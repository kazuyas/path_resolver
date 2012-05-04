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

#include "trema.h"
#include "pathresolver.h"


static void calculate( tree_t *tree, const topology_cache_t *cache );
static void add_node_to_tree( tree_t *tree, node_t *node );
static void add_link_to_tree( tree_t *tree, link_t *link );


tree_t *
create_tree( const uint64_t root, const topology_cache_t *cache ) {
  if ( cache == NULL ) {
    return NULL;
  }

  tree_t *tree = ( tree_t * )malloc( sizeof( tree_t ) );
  if ( tree == NULL ) {
    return NULL;
  }
  
  tree->root_dpid = root;
  calculate( tree, cache );
    
  return tree;
}


list_element *
resolve_path_from_tree( tree_t *tree, uint64_t from, uint64_t to ) {
  list_element *path;
  create_list( &path );

  uint64_t current;
  for ( current = to; ; current = from ) {
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
    
    if ( from == link->from )
      break;
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
calculate( tree_t *tree, const topology_cache_t *cache ) {
  heap_t *heap = create_heap( compare_heap_link, NULL, cache->link_num );
  die_if_NULL( heap );

  node_t *from_node = lookup_hash_entry( cache->node_table, &tree->root_dpid );
  die_if_NULL( from_node );
  
  for ( ; tree->node_num < cache->node_num; ) {
    // Update phase
    dlist_element *element;
    for ( element = get_first_element( from_node->out_links );
          element != NULL;
          element = element->next ) {
      push_to_heap( heap, ( link_t * )element->data );
    }

    // Selection phase
    link_t *candidate_link; 
    node_t *candidate_node;
    for ( ;; ) {
      candidate_link = pop_from_heap( heap );
      candidate_node = lookup_hash_entry( tree->node_table,
                                          &candidate_link->to );
      if ( candidate_node == NULL ) {
        break;
      }
    }

    // Add the candidate node into tree.
    add_node_to_tree( tree, candidate_node );
    add_link_to_tree( tree, candidate_link );
  }
}


static void
add_node_to_tree( tree_t *tree, node_t *node ) {
  die_if_NULL( tree );
  die_if_NULL( node );

  node_t *treenode = ( node_t * )malloc( sizeof( node_t ) );
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
add_link_to_tree( tree_t *tree, link_t *link ) {
  die_if_NULL( tree );
  die_if_NULL( link );

  link_t *treelink = ( link_t * )malloc( sizeof( link_t ) );
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





