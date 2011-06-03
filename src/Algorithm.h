/*
Algorithm.h

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


#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include "Common.h"
#include "Network.h"

// Interface Class of a Heaviest Hitters algorithm
class Algorithm {
public:
    virtual void QueryHeaviest(uint k, HittersQueryResult & result) = 0;  // Calculates and returns the k Heaviest Hitters in the provided result container
    virtual void Append(Packet& packet) = 0;  // Executed when a new item is received
    virtual void Expire(Packet& packet) = 0;  // Executed when an item is served
    virtual ~Algorithm() {}  // Does nothing but is required for safe destruction
};

// An Algorithm class that implements the Algorithm interface but does no processing
class NoProcessingAlgorithm: public Algorithm {
public:
    virtual void QueryHeaviest(uint k, HittersQueryResult & result) {}  // Do nothing
    virtual void Append(Packet& packet) {}  // Do nothing
    virtual void Expire(Packet& packet) {}  // Do nothing
};


#endif /* ALGORITHM_H_ */
