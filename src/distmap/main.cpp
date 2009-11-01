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
/*
    MessageBus connectionPool( service );
    Configuration conf( service );

    Membership membership( service, conf, connectionPool );
    Server server( service, conf, membership, connectionPool );

    INFO( "Started Distmap Server" );
    service.run();

    INFO( "Stopped Distmap Server" );
*/
    Message msg;
    msg.set_type( Message::Ping );
    msg.mutable_ping();
/*
    NodeList* nodeList = msg.mutable_nodelist();
    nodeList->add_node( std::string("pepe") );
    nodeList->add_node( "manolo" );
    // nodeList->add_node( "fulano" );
*/
    INFO( "Msg size: " << msg.ByteSize() );

    asio::streambuf streambuf( 512 );
    std::ostream out( &streambuf );

    msg.SerializeToOstream(&out);

    char buffer[1024];
    size_t size = streambuf.sgetn( buffer, sizeof(buffer) );
    dumpBuffer( std::cout, buffer, size );

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
