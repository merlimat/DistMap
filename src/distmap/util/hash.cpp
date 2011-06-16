/*
 * hash.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: mat
 */

#include "hash.hpp"

#include <distmap/util/md5.h>

#include <stdio.h>

uint64_t StringHash( const std::string& str )
{
    md5_state_t state;
    md5_init( &state );
    md5_append( &state, (const uint8_t*) str.c_str(), str.length() );

    uint8_t buffer[16];
    md5_finish( &state, buffer );
    uint64_t res = 0ULL;
    for ( int i = 0; i < 8; i++ )
        res = (res << 8) + buffer[i];
    return res;
}
