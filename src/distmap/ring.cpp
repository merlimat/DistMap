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
    if ( m_physicalNodes.count( node ) != 0 )
        return;

    INFO( "Added node: " << node << " " << n );
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
    INFO( "removed node: " << node );
    m_physicalNodes.erase( node );
    m_ring.right.erase( node );
}

const std::string& Ring::node( const std::string& key )
{
    ASSERT( ! m_ring.empty() );
    uint64_t hash = StringHash( key.c_str(), key.length() );

    StringMap::left_const_iterator it = m_ring.left.lower_bound( hash );
    if ( it == m_ring.left.end() )
        it = m_ring.left.begin();

    return it->second;
}

const std::string& Ring::nextNode( const std::string& node ) const
{
    ASSERT( ! m_ring.empty() );
    uint64_t hash = StringHash( node.c_str(), node.length() );
    StringMap::left_const_iterator it = m_ring.left.lower_bound( hash );
    if ( it == m_ring.left.end() )
        it = m_ring.left.begin();

    return it->second;
}

const std::string& Ring::nextPhysicalNode( const std::string& node ) const
{
    ASSERT( ! m_physicalNodes.empty() );
    StringSet::const_iterator it = m_physicalNodes.find( node );
    ++it;
    if ( it == m_physicalNodes.end() )
        it = m_physicalNodes.begin();

    return *it;
}

void Ring::preferenceList( const std::string& key,
                           StringList& nodes,
                           uint32_t n )
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
