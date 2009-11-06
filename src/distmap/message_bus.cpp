/*
 * message_bus.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: mat
 */

#include "message_bus.hpp"

#include <distmap/util/log.hpp>

namespace distmap
{

MessageBus::MessageBus( asio::io_service& service ) :
    m_service( service )
{
    TRACE( "ConnectionPool::ConnectionPool" );
}

MessageBus::~MessageBus()
{
    TRACE( "ConnectionPool::~ConnectionPool" );
}

void MessageBus::send( const std::string& node,
                       const SharedBuffer& msg,
                       const SendCallback& callback )
{
    tcp::endpoint endpoint = getEndpointAddress( node );
    ClientConnectionPtr cnx = getConnection( node );
    cnx->connect( endpoint, bind( &MessageBus::handleConnect, this, cnx, msg,
            callback, ph::error ) );
}

void MessageBus::sendAndReceive( const std::string& node,
                                 const SharedBuffer& msg,
                                 const SendReceiveCallback& callback )
{
    tcp::endpoint endpoint = getEndpointAddress( node );
    ClientConnectionPtr cnx = getConnection( node );
    cnx->connect( endpoint, bind( &MessageBus::handleConnectSendReceive, this,
            cnx, msg, callback, ph::error ) );
}

void MessageBus::handleConnect( const ClientConnectionPtr& cnx,
                                const SharedBuffer& msg,
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

void MessageBus::handleConnectSendReceive( const ClientConnectionPtr& cnx,
                                           const SharedBuffer& msg,
                                           const SendReceiveCallback& callback,
                                           const sys::error_code& error )
{
    if ( error )
    {
        DEBUG( "Error connecting to " << cnx->address() );
        callback( error, SharedBuffer() );
        return;
    }

    cnx->sendAndReceive( msg, callback );
}

tcp::endpoint MessageBus::getEndpointAddress( const std::string& node ) const
{
    size_t idx = node.find( ':' );
    std::string ipAddress = node.substr( 0, idx );
    uint16_t port = atoi( node.substr( idx + 1 ).c_str() );
    TRACE( "ClientConnection " << ipAddress << ':' << port );
    return tcp::endpoint( ip::address::from_string( ipAddress.c_str() ), port );
}

ClientConnectionPtr MessageBus::getConnection( const std::string& address )
{
    Pool::iterator it = m_pool.find( address );
    if ( it == m_pool.end() )
        return ClientConnectionPtr( new ClientConnection( m_service, *this,
                address ) );

    // Extract from pool
    ClientConnection* cnx = it->second;
    if ( cnx->next() != NULL )
    {
        m_pool.insert( std::make_pair( address, cnx->next() ) );
        cnx->setNext( NULL );
    }

    return ClientConnectionPtr( cnx );
}

void MessageBus::release( ClientConnection* cnx )
{
    TRACE( "Connection released to pool: " << cnx->address() );
    std::pair<Pool::iterator, bool> res = m_pool.insert( std::make_pair(
            cnx->address(), cnx ) );
    if ( res.second )
    {
        // There was anothe connection in the list, enqueue
        ClientConnection* oldCnx = res.first->second;
        cnx->setNext( oldCnx );
    }
}

}
