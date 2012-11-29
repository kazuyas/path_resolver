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


#include "heap_utility.h"


int
compare_heap_uint64( const void *value1, const void *value2 ) {
  const uint64_t l1 = *( const uint64_t * )value1;
  const uint64_t l2 = *( const uint64_t * )value2;

  if ( l1 == l2 ) {
    return 0;
  }

  return ( l1 < l2 ) ? -1 : 1;
}


int
compare_heap_uint32( const void *value1, const void *value2 ) {
  const uint32_t l1 = *( const uint32_t * )value1;
  const uint32_t l2 = *( const uint32_t * )value2;

  if ( l1 == l2 ) {
    return 0;
  }

  return ( l1 < l2 ) ? -1 : 1;
}


int
compare_heap_uint16( const void *value1, const void *value2 ) {
  const uint16_t l1 = *( const uint16_t * )value1;
  const uint16_t l2 = *( const uint16_t * )value2;

  if ( l1 == l2 ) {
    return 0;
  }

  return ( l1 < l2 ) ? -1 : 1;
}


int
compare_heap_uint8( const void *value1, const void *value2 ) {
  const uint8_t l1 = *( const uint8_t * )value1;
  const uint8_t l2 = *( const uint8_t * )value2;

  if ( l1 == l2 ) {
    return 0;
  }

  return ( l1 < l2 ) ? -1 : 1;
}


/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

