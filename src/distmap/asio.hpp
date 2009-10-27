/*
 * asio.hpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#ifndef ASIO_HPP_
#define ASIO_HPP_

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>

namespace distmap
{

namespace asio = boost::asio;
namespace ip = boost::asio::ip;
using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace ph = boost::asio::placeholders;

using boost::bind;
namespace sys = boost::system;

}

#endif /* ASIO_HPP_ */
