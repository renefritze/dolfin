// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __FIXED_POINT_ITERATION_H
#define __FIXED_POINT_ITERATION_H

#include <dolfin/constants.h>
#include <dolfin/Event.h>
#include <dolfin/Iteration.h>

namespace dolfin
{
  class Solution;
  class RHS;
  class TimeSlab;
  class Element;
  class ElementGroup;
  class ElementGroupList;
  class Iteration;

  /// Damped fixed point iteration on a time slab.
  ///
  /// The fixed point iteration is implemented as a state machine,
  /// with the class FixedPointIteration working as interface to the
  /// outside world ("context") and the class Iteration working as a
  /// base class ("state") for the different subclasses implementing
  /// state-specific behavior.

  class FixedPointIteration
  {
  public:

    /// Constructor
    FixedPointIteration(Solution& u, RHS& f);
    
    /// Destructor
    ~FixedPointIteration();

    /// Fixed point iteration on time slab
    bool iterate(TimeSlab& timeslab);

    /// Fixed point iteration on element group list (time slab)
    bool iterate(ElementGroupList& list);

    /// Fixed point iteration on element group
    bool iterate(ElementGroup& group);
    
    /// Fixed point iteration on element
    bool iterate(Element& element);

    // Compute L2 norm of element residual for element group list
    real residual(ElementGroupList& list);

    // Compute L2 norm of element residual for element group
    real residual(ElementGroup& group);

    // Compute absolute value of element residual for element
    real residual(Element& element);
    
    /// Update initial data for element group
    void init(ElementGroup& group);

    // Update initial data for element
    void init(Element& element);

    /// Reset element group list
    void reset(ElementGroupList& groups);

    /// Reset element group
    void reset(ElementGroup& group);

    // Reset element
    void reset(Element& element);

    /// Display a status report
    void report() const;

  private:

    // Start iteration on element group list
    void start(ElementGroupList& list);

    // Start iteration on element group
    void start(ElementGroup& group);

    // Start iteration on element
    void start(Element& element);

    // Update time element group list
    void update(ElementGroupList& list);
    
    // Update element group
    void update(ElementGroup& group);

    // Update element
    void update(Element& element);

    // Stabilize element group list
    void stabilize(ElementGroupList& list,
		   const Iteration::Residuals& r, unsigned int n);

    // Stabilize element group
    void stabilize(ElementGroup& group,
		   const Iteration::Residuals& r, unsigned int n);

    // Stabilize element
    void stabilize(Element& element,
		   const Iteration::Residuals& r, unsigned int n);

    // Check convergence for element group list
    bool converged(ElementGroupList& list, Iteration::Residuals& r, unsigned int n);

    // Check convergence for element group
    bool converged(ElementGroup& group, Iteration::Residuals& r, unsigned int n);

    // Check convergence for element
    bool converged(Element& element, Iteration::Residuals& r, unsigned int n);

    // Check divergence for element group list
    bool diverged(ElementGroupList& list, Iteration::Residuals& r,
		  unsigned int n, Iteration::State& newstate);
    
    // Check divergence for element group
    bool diverged(ElementGroup& group, Iteration::Residuals& r,
		  unsigned int n, Iteration::State& newstate);

    // Check divergence for element
    bool diverged(Element& element, Iteration::Residuals& r,
		  unsigned int n, Iteration::State& newstate);

    // Change state
    void changeState(Iteration::State newstate);

    //--- Data for fixed point iteration

    // Solution
    Solution& u;

    // Right-hand side f
    RHS& f;

    // Maximum number of iterations
    unsigned int maxiter;

    // Maximum allowed divergence
    real maxdiv;

    // Maximum allowed convergence
    real maxconv;

    // Tolerance for discrete residual
    real tol;

    // Current state
    Iteration* state;

  };

}

#endif
