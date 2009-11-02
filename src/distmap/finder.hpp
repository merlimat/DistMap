/*
 * finder.hpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#ifndef FINDER_HPP_
#define FINDER_HPP_

#include <distmap/asio.hpp>

namespace distmap
{

class Configuration;
class Membership;

class Finder
{
public:
    Finder( asio::io_service& service,
            Membership& membership,
            Configuration& conf );
    ~Finder();

    void announceMyself( const std::string& nodeName );
    void nodeIsDown( const std::string& nodeName );

private:
    void receiveMessage();
    void handleReceiveFrom( const sys::error_code& error, size_t size );
    void handleMsgSent( const sys::error_code& error, size_t size );

    asio::io_service& m_service;
    Membership& m_membership;

    udp::socket m_socket;
    udp::endpoint m_senderEndpoint;
    udp::endpoint m_multicastEndpoint;

    boost::array<char, 1024> m_receiveBuffer;
};

}

#endif /* FINDER_HPP_ */
