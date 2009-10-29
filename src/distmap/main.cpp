#include <distmap/configuration.hpp>
#include <distmap/membership.hpp>
#include <distmap/server.hpp>
#include <distmap/util/log.hpp>
#include <signal.h>

#include <google/utilities.h>

using namespace distmap;

asio::io_service* s_serviceRef = NULL;

void signalHandler( int signo )
{
    INFO( "signal #" << signo << " received" );
    s_serviceRef->stop();
}

int main()
{
    asio::io_service service;
    s_serviceRef = &service;

    signal( SIGINT, signalHandler );
    signal( SIGQUIT, signalHandler );

    Configuration conf( service );

    Membership membership( service, conf );
    Server server( service, conf, membership );

    INFO( "Started Distmap Server" );
    service.run();

    INFO( "Stopped Distmap Server" );

    return 0;
}
