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


#include "heap.h"


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
  uint16_t total_cost; // only for tree
  void *data;
} link_t;


typedef struct {
  hash_table *node_table;
  size_t node_num;
  hash_table *link_table;
  size_t link_num;
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
} tree_t;


typedef struct {
  uint64_t datapath_id;
  uint16_t out_port;
} hop_t;


tree_t *create_tree( const topology_cache_t *cache, const uint64_t root, const hash_table *costmap );
void destroy_tree( tree_t *tree );
list_element *resolve_path_from_tree( const tree_t *tree, const uint64_t to );
void destroy_path( list_element *path );

topology_cache_t *create_topology_cache();
void destroy_topology_cache( topology_cache_t *cache );

node_t *add_node_to_cache( topology_cache_t *cache, const uint64_t datapath_id, void *data );
void del_node_from_cache( topology_cache_t *cache, const uint64_t datapath_id );
link_t *add_link_to_cache( topology_cache_t *cache, const uint64_t id, const uint64_t from, const uint16_t from_port, const uint64_t to, const uint16_t to_port, void *data );
void del_link_from_cache( topology_cache_t *cache, const uint64_t id );

int compare_heap_node( const void *value1, const void *value2 );
bool compare_hash_node( const void *value1, const void *value2 );
unsigned int hash_node( const void *value );
int compare_heap_link( const void *value1, const void *value2 );
bool compare_hash_link( const void *value1, const void *value2 );
unsigned int hash_link( const void *value );

int compare_heap_uint64( const void *value1, const void *value2 );
int compare_heap_uint32( const void *value1, const void *value2 );
int compare_heap_uint16( const void *value1, const void *value2 );
int compare_heap_uint8( const void *value1, const void *value2 );


#endif // PATHRESOLVER_H

/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

