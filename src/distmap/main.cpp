#include <distmap/configuration.hpp>
#include <distmap/membership.hpp>
#include <distmap/server.hpp>
#include <distmap/util/log.hpp>
#include <signal.h>

#include <distmap/connection_pool.hpp>

#include <google/utilities.h>

using namespace distmap;

asio::io_service service;

void signalHandler( int signo )
{
    INFO( "signal #" << signo << " received" );
    service.stop();
}

int main()
{
    signal( SIGINT, signalHandler );
    signal( SIGQUIT, signalHandler );

    ConnectionPool connectionPool( service );
    Configuration conf( service );

    Membership membership( service, conf, connectionPool );
    Server server( service, conf, membership, connectionPool );

    INFO( "Started Distmap Server" );
    service.run();

    INFO( "Stopped Distmap Server" );

    return 0;
}
