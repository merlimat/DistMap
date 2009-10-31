/*
 * connection_pool.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: mat
 */

#include "connection_pool.hpp"

#include <distmap/util/log.hpp>

namespace distmap
{

ConnectionPool::ConnectionPool( asio::io_service& service ) :
    m_service( service )
{
    TRACE( "ConnectionPool::ConnectionPool" );
}

ConnectionPool::~ConnectionPool()
{
    TRACE( "ConnectionPool::~ConnectionPool" );
}

void ConnectionPool::send( const std::string& node,
                           SharedBuffer& msg,
                           const SendCallback& callback )
{
    tcp::endpoint endpoint = getEndpointAddress( node );
    ClientConnectionPtr cnx( new ClientConnection( m_service ) );
    cnx->connect( endpoint, bind( &ConnectionPool::handleConnect, this, cnx, msg,
            callback, ph::error ) );
}

void ConnectionPool::sendAndReceive( const std::string& node,
                                     const SharedBuffer& msg,
                                     const SendReceiveCallback& )
{
}

void ConnectionPool::handleConnect( const ClientConnectionPtr& cnx,
                                    SharedBuffer& msg,
                                    const SendCallback& callback,
                                    const sys::error_code& error )
{
    if ( error )
    {
        DEBUG( "Error connecting to " << cnx->socket().remote_endpoint() );
        callback( error );
        return;
    }

    cnx->send( msg, callback );
}

tcp::endpoint ConnectionPool::getEndpointAddress( const std::string& node ) const
{
    size_t idx = node.find( ':' );
    std::string ipAddress = node.substr( 0, idx );
    uint16_t port = atoi( node.substr( idx + 1 ).c_str() );
    TRACE( "ClientConnection " << ipAddress << ':' << port );
    return tcp::endpoint( ip::address::from_string( ipAddress.c_str() ), port );
}

}
