/*
PredicateConstraint.h

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

#ifndef PREDICATECONSTRAINT_H
#define PREDICATECONSTRAINT_H

#include "Common.h"
#include <tclap/Constraint.h>

//A Constraint that constrains the Arg to values where the predicate returns true.
 template<class T>
class PredicateConstraint : public TCLAP::Constraint<T>{
public:

    /**
     * Constructor.
     * \param predicate - a predicate used to check whether the constraint is OK (true).
     * \param typeDesc - the description of the predicate
     */
    PredicateConstraint(boost::function<bool(T)> predicate, std::string typeDesc)
    : _predicate(predicate),
      _typeDesc(typeDesc){}

    // Virtual destructor.
    virtual ~PredicateConstraint() {}

    // Returns a description of the Constraint.
    virtual std::string description() const {return _typeDesc; }

    // Returns the short ID for the Constraint.
    virtual std::string shortID() const {return _typeDesc; }

    /**
     * The method used to verify that the value parsed from the command
     * line meets the constraint.
     * \param value - The value that will be checked.
     */
    virtual bool check(const T& value) const { return _predicate(value); }
    
protected:

    // The predicate used to check whether the constraint is OK (true).
    boost::function<bool(T)> _predicate;

    //The string used to describe the predicate of this constraint.
    std::string _typeDesc;

};

#endif 

