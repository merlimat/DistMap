/*
 * finder.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "finder.hpp"

#include <distmap/configuration.hpp>
#include <distmap/membership.hpp>
#include <distmap/message.hpp>
#include <distmap/util/log.hpp>
#include <distmap/distmap.pb.h>

namespace distmap
{

Finder::Finder( asio::io_service& service,
                Membership& membership,
                Configuration& conf ) :
    m_service( service ), m_membership( membership ), m_socket( service ),
            m_senderEndpoint()
{
    TRACE( "Finder::Finder" );
    m_multicastEndpoint = conf.multicastChannel();
    udp::endpoint listenEndpoint( ip::address_v4::any(),
            conf.multicastChannel().port() );

    m_socket.open( listenEndpoint.protocol() );
    m_socket.set_option( udp::socket::reuse_address( true ) );
    m_socket.bind( listenEndpoint );

    // Join the multicast group.
    m_socket.set_option( ip::multicast::join_group(
            m_multicastEndpoint.address() ) );

    TRACE( "UDP listen address: " << listenEndpoint );
    receiveMessage();
}

Finder::~Finder()
{
    TRACE( "Finder::~Finder" );
}

void Finder::receiveMessage()
{
    m_socket.async_receive_from( asio::buffer( m_receiveBuffer ),
            m_senderEndpoint, bind( &Finder::handleReceiveFrom, this,
                    ph::error, ph::bytes_transferred ) );
}

void Finder::handleReceiveFrom( const sys::error_code& error, size_t size )
{
    TRACE( "Finder::handleReceiveFrom. error=" << error.message() << " size=" << size );
    if ( !error )
    {
        Message msg;
        msg.ParseFromArray( m_receiveBuffer.data(), size );
        switch ( msg.type() )
        {
        case Message::Announce:
            m_membership.receivedAnnounce( msg.announce().node() );
            break;

        default:
            WARN( "Invalid message received from multicast channel." );
            break;
        }
    }

    receiveMessage();
}

void Finder::announceMyself( const std::string& nodeName )
{
    SharedBuffer data = CreateAnnounceMsg( nodeName );
    m_socket.async_send_to( data, m_multicastEndpoint, bind(
            &Finder::handleMsgSent, this, ph::error, ph::bytes_transferred ) );
    TRACE( "announce message sent." );
}

void Finder::nodeIsDown( const std::string& )
{
}

void Finder::handleMsgSent( const sys::error_code& error, size_t size )
{
    TRACE( "Msg sent. error='" << error.message() << "' size=" << size );
}

}
