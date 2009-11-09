/*
 * ring.cpp
 *
 *  Created on: Nov 9, 2009
 *      Author: mat
 */

#include "ring.hpp"

#include <distmap/util/log.hpp>

#include <stdio.h>

namespace distmap
{

Ring::Ring()
{
    TRACE( "Ring::Ring()" );
}

Ring::~Ring()
{
    TRACE( "Ring::~Ring()" );
}

void Ring::add( const std::string& node, uint32_t n )
{
    TRACE( "added node: " << node << " " << n );
    m_physicalNodes.insert( node );

    char name[node.length() + 10];
    for ( uint32_t i = 0; i < n; i++ )
    {
        int len = snprintf( name, sizeof(name), "%s_%u", node.c_str(), i );
        uint64_t hash = StringHash( name, len );
        m_ring.insert( StringMap::value_type( hash, node ) );
    }
}

void Ring::remove( const std::string& node )
{
    TRACE( "removed node: " << node );
    m_physicalNodes.erase( node );
    uint32_t n = m_ring.right.erase( node );
}

const std::string& Ring::node( const std::string& key )
{
    uint64_t hash = StringHash( key.c_str(), key.length() );

    StringMap::left_const_iterator it = m_ring.left.lower_bound( hash );
    if ( it == m_ring.left.end() )
        it = m_ring.left.begin();

    return it->second;
}

void Ring::preferenceList( const std::string& key, StringList& nodes, uint32_t n )
{
    uint64_t hash = StringHash( key.c_str(), key.length() );
    n = std::min<uint32_t>( n, m_physicalNodes.size() );

    StringSet set;

    StringMap::left_const_iterator it = m_ring.left.lower_bound( hash );
    while ( nodes.size() < n )
    {
        if ( it == m_ring.left.end() )
            it = m_ring.left.begin();

        if ( set.count( it->second ) == 0 )
        {
            nodes.push_back( it->second );
            set.insert( it->second );
        }

        ++it;
    }
}

}
