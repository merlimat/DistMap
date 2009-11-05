/*
 * message_bus.hpp
 *
 *  Created on: Oct 29, 2009
 *      Author: mat
 */

#ifndef MESSAGE_BUS_HPP_
#define MESSAGE_BUS_HPP_

#include <distmap/asio.hpp>
#include <distmap/util/log.hpp>
#include <boost/unordered_map.hpp>

namespace distmap
{

class ClientConnection;
class MessageBus;

typedef IntrusivePoolBase<ClientConnection, MessageBus> ClientConnectionBase;

class ClientConnection: public ClientConnectionBase
{
public:
    typedef boost::intrusive_ptr<ClientConnection> ClientConnectionPtr;

    typedef boost::function<void( const sys::error_code& )> SendCallback;
    typedef boost::function<void( const sys::error_code&, const SharedBuffer& )>
            SendReceiveCallback;

    ClientConnection( asio::io_service& service, MessageBus& messageBus ) :
        ClientConnectionBase( messageBus ), m_socket( service )
    {
    }

    template<typename Callback>
    void connect( const tcp::endpoint& addr, const Callback& callback )
    {
        m_endpoint = addr;
        m_socket.async_connect( addr, callback );
    }

    void send( const SharedBuffer& buffer, const SendCallback& callback )
    {
        asio::async_write( m_socket, buffer, bind(
                &ClientConnection::handleSend, this, ptr(), callback,
                ph::error, ph::bytes_transferred ) );
    }

    void sendAndReceive( const SharedBuffer& buffer,
                         const SendReceiveCallback& callback )
    {
        asio::async_write( m_socket, buffer, bind(
                &ClientConnection::handleSendReceive, this, ptr(), callback,
                ph::error, ph::bytes_transferred ) );
    }

    const tcp::socket& socket() const
    {
        return m_socket;
    }

    const tcp::endpoint endpoint() const
    {
        return m_endpoint;
    }

private:
    void handleSend( const ClientConnectionPtr&,
                     const SendCallback& callback,
                     const sys::error_code& error,
                     size_t )
    {
        callback( error );
    }

    void handleSendReceive( const ClientConnectionPtr& cnx,
                            const SendReceiveCallback& callback,
                            const sys::error_code& error,
                            size_t )
    {
        if ( error )
        {
            TRACE( "Error writing data: " << error.message() );
            callback( error, SharedBuffer() );
            return;
        }

        // Read the response from server
        SharedBuffer buffer;
        m_socket.async_read_some( buffer, bind(
                &ClientConnection::handleReceive, this, cnx, buffer, callback,
                true, ph::error, ph::bytes_transferred ) );
    }

    void handleReceive( const ClientConnectionPtr& cnx,
                        SharedBuffer& buffer,
                        const SendReceiveCallback& callback,
                        bool isFirstPart,
                        const sys::error_code& error,
                        size_t size )
    {
        if ( error || size == 0 )
        {
            TRACE( "Error reading data: " << error.message() );
            callback( error, buffer );
            return;
        }

        if ( isFirstPart )
        {
            uint32_t msgSize = readMessageSize( buffer );
            TRACE( "Msg size: " << msgSize << " -- Read Size: " << size );

            if ( msgSize > size )
            {
                // Schedule a complete read
                buffer.resize( msgSize );
                asio::async_read( m_socket, asio::buffer( buffer.data() + size,
                        msgSize - size ), bind(
                        &ClientConnection::handleReceive, this, cnx, buffer,
                        callback, false, ph::error, ph::bytes_transferred ) );
                return;
            }
        }

        callback( error, buffer );
    }

    tcp::socket m_socket;
    tcp::endpoint m_endpoint;
};

typedef ClientConnection::ClientConnectionPtr ClientConnectionPtr;

class MessageBus
{
public:
    MessageBus( asio::io_service& service );
    ~MessageBus();

    typedef boost::function<void( const sys::error_code& )> SendCallback;
    typedef boost::function<void( const sys::error_code&, const SharedBuffer& )>
            SendReceiveCallback;

    void send( const std::string& node,
               const SharedBuffer& msg,
               const SendCallback& );
    void sendAndReceive( const std::string& node,
                         const SharedBuffer& msg,
                         const SendReceiveCallback& );

    void release( ClientConnection* cnx );

private:
    void getConnection( const std::string& node );
    tcp::endpoint getEndpointAddress( const std::string& node ) const;

    void handleConnect( const ClientConnectionPtr& cnx,
                        const SharedBuffer& msg,
                        const SendCallback& callback,
                        const sys::error_code& error );
    void handleConnectSendReceive( const ClientConnectionPtr& cnx,
                                   const SharedBuffer& msg,
                                   const SendReceiveCallback& callback,
                                   const sys::error_code& error );

    asio::io_service& m_service;

    typedef std::map<std::string, ClientConnection*> Pool;
    Pool m_pool;
};

}

#endif /* MESSAGE_BUS_HPP_ */
