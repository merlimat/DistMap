/*
 * connection.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#include "connection.hpp"

#include <distmap/util/log.hpp>

namespace distmap
{

Connection::Connection( asio::io_service& service ) :
    m_service( service ), m_socket( service )
{
    DEBUG( "Connection::Connection()" );
}

Connection::~Connection()
{
    if ( m_socket.is_open() )
    {
        DEBUG( "Connection::~Connection() " << m_socket.remote_endpoint() );
    }
    else
    {
        DEBUG( "Connection::~Connection()" );
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
    m_socket.async_read_some( buffer.prepare( 512 ), bind(
            &Connection::handleReceive, this, ptr(), buffer, true, ph::error,
            ph::bytes_transferred ) );
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
        std::istream s( buffer.buffer() );
        uint32_t msgSize;
        s.get( (char*)&msgSize, sizeof(msgSize) );
        msgSize = ntohl( msgSize );
        TRACE( "Msg size: " << msgSize << " -- Read Size: " << size );

        if ( msgSize > size )
        {
            // Schedule a complete read
            asio::async_read( m_socket, buffer.prepare( msgSize - size ), bind(
                    &Connection::handleReceive, this, cnx, buffer, false,
                    ph::error, ph::bytes_transferred ) );
            return;
        }
    }

    TRACE( "Got the complete message" );
    /// Do something....


    receiveMessage();
}

}
