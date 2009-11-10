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

/// STL Logging facilities

template<typename Stream, typename First, typename Second>
inline Stream& operator<<( Stream& out, const std::pair<First, Second>& p )
{
    return out << '(' << p.first << ", " << p.second << ')';
}

template<typename Stream, class Iter>
inline void PrintSequence( Stream& out, Iter begin, Iter end )
{
    using ::operator<<;
    // Output at most 100 elements -- appropriate if used for logging.
    for ( int i = 0; begin != end && i < 100; ++i, ++begin )
    {
        if ( i > 0 )
            out << ", ";
        out << *begin;
    }
    if ( begin != end )
    {
        out << " ...";
    }
}

#define OUTPUT_TWO_ARG_CONTAINER(Sequence) \
template<typename Stream, typename T1, typename T2> \
inline Stream& operator<<( Stream& out, \
                                const Sequence<T1, T2>& seq) { \
	out << '['; \
	PrintSequence(out, seq.begin(), seq.end()); \
	out << ']'; \
	return out; \
}

#include <vector>
#include <deque>
#include <list>

OUTPUT_TWO_ARG_CONTAINER(std::vector)
OUTPUT_TWO_ARG_CONTAINER(std::deque)
OUTPUT_TWO_ARG_CONTAINER(std::list)

#define OUTPUT_THREE_ARG_CONTAINER(Sequence) \
template<typename Stream, typename T1, typename T2, typename T3> \
inline Stream& operator<<( Stream& out, \
                           const Sequence<T1, T2, T3>& seq) { \
    out << '{'; \
    PrintSequence(out, seq.begin(), seq.end()); \
    out << '}'; \
    return out; \
}

#include <set>

OUTPUT_THREE_ARG_CONTAINER(std::set)
OUTPUT_THREE_ARG_CONTAINER(std::multiset)

#undef OUTPUT_THREE_ARG_CONTAINER

#define OUTPUT_FOUR_ARG_CONTAINER(Sequence) \
template<typename Stream, typename T1, typename T2, typename T3, typename T4> \
inline Stream& operator<<( Stream& out, \
                           const Sequence<T1, T2, T3, T4>& seq) { \
	out << '{'; \
	PrintSequence(out, seq.begin(), seq.end()); \
	out << '}'; \
	return out; \
}

#include <map>

OUTPUT_FOUR_ARG_CONTAINER(std::map)
OUTPUT_FOUR_ARG_CONTAINER(std::multimap)


#endif /* LOG_HPP_ */
