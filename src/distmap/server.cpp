/*
 * server.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#include "server.hpp"

#include <distmap/configuration.hpp>
#include <distmap/connection.hpp>
#include <distmap/membership.hpp>
#include <distmap/util/log.hpp>

namespace distmap
{

Server::Server( asio::io_service& service,
                Configuration& conf,
                Membership& membership,
                MessageBus& messageBus ) :
    m_service( service ), m_conf( conf ), m_membership( membership ),
            m_messageBus( messageBus ), m_acceptor( service )
{
    TRACE( "Server::Server()" );

    bindAddress();
    TRACE( "Listening on TCP: " << m_acceptor.local_endpoint() );

    startAccept();

    std::stringstream ss;
    ss << m_acceptor.local_endpoint();
    m_membership.setNodeName( ss.str() );
    m_membership.announce();
}

Server::~Server()
{
    TRACE( "Server::~Server()" );
}

void Server::bindAddress()
{
    uint16_t port = m_conf.listenPort();
    uint16_t maxPort;
    if ( m_conf.portIsFixed() )
        maxPort = port;
    else
        maxPort = port + 1000;

    if ( m_conf.listenIP().is_v4() )
        m_acceptor.open( tcp::v4() );
    else
        m_acceptor.open( tcp::v6() );

    sys::error_code ec;

    do
    {
        tcp::endpoint localAddr( m_conf.listenIP(), port );
        m_acceptor.bind( localAddr, ec );
        if ( ec )
        {
            TRACE( "Failed to bind to " << localAddr << ": " << ec.message() );
        }

        ++port;
    } while ( ec && port < maxPort );

    if ( ec )
    {
        ERROR( "Failed to bind to address: "
                << tcp::endpoint( m_conf.listenIP(), port-1)
                << " : " << ec.message() );
    }
    else
    {
        m_conf.setListenEndpoint( m_acceptor.local_endpoint().address(),
                m_acceptor.local_endpoint().port() );
    }
}

void Server::startAccept()
{
    ConnectionPtr cnx( new Connection( m_service ) );

    m_acceptor.async_accept( cnx->socket(), bind( &Server::handleAccept, this,
            cnx, ph::error ) );
}

void Server::handleAccept( const ConnectionPtr& cnx,
                           const sys::error_code& error )
{
    TRACE( "handleAccept() error=" << error.message() );
    if ( !error )
    {
        cnx->start();
        startAccept();
    }
}

}
