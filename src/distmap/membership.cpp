/*
 * membership.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "membership.hpp"

#include <distmap/util/log.hpp>

Membership::Membership( asio::io_service& service ) :
    m_service( service )
{
    TRACE( "Membership::Membership()" );
}

Membership::~Membership()
{
    TRACE( "Membership::~Membership()" );
}
