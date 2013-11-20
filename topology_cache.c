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


#include "pathresolver.h"
#include "log.h"
#include "checks.h"
#include "wrapper.h"


topology_cache_t *
create_topology_cache() {
  topology_cache_t *cache = xcalloc( 1, sizeof( topology_cache_t ) );
  die_if_NULL( cache );

  cache->node_table = create_hash( compare_hash_node, hash_node );
  cache->link_table_by_id = create_hash( compare_hash_link_by_id, hash_link_by_id );
  cache->link_table_by_ends = create_hash( compare_hash_link_by_ends, hash_link_by_ends );
  cache->node_num = 0;
  cache->link_num = 0;
  cache->link_id = 0;

  return cache;
}


void
destroy_topology_cache( topology_cache_t *cache ) {
  die_if_NULL( cache );

  hash_iterator iter;
  hash_entry *entry;

  init_hash_iterator( cache->node_table, &iter );
  while ( ( entry = iterate_hash_next( &iter ) ) != NULL ) {
    node_t *node = entry->value;
    delete_dlist( node->in_links );
    delete_dlist( node->out_links );
    memset( node, 0, sizeof( node_t ) );
    xfree( node );
  }
  delete_hash( cache->node_table );

  init_hash_iterator( cache->link_table_by_id, &iter );
  while ( ( entry = iterate_hash_next( &iter ) ) != NULL ) {
    link_t *link = entry->value;
    memset( link, 0, sizeof( link_t ) );
    xfree( link );
  }
  delete_hash( cache->link_table_by_id );
  delete_hash( cache->link_table_by_ends );

  memset( cache, 0, sizeof( topology_cache_t ) );
  xfree( cache );

  return;
}


node_t *
add_node_to_cache( topology_cache_t *cache, const uint64_t datapath_id, void *data ) {
  die_if_NULL( cache );

  node_t *node = xcalloc( 1, sizeof( node_t ) );
  die_if_NULL( node );

  node->datapath_id = datapath_id;
  node->data = data;
  node->in_links = create_dlist();
  node->out_links = create_dlist();

  if ( lookup_hash_entry( cache->node_table, &node->datapath_id ) != NULL ) {
    error( "%s : Node (datapath_id=0x%lx) exists.", __func__, datapath_id );
    xfree( node );
    return NULL;
  }
  insert_hash_entry( cache->node_table, &node->datapath_id, node );
  cache->node_num++;

  return node;
}


void
del_node_from_cache( topology_cache_t *cache, const uint64_t datapath_id ) {
  die_if_NULL( cache );

  node_t *node = delete_hash_entry( cache->node_table, &datapath_id );
  if ( node != NULL ) {
    delete_dlist( node->in_links );
    delete_dlist( node->out_links );
    memset( node, 0, sizeof( node_t ) );
    xfree( node );
    cache->node_num--;
  }

  return;
}


uint64_t
get_link_id( topology_cache_t *cache ) {
  die_if_NULL( cache );

  return ++cache->link_id;
}


link_t *
lookup_link_by_ends( topology_cache_t *cache, const uint64_t from, const uint16_t from_port, const uint64_t to, const uint16_t to_port ) {
  link_t link = { 0, from, from_port, to, to_port, NULL };
  
  return ( link_t * )lookup_hash_entry( cache->link_table_by_ends, &link );
}


link_t *
add_link_to_cache( topology_cache_t *cache, const uint64_t id, const uint64_t from, const uint16_t from_port, const uint64_t to, const uint16_t to_port, void *data ) {
  die_if_NULL( cache );

  node_t *from_node = lookup_hash_entry( cache->node_table, &from );
  node_t *to_node = lookup_hash_entry( cache->node_table, &to );
  if ( from_node == NULL || to_node == NULL ) {
    error( "%s : not found.\n", __func__ );
    return NULL;
  }

  link_t *link = xcalloc( 1, sizeof( link_t ) );
  die_if_NULL( link );

  link->id = id;
  link->from = from;
  link->from_port = from_port;
  link->to = to;
  link->to_port = to_port;
  link->data = data;

  if ( lookup_hash_entry( cache->link_table_by_id, &link->id ) != NULL ) {
    error( "%s : Link (id=0x%lx) exists.", __func__, id );
    return NULL;
  }
  insert_hash_entry( cache->link_table_by_id, &link->id, link );
  insert_hash_entry( cache->link_table_by_ends, link, link );

  insert_before_dlist( from_node->out_links, link );
  insert_before_dlist( to_node->in_links, link );

  cache->link_num++;

  return link;
}


void
del_link_from_cache( topology_cache_t *cache, const uint64_t id ) {
  die_if_NULL( cache );

  link_t *link = delete_hash_entry( cache->link_table_by_id, &id );
  if ( link == NULL ) {
    return;
  }
  delete_hash_entry( cache->link_table_by_ends, link );

  node_t *from_node = lookup_hash_entry( cache->node_table, &link->from );
  if ( from_node != NULL ) {
    delete_dlist_element( find_element( from_node->out_links, link ) );
  }

  node_t *to_node = lookup_hash_entry( cache->node_table, &link->to );
  if ( to_node != NULL ) {
    delete_dlist_element( find_element( to_node->in_links, link ) );
  }

  memset( link, 0, sizeof( link_t ) );
  xfree( link );

  cache->link_num--;

  return;
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
compare_heap_link_by_id( const void *value1, const void *value2 ) {
  const heap_link_t *link1 = ( const heap_link_t * )value1;
  const heap_link_t *link2 = ( const heap_link_t * )value2;

  return compare_heap_uint16( &link1->cost, &link2->cost );
}


bool
compare_hash_link_by_id( const void *value1, const void *value2 ) {
  const link_t *link1 = ( const link_t * )value1;
  const link_t *link2 = ( const link_t * )value2;

  return compare_datapath_id( &link1->id, &link2->id );
}


unsigned int
hash_link_by_id( const void *value ) {
  const link_t *link = ( const link_t * )value;

  return hash_datapath_id( &link->id );
}


bool
compare_hash_link_by_ends( const void *value1, const void *value2 ) {
  const link_t *link1 = ( const link_t * )value1;
  const link_t *link2 = ( const link_t * )value2;

  if ( link1->from != link2->from ) {
    return false;
  } else if ( link1->from_port != link2->from_port ) {
    return false;
  } else if ( link1->to != link2->to ) {
    return false;
  } else if ( link1->to_port != link2->to_port ) {
    return false;
  } 
  
  return true;
}


unsigned int
hash_link_by_ends( const void *value ) {
  link_t link;

  memcpy( &link, value, sizeof( link_t ) );
  link.id = 0;
  link.data = NULL;
  
  return hash_core( &link, sizeof( link_t ) );
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
