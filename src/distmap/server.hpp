/*
 * server.hpp
 *
 *  Created on: Oct 27, 2009
 *      Author: mat
 */

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <distmap/asio.hpp>

namespace distmap
{

class Configuration;
class Membership;
class Connection;
class MessageBus;
typedef boost::intrusive_ptr<Connection> ConnectionPtr;

class Server
{
public:
    Server( asio::io_service& service,
            Configuration& conf,
            Membership& membership,
            MessageBus& messageBus );
    ~Server();

private:
    void bindAddress();
    void startAccept();
    void handleAccept( const ConnectionPtr& cnx, const sys::error_code& error );

    asio::io_service& m_service;
    Configuration& m_conf;
    Membership& m_membership;
    MessageBus& m_messageBus;

    tcp::acceptor m_acceptor;
};

}

#endif /* SERVER_HPP_ */
