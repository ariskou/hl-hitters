/*
Main.cpp

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

    std::pair<Experiment::Params,uint> result = readExperimentParams(argc, argv);  // Get the experiment params from the command line
    Experiment::RunExperiment(result.first, result.second);  // Run the experiment

    return 0;
}

