/*
 * util.cpp
 *
 *  Created on: Nov 1, 2009
 *      Author: mat
 */

#include "util.hpp"

#include <iostream>
#include <stdio.h>

void dumpBuffer( std::ostream& ss, const void* ptr, uint32_t size )
{
    ss << "------------------------------------------------" << std::endl;
    ss << " size=" << size << std::endl;

    char buffer[200];

    struct
    {
        char *data;
        uint32_t size;
    }buf;

    buf.data = (char*) ptr;
    buf.size = size;

    while ( buf.size > 0 )
    {
        uint8_t* tmp = (uint8_t*) buf.data;
        uint32_t outLen = std::min( (uint32_t) 16, buf.size );
        uint32_t outLen2 = outLen;

        uint32_t address = tmp - (uint8_t*) ptr;
        uint32_t i = sprintf( buffer, "%08X  ", address );
        sprintf( buffer + i, ">                            "
                "                      "
                "     |" );

        uint32_t idx;
        uint32_t idx2;
        uint32_t relPos;
        for ( idx = 0, idx2 = 51, relPos = 0; outLen2; outLen2--, idx += 3, idx2++ )
        {
            buffer[60] = '|';
            uint8_t ucTmp = *tmp++;

            sprintf( buffer + i + idx, "%02X", (unsigned short) ucTmp );
            buffer[i + idx + 2] = ' ';
            buffer[i + idx2] = isprint( ucTmp ) ? ucTmp : '.';

            if ( !(++relPos & 7) ) // extra blank after 8 bytes

            {
                idx++;
                buffer[i + idx + 2] = ' ';
            }
        }

        buffer[i + idx2] = '|';
        buffer[i + idx2 + 1] = '\0';

        ss << buffer << '\n';

        buf.data += outLen;
        buf.size -= outLen;
    }
    ss << "------------------------------------------------" << std::endl;
}
