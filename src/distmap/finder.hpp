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

class FinderCommand
{
public:
    enum Type
    {
        Invalid = 0, Announce = 1, NodeIsDown = 2
    };

    FinderCommand( Type type, const std::string& node );

    Type type() const
    {
        return m_type;
    }

    const std::string& node() const
    {
        return m_node;
    }

    size_t serialize( char* buffer, size_t size );
    static FinderCommand parse( const char* buffer, size_t size );

private:
    Type m_type;
    std::string m_node;
};

////////////////////////////////////////////////////////////////////////////////

class Membership;

class Finder
{
public:
    Finder( asio::io_service& service, Membership& membership );
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

    boost::array<char,1024> m_receiveBuffer;
};

}

#endif /* FINDER_HPP_ */
