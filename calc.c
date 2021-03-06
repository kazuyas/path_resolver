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
#include "pathresolver.h"
#include "log.h"
#include "wrapper.h"
#include "checks.h"


static tree_t *_create_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap, bool reverse );
static void calculate( tree_t *tree, const topology_cache_t *cache, const hash_table *costmap );
static int insert( heap_t *heap, node_t *from_node, uint16_t cost, heap_link_t *heap_links, bool reverse );
static link_t *select_candidate( heap_t *heap, tree_t *tree, bool reverse );
static void add_node_to_tree( tree_t *tree, node_t *treenode, node_t *node );
static void add_link_to_tree( tree_t *tree, link_t *treelink, link_t *link );


tree_t *
create_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap ) {
  die_if_NULL( cache );

  tree_t *tree = _create_tree( cache, root, costmap, false );

  return tree;
}


tree_t *
create_reverse_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap ) {
  die_if_NULL( cache );

  tree_t *tree = _create_tree( cache, root, costmap, true );

  return tree;
}


static tree_t *
_create_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap, bool reverse ) {
  tree_t *tree = xmalloc( sizeof( tree_t ) );
  if ( tree == NULL ) {
    return NULL;
  }

  tree->root_dpid = root;
  tree->node_table = create_hash( compare_hash_node, hash_node );
  tree->link_table = create_hash( compare_hash_link_by_id, hash_link_by_id );
  tree->node = xcalloc( cache->node_num, sizeof( node_t ) );
  tree->link = xcalloc( cache->node_num - 1, sizeof( link_t ) );

  tree->node_num = 0;
  tree->link_num = 0;

  tree->reverse = reverse;

  calculate( tree, cache, costmap );

  return tree;
}


void
destroy_tree( tree_t *tree ) {
  die_if_NULL( tree );

  for ( unsigned int i = 0; i < tree->node_num; i++ ) {
    delete_dlist( tree->node[ i ].in_links );
    delete_dlist( tree->node[ i ].out_links );
  }

  delete_hash( tree->node_table );
  delete_hash( tree->link_table );

  memset( tree->node, 0, sizeof( node_t ) * tree->node_num );
  xfree( tree->node );

  memset( tree->link, 0, sizeof( link_t ) * tree->link_num );
  xfree( tree->link );

  memset( tree, 0, sizeof( tree_t ) );
  xfree( tree );
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

    hop_t *hop = xmalloc( sizeof( hop_t ) );
    hop->datapath_id = link->from;
    hop->out_port = link->from_port;

    append_to_tail( &path, hop );

    current = link->from;
  }

  return path;
}


list_element *
resolve_path_from_reverse_tree( const tree_t *tree, const uint64_t from ) {
  list_element *path;
  create_list( &path );

  uint64_t current;
  for ( current = from; current != tree->root_dpid; ) {
    node_t *node = lookup_hash_entry( tree->node_table, &current );
    if ( node == NULL ) {
      return NULL;
    }

    dlist_element *dl_element = get_first_element( node->out_links );
    link_t *link = dl_element->data;

    hop_t *hop = xmalloc( sizeof( hop_t ) );
    hop->datapath_id = link->from;
    hop->out_port = link->from_port;

    insert_in_front( &path, hop );

    current = link->to;
  }

  return path;
}


void
destroy_path( list_element *path ) {
  list_element *curr;
  for ( curr = path; curr != NULL; curr = curr->next ) {
    xfree( curr->data );
  }
  delete_list( path );
}


static void
calculate( tree_t *tree, const topology_cache_t *cache, const hash_table *costmap ) {
  UNUSED( costmap );
  heap_t *heap = create_heap( compare_heap_link_by_id, cache->link_num );
  die_if_NULL( heap );

  heap_link_t *heap_links = xcalloc( cache->link_num, sizeof( heap_link_t ) );
  die_if_NULL( heap_links );
  heap_link_t *heap_links_start = heap_links;

  node_t *treenode = tree->node;
  link_t *treelink = tree->link;

  node_t *node = lookup_hash_entry( cache->node_table, &tree->root_dpid );
  die_if_NULL( node );
  add_node_to_tree( tree, treenode++, node );
  uint16_t cost = 0;

  for ( ; tree->node_num < cache->node_num; ) {
    // Update phase
    heap_links += insert( heap, node, cost, heap_links, tree->reverse );

    // Selection phase
    link_t *candidate = select_candidate( heap, tree, tree->reverse );
    if ( candidate == NULL ) { // REVISIT
      info( "Some nodes may be unconnected." );
      break;
    }

    // Add the candidate node into tree.
    uint64_t next_dpid = tree->reverse ? candidate->from : candidate->to;
    node_t *next_node = lookup_hash_entry( cache->node_table, &next_dpid );
    add_node_to_tree( tree, treenode++, next_node );
    add_link_to_tree( tree, treelink++, candidate );

    // Prepare for next routine
    node = next_node;
  }

  xfree( heap_links_start );
  destroy_heap( heap );
}


static int
insert( heap_t *heap, node_t *node, uint16_t cost, heap_link_t *heap_links, bool reverse ) {
  int insert_num = 0;
  dlist_element *element;
  dlist_element *initial = reverse ? node->in_links : node->out_links;

  for ( element = get_first_element( initial ); 
        element != NULL && element->data != NULL; // REVISIT
        element = element->next ) {
    heap_link_t *heap_link = heap_links++;
    heap_link->link = ( link_t * )element->data;
    heap_link->cost = ( uint16_t )( cost + 1 );
    push_to_heap( heap, heap_link );
    insert_num++;
  }
  return insert_num;
}


static link_t *
select_candidate( heap_t *heap, tree_t *tree, bool reverse ) {
  link_t *candidate = NULL;
  for ( ;; ) {
    heap_link_t *heap_link = pop_from_heap( heap );
    if ( heap_link == NULL ) {
      break;
    }

    link_t *link = heap_link->link;
    void *key = reverse ? &link->from : &link->to;
    if ( lookup_hash_entry( tree->node_table, key ) == NULL ) {
      candidate = link;
      break;
    }
  }
  return candidate;
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
