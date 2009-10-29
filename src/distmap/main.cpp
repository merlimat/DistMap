
#include <distmap/configuration.hpp>
#include <distmap/membership.hpp>
#include <distmap/server.hpp>
#include <distmap/util/log.hpp>

#include <google/utilities.h>

using namespace distmap;

int main()
{
    asio::io_service service;
    Configuration conf( service );

    Membership membership( service, conf );
    Server server( service, conf, membership );

    INFO( "Started Distmap Server" );
    service.run();

    return 0;
}
