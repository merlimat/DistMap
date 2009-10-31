/*
 * membership.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "membership.hpp"

#include <distmap/configuration.hpp>
#include <distmap/connection_pool.hpp>
#include <distmap/util/log.hpp>

namespace distmap
{

Membership::Membership( asio::io_service& service,
                        Configuration& conf,
                        ConnectionPool& connectionPool ) :
    m_service( service ), m_conf( conf ), m_connectionPool( connectionPool ),
            m_finder( service, *this, conf ), m_announceTimer( service )
{
    TRACE( "Membership::Membership() -- NodeName: " << m_conf.nodeName() );
}

Membership::~Membership()
{
    TRACE( "Membership::~Membership()" );
}

void Membership::announce()
{
    DEBUG( "Announce myself to multicast channel. nodeName=" << m_node );
    m_finder.announceMyself( m_node );
    m_announceTimer.expires_from_now( ptime::seconds( 3 ) );
    m_announceTimer.async_wait( bind( &Membership::announceTimeout, this,
            ph::error ) );
}

void Membership::receivedAnnounce( const std::string& nodeName )
{
    if ( nodeName == m_node )
    {
        TRACE( "Ignore self message" );
        return;
    }

    INFO( "New node announced: " << nodeName );
    // Say hello to node..

}

void Membership::announceTimeout( const sys::error_code& error )
{
    TRACE( "announceTimeout error=" << error.message() );
    announce();
}

} // namespace distmap
