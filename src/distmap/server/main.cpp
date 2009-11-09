#include <distmap/configuration.hpp>
#include <distmap/membership.hpp>
#include <distmap/message_bus.hpp>
#include <distmap/server.hpp>
#include <distmap/util/log.hpp>
#include <signal.h>

#include <google/utilities.h>

#include <distmap/distmap.pb.h>
#include <distmap/util/util.hpp>

using namespace distmap;

asio::io_service service;

void signalHandler( int signo )
{
    INFO( "signal #" << signo << " received" );
    service.stop();
}

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    signal( SIGINT, signalHandler );
    signal( SIGQUIT, signalHandler );

    MessageBus connectionPool( service );
    Configuration conf( service );

    Membership membership( service, conf, connectionPool );
    Server server( service, conf, membership, connectionPool );

    INFO( "Started Distmap Server" );
    service.run();

    INFO( "Stopped Distmap Server" );

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
