// Copyright (C) 2004 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __ADAPTIVE_ITERATION_LEVEL_3_H
#define __ADAPTIVE_ITERATION_LEVEL_3_H

#include <dolfin/NewArray.h>
#include <dolfin/Iteration.h>

namespace dolfin
{

  /// State-specific behavior of fixed point iteration for stiff (level 3) problems.
  /// Adaptive damping used on the time slab level.

  class AdaptiveIterationLevel3 : public Iteration
  {
  public:
    
    AdaptiveIterationLevel3(Solution& u, RHS& f, FixedPointIteration& fixpoint,
			    unsigned int maxiter, real maxdiv, real maxconv, real tol);
    
    ~AdaptiveIterationLevel3();
    
    State state() const;

    void start(TimeSlab& timeslab);
    void start(NewArray<Element*>& elements);
    void start(Element& element);

    void update(TimeSlab& timeslab);
    void update(NewArray<Element*>& elements);
    void update(Element& element);
    
    void stabilize(TimeSlab& timeslab, const Residuals& r, unsigned int n);
    void stabilize(NewArray<Element*>& elements, const Residuals& r, unsigned int n);
    void stabilize(Element& element, const Residuals& r, unsigned int n);
    
    bool converged(TimeSlab& timeslab, Residuals& r, unsigned int n);
    bool converged(NewArray<Element*>& elements, Residuals& r, unsigned int n);
    bool converged(Element& element, Residuals& r, unsigned int n);

    bool diverged(TimeSlab& timeslab, Residuals& r, unsigned int n, Iteration::State& newstate);
    bool diverged(NewArray<Element*>& elements, Residuals& r, unsigned int n, Iteration::State& newstate);
    bool diverged(Element& element, Residuals& r, unsigned int n, Iteration::State& newstate);

    void report() const;

  private:

    // Gauss-Jacobi iteration on time slab
    void updateGaussJacobi(TimeSlab& timeslab);

    // Gauss-Seidel iteration on time slab
    void updateGaussSeidel(TimeSlab& timeslab);
    
    // Compute divergence
    real computeDivergence(TimeSlab& timeslab, const Residuals& r);

    // Initialize additional data
    void initData(Values& values);

    // Copy data from time slab
    void copyData(const TimeSlab& timeslab, Values& values);

    // Copy data to time slab
    void copyData(const Values& values, TimeSlab& timeslab) const;

    // Compute size of data
    unsigned int dataSize(const TimeSlab& timeslab) const;

    //--- Iteration data ---
   
    // Solution values for divergence computation
    Values x0;
 
    // Solution values for Gauss-Jacobi iteration
    Values x1;
   
    // Number of values in current time slab
    unsigned int datasize;

  };

}

#endif
