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


topology_cache_t *
create_topology_cache() {
  topology_cache_t *cache = ( topology_cache_t * )malloc( sizeof( topology_cache_t ) );
  die_if_NULL( cache );

  memset( cache, 0, sizeof( topology_cache_t ) );
  cache->node_table = create_hash( compare_hash_node, hash_node );
  cache->link_table = create_hash( compare_hash_link, hash_link );
  
  return cache;
}


void
destroy_topology_cache( topology_cache_t *cache ) {
  die_if_NULL( cache );

  delete_hash( cache->node_table );
  delete_hash( cache->link_table );

  memset( cache, 0, sizeof( topology_cache_t ) );
  free( cache );

  return;
}


node_t *
add_node_to_cache( topology_cache_t *cache, uint64_t datapath_id, void *data ) {
  die_if_NULL( cache );
  
  node_t *node = ( node_t * )malloc( sizeof( node_t ) );
  die_if_NULL( node );

  memset( node, 0, sizeof( node_t ) );
  node->datapath_id = datapath_id;
  node->data = data;
  node->in_links = create_dlist();
  node->out_links = create_dlist();

  if ( lookup_hash_entry( cache->node_table, &node->datapath_id ) != NULL ) {
    error( "%s : Node (datapath_id=0x%lx) exists.", __func__ );
    return NULL;
  }
  insert_hash_entry( cache->node_table, &node->datapath_id, node );
  
  return node;
}


void
del_node_from_cache( topology_cache_t *cache, uint64_t datapath_id ) {
  die_if_NULL( cache );

  node_t *node = delete_hash_entry( cache->node_table, &datapath_id );
  if ( node != NULL ) {
    delete_dlist( node->in_links );
    delete_dlist( node->out_links );
    memset( node, 0, sizeof( node_t ) );
    free( node );
  }

  return;
}


link_t *
add_link_to_cache( topology_cache_t *cache, uint64_t id, uint64_t from, uint16_t from_port, uint64_t to, uint16_t to_port, void *data ) {
  die_if_NULL( cache );

  node_t *from_node = lookup_hash_entry( cache->node_table, &from ); 
  node_t *to_node = lookup_hash_entry( cache->node_table, &to ); 
  if ( from_node == NULL || to_node == NULL ) {
    error( "%s : not found.\n", __func__ );
    return NULL;
  }

  link_t *link = ( link_t * )malloc( sizeof( link_t ) );
  die_if_NULL( link );
  
  memset( link, 0, sizeof( link_t ) );
  link->id = id;
  link->from = from;
  link->from_port = from_port;
  link->to = to;
  link->to_port = to_port;
  link->data = data;

  if ( lookup_hash_entry( cache->link_table, &link->id ) != NULL ) {
    error( "%s : Link (id=0x%lx) exists.", __func__ );
    return NULL;
  }
  insert_hash_entry( cache->link_table, &link->id, link );

  insert_before_dlist( from_node->out_links, link );
  insert_before_dlist( to_node->in_links, link );

  return link;
}


void
del_link_from_cache( topology_cache_t *cache, uint64_t id ) {
  die_if_NULL( cache );

  link_t *link = delete_hash_entry( cache->link_table, &id );
  if ( link == NULL ) {
    return;
  }

  node_t *from_node = lookup_hash_entry( cache->node_table, &link->from );
  if ( from_node != NULL ) {
    delete_dlist_element( find_element( from_node->out_links, link ) );
  }

  node_t *to_node = lookup_hash_entry( cache->node_table, &link->to );
  if ( to_node != NULL ) {
    delete_dlist_element( find_element( to_node->in_links, link ) );
  }
  
  memset( link, 0, sizeof( link_t ) );
  free( link );
  
  return;
}


int
compare_heap_node( const void *value1, const void *value2 ) {
  const node_t *node1 = ( const node_t * )value1;
  const node_t *node2 = ( const node_t * )value2;

  return compare_heap_uint64( &node1->datapath_id, &node2->datapath_id );
}


bool
compare_hash_node( const void *value1, const void *value2 ) {
  const node_t *node1 = ( const node_t * )value1;
  const node_t *node2 = ( const node_t * )value2;

  return compare_datapath_id( &node1->datapath_id, &node2->datapath_id );
}


unsigned int
hash_node( const void *value ) {
  const node_t *node = ( const node_t * )value;

  return hash_datapath_id( &node->datapath_id );
}


int
compare_heap_link( const void *value1, const void *value2 ) {
  const link_t *link1 = ( const link_t * )value1;
  const link_t *link2 = ( const link_t * )value2;

  return compare_heap_uint64( &link1->id, &link2->id );
}


bool
compare_hash_link( const void *value1, const void *value2 ) {
  const link_t *link1 = ( const link_t * )value1;
  const link_t *link2 = ( const link_t * )value2;

  return compare_heap_uint64( &link1->id, &link2->id );
}


unsigned int
hash_link( const void *value ) {
  const link_t *link = ( const link_t * )value;

  return hash_datapath_id( &link->id );
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
