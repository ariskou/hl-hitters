/*
MUE2011_Paper_Fig2.cpp

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

#include "Experiment.h"
#include "CommandLine.h"


int main(int argc, char **argv){
    using namespace boost::assign;
    Experiment::Params p;

    // Set the constant parameters
    p.seq_size = 1000000;
    p.flow_count =  150;
    p.k_heaviest = 1;
    p.random_seed = 1;
    p.validation = false;

    uint times = 10;  // Run each experiment 10 times


    p.number = 0;

    std::vector<uint> max_queue_sizes;  // The queue sizes simulated
    max_queue_sizes += 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 30, 40, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500;

    std::vector<Experiment::AlgorithmType> alg_types;  // The algorithms simulated
    alg_types += Experiment::NOPROCESSING, Experiment::BRUTEFORCE, Experiment::HLHITTERS;

    // Run every combination of queue size and algorithm (Cartesian product)
    foreach(Experiment::AlgorithmType alg_type, alg_types){
        foreach(uint max_queue_size, max_queue_sizes){
            p.alg_type = alg_type;
            p.max_queue_size = max_queue_size;
            p.number ++;
            Experiment::RunExperiment(p, times);
        }
    }

    return 0;
}

