/*
CommandLine.h

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

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <tclap/CmdLine.h>
#include "PredicateConstraint.h"
#include "Experiment.h"

// Helper function to read Experiment execution parameters from a command line
// Uses the TCLAP library
std::pair<Experiment::Params,uint> readExperimentParams(int argc, char **argv){
    using namespace boost::assign;
    using namespace boost::lambda;
    using namespace TCLAP;

    // Wrap everything in a try block.  Do this every time,
    // because exceptions will be thrown for problems.
    try {
        // Define the command line object.
        CmdLine cmd("HL-Hitters simulation in C++", ' ', "0.3");

        PredicateConstraint<uint> posIntConstraint(_1>0, "A positive integer");
        ValueArg<uint> numArg("n", "numexec", "Number of identical sequential executions to perform (default=1)", false, 1, &posIntConstraint);
        cmd.add( numArg );

        ValueArg<bool> valArg("v", "validate", "Validate the query results of HL-Hitters against BruteForce (only available when alg=hlhitters, default=0)", false, 0, "0|1");
        cmd.add( valArg );

        ValueArg<uint> rngArg("r", "rng", "Seed to use for random number generator (default=1)", false, 1, &posIntConstraint);
        cmd.add( rngArg );

        ValueArg<uint> kArg("k", "k", "Number of heaviest hitters to query (default=1)", false, 1, &posIntConstraint);
        cmd.add( kArg );

        std::map<std::string,Experiment::AlgorithmType> allowedAlgorithms;
        insert( allowedAlgorithms )( "noprocessing", Experiment::NOPROCESSING )
                                   ( "bruteforce", Experiment::BRUTEFORCE )
                                   ( "hlhitters", Experiment::HLHITTERS);
        std::vector<std::string> allowedAlgorithmsStr;
        allowedAlgorithmsStr += "noprocessing", "bruteforce", "hlhitters";


        ValuesConstraint<std::string> allowedAlgorithmsConstraint( allowedAlgorithmsStr );
        ValueArg<std::string> algArg("a", "alg", "Algorithm to use (default=noprocessing)", false, allowedAlgorithmsStr[0], &allowedAlgorithmsConstraint);
        cmd.add( algArg );

        ValueArg<uint> queueArg("q", "queue", "Maximum queue size in items (default=50)", false, 50, &posIntConstraint);
        cmd.add( queueArg );

        ValueArg<uint> flowsArg("f", "flows", "Number of flows to use (default=100)", false, 100, &posIntConstraint);
        cmd.add( flowsArg );

        ValueArg<uint> seqArg("s", "seqsize", "Number of items to process (default=10000)", false, 10000, &posIntConstraint);
        cmd.add( seqArg );

        ValueArg<uint> expArg("e", "expnum", "Experiment Number (default=1)", false, 1, &posIntConstraint);
        cmd.add( expArg );

        // Parse the args.
        cmd.parse( argc, argv );

        if(allowedAlgorithms[algArg.getValue()]!=Experiment::HLHITTERS  && valArg.getValue())
            throw ArgException("Cannot validate results for algorithms other than hlhitters", "alg & validate");

        Experiment::Params p;
        p.number = expArg.getValue();
        p.seq_size = seqArg.getValue();
        p.flow_count =  flowsArg.getValue();
        p.max_queue_size = queueArg.getValue();
        p.alg_type = allowedAlgorithms[algArg.getValue()];
        p.k_heaviest = kArg.getValue();
        p.random_seed = rngArg.getValue();
        p.validation = valArg.getValue();

        uint numexec = numArg.getValue();

        return std::make_pair(p, numexec);

    } catch (ArgException &e) {  // catch any exceptions
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        exit(-1);
    }
}



#endif /* COMMANDLINE_H_ */
