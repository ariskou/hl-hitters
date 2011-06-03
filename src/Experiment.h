/*
Experiment.h

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


#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "Common.h"
#include "Network.h"
#include "Algorithm.h"
#include "BruteForceAlgorithm.h"
#include "HLHittersAlgorithm.h"
#include "Timer.h"

class Validator;

// An Experiment sets up a router queue, flows and runs the selected algorithm on the packets
class Experiment {
public:
    // the types of heaviest hitter algorithms
    // NOPROCESSING performs no heaviest hitter processing - used to calculate the overhead of the packets just passing through the queue
    // BRUTEFORCE uses a simple direct counting method rather inefficient
    // HLHITTERS used the proposed HL-Hitters data structure and associated algorithm
    enum AlgorithmType {NOPROCESSING, BRUTEFORCE, HLHITTERS};

    // The Experiment needs a large number of input parameters which have been grouped into this struct
    struct Params{
        uint number;  // The Experiment's number/ID
        uint seq_size;  // The number of packets to generate and process
        uint flow_count;  // The number of distinct flows
        uint max_queue_size;  // The maximum size of the queue in packets
        uint k_heaviest;  // The number of Heaviest hitter flows to find
        uint random_seed;  // The seed to the random number generator
        AlgorithmType alg_type;  // The algorithm type to use (see AlgorithmType enum)
        bool validation;  // Whether to validate the results by running in parallel to the HL-Hitters algorithm a BruteForce instance and check at each step the results against each other.
    };

    // Helper functions for printing
    friend std::ostream& operator<< (std::ostream &, Experiment &);
    static std::string AlgTypeStr(AlgorithmType algt){
        switch (algt){
        case NOPROCESSING: return "NoProcessing";
        case BRUTEFORCE:  return "BruteForce";
        case HLHITTERS:  return "HL-Hitters";
        default: return "";
        }
    }

protected:
    Experiment::Params params;  // The parameters to the experiment
    uint iteration;  // The current iteration number
    std::vector<Flow> flows;  // A vector of the flow objects
    MaxQueue<Packet> queue;  // The queue in the router
    Algorithm * algorithm;  // The selected algorithm


    // A of FlowP-Count Pairs used in AppendPacket().
    // Kept at class level to amortize initialization/allocation costs
    HittersQueryResult results;

    // A class which handles the validation of the results in the experiment
    class Validator{
    private:
        BruteForceAlgorithm * validator;  // The validation algorithm
        Experiment * experiment;  // The experiment which is to be validated

        // Two vectors of FlowP-Count Pairs used in Validate().
        // Kept at class level to amortize initialization/allocation costs
        HittersQueryResult valid_results;
        HittersQueryResult checked_results;

        uint flow_count;

        typedef HittersQueryResult::iterator HittersQueryResultIt;

        // Compares FlowCount pairs, first by count, then by flow id
        static bool IsBigger(const FlowCountPair & p1, const FlowCountPair & p2){
            if(p1.second > p2.second)
                return true;
            else if(p1.second < p2.second)
                return false;
            else
                return p1.first->id > p2.first->id;
        }

        // Returns true if the two ranges have the same contents, in any order
        bool AreRangesEqual(HittersQueryResultIt ubegin, HittersQueryResultIt uend, HittersQueryResultIt vbegin){
            HittersQueryResultIt vend = vbegin + (uend - ubegin);
            std::sort(ubegin, uend, IsBigger);
            std::sort(vbegin, vend, IsBigger);
            return std::equal(ubegin, uend, vbegin);
        }

        struct IsDifferentFrom{
            uint count;
            IsDifferentFrom(uint c):count(c){};
            bool operator()(const FlowCountPair & p){ return count!=p.second; }
        };
        // Returns true if the two results are the same, allowing for FlowCounts with the same counts to be in different orders within their groups
        bool AreResultsEqual(HittersQueryResult & res1, HittersQueryResult & res2){

            HittersQueryResultIt it1 = res1.begin(), it2 = res2.begin();
            while(it1 != res1.end()){
                HittersQueryResultIt it1last = std::find_if(it1, res1.end(), IsDifferentFrom(it1->second));
                if(!AreRangesEqual(it1, it1last, it2))
                    return false;
                it2 += it1last - it1;
                it1 = it1last;
            }
            return true;
        }

    public:
        Validator(Experiment * experiment)
        :experiment(experiment)
        {
            validator = new BruteForceAlgorithm;  // Create validating BruteForce algorithm
            flow_count = experiment->GetParams().flow_count;
            valid_results.reserve(flow_count);  // Reserve memory
            checked_results.reserve(flow_count);  // Reserve memory
        }
        // Validate the results of HL-Hitters against those of BruteForce
        void Validate(){
            valid_results.clear();
            checked_results.clear();

            validator->QueryHeaviest(flow_count, valid_results);  // Get results from BruteForce
            experiment->algorithm->QueryHeaviest(flow_count, checked_results);  // Get results from HL-Hitters

            if( !AreResultsEqual(valid_results, checked_results) ){  // Compare the maps, must be identical! If not print error message
                std::cout << "At iteration "<< experiment->GetCurrentIteration() << " validation failed!" << std::endl;
                std::cout << "Valid Results   :" << valid_results << std::endl;
                std::cout << "Invalid Results :" << checked_results << std::endl;
                ::exit(-1);
            }
        }

        void Append(Packet& packet) {
            validator->Append(packet);
        }

        void Expire(Packet& packet) {
            validator->Expire(packet);
        }
    };


    Validator * validator;  // The validator object when validation is enabled


public:
    // Constructor, needs the experiment parameters as arguments
    Experiment(Params p) : params(p), queue(p.max_queue_size)
    {
        srand(params.random_seed);  // Initialize RNG with provided seed
        iteration = 0;  // Initialize current iteration
        for(uint i=1; i<=params.flow_count; ++i) // Create Flow objects
            flows.push_back(Flow(i));

        switch (params.alg_type){  // Create Heaviest Hitters algorithm to use
        case NOPROCESSING: algorithm = new NoProcessingAlgorithm(); break;
        case BRUTEFORCE: algorithm = new BruteForceAlgorithm; break;
        case HLHITTERS: algorithm = new HLHittersAlgorithm(params.max_queue_size);
        }

        results.reserve(params.k_heaviest);  // Reserve memory for results

        if(params.validation){  // Check for enabled validation
            validator = new Validator(this);  // Create validating BruteForce algorithm
        }else
            validator = NULL;  // No validation
    }

    // Runs an experiment from start to finish
    void UniformExperiment(){
        // fill up queue
        while( queue.size() < params.max_queue_size )
            AppendPacket();

        // keep queue full
        while( iteration < params.seq_size - params.max_queue_size ){
            RemovePacket();
            AppendPacket();
        }

        // empty the queue
        while( queue.size() > 0 )
            RemovePacket();
    }


    // Helper function for executing an Experiment multiple times and collecting statistics
    static void RunExperiment(Experiment::Params params, uint times);

    Experiment::Params GetParams(){
        return params;
    }

    uint GetCurrentIteration(){
        return iteration;
    }

    Algorithm * GetCurrentAlgorithm(){
        return algorithm;
    }

protected:


    // Generate a new packet uniformly
    Packet NextPacket(){
        Flow& flow = flows[rand()%params.flow_count];
        return flow.NewPacket();
    }

    // Append(receive) a new packet to the queue
    void AppendPacket(){
        ++iteration;
        Packet packet_in = NextPacket();// Generate a new packet

        queue.push_back(packet_in);  // Add it to the queue
        algorithm->Append(packet_in);  // Run the heaviest hitter Append algorithm to record the new packet

        algorithm->QueryHeaviest(params.k_heaviest, results);  // Run the heaviest hitter Query algorithm to get the heaviest-k hitters

        if(params.validation){  // If validation is enabled
            validator->Append(packet_in);  // Update the BruteForce algorithm as well
            validator->Validate();  // and validate
        }
    }

    // Remove(serve) a packet from the queue
    void RemovePacket(){
        ++iteration;
        Packet packet_out = queue.front();  // Get the to-be-served packet
        queue.pop_front();  // Remove it from the queue
        algorithm->Expire(packet_out);  // Run the heaviest hitter Expire algorithm to record the new packet

        if(params.validation){  // If validation is enabled
            validator->Expire(packet_out);  // Update the BruteForce algorithm as well
            validator->Validate();  // and validate
        }
    }
};

// Helper function for printing
std::ostream& operator<< (std::ostream &out, Experiment::Params &p){
    out << "Num:" << p.number << ", SeqSize:" << p.seq_size << ", FlowCount:" << p.flow_count
        << ", QSize:" << p.max_queue_size << ", AlgType:" << Experiment::AlgTypeStr(p.alg_type) << ", K:" << p.k_heaviest
        << ", RngSeed:" << p.random_seed << ", ValidatingResults:" << p.validation;
    return out;
}

// Helper function for printing
std::ostream& operator<< (std::ostream &out, Experiment &exp){
    out << exp.params;
    return out;
}


// Helper function for executing an Experiment multiple times and collecting statistics
void Experiment::RunExperiment(Experiment::Params params, uint times){
    MultiShotTimer timer;  // Create a timer
    for(uint i = 0; i < times; i++){ // Run multiple experiments
        Experiment exp(params);  // Create the experiment
        timer.Start();  // Start timing
        exp.UniformExperiment();  // Run experiment
        timer.Stop();  // Stop timing
    }

    // Print experiment info plus timing info
    std::cout << "Ran as: " << params << ", ";
    std::cout << "Execution Time Statistics: " << timer << std::endl;
}



#endif /* EXPERIMENT_H_ */
