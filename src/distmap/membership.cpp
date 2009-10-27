/*
 * membership.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "membership.hpp"

#include <distmap/util/log.hpp>

namespace distmap {

Membership::Membership( asio::io_service& service ) :
    m_service( service ),
    m_finder( service, *this )
{
    TRACE( "Membership::Membership()" );

    m_finder.discoverExternalIP();
}

Membership::~Membership()
{
    TRACE( "Membership::~Membership()" );
}



}  // namespace distmap
