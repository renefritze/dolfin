// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Updates by Johan Jansson 2003

#ifndef __TIME_SLAB_H
#define __TIME_SLAB_H

#include <dolfin/dolfin_log.h>
#include <dolfin/constants.h>
#include <dolfin/ElementGroup.h>

namespace dolfin {

  class Element;
  class Partition;
  class Adaptivity;
  class RHS;
  class Solution;

  /// A TimeSlab represents (a subsystem of) the system of ODEs
  /// between synchronized time levels t0 and t1. 

  class TimeSlab {
  public:

    /// Create time slab, including one iteration
    TimeSlab(real t0, real t1);

    /// Destructor
    virtual ~TimeSlab();

    /// Check if the given time is within the time slab
    bool within(real t) const;
    
    /// Check if the time slab reached the given end time
    bool finished() const;
    
    /// Return start time
    real starttime() const;
    
    /// Return end time
    real endtime() const;
    
    /// Return length of time slab
    real length() const;

    /// Check if time slab can be accepted for given tolerance
    virtual bool accept(RHS& f, real TOL) = 0;

    /// Count the number of element groups contained in the time slab
    virtual void countElementGroups(unsigned int& size) = 0;
    
    /// Add element groups contained in the time slab to the list
    virtual void addElementGroups(NewArray<ElementGroup*>& groups, unsigned int& pos) = 0;

    ///--- Output ---

    /// Display structure of time slab
    virtual void show(unsigned int depth = 0) const = 0;

    /// Output
    friend LogStream& operator<<(LogStream& stream, const TimeSlab& timeslab);

  protected:
    
    // Specify and adjust the time step
    void setsize(real K, const Adaptivity& adaptivity);

    //--- Time slab data ---

    // Start and end time for time slab
    real t0;
    real t1;

    // True if we reached the given end time
    bool reached_endtime;

    // Element group contained in this time slab
    ElementGroup group;

  };

}

#endif
