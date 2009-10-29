/*
 * configuration.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#include "configuration.hpp"

#include <distmap/util/log.hpp>
#include <sstream>

namespace distmap
{

Configuration::Configuration( asio::io_service& service ) :
    m_service( service ), m_multicastChannel(), m_listenIP()
{
    m_multicastChannel = udp::endpoint(
            ip::address::from_string( "226.0.0.1" ), 22000 );
    m_listenIP = discoverExternalIP();
    m_listenPort = 22000;
    m_portIsFixed = false;

    std::stringstream ss;
    ss << "node_" << getpid() << '@' << m_listenIP;
    m_nodeName = ss.str();
}

Configuration::~Configuration()
{
    TRACE( "Configuration::~Configuration()" );
}

ip::address Configuration::discoverExternalIP()
{
    TRACE( "Multicast address: " << m_multicastChannel );
    udp::socket socket( m_service );
    socket.connect( m_multicastChannel );
    udp::endpoint endpoint = socket.local_endpoint();
    DEBUG( "Local address is: " << endpoint.address() );
    return endpoint.address();
}

}
