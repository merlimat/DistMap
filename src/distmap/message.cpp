/*
 * message.cpp
 *
 *  Created on: Nov 10, 2009
 *      Author: mat
 */

#include "message.hpp"

namespace distmap
{

SharedBuffer CreateNodeListMsg( const StringSet& stringSet )
{
    Message msg;
    msg.set_type( Message::NodeList );
    NodeList* nodeList = msg.mutable_nodelist();

    foreach ( const std::string& node, stringSet )
        nodeList->add_node( node );

    return writeMessageWithSize( msg );
}

SharedBuffer CreateAnnounceMsg( const std::string& node )
{
    Message msg;
    msg.set_type( Message::Announce );
    msg.mutable_announce()->set_node( node );

    return writeMessage( msg );
}

SharedBuffer CreatePingMsg( const StringSet& nodeList )
{
    Message msg;
    msg.set_type( Message::Ping );

    Ping* ping = msg.mutable_ping();
    if ( !nodeList.empty() )
    {
        NodeList* list = ping->mutable_nodelist();

        foreach ( const std::string& node, nodeList )
            list->add_node( node );
    }
    return writeMessageWithSize( msg );
}

SharedBuffer CreatePongMsg( const StringSet& nodeList )
{
    Message msg;
    msg.set_type( Message::Pong );

    Pong* pong = msg.mutable_pong();
    if ( !nodeList.empty() )
    {
        NodeList* list = pong->mutable_nodelist();

        foreach ( const std::string& node, nodeList )
            list->add_node( node );
    }

    return writeMessageWithSize( msg );
}

} // namespace distmap
