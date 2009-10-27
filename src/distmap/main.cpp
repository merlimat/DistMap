#include <distmap/util/log.hpp>

#include <distmap/membership.hpp>

int main()
{
    INFO( "Started Distmap Server" );

    distmap::asio::io_service service;

    distmap::Membership m( service );

    service.run();

    return 0;
}
