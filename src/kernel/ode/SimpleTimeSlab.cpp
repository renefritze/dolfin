// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <iostream>

#include <dolfin/dolfin_log.h>
#include <dolfin/Element.h>
#include <dolfin/TimeSteppingData.h>
#include <dolfin/Partition.h>
#include <dolfin/RHS.h>
#include <dolfin/Solution.h>
#include <dolfin/SimpleTimeSlab.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
SimpleTimeSlab::SimpleTimeSlab(real t0, real t1, RHS& f, 
			       TimeSteppingData& data, Solution& solution) : TimeSlab(t0, t1)
{
  create(f, data, solution);
}
//-----------------------------------------------------------------------------
SimpleTimeSlab::~SimpleTimeSlab()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void SimpleTimeSlab::update(RHS& f, TimeSteppingData& data, Solution& solution)
{
  updateElements(f, data, solution);
}
//-----------------------------------------------------------------------------
void SimpleTimeSlab::create(RHS& f, TimeSteppingData& data, Solution& solution)
{
  // FIXME: choose element and order here
  Element::Type type = Element::cg;
  int q = 1;

  // Get initial time step (same for all components)
  real k = data.regulator(0).timestep();

  // Set size of this time slab
  setsize(k, data);

  // Create elements
  for (unsigned int i = 0; i < data.size(); i++)
  {
    // Create element
    Element *element = solution.createElement(type, q, i, t0, t1);
    
    // Write debug info
    data.debug(*element, TimeSteppingData::create);

    // Add element to array
    elements.push_back(element);
  }
}
//-----------------------------------------------------------------------------
