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

class Message;

void serialize( const SharedBuffer& buffer, const Message& message );

}

#endif /* MESSAGE_HPP_ */
