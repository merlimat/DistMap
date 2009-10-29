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
}

}
