/*
 * timer.hpp
 *
 *  Created on: Oct 26, 2009
 *      Author: mat
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#if !defined(__APPLE__)

// Use high-precision clock when available

#include <ctime>

class Timer
{
public:
    Timer()
    {
        restart();
    }

    void restart()
    {
        struct timespec ts;
        clock_gettime( CLOCK_REALTIME, &ts );
        m_start = ts.tv_sec + ts.tv_nsec / 1000000000.0;
    }

    double elapsed()
    {
        struct timespec ts;
        clock_gettime( CLOCK_REALTIME, &ts );
        double now = ts.tv_sec + ts.tv_nsec / 1000000000.0;
        return now - m_start;
    }

private:
    double m_start;
};

#else
// Fallback to low-precision timer
#include <boost/timer.hpp>
typedef boost::timer Timer;

#endif

#endif /* TIMER_HPP_ */
