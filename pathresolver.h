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


#ifndef LIBPATHRESOLVER_H
#define LIBPATHRESOLVER_H


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
  hash_table *link_table;
} topology_cache_t;


topology_cache_t *create_topology_cache();
void destroy_topology_cache( topology_cache_t *cache );

node_t *add_node_to_cache( topology_cache_t *cache, uint64_t datapath_id, void *data );
void del_node_from_cache( topology_cache_t *cache, uint64_t datapath_id );
link_t *add_link_to_cache( topology_cache_t *cache, uint64_t id, uint64_t from, uint16_t from_port, uint64_t to, uint16_t to_port, void *data );
void del_link_from_cache( topology_cache_t *cache, uint64_t id );

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


#endif // LIBPATHRESOLVER_H

/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

