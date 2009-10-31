/*
 * message.cpp
 *
 *  Created on: Oct 31, 2009
 *      Author: mat
 */

#include "message.hpp"

namespace distmap
{

Message::Message( MessageType type ) :
        m_type( type ),
        m_buffer( 4096 )
{
}

Message::~Message()
{
}

}
