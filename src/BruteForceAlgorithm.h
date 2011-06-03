/*
BruteForceAlgorithm.h

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

#ifndef BRUTEFORCEALGORITHM_Hs_
#define BRUTEFORCEALGORITHM_H_

#include "Common.h"
#include "Algorithm.h"

// Direct Counting Heaviest Hitters algorithm used to compare both performance and accuracy of result against
class BruteForceAlgorithm : public Algorithm {
    // Returns true if p>q, used in sorting
    static bool IsBigger(FlowCountPair p, FlowCountPair q)  { return p.second > q.second; }

public:
    // A vector of FlowP-Count Pairs used in QueryHeaviest as a intermediate sorting container.
    // Kept at class level to amortize initialization/allocation costs
    HittersQueryResult flow_counts;

    // A map (hash table) of Flow->Count which is used in Append and Expire to record the counts
    FlowCountMap flow_count_dict;

    // Executed when a new item is received
    virtual void Append(Packet & packet){
        FlowCountMap::iterator it = flow_count_dict.find(packet.flowp);  // Find the flow
        if(it != flow_count_dict.end())  // If it was found
            (it->second)++;  // Increment the count
        else  // If it wasn't found
            flow_count_dict[packet.flowp]=1;  // Create a new entry with a count of 1
    }

    // Executed when an item is served
    virtual void Expire(Packet& packet){
        FlowCountMap::iterator it = flow_count_dict.find(packet.flowp);  // Find the flow
        if(it->second > 1)  // If this was not the flow's last packet in the queue
            (it->second)--;  // Decrement the count
        else  // If it was the flow's last packet in the queue
            flow_count_dict.erase(it);  // Remove the entry altogether
    }

    // Calculates and returns the k Heaviest Hitters in the provided result container
    virtual void QueryHeaviest(uint k, HittersQueryResult & result){
        if (flow_count_dict.size() == 0)  // If there are no counted items (queue is empty)
            return;

        // Clear flow_counts in preparation for use
        flow_counts.clear();

        // Copy map to vector for sorting on counts
        std::copy(flow_count_dict.begin(), flow_count_dict.end(), back_inserter(flow_counts));

        // Sort from biggest to smallest count
        std::sort(flow_counts.begin(), flow_counts.end(), IsBigger);

        // Now copy the results to the result vector
        HittersQueryResult::iterator cur_count_it = flow_counts.begin();
        for(uint i=0;(i<k) && (cur_count_it!=flow_counts.end()) && (cur_count_it->second>0); ++cur_count_it, ++i){
            result.push_back(*cur_count_it);
        }
    }
};

#endif /* BRUTEFORCEALGORITHM_H_ */
