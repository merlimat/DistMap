/*
 * monitor.hpp
 *
 *  Created on: Nov 10, 2009
 *      Author: mat
 */

#ifndef MONITOR_HPP_
#define MONITOR_HPP_

#include <distmap/asio.hpp>

namespace distmap
{

class Membership;
class MessageBus;

class Monitor
{
public:
    Monitor( asio::io_service& service,
             Membership& membership,
             MessageBus& messageBus );
    ~Monitor();

    void startMonitoring( const std::string& node );
    void stopMonitoring();

private:
    void handlePing( const std::string& node,
                     const sys::error_code& error,
                     const SharedBuffer& buffer );
    void handleTimeout( const std::string& node, const sys::error_code& error );
    void handleSendNextPing( const std::string& node, const sys::error_code& error );

    asio::deadline_timer m_timer;
    Membership& m_membership;
    MessageBus& m_messageBus;
    std::string m_monitoredNode;
};

} // namespace distmap


#endif /* MONITOR_HPP_ */
