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
  dlist_element *ingress_links;
  dlist_element *egress_links;
  void *data;
} node_t;


typedef struct {
  uint64_t id;
  node_t *from;
  node_t *to;
  void *data;
} link_t;


typedef struct {
  hash_table node_hash;
  hash_table link_hash;
} topology_cache_t;


topology_cache_t *create_topology_cache();
bool destroy_topology_cache( topology_cache *cache );

bool add_node_to_cache( topology_cache *cache, node_t node );
bool del_node_from_cache( topology_cache *cache, uint64_t datapath_id );
bool add_link_to_cache( topology_cache *cache, link_t link );
bool del_link_from_cache( topology_cache *cache, uint64_t id );

void *set_index_for_node( void *value, int index );
int *compare_node( void *value1, void *value2 );


#endif // LIBPATHRESOLVER_H

/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

