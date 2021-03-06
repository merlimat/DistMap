/*
 * connection.hpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include <distmap/asio.hpp>

namespace distmap
{

class Membership;

class Connection: public IntrusiveBase<Connection>
{
public:
    typedef boost::intrusive_ptr<Connection> ConnectionPtr;

    Connection( asio::io_service& service, Membership& membership );
    ~Connection();

    void start();
    void receiveMessage();

private:
    tcp::socket& socket();

    void handleReceive( const ConnectionPtr& ptr,
                        SharedBuffer& buffer,
                        bool isFirstPart,
                        const sys::error_code& error,
                        size_t size );

    void handleSend( const ConnectionPtr& ptr,
                     const sys::error_code& error,
                     size_t size );

    asio::io_service& m_service;
    Membership& m_membership;
    tcp::socket m_socket;

    friend class Server;
};

typedef Connection::ConnectionPtr ConnectionPtr;

}

#endif /* CONNECTION_HPP_ */
