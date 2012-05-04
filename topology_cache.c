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


void
set_index_for_node( void *value, int index ) {
  UNUSED( value );
  UNUSED( index );
}


bool
compare_node( const void *value1, const void *value2 ) {
  const node_t *node1 = ( const node_t * )value1;
  const node_t *node2 = ( const node_t * )value2;

  return compare_datapath_id( &node1->datapath_id, &node2->datapath_id );
}


unsigned int
hash_node( const void *value ) {
  const node_t *node = ( const node_t * )value;

  return hash_datapath_id( &node->datapath_id );
}


bool
compare_link( const void *value1, const void *value2 ) {
  const link_t *link1 = ( const link_t * )value1;
  const link_t *link2 = ( const link_t * )value2;

  return compare_datapath_id( &link1->id, &link2->id );
}


unsigned int
hash_link( const void *value ) {
  const link_t *link = ( const link_t * )value;

  return hash_datapath_id( &link->id );
}


topology_cache_t *
create_topology_cache() {
  topology_cache_t *cache = ( topology_cache_t * )malloc( sizeof( topology_cache_t ) );
  if ( cache == NULL ) {
    return NULL;
  }

  memset( cache, 0, sizeof( topology_cache_t ) );
  cache->node_hash = create_hash( compare_node, hash_node );
  cache->link_hash = create_hash( compare_link, hash_link );
  
  return cache;
}


bool 
destroy_topology_cache( topology_cache_t *cache ) {
  UNUSED( cache );
  return true;
}


bool 
add_node_to_cache( topology_cache_t *cache, node_t node ) {
  UNUSED( cache );
  UNUSED( node );
  return true;
}


bool 
del_node_from_cache( topology_cache_t *cache, uint64_t datapath_id ) {
  UNUSED( cache );
  UNUSED( datapath_id );
  return true;
}


bool 
add_link_to_cache( topology_cache_t *cache, link_t link ) {
  UNUSED( cache );
  UNUSED( link );
  return true;
}


bool 
del_link_from_cache( topology_cache_t *cache, uint64_t id ) {
  UNUSED( cache );
  UNUSED( id );
  return true;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */
