/*
HLHittersAlgorithm.h

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


#ifndef HLHITTERSALGORITHM_H_
#define HLHITTERSALGORITHM_H_

#include "Common.h"
#include "Algorithm.h"

// Implements the HL-Hitters data structure and algorithms
class HLHittersAlgorithm : public Algorithm {
public:
    // Used in the FlowCount List as the data element
    struct FlowCount{
        FlowP flowp;
        uint count;
    };
    typedef std::list<FlowCount> FlowCountList;  // the doubly linked list type in the data structure
    typedef FlowCountList::iterator FlowCountListIt;
    typedef FlowCountList::reverse_iterator FlowCountListItR;

    // Used in the SameCountRange (SCR) Vector as the type of the vector element
    class SameCountRange{
    public:
        // A pointer to the FlowCountList, used to provide access to the end() iterator as an "Invalid Iterator" marker.
        // In a traditional Doubly Linked List without iterator we would just use a null pointer value.
        FlowCountListIt invalid_list_it;
        FlowCountListIt first_list_it, last_list_it; // The first and last nodes in the same count range. These are inclusive.
    public:

        // Get the first node in the SCR
        FlowCountListIt First() const {
            return first_list_it;
        }

        // Get the last node in the SCR
        FlowCountListIt Last() const {
            return last_list_it;
        }

        // Constructor, creates a new SCR. Needs an iterator which it will use as an "Invalid Iterator" marker, the countlist's end().
        SameCountRange(FlowCountListIt invalid_list_it)
        :invalid_list_it(invalid_list_it){  Clear();  }

        // Returns true if the SCR contains no nodes
        bool Empty() const {
            return first_list_it==invalid_list_it && last_list_it==invalid_list_it;
        }

        // Returns true if the SCR contains exactly one node
        bool One() const {
            return !Empty() && first_list_it==last_list_it;
        }

        // Clears the SCR. Empty() will return true afterwards.
        void Clear(){
            first_list_it = last_list_it = invalid_list_it;
        }


        // Set the first node in the SCR
        void SetFirst(FlowCountListIt f){
            first_list_it = f;
        }

        // Set the second node in the SCR
        void SetLast(FlowCountListIt l){
            last_list_it = l;
        }

        // Set both the first and second nodes in the SCR
        void SetFirstLast(FlowCountListIt f, FlowCountListIt l){
            first_list_it = f;
            last_list_it = l;
        }

    };
protected:

    typedef std::pair<const FlowP, FlowCountListIt> FlowMapPair;  // The type of element in an FlowMap
    typedef boost::unordered_map<const FlowP, FlowCountListIt> FlowMap;  // The hash table type in the data structure
    typedef FlowMap::iterator FlowMapIt;  // The type of the FlowMap iterator

    typedef std::vector<SameCountRange> SameCountRangeVector; // The vector type in the data structure

    const uint max_queue_size;  // The maximum number of elements allowed in the queue

    SameCountRangeVector rangevector;  // The vector in the data structure
    FlowCountList countlist;  // The doubly linked list in the data structure
    FlowMap flowmap;  // The hash table in the data structure

public:
    HLHittersAlgorithm(uint max_queue_size)
    :max_queue_size(max_queue_size)
    {
        rangevector.assign(max_queue_size+1, SameCountRange(countlist.end())); // Initialize the vector
        flowmap.max_load_factor(max_queue_size); // Configure the load factor on the hash table
        flowmap.rehash(max_queue_size);

    }


    // Calculates and returns the k Heaviest Hitters in the provided result container
    virtual void QueryHeaviest(uint k, HittersQueryResult & result){
        FlowCountListItR cur_count_it=countlist.rbegin();  // Start at the end (the higher counts) of the count list
        for(uint i=0;i<k && (cur_count_it!=countlist.rend()); ++cur_count_it, ++i){  // and iterate getting flow counts
            FlowCountPair fc(cur_count_it->flowp, cur_count_it->count);
            result.push_back(fc);
        }
    }


    // Executed when a new item is received
    virtual void Append(Packet & packet)
    {
        FlowP flowp = packet.flowp;  // Get the flow of the packet
        FlowMapIt flowit = flowmap.find(flowp);  // Get an iterator to the flow's entry in the flow map (may fail)

        FlowCountListIt move_to_it;
        FlowCount fc;

        // If the iterator was valid, the flow was recorded in the map.
        // We will need to remove the corresponding entry in the count list first.
        if(flowit != flowmap.end()){
            FlowCountListIt old_listit = flowit->second;  // Get an iterator the flow's old (before the append) count list node

            fc = *old_listit;  // Copy the contents of the count node
            SameCountRange & old_smc = rangevector[old_listit->count];  // Get the old count's corresponding SCR entry

            move_to_it = GetNextListIt(old_smc.Last());  // Record the list node which is the first in the next valid SCR

            RemoveFromRange(old_listit);  // Remove the flow from the same count range it currently is in (old)
            countlist.erase(old_listit);  // Remove the old count node from the list
        }else{  // The iterator was not valid, the flow was not recorded in the map. Create a new count list node.
            move_to_it = countlist.begin();  // Will be created as the first node in the count node

            fc.count = 0;  // Create the flow count
            fc.flowp = flowp;  // Create the flow count
        }


        fc.count++;  // Increment count (if newly created, it was 0 upon creation)

        FlowCountListIt new_listit = countlist.insert(move_to_it, fc);  // Create/move the flow count into a count list node
        flowmap[flowp] = new_listit;  // Set the flow map to point at the new list node
        AddToRange(new_listit);  // Update the related SCR element
    }

    // Executed when an item is served
    virtual void Expire(Packet& packet){
        FlowP flowp = packet.flowp;  // Get the flow of the packet
        FlowCountListIt old_listit = flowmap[flowp];  // Get an iterator to the flow's entry in the flow map (may fail)

        FlowCount fc= *old_listit;  // Copy the contents of the count node

        SameCountRange & old_scr = rangevector[old_listit->count];  // Get the old count's corresponding SCR entry

        FlowCountListIt firstit_old_scr = old_scr.First();  // Get an iterator to the first node in the SCR
        FlowCountListIt previous_of_firstit_old_scr;

        bool old_scr_is_first_valid;  // Use this flag to record whether the old SCR was the first valid one or not
        if(firstit_old_scr == countlist.begin())
            old_scr_is_first_valid = true;  // It was the first, we don't need to record anything
        else{
            old_scr_is_first_valid = false;  // It was not
            previous_of_firstit_old_scr = GetPrevListIt(firstit_old_scr);  // We need to record the node which precedes the fist node in the old SCR
        }

        RemoveFromRange(old_listit);  // Remove the flow from the same count range it currently is in

        countlist.erase(old_listit);  // Remove the count node from the list
        fc.count--;  // Decrement count

        if(fc.count>=1){  // If after the removal of this packet there will still be packets belonging to this flow in the queue
            FlowCountListIt new_insert_pos_it;  // Create an iterator which will store the position in the queue to where the new list node will be inserted at
            if(old_scr_is_first_valid){  // If there is no previous valid SCR (i.e., the SCR of old_listit was the first valid one)
                new_insert_pos_it = countlist.begin();  // Then we insert the new list node at the beginning of the list
            }else{  // Then there is a previous valid SCR, but we don't know whether it is of the new count or smaller.
                if(previous_of_firstit_old_scr->count == fc.count){  // Is the previous SCR of the same count?
                    new_insert_pos_it = rangevector[previous_of_firstit_old_scr->count].First();  // Yes, add to beginning of this SCR
                }else{  // No,
                    new_insert_pos_it = GetNextListIt(previous_of_firstit_old_scr);  // Insert (a new single SCR) after the previous SCR.
                }
            }
            FlowCountListIt new_listit = countlist.insert(new_insert_pos_it, fc);  // Create/move the flow count into a count list node
            AddToRange(new_listit);  // Update the related SCR element

            flowmap[flowp] = new_listit;  // Set the flow map to point at the new list node
        }else{  // If count==0, we discard the flow
            flowmap.erase(flowp);  // Delete the map entry
        }
    }

protected:
    // Add a list iterator to its corresponding SameCountRange element in the vector
    void AddToRange(FlowCountListIt listit)
    {
        uint new_count = listit->count;

        SameCountRange & new_scr = rangevector[new_count];
        if(new_scr.Empty()){
            // This was empty. create with one entry
            new_scr.SetFirstLast(listit, listit);
        }else{
            // Replace the beginning of the range with this one
            new_scr.SetFirst(listit);
        }
    }

    // Remove a list iterator from its corresponding SameCountRange element in the vector
    void RemoveFromRange(FlowCountListIt listit)
    {
        uint old_count = listit->count;
        SameCountRange & old_scr = rangevector[old_count];
        if(old_scr.One()){
            // This was the last flow with this count -><- (delete)
            old_scr.Clear();
        }else if(listit == old_scr.First()){
            // This was the first in the range ->
            old_scr.SetFirst( GetNextListIt(old_scr.First()) );  // Make the range start at the next list node
        }else if(listit == old_scr.Last()){
            // This was the last in the range ->
            old_scr.SetLast ( GetPrevListIt(old_scr.Last()) );  // Make the range start at the previous list node
        }
    }

    // Get a new iterator pointing to the next item in the list (Helper)
    FlowCountListIt GetNextListIt(FlowCountListIt it) const {
        FlowCountListIt next = it;
        ++next;
        return next;
    }

    // Get a new iterator pointing to the previous item in the list (Helper)
    FlowCountListIt GetPrevListIt(FlowCountListIt it) const {
        FlowCountListIt prev = it;
        --prev;
        return prev;
    }
public:
    friend std::ostream& operator<< (std::ostream &, FlowCount &);
    friend std::ostream& operator<< (std::ostream &, HLHittersAlgorithm &);
    friend std::ostream& operator<< (std::ostream &, SameCountRange &);

};


// Helper function for printing
std::ostream& operator<< (std::ostream &out, HLHittersAlgorithm::FlowCount & fc){
    out << "FC[" << fc.count << ", " << *(fc.flowp) << "]";
    return out;
}

// Helper function for printing
std::ostream& operator<< (std::ostream &out, HLHittersAlgorithm::SameCountRange & scr){
    if(scr.Empty()){
        out << "S[/]";
    }else if(scr.One()){
        out << "S["<< *(scr.First()) << "]";
    }else{
        out << "S[";
        HLHittersAlgorithm::FlowCountListIt cur=scr.First();
        for(; cur!=scr.Last() ;++cur)
            out << *cur << ",";
        out << *cur << "]";
    }
    return out;
}

// Helper function for printing
std::ostream& operator<< (std::ostream &out, HLHittersAlgorithm & hl){
    out << "   Flow Count List:" << std::endl;
    out << "     " << hl.countlist << std::endl;
    out << "   Same Count Range Vector:" << std::endl;
    out << "     " << hl.rangevector << std::endl;
    return out;
}



#endif /* HLHITTERSALGORITHM_H_ */
