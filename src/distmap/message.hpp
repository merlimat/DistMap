/*
 * message.hpp
 *
 *  Created on: Oct 31, 2009
 *      Author: mat
 */

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <distmap/asio.hpp>

namespace distmap
{

enum MessageType
{
    MessagePing,
    MessagePong,
    MessageNodeIsDown,
    MessageNodeList,

    MessageGet,
    MessageSet,
};

class Message
{
public:
    Message( MessageType type );
    ~Message();

private:
    MessageType m_type;
    SharedBuffer m_buffer;
};

}

#endif /* MESSAGE_HPP_ */
