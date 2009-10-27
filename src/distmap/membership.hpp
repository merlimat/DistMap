/*
 * membership.hpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#ifndef MEMBERSHIP_HPP_
#define MEMBERSHIP_HPP_

#include <distmap/asio.hpp>

#include <distmap/finder.hpp>

namespace distmap {


class Membership
{
public:
    Membership( asio::io_service& service );
    ~Membership();

private:
    asio::io_service& m_service;
    Finder m_finder;
};

}  // namespace distmap

#endif /* MEMBERSHIP_HPP_ */
