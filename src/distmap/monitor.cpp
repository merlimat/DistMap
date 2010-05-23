/*
 * monitor.cpp
 *
 *  Created on: Nov 10, 2009
 *      Author: mat
 */

#include "monitor.hpp"

#include <distmap/membership.hpp>
#include <distmap/message_bus.hpp>
#include <distmap/message.hpp>
#include <distmap/util/log.hpp>
#include <distmap/distmap.pb.h>

namespace distmap
{

Monitor::Monitor( asio::io_service& service,
                  Membership& membership,
                  MessageBus& messageBus ) :
    m_timer( service ), m_membership( membership ), m_messageBus( messageBus )
{
    TRACE( "Monitor::Monitor()" );
}

Monitor::~Monitor()
{
    TRACE( "Monitor::~Monitor()" );
}

void Monitor::startMonitoring( const std::string& node )
{
    if ( node == m_monitoredNode )
        return;

    TRACE( "Monitoring node " << node );
    m_monitoredNode = node;

    handleSendNextPing( node, sys::error_code() );
}

void Monitor::stopMonitoring()
{
    TRACE( "Stop monitoring" );
    m_monitoredNode = "";
    m_timer.cancel();
}

void Monitor::handlePing( const std::string& node,
                          const sys::error_code& error,
                          const SharedBuffer& buffer )
{
    if ( node != m_monitoredNode )
    {
        // We are already monitor a different node, ignore
        return;
    }
    else if ( error )
    {
        m_monitoredNode = "";
        m_membership.nodeIsDown( node );
        return;
    }

    Message msg;
    readMessageWithSize( buffer, msg );
    switch ( msg.type() )
    {
    case Message::Pong:
    {
        TRACE( "Receive Pong message from " << node );
        m_timer.expires_from_now( ptime::seconds( 5 ) );
        m_timer.async_wait( bind( &Monitor::handleSendNextPing, this, node,
                ph::error ) );
        break;
    }
    default:
    {
        WARN( "Invalid message received from monitored node: " << node );
        m_monitoredNode = "";
        m_membership.nodeIsDown( node );
        break;
    }
    }
}

void Monitor::handleTimeout( const std::string& node,
                             const sys::error_code& error )
{
    if ( error != asio::error::operation_aborted && node == m_monitoredNode )
    {
        TRACE( "Timeout in ping to node " << node );
        m_monitoredNode = "";
        m_membership.nodeIsDown( node );
    }
}

void Monitor::handleSendNextPing( const std::string& node,
                                  const sys::error_code& error )
{
    if ( error != asio::error::operation_aborted && node == m_monitoredNode )
    {
        TRACE( "Send next ping to: " << node );

        SharedBuffer msg;
        if ( !m_membership.hasChanged() )
            msg = CreatePingMsg();
        else {
            DEBUG( "Resending the node list with ping message" );
            msg = CreatePingMsg( m_membership.nodeList() );
        }

        m_messageBus.sendAndReceive( node, msg, bind( &Monitor::handlePing,
                this, node, _1, _2 ) );
        m_timer.expires_from_now( ptime::seconds( 5 ) );
        m_timer.async_wait( bind( &Monitor::handleTimeout, this, node,
                ph::error ) );
    }
}

} // namespace distmape
