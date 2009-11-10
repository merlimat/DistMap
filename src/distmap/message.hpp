/*
 * message.hpp
 *
 *  Created on: Oct 31, 2009
 *      Author: mat
 */

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <distmap/asio.hpp>
#include <distmap/distmap.pb.h>

#include <set>

namespace distmap
{

typedef std::set<std::string> StringSet;

SharedBuffer CreateNodeListMsg( const StringSet& stringSet );
SharedBuffer CreateAnnounceMsg( const std::string& node );

}

#endif /* MESSAGE_HPP_ */
