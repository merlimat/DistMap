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

    ClientConnection( asio::io_service& service,
                      MessageBus* messageBus,
                      const std::string& address ) :
        ClientConnectionBase( messageBus ), m_socket( service ), m_address(
                address ), m_error( false )
    {
        TRACE( "ClientConnection::ClientConnection( " << m_address << " )" );
    }

    ~ClientConnection()
    {
        TRACE( "ClientConnection::~ClientConnection( " << m_address << " )" );
    }

    template<typename Callback>
    void connect( const tcp::endpoint& addr, const Callback& callback )
    {
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

    const std::string& address() const
    {
        return m_address;
    }

    bool error() const
    {
        return m_error;
    }

    void keepAlive()
    {
        SharedBuffer buffer( 1 );
        m_socket.async_read_some( buffer, bind(
                &ClientConnection::handleKeepAlive, this, ptr(), ph::error,
                ph::bytes_transferred ) );
    }

    void cancelKeepAlive()
    {
        m_socket.cancel();
    }

private:
    void handleSend( const ClientConnectionPtr&,
                     const SendCallback& callback,
                     const sys::error_code& error,
                     size_t )
    {
        if ( error )
            m_error = true;
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
            m_error = true;
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
            m_error = true;
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

    void handleKeepAlive( const ClientConnectionPtr& cnx,
                          const sys::error_code& error,
                          size_t )
    {
        if ( error && error != asio::error::operation_aborted )
        {
            TRACE( "Connection closed by remote host when in pool. " << m_address );
            m_error = true;
        }
    }

    tcp::socket m_socket;
    std::string m_address;
    bool m_error;
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

    void release( const ClientConnectionPtr& cnx );

private:
    ClientConnectionPtr getConnection( const std::string& node );
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

    typedef std::list<ClientConnectionPtr> ConnectionList;
    typedef std::map<std::string, ConnectionList> Pool;
    Pool m_pool;
};

}

#endif /* MESSAGE_BUS_HPP_ */
