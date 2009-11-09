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
    TRACE( "MessageBus::MessageBus" );
}

MessageBus::~MessageBus()
{
    TRACE( "MessageBus::~MessageBus" );
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
    ConnectionList& list = m_pool[address];

    // Extract from pool
    ClientConnectionPtr cnx;
    while ( !list.empty() )
    {
        cnx = list.front();
        list.pop_front();
        if ( !cnx->error() )
        {
            cnx->cancelKeepAlive();
            cnx->setPool( this );
            TRACE( "Connection found open in pool" );
            return cnx;
        }

        TRACE( "Connection in pool was remotely closed" );
    }

    // An open connection was not found in the pool
    return ClientConnectionPtr( new ClientConnection( m_service, this, address ) );
}

void MessageBus::release( const ClientConnectionPtr& cnx )
{
    cnx->setPool( NULL );
    if ( cnx->error() )
    {
        return;
    }

    TRACE( "Connection released to pool: " << cnx->address() );
    cnx->keepAlive();
    m_pool[cnx->address()].push_front( cnx );
}

}
