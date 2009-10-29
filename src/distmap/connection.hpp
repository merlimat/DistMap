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

class Connection : public IntrusiveBase<Connection>
{
public:
    Connection( asio::io_service& service );
    ~Connection();

    void start();

private:
    tcp::socket& socket();

    asio::io_service& m_service;
    tcp::socket m_socket;

    friend class Server;
};

typedef boost::intrusive_ptr<Connection> ConnectionPtr;

}

#endif /* CONNECTION_HPP_ */
