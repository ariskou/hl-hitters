/*
Common.h

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

#ifndef COMMON_H_
#define COMMON_H_


#include <deque>
#include <vector>
#include <map>
#include <list>
#include <utility>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include <boost/unordered_map.hpp>  // For a hash table implementation of a map
#include <boost/lambda/lambda.hpp>  // Lambda expression support
#include <boost/lambda/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>  // To make for loops more intuitive
#include <boost/assign/std/vector.hpp>  // For easily creating vectors with data
#include <boost/date_time/posix_time/posix_time_types.hpp>  // For timing execution

#define foreach         BOOST_FOREACH
#define reverse_foreach BOOST_REVERSE_FOREACH

typedef unsigned int uint; // To avoid signed/unsigned comparison warnings, we never actually need signed ints

// Print lists
template <class T>
std::ostream& operator<< (std::ostream &out, std::list<T> &l){
    out << "L(" << l.size() << "): ";
    foreach(T & t, l)
        out << t << " <-> ";
    out << "End";
    return out;
}

// Print vectors
template <class T>
std::ostream& operator<< (std::ostream &out, std::vector<T> &v){
    out << "V(" << v.size() << "): ";
    uint i=0;
    foreach(T & t, v){
        out << i << ":" << t << ", ";
        i++;
    }
    out << "End";
    return out;
}

// Print double ended queues
template <class T>
std::ostream& operator<< (std::ostream &out, std::deque<T> &d){
    out << "Q(" << d.size() << "): ";
    uint i=0;
    foreach(T & t, d){
        out << i << ":" << t << ", ";
        i++;
    }
    out << "End";
    return out;
}


// Print unordered (hash tabled based) maps
template <class K, class V>
std::ostream& operator<< (std::ostream &out, boost::unordered_map<K,V> &m){
    out << "M(" << m.size() << "): ";
    typename boost::unordered_map<K,V>::iterator it = m.begin();
    for(; it!=m.end(); ++it)
        out << *it <<", ";
    out << "End";
    return out;
}


#endif /* GENERAL_DECLS_H_ */
