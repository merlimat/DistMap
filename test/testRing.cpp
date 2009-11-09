/*
 * testRing.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: mat
 */

#include <distmap/ring.hpp>
#include <distmap/util/log.hpp>

#include <stdio.h>
#include <map>

int main()
{
    distmap::Ring ring;
    Timer timer;

    // Add some nodes
    const uint32_t Nodes = 10;
    char name[1024];
    for ( uint32_t i = 0; i < Nodes; i++ )
    {
        snprintf( name, sizeof(name), "node_%u", i );
        ring.add( name, 100 );
    }

    double time = timer.elapsed();
    INFO( "Insert time: " << time / Nodes * 1000 << " ms/node" );

    timer.restart();
    const uint32_t Gets = 100000;

    std::map<std::string, int> map;

    for ( uint32_t i = 0; i < Gets; i++ )
    {
        snprintf( name, sizeof(name), "key_%u", i );
        // INFO( name << " --> " << ring.node( name ) );
        map[ ring.node( name ) ] += 1;
    }

    time = timer.elapsed();
    INFO( "Get time: " << time / Gets * 1000 << " ms/node" );
    // INFO( "Node for 'xx': " << ring.node("xx") );

    distmap::StringList nodes;
    ring.preferenceList( "xxx", nodes, 3 );
    for ( distmap::StringList::iterator it = nodes.begin(); it != nodes.end(); ++it )
        INFO( *it );
    // for ( std::map<std::string,int>::iterator it = map.begin(); it != map.end(); ++it )
    //     INFO( it->first << " : " << it->second );
}
