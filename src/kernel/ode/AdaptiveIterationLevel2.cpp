// Copyright (C) 2004 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2004.

#include <cmath>
#include <dolfin/dolfin_math.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/Solution.h>
#include <dolfin/RHS.h>
#include <dolfin/TimeSlab.h>
#include <dolfin/Element.h>
#include <dolfin/FixedPointIteration.h>
#include <dolfin/AdaptiveIterationLevel2.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
AdaptiveIterationLevel2::AdaptiveIterationLevel2(Solution& u, RHS& f,
						 FixedPointIteration & fixpoint, 
						 unsigned int maxiter,
						 real maxdiv, real maxconv,
						 real tol) :
  Iteration(u, f, fixpoint, maxiter, maxdiv, maxconv, tol), 
  method(gauss_jacobi), datasize(0), m(0), j(0),
  alpha(1), gamma(1.0/sqrt(2.0)), r0(0)
{
  // method = gauss_seidel;
}
//-----------------------------------------------------------------------------
AdaptiveIterationLevel2::~AdaptiveIterationLevel2()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Iteration::State AdaptiveIterationLevel2::state() const
{
  return stiff2;
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::start(TimeSlab& timeslab)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::start(NewArray<Element*>& elements)
{
  // Compute total number of values in element list
  datasize = dataSize(elements);
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::start(Element& element)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::update(TimeSlab& timeslab)
{
  // Simple update of time slab
  timeslab.update(fixpoint);
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::update(NewArray<Element*>& elements)
{
  // Choose update method
  if ( method == gauss_jacobi )
    updateGaussJacobi(elements);
  else
    updateGaussSeidel(elements);
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::update(Element& element)
{
  // Choose update method
  if ( method == gauss_jacobi )
    element.update(f, alpha, x1.values + x1.offset);
  else
    element.update(f, alpha);
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::stabilize(TimeSlab& timeslab,
					const Residuals& r, unsigned int n)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::stabilize(NewArray<Element*>& elements,
					const Residuals& r, unsigned int n)
{
  // Make at least one iteration before stabilizing
  if ( n == 0 )
    return;

  //cout << "Residual: " << r.r1 << " --> " << r.r2 << endl;

  // Take action depending on j, the remaining number of iterations
  // with small alpha.
  //
  //     j = 0 : increasing alpha (or alpha = 1)
  //     j = 1 : last stabilizing iteration
  //     j > 1 : still stabilizing

  switch ( j ) {
  case 0:
    // Increase alpha with a factor 2 towards alpha = 1
    if ( r.r2 > 0.5*r.r1 )
      alpha = 2.0 * alpha / (1.0 + 2.0*alpha);
    break;
  case 1:
    // Continue with another round of stabilizing steps if it seems to work
    if ( pow(r.r2/r0, 1.0/static_cast<real>(m)) < 0.75 )
    {
      cout << "Trying again" << endl;
      
      // Choose same value for m as last time
      j = m;
      
      // Choose a slightly larger alpha if convergence is monotone
      if ( r.r2 < 0.75*r.r1 && r.r1 < 0.75*r0 )
	alpha *= 1.1;
      
      // Save residual at start of stabilizing iterations
      r0 = r.r2;
    }
    else
    {
      // Finish stabilization
      j = 0;
    }
    break;
  default:
    // Decrease number of remaining iterations with small alpha
    j -= 1;
  }
  
  // Check stabilization is needed
  if ( r.r2 > r.r1 && j == 0 )
  {
    // Compute divergence rate
    real rho = computeDivergence(elements, r);
    cout << "  rho   = " << rho << endl;

    // Compute alpha
    alpha = computeAlpha(rho);
    cout << "  alpha = " << alpha << endl;

    // Compute number of damping steps
    m = computeSteps(rho);
    j = m;
    cout << "  m     = " << m << endl;
    
    // Save residual at start of stabilizing iterations
    r0 = r.r2;
  }
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::stabilize(Element& element, 
					const Residuals& r, unsigned int n)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
bool AdaptiveIterationLevel2::converged(TimeSlab& timeslab, 
					Residuals& r, unsigned int n)
{
  // Convergence handled locally when the slab contains only one element list
  if ( timeslab.leaf() )
    return n > 0;

  // Compute residual
  r.r1 = r.r2;
  r.r2 = residual(timeslab);

  // Save initial residual
  if ( n == 0 )
    r.r0 = r.r2;
  
  return r.r2 < tol;
}
//-----------------------------------------------------------------------------
bool AdaptiveIterationLevel2::converged(NewArray<Element*>& elements, 
					Residuals& r, unsigned int n)
{
  // Compute residual
  r.r1 = r.r2;
  r.r2 = residual(elements);
  
  // Save initial residual
  if ( n == 0 )
    r.r0 = r.r2;

  return r.r2 < tol & n > 0;
}
//-----------------------------------------------------------------------------
bool AdaptiveIterationLevel2::converged(Element& element, 
					Residuals& r, unsigned int n)
{
  // Iterate one time on each element
  return n > 0;
}
//-----------------------------------------------------------------------------
bool AdaptiveIterationLevel2::diverged(TimeSlab& timeslab, 
				       Residuals& r, unsigned int n,
				       Iteration::State& newstate)
{
  cout << "Time slab residual: " << r.r1 << " --> " << r.r2 << endl;

  // Make at least two iterations
  if ( n < 2 )
    return false;
  
  // Check if the solution converges
  if ( r.r2 < maxconv * r.r1 )
    return false;
  
  // Notify change of strategy
  dolfin_info("Adaptive damping is not enough, trying a stabilizing time step sequence.");
  
  // Check if we need to reset the element
  if ( r.r2 > r.r0 )
    timeslab.reset(fixpoint);

  // Change state
  newstate = stiff3;
  
  return true;
}
//-----------------------------------------------------------------------------
bool AdaptiveIterationLevel2::diverged(NewArray<Element*>& elements, 
				       Residuals& r, unsigned int n,
				       Iteration::State& newstate)
{
  // Don't check divergence for element list, since we want to handle
  // the stabilization ourselves (and not change state).
  return false;
}
//-----------------------------------------------------------------------------
bool AdaptiveIterationLevel2::diverged(Element& element, 
				       Residuals& r, unsigned int n,
				       Iteration::State& newstate)
{
  // Don't check divergence for elements
  return false;
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::report() const
{
  cout << "System is stiff, solution computed with adaptively stabilized "
       << "fixed point iteration." << endl;
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::updateGaussJacobi(NewArray<Element*>& elements)
{  
  // Initialize values
  initData(x1);

  // Compute new values
  for (unsigned int i = 0; i < elements.size(); i++)
  {
    // Get the element
    Element* element = elements[i];
    dolfin_assert(element);
    
    // Iterate element
    fixpoint.iterate(*element);
    
    // Increase offset
    x1.offset += element->size();
  }

  // Copy values to elements
  copyData(x1, elements);
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::updateGaussSeidel(NewArray<Element*>& elements)
{
  // Simple update of element list
  for (unsigned int i = 0; i < elements.size(); i++)
  {
    // Get the element
    Element* element = elements[i];
    dolfin_assert(element);
    
    // Iterate element
    fixpoint.iterate(*element);
  }
}
//-----------------------------------------------------------------------------
real AdaptiveIterationLevel2::computeDivergence(NewArray<Element*>& elements,
						const Residuals& r)
{
  // Successive residuals
  real r1 = r.r1;
  real r2 = r.r2;

  // Successive convergence factors
  real rho2 = r2 / r1;
  real rho1 = rho2;

  // Save current alpha and change alpha to 1 for divergence computation
  real alpha0 = alpha;
  alpha = 1.0;

  // Save solution values before iteration
  initData(x0);
  copyData(elements, x0);

  for (unsigned int n = 0; n < maxiter; n++)
  {
    // Update element list
    update(elements);
    
    // Compute residual
    r1 = r2;
    r2 = residual(elements);
  
    // Compute divergence
    rho1 = rho2;
    rho2 = r2 / (DOLFIN_EPS + r1);

    cout << "rho = " << rho2 << endl;

    if ( abs(rho2-rho1) < 0.1 * rho1 )
    {
      dolfin_debug1("Computed divergence rate in %d iterations", n + 1);
      break;
    }
    
  }

  // Restore alpha
  alpha = alpha0;

  // Restore solution values
  copyData(x0, elements);

  return rho2;
}
//-----------------------------------------------------------------------------
real AdaptiveIterationLevel2::computeAlpha(real rho) const
{
  return gamma / (1.0 + rho);
}
//-----------------------------------------------------------------------------
unsigned int AdaptiveIterationLevel2::computeSteps(real rho) const
{
  return ceil_int(1.0 + log(rho) / log(1.0/(1.0-gamma*gamma)));
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::initData(Values& values)
{
  // Reallocate data if necessary
  if ( datasize > values.size )
    values.init(datasize);

  // Reset offset
  values.offset = 0;
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::copyData(const NewArray<Element*>& elements,
				       Values& values)
{
  // Copy data from element list
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++)
  {
    // Get the element
    Element* element = elements[i];
    dolfin_assert(element);

    // Get values from element
    element->get(values.values + offset);

    // Increase offset
    offset += element->size();
  }
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::copyData(const Values& values,
				 NewArray<Element*>& elements) const
{
  // Copy data to elements list
  unsigned int offset = 0;
  for (unsigned int i = 0; i < elements.size(); i++)
  {
    // Get the element
    Element* element = elements[i];
    dolfin_assert(element);

    // Set values for element
    element->set(values.values + offset);

    // Increase offset
    offset += element->size();
  }
}
//-----------------------------------------------------------------------------
unsigned int AdaptiveIterationLevel2::dataSize(const NewArray<Element*>& elements) const
{
  // Compute number of values
  int size = 0;
  
  for (unsigned int i = 0; i < elements.size(); i++)
  {
    // Get the element
    Element* element = elements[i];
    dolfin_assert(element);

    // Add size of element
    size += element->size();
  }
  
  return size;
}
//-----------------------------------------------------------------------------
AdaptiveIterationLevel2::Values::Values() : values(0), size(0), offset(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
AdaptiveIterationLevel2::Values::~Values()
{
  if ( values )
    delete [] values;
  values = 0;
}
//-----------------------------------------------------------------------------
void AdaptiveIterationLevel2::Values::init(unsigned int size)
{
  dolfin_assert(size > 0);

  if ( values )
    delete [] values;
  
  values = new real[size];
  dolfin_assert(values);
  this->size = size;
  offset = 0;
}
//-----------------------------------------------------------------------------
