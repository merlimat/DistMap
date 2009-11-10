/*
 * connection.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#include "connection.hpp"

#include <distmap/membership.hpp>
#include <distmap/message.hpp>
#include <distmap/util/log.hpp>
#include <distmap/distmap.pb.h>

#include <distmap/util/util.hpp>

namespace distmap
{

Connection::Connection( asio::io_service& service, Membership& membership ) :
    m_service( service ), m_membership( membership ), m_socket( service )
{
    TRACE( "Connection::Connection()" );
}

Connection::~Connection()
{
    if ( m_socket.is_open() )
    {
        TRACE( "Connection::~Connection() " << m_socket.remote_endpoint() );
    }
    else
    {
        TRACE( "Connection::~Connection()" );
    }
}

tcp::socket& Connection::socket()
{
    return m_socket;
}

void Connection::start()
{
    TRACE( "Connection from: " << m_socket.remote_endpoint() );
    receiveMessage();
}

void Connection::receiveMessage()
{
    SharedBuffer buffer;
    m_socket.async_read_some( buffer, bind( &Connection::handleReceive, this,
            ptr(), buffer, true, ph::error, ph::bytes_transferred ) );
}

void Connection::handleReceive( const ConnectionPtr& cnx,
                                SharedBuffer& buffer,
                                bool isFirstPart,
                                const sys::error_code& error,
                                size_t size )
{
    if ( error || size == 0 )
    {
        TRACE( "Error reading data: " << error.message() );
        return;
    }

    if ( isFirstPart )
    {
        uint32_t msgSize = readMessageSize( buffer );
        TRACE( "Msg size: " << msgSize << " -- Read Size: " << size );

        if ( msgSize > size )
        {
            // Schedule a complete read
            buffer.resize( msgSize );
            asio::async_read( m_socket, asio::buffer( buffer.data() + size,
                    msgSize - size ), bind( &Connection::handleReceive, this,
                    cnx, buffer, false, ph::error, ph::bytes_transferred ) );
            return;
        }
    }

    TRACE( "Got the complete message" );
    /// Do something....
    Message msg;
    readMessageWithSize( buffer, msg );
    switch ( msg.type() )
    {
    case Message::NodeList:
    {
        TRACE( "Node list received: " );
        m_membership.receivedNodeList( msg.nodelist() );
        receiveMessage();
        break;
    }
    case Message::Ping:
    {
        TRACE( "Ping msg received" );
        asio::async_write( m_socket, CreatePongMsg(), bind(
                &Connection::handleSend, this, cnx, ph::error,
                ph::bytes_transferred ) );
        break;
    }
    default:
    {
        ERROR( "Invalid msg received." );
        dumpBuffer( std::cerr, buffer.data(), buffer.size() );
        break;
    }
    }
}

void Connection::handleSend( const ConnectionPtr& ptr,
                             const sys::error_code& error,
                             size_t size )
{
    if ( !error )
    {
        receiveMessage();
    }
}

}
