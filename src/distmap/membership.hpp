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

namespace distmap
{

class Configuration;
class MessageBus;

class Membership
{
public:
    Membership( asio::io_service& service,
                Configuration& conf,
                MessageBus& connectionPool );
    ~Membership();

    const std::string& selfNode();

    void setNodeName( const std::string& nodeName )
    {
        m_node = nodeName;
    }

    void announce();

    void receivedAnnounce( const std::string& nodeName );

private:
    void announceTimeout( const sys::error_code& );

    asio::io_service& m_service;
    Configuration& m_conf;
    MessageBus& m_messageBus;
    Finder m_finder;

    asio::deadline_timer m_announceTimer;

    std::string m_node;
};

} // namespace distmap

#endif /* MEMBERSHIP_HPP_ */
