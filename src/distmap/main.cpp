#include <distmap/util/log.hpp>

#include <distmap/membership.hpp>

int main()
{
    INFO( "Started Distmap Server" );

    asio::io_service service;

    Membership m( service );

    service.run();

    return 0;
}
