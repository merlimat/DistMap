/*
 * log.hpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#ifndef LOG_HPP_
#define LOG_HPP_

#include <distmap/util/timer.hpp>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>

#include <google/utilities.h>

#ifndef NDEBUG
#  define DEBUG(msg)  LOG("DEBUG", msg)
#  define TRACE(msg)  { \
    if ( IsTraceActive(__FILE__) ) \
        LOG("TRACE", msg); \
}
#  define ASSERT(condition)  { \
    if ( ! (condition) ) \
        FATAL( "Assert failed: " #condition ); \
}
#else
#  define DEBUG(msg) {}
#  define TRACE(msg)  {}
#  define ASSERT(condition)  {}
#endif

#define INFO(msg)  LOG("INFO ", msg)
#define WARN(msg)  LOG("WARN ", msg)
#define ERROR(msg) LOG("ERROR", msg)
#define FATAL(msg) { LOG("FATAL", msg << std::endl << " ======= STACK TRACE ======= " ); \
                     google::DumpStackTraceAndExit(); }

#define CHECK(condition) { if (!condition) WARN( "Failed check: " #condition ); }

class LogTimestamp
{
    static Timer s_start_time;
    double m_time;
public:
    LogTimestamp() :
        m_time( s_start_time.elapsed() )
    {
    }

    template<typename Stream>
    friend Stream& operator<<( Stream& s, const LogTimestamp& ts )
    {
        s.setf( std::ios::fixed );
        s << std::setprecision( 6 ) << ts.m_time;
        return s;
    }
};

class SimpleName
{
    const char* m_ptr;
public:
    SimpleName( const char* filename )
    {
        m_ptr = filename + strlen( filename );
        while ( m_ptr > filename && *m_ptr != '/' )
            --m_ptr;
        if ( *m_ptr == '/' )
            ++m_ptr;
    }

    const char* name() const
    {
        return m_ptr;
    }

    template<typename Stream>
    friend Stream& operator<<( Stream& s, const SimpleName& sn )
    {
        return s << sn.m_ptr;
    }
};

bool IsTraceActive( const char* filename );

#define LOG(level,msg) { std::cerr << LogTimestamp() << (" [" level "] (") << SimpleName(__FILE__) << ':' << __LINE__ << ") " << msg << std::endl;  }

#endif /* LOG_HPP_ */
