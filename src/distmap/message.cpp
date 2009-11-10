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

    for ( StringSet::const_iterator it = stringSet.begin(), end =
            stringSet.end(); it != end; ++it )
        nodeList->add_node( *it );

    return writeMessageWithSize( msg );
}

SharedBuffer CreateAnnounceMsg( const std::string& node )
{
    Message msg;
    msg.set_type( Message::Announce );
    msg.mutable_announce()->set_node( node );

    return writeMessage( msg );
}

} // namespace distmap
