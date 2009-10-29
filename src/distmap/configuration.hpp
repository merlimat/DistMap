/*
 * configuration.hpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>
#include <distmap/asio.hpp>

namespace distmap
{

class Configuration
{
public:
    Configuration( asio::io_service& service );
    ~Configuration();

    const udp::endpoint& multicastChannel() const
    {
        return m_multicastChannel;
    }

    const ip::address& listenIP() const
    {
        return m_listenIP;
    }

    uint16_t listenPort() const
    {
        return m_listenPort;
    }

    bool portIsFixed() const
    {
        return m_portIsFixed;
    }

    const std::string& nodeName() const
    {
        return m_nodeName;
    }

    void setListenEndpoint( const ip::address& addr, uint16_t port )
    {
        m_listenIP = addr;
        m_listenPort = port;
    }

private:
    ip::address discoverExternalIP();

    asio::io_service& m_service;
    udp::endpoint m_multicastChannel;
    ip::address m_listenIP;
    uint16_t m_listenPort;

    bool m_portIsFixed;

    std::string m_nodeName;
};

}

#endif /* CONFIGURATION_HPP_ */
