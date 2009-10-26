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

#ifndef NDEBUG
#  define DEBUG(msg)  LOG("DEBUG", msg)
#  define TRACE(msg)  LOG("TRACE", msg)
#else
#  define DEBUG(msg) {}
#  define TRACE(msg)  {}
#endif

#define INFO(msg)  LOG("INFO ", msg)
#define WARN(msg)  LOG("WARN ", msg)
#define ERROR(msg) LOG("ERROR", msg)

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

    template<typename Stream>
    friend Stream& operator<<( Stream& s, const SimpleName& sn )
    {
        return s << sn.m_ptr;
    }
};

#define LOG(level,msg) { std::cerr << LogTimestamp() << (" [" level "] (") << SimpleName(__FILE__) << ':' << __LINE__ << ") " << msg << std::endl;  }

#endif /* LOG_HPP_ */
