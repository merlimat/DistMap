/*
 * connection_pool.hpp
 *
 *  Created on: Oct 29, 2009
 *      Author: mat
 */

#ifndef CONNECTION_POOL_HPP_
#define CONNECTION_POOL_HPP_

#include <distmap/asio.hpp>
#include <distmap/util/log.hpp>
#include <map>

namespace distmap
{

class ClientConnection: public IntrusiveBase<ClientConnection>
{
public:
    typedef boost::intrusive_ptr<ClientConnection> ClientConnectionPtr;

    // typedef boost::function<void( const sys::error_code&, const SharedBuffer& )>
    //        Callback;

    ClientConnection( asio::io_service& service ) :
        m_socket( service )
    {
    }

    template<typename Callback>
    void connect( const tcp::endpoint& addr, const Callback& callback )
    {
        m_socket.async_connect( addr, callback );
    }

    template<typename Buffer, typename Callback>
    void send( const Buffer& buffer, const Callback& callback )
    {
        asio::async_write( m_socket, buffer, bind(
                &ClientConnection::handleSimpleSend, this, ptr(), callback,
                ph::error, ph::bytes_transferred ) );
    }

    template<typename Buffer, typename Callback>
    void sendAndReceive( const Buffer& buffer, const Callback& callback )
    {
        asio::async_write( m_socket, buffer, bind(
                &ClientConnection::handleSend, this, ptr(), callback,
                ph::error, ph::bytes_transferred ) );
    }

    const tcp::socket& socket() const
    {
        return m_socket;
    }

private:
    template<typename Callback>
    void handleSimpleSend( const ClientConnectionPtr& ,
                           const Callback& callback,
                           const sys::error_code& error,
                           size_t )
    {
        callback( error );
    }

    template<typename Callback>
    void handleSend( const ClientConnectionPtr& cnx,
                     const Callback& callback,
                     const sys::error_code& error,
                     size_t size )
    {
        if ( error )
        {
            TRACE( "Error writing data: " << error.message() );
            callback( error, SharedBuffer() );
            return;
        }

        // Read the response from server
        SharedBuffer buffer( 4096 );
        m_socket.async_read_some( buffer, bind(
                &ClientConnection::handleReceive, this, cnx, buffer, callback,
                true, ph::error, ph::bytes_transferred ) );
    }

    template<typename Callback>
    void handleReceive( const ClientConnectionPtr& cnx,
                        SharedBuffer& buffer,
                        const Callback& callback,
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
            size_t msgSize = ntohl( *(uint32_t*) buffer.data() );
            TRACE( "Msg size: " << msgSize << " -- Read Size: " << size );
            buffer.resize( msgSize );

            if ( msgSize > size )
            {
                // Schedule a complete read
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
};

typedef ClientConnection::ClientConnectionPtr ClientConnectionPtr;

class ConnectionPool
{
public:
    ConnectionPool( asio::io_service& service );
    ~ConnectionPool();

    typedef boost::function<void( const sys::error_code& )> SendCallback;
    typedef boost::function<void( const sys::error_code&, const SharedBuffer& )>
            SendReceiveCallback;

    void send( const std::string& node, SharedBuffer& msg, const SendCallback& );
    void sendAndReceive( const std::string& node,
                         const SharedBuffer& msg,
                         const SendReceiveCallback& );

private:
    tcp::endpoint getEndpointAddress( const std::string& node ) const;

    void handleConnect( const ClientConnectionPtr& cnx,
                        SharedBuffer& msg,
                        const SendCallback& callback,
                        const sys::error_code& error );
    asio::io_service& m_service;
};

}

#endif /* CONNECTION_POOL_HPP_ */
