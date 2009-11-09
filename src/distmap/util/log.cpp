/*
 * log.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#include "log.hpp"

#include <distmap/asio.hpp>

#include <stdlib.h>
#include <set>

Timer LogTimestamp::s_start_time;

static boost::char_separator<char> sep( "," );
typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;

class LogInitializer
{
public:
    LogInitializer()
    {
        char* s = getenv( "TRACE" );
        if ( s != NULL )
        {
            std::string str( s );
            Tokenizer tokens( str, sep );
            foreach( std::string t, tokens )
            {
                m_set.insert( t );
            }
        }
    }

    bool isActive( const std::string& name )
    {
        return m_set.find( name ) != m_set.end();
    }

private:
    std::set<std::string> m_set;
};

static LogInitializer logInitializer;

bool IsTraceActive( const char* filename )
{
    SimpleName sname( filename );
    std::string s = sname.name();
    s = s.substr( 0, s.find_last_of( "." ) );
    return logInitializer.isActive( s );
}
