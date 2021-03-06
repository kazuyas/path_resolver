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


#ifndef PATHRESOLVER_H
#define PATHRESOLVER_H


#include <stdint.h>
#include <sys/types.h>
#include "heap.h"
#include "heap_utility.h"
#include "hash_table.h"
#include "linked_list.h"
#include "doubly_linked_list.h"


typedef struct {
  uint64_t datapath_id;
  dlist_element *in_links;
  dlist_element *out_links;
  void *data;
} node_t;


typedef struct {
  uint64_t id;
  uint64_t from;
  uint16_t from_port;
  uint64_t to;
  uint16_t to_port;
  void *data;
} link_t;


typedef struct {
  hash_table *node_table;
  size_t node_num;
  hash_table *link_table_by_id;
  hash_table *link_table_by_ends;
  size_t link_num;
  uint64_t link_id;
  hash_table *tree_table;
  size_t tree_num;
} topology_cache_t;


typedef struct {
  uint64_t root_dpid;
  node_t *node;
  hash_table *node_table;
  size_t node_num;
  link_t *link;
  hash_table *link_table;
  size_t link_num;
  bool reverse;
} tree_t;


typedef struct {
  uint64_t datapath_id;
  uint16_t out_port;
} hop_t;


typedef struct {
  link_t *link;
  uint16_t cost;
} heap_link_t;


tree_t *create_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap );
tree_t *create_reverse_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap );
void destroy_tree( tree_t *tree );
list_element *resolve_path_from_tree( const tree_t *tree, const uint64_t to );
list_element *resolve_path_from_reverse_tree( const tree_t *tree, const uint64_t from );
void destroy_path( list_element *path );

topology_cache_t *create_topology_cache();
void destroy_topology_cache( topology_cache_t *cache );

node_t *add_node_to_cache( topology_cache_t *cache, const uint64_t datapath_id, void *data );
void del_node_from_cache( topology_cache_t *cache, const uint64_t datapath_id );

uint64_t get_link_id( topology_cache_t *cache );
link_t *lookup_link_by_ends( topology_cache_t *cache, const uint64_t from, const uint16_t from_port, const uint64_t to, const uint16_t to_port );
link_t *add_link_to_cache( topology_cache_t *cache, const uint64_t id, const uint64_t from, const uint16_t from_port, const uint64_t to, const uint16_t to_port, void *data );
void del_link_from_cache( topology_cache_t *cache, const uint64_t id );

int compare_heap_node( const void *value1, const void *value2 );
bool compare_hash_node( const void *value1, const void *value2 );
unsigned int hash_node( const void *value );
int compare_heap_link_by_id( const void *value1, const void *value2 );
bool compare_hash_link_by_id( const void *value1, const void *value2 );
unsigned int hash_link_by_id( const void *value );
bool compare_hash_link_by_ends( const void *value1, const void *value2 );
unsigned int hash_link_by_ends( const void *value );


#endif // PATHRESOLVER_H

/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

