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

void ConnectionPool::connect( const std::string& nodeName,
                              const ClientConnectionCallback& callback )
{
    size_t idx = nodeName.find( ':' );
    std::string ipAddress = nodeName.substr( 0, idx );
    uint16_t port = atoi( nodeName.substr( idx + 1 ).c_str() );
    TRACE( "ClientConnection " << ipAddress << ':' << port );
    tcp::endpoint
            endpoint( ip::address::from_string( ipAddress.c_str() ), port );

    ClientConnectionPtr cnx( new ClientConnection( m_service ) );
    cnx->connect( endpoint, bind( &ConnectionPool::handleConnect, this, cnx,
            callback, ph::error ) );
}

void ConnectionPool::handleConnect( const ClientConnectionPtr& cnx,
                                    const ClientConnectionCallback& callback,
                                    const sys::error_code& error )
{
    callback( error, cnx );
}

void ConnectionPool::release( const ClientConnectionPtr& cnx )
{
    DEBUG( "Release connection to " << cnx->socket().remote_endpoint() );
}

}
