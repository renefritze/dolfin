// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <cmath>
#include <dolfin/dolfin_math.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/Solution.h>
#include <dolfin/RHS.h>
#include <dolfin/Element.h>
#include <dolfin/ElementGroup.h>
#include <dolfin/ElementGroupList.h>
#include <dolfin/ElementGroupIterator.h>
#include <dolfin/FixedPointIteration.h>
#include <dolfin/NonStiffIteration.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
NonStiffIteration::NonStiffIteration(Solution& u, RHS& f,
				     FixedPointIteration& fixpoint,
				     unsigned int maxiter,
				     real maxdiv, real maxconv, real tol,
				     unsigned int depth) :
  Iteration(u, f, fixpoint, maxiter, maxdiv, maxconv, tol, depth)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
NonStiffIteration::~NonStiffIteration()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Iteration::State NonStiffIteration::state() const
{
  return nonstiff;
}
//-----------------------------------------------------------------------------
void NonStiffIteration::start(ElementGroupList& list)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonStiffIteration::start(ElementGroup& group)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonStiffIteration::start(Element& element)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonStiffIteration::update(ElementGroupList& list, Increments& d)
{
  // Iterate on each element group and compute the l2 norm of the increments
  real increment = 0.0;
  for (ElementGroupIterator group(list); !group.end(); ++group)
    increment += sqr(fixpoint.iterate(*group));

  d = sqrt(increment);
}
//-----------------------------------------------------------------------------
void NonStiffIteration::update(ElementGroup& group, Increments& d)
{
  // Iterate on each element and compute the l2 norm of the increments
  real increment = 0.0;
  for (ElementIterator element(group); !element.end(); ++element)
    increment += sqr(fixpoint.iterate(*element));

  d = sqrt(increment);
}
//-----------------------------------------------------------------------------
void NonStiffIteration::update(Element& element, Increments& d)
{
  // Simple update of element
  d = fabs(element.update(f));

  //if ( element.index() == 1 )
  //  cout << "  new value = " << element.endval() << endl;
}
//-----------------------------------------------------------------------------
void NonStiffIteration::stabilize(ElementGroupList& list,
				  const Increments& d, unsigned int n)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonStiffIteration::stabilize(ElementGroup& group,
				 const Increments& d,  unsigned int n)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NonStiffIteration::stabilize(Element& element,
				  const Increments& d, unsigned int n)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
bool NonStiffIteration::converged(ElementGroupList& list,
				  const Increments& d, unsigned int n)
{
  // First check increment
  if ( d.d2 > tol || n == 0 )
    return false;

  // If increment is small, then check residual
  return residual(list) < tol;
}
//-----------------------------------------------------------------------------
bool NonStiffIteration::converged(ElementGroup& group,
				  const Increments& d, unsigned int n)
{
  return d.d2 < tol && n > 0;
}
//-----------------------------------------------------------------------------
bool NonStiffIteration::converged(Element& element,
				  const Increments& d, unsigned int n)
{
  return d.d2 < tol && n > 0;
}
//-----------------------------------------------------------------------------
bool NonStiffIteration::diverged(ElementGroupList& list, const Increments& d,
				 unsigned int n, State& newstate)
{
  // Make at least two iterations
  if ( n < 2 )
    return false;

  // Check if the solution converges
  if ( d.d2 < maxconv * d.d1 )
    return false;

  // Notify change of strategy
  dolfin_info("Problem appears to be stiff, need to stabilize time slab iterations.");
  
  // Reset group list
  reset(list);

  // Change state
  newstate = stiff3;

  return true;
}
//-----------------------------------------------------------------------------
bool NonStiffIteration::diverged(ElementGroup& group, const Increments& d,
				 unsigned int n, State& newstate)
{
  // Make at least two iterations
  if ( n < 2 )
    return false;

  // Check if the solution converges
  if ( d.d2 < maxconv * d.d1 )
    return false;

  // Notify change of strategy
  dolfin_info("Problem appears to be stiff, need to stabilize element group iterations.");
  
  // Reset element group
  reset(group);

  // Change state
  newstate = stiff2;

  return true;
}
//-----------------------------------------------------------------------------
bool NonStiffIteration::diverged(Element& element, const Increments& d,
				 unsigned int n, State& newstate)
{
  // Make at least two iterations
  if ( n < 2 )
    return false;
  
  // Check if the solution converges
  if ( d.d2 < maxconv * d.d1 )
    return false;

  // Notify change of strategy
  dolfin_info("Problem appears to be stiff, need to stabilize element iterations.");
  
  // Reset element
  reset(element);

  // Change state
  newstate = stiff1;

  return true;
}
//-----------------------------------------------------------------------------
void NonStiffIteration::report() const
{
  cout << "System is non-stiff, solution computed with "
       << "simple fixed point iteration." << endl;
}
//-----------------------------------------------------------------------------
