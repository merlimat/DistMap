/*
 * membership.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "membership.hpp"

#include <distmap/configuration.hpp>
#include <distmap/message.hpp>
#include <distmap/message_bus.hpp>
#include <distmap/util/log.hpp>

#include <distmap/distmap.pb.h>

namespace distmap
{

Membership::Membership( asio::io_service& service,
                        Configuration& conf,
                        MessageBus& messageBus ) :
    m_service( service ), m_conf( conf ), m_messageBus( messageBus ), m_finder(
            service, *this, conf ), m_monitor( service, *this, messageBus ),
            m_announceTimer( service )
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

    DEBUG( "New node announced: " << nodeName );
    m_ring.add( nodeName );

    doMonitoring();

    // Send node list to new node
    if ( m_ring.nextNode( nodeName ) == m_node )
    {
        // Only one node will send the complete list to
        // the newcomer
        SharedBuffer data = CreateNodeListMsg( m_ring.physicalNodes() );
        m_messageBus.send( nodeName, data, bind(
                &Membership::handleMessageSent, this, ph::error ) );
    }
}

void Membership::receivedNodeList( const NodeList& nodeList )
{
    m_announceTimer.cancel();
    for ( int i = 0; i < nodeList.node_size(); i++ )
    {
        m_ring.add( nodeList.node( i ) );
        TRACE( " - Node: " << nodeList.node( i ) );
    }

    doMonitoring();
    DEBUG( "Node List: " << m_ring.physicalNodes() );
}

void Membership::announceTimeout( const sys::error_code& error )
{
    TRACE( "announceTimeout error=" << error.message() );
    if ( error != asio::error::operation_aborted )
        announce();
}

void Membership::handleMessageSent( const sys::error_code& error )
{

}

void Membership::nodeIsDown( const std::string& node )
{
    TRACE( "Node is down: " << node );
    m_finder.nodeIsDown( node );
    m_ring.remove( node );

    doMonitoring();
}

void Membership::doMonitoring()
{
    const std::string& nextNode = m_ring.nextPhysicalNode( m_node );
    if ( nextNode == m_node )
    {
        m_monitor.stopMonitoring();
        announce();
    }
    else
        m_monitor.startMonitoring( nextNode );
}

} // namespace distmap
