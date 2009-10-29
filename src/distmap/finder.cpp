/*
 * finder.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "finder.hpp"

#include <distmap/util/log.hpp>

#include <time.h>
#include <unistd.h>
#include <google/utilities.h>

namespace distmap
{

FinderCommand::FinderCommand( Type type, const std::string& node ) :
    m_type( type ), m_node( node )
{
}

SharedBuffer FinderCommand::serialize()
{
    size_t size = m_node.length() + 1;
    SharedBuffer buffer( size );
    char* ptr = buffer.data();
    ptr[0] = m_type;
    strncpy( ptr + 1, m_node.c_str(), size-1 );
    return buffer;
}

FinderCommand::FinderCommand FinderCommand::parse( const char* buffer,
                                                   size_t size )
{
    if ( size < 3 )
        return FinderCommand( Invalid, "" );

    char type = buffer[0];
    if ( type != Announce && type != NodeIsDown )
        return FinderCommand( Invalid, "" );

    std::string name( buffer + 1, size - 1 );
    return FinderCommand( static_cast<Type> ( type ), name );
}

////////////////////////////////////////////////////////////////////////////////

Finder::Finder( asio::io_service& service, Membership& membership ) :
    m_service( service ), m_membership( membership ), m_socket( service ),
            m_senderEndpoint()
{
    TRACE( "Finder::Finder" );

    ip::address listenerAddr = ip::address::from_string( "0.0.0.0" );
    ip::address multicastAddr = ip::address::from_string( "226.0.0.1" );
    const uint16_t port = 20000;

    m_multicastEndpoint = udp::endpoint( multicastAddr, port );

    udp::endpoint listenEndpoint( listenerAddr, port );
    m_socket.open( listenEndpoint.protocol() );
    m_socket.set_option( udp::socket::reuse_address( true ) );
    m_socket.bind( listenEndpoint );

    // Join the multicast group.
    m_socket.set_option( ip::multicast::join_group( multicastAddr ) );
}

Finder::~Finder()
{
    TRACE( "Finder::~Finder" );
}

void Finder::receiveMessage()
{
    m_socket.async_receive_from( asio::buffer( m_receiveBuffer ),
            m_senderEndpoint, bind( &Finder::handleReceiveFrom, this,
                    ph::error, ph::bytes_transferred ) );
}

void Finder::handleReceiveFrom( const sys::error_code& error, size_t size )
{
    TRACE( "Finder::handleReceiveFrom. error=" << error.message() << " size=" << size );
    TRACE( "Sender endpoint: " << m_senderEndpoint );

    receiveMessage();
}

void Finder::announceMyself( const std::string& nodeName )
{
    FinderCommand cmd( FinderCommand::Announce, nodeName );

    SharedBuffer buffer = cmd.serialize();
    TRACE( "announce message sent. size=" << buffer.size() );
    m_socket.async_send_to( buffer, m_multicastEndpoint, bind(
            &Finder::handleMsgSent, this, ph::error, ph::bytes_transferred ) );
}

void Finder::nodeIsDown( const std::string& )
{
}

void Finder::handleMsgSent( const sys::error_code& error, size_t size )
{
    TRACE( "Msg sent. error='" << error.message() << "' size=" << size );
}

}
