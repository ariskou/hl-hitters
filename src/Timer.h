/*
Timer.h

Copyright 2011 Remous-Aris Koutsiamanis

This file is part of HL-Hitters.

HL-Hitters is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

HL-Hitters is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with HL-Hitters.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TIMER_H_
#define TIMER_H_

#include "Common.h"

// Implements a timer which can count the duration of one event
class OneShotTimer{
    boost::posix_time::ptime requestStart, requestEnd;
public:
    // Starts timing
    void Start(){
        requestStart = boost::posix_time::microsec_clock::universal_time();
    }
    // Stops timing
    void Stop(){
        requestEnd = boost::posix_time::microsec_clock::universal_time();
    }

    // Return the timed duration in seconds
    double Duration(){
        boost::posix_time::time_duration dur = requestEnd - requestStart;
        return dur.total_seconds() + (dur.total_microseconds()/1e6);
    }
};


// Implements a timer which can count the duration of multiple event and provide their statistical properties
class MultiShotTimer{
    std::vector<double> durations;  // Records the durations
    OneShotTimer timer;  // Use a single shot timer
public:
    // Starts timing one event
    void Start(){
        timer.Start();
    }

    // Stops timing the event
    void Stop(){
        timer.Stop();
        durations.push_back(timer.Duration());
    }

    // Returns the number of timings made
    uint Count(){
        return durations.size();
    }

    // Returns the sum of the timings
    double Sum(){
        double sum=0.0;
        foreach(double d, durations) sum+=d;
        return sum;
    }

    // Returns the sum of the square of the timings
    double Sum2(){
        double sum=0.0;
        foreach(double d, durations) sum+=d*d;
        return sum;
    }

    // Returns the mean of the timings
    double Mean(){
        return Sum()/Count();
    }

    // Returns the mean of the square of the timings
    double Mean2(){
        return Sum2()/Count();
    }

    // Returns the variance of the timings
    double Variance(){
        return Mean2()-Mean()*Mean();
    }

    // Returns the standard deviation of the timings
    double StandardDeviation(){
        return sqrt(Variance());
    }

    friend std::ostream& operator<< (std::ostream &, MultiShotTimer &);
};

// Helper function for printing
std::ostream& operator<< (std::ostream &out, MultiShotTimer &mst){
    out << "Count:" << mst.Count() << ", Mean:" << mst.Mean() << ", StDev:" << mst.StandardDeviation() << ", Sum:" << mst.Sum()
        << ", Mean2:" << mst.Mean2() << ", Variance:" << mst.Variance() << ", Sum2:" << mst.Sum2();
    return out;
}

#endif /* TIMER_H_ */
