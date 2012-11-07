/* 
 * File:   Stopwatch.hpp
 * Author: uli
 *
 * Created on 23. August 2012, 02:46
 */

#ifndef STOPWATCH_HPP
#define	STOPWATCH_HPP

#include <boost/chrono/thread_clock.hpp>
#include <boost/chrono/chrono_io.hpp>
#include <string>
#include <iostream>

/**
 * A stopwatch measuring a thread's time using Boost Chrono
 * std::chrono currently doesn't offer a thread clock
 */
class Stopwatch {
public:

    Stopwatch() : duration(0.0), t1(boost::chrono::thread_clock::now()), t2(boost::chrono::thread_clock::now()) {

    }

    void start() {
        t1 = boost::chrono::thread_clock::now();
    }

    void stop() {
        t2 = boost::chrono::thread_clock::now();
        duration += (t2 - t1);
    }

    void printToCout(const std::string& msg) {
        std::cout << msg << " " << duration << std::endl;
    }

    void reset() {
        duration = Duration::zero();
    }
    
    double getMicroseconds() {
	MicroDuration us = MicroDuration::zero();
	us += duration;
	return us.count();
    }
private:
    typedef boost::chrono::duration<double> Duration;
    typedef boost::chrono::duration<double, boost::micro> MicroDuration;
    Duration duration;
    boost::chrono::thread_clock::time_point t1, t2;
};

#endif	/* STOPWATCH_HPP */

