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


#ifndef HEAP_UTILITY_H
#define HEAP_UTILITY_H


#include <stdint.h>
#include <sys/types.h>
#include "heap.h"


int compare_heap_uint64( const void *value1, const void *value2 );
int compare_heap_uint32( const void *value1, const void *value2 );
int compare_heap_uint16( const void *value1, const void *value2 );
int compare_heap_uint8( const void *value1, const void *value2 );


#endif // HEAP_UTILITY_H

/*
 * Local variables:
 * c-basic-offset: 2
 * indent-tabs-mode: nil
 * End:
 */

