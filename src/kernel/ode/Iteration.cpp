// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <cmath>
#include <dolfin/dolfin_math.h>
#include <dolfin/Solution.h>
#include <dolfin/RHS.h>
#include <dolfin/TimeSlab.h>
#include <dolfin/Element.h>
#include <dolfin/ElementGroup.h>
#include <dolfin/ElementGroupList.h>
#include <dolfin/Iteration.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Iteration::Iteration(Solution& u, RHS& f, FixedPointIteration& fixpoint,
		     unsigned int maxiter, real maxdiv, real maxconv, real tol,
		     unsigned int depth) : 
  u(u), f(f), fixpoint(fixpoint), 
  maxiter(maxiter), maxdiv(maxdiv), maxconv(maxconv), tol(tol),
  alpha(1), gamma(1.0/sqrt(2.0)), r0(0), m(0), j(0), _depth(depth), datasize(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Iteration::~Iteration()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
unsigned int Iteration::depth() const
{
  return _depth;
}
//-----------------------------------------------------------------------------
void Iteration::down()
{
  ++_depth;
}
//-----------------------------------------------------------------------------
void Iteration::up()
{
  --_depth;
}
//-----------------------------------------------------------------------------
void Iteration::init(ElementGroup& group)
{
  // Update initial data for elements
  for (ElementIterator element(group); !element.end(); ++element)
    init(*element);
}
//-----------------------------------------------------------------------------
void Iteration::init(Element& element)
{
  // Get initial value
  real u0 = u(element.index(), element.starttime());
  
  // Reset element
  element.update(u0);
}
//-----------------------------------------------------------------------------
void Iteration::reset(ElementGroupList& list)
{
  // Reset all elements
  for (ElementIterator element(list); !element.end(); ++element)
    reset(*element);
}
//-----------------------------------------------------------------------------
void Iteration::reset(ElementGroup& group)
{
  // Reset all elements
  for (ElementIterator element(group); !element.end(); ++element)
    reset(*element);
}
//-----------------------------------------------------------------------------
void Iteration::reset(Element& element)
{
  // Get initial value
  real u0 = u(element.index(), element.starttime());

  // Reset element
  element.set(u0);
}
//-----------------------------------------------------------------------------
real Iteration::residual(ElementGroupList& list)
{
  real r = 0.0;
  for (ElementIterator element(list); !element.end(); ++element)
    r += sqr(residual(*element));

  return sqrt(r);
}
//-----------------------------------------------------------------------------
real Iteration::residual(ElementGroup& group)
{
  real r = 0.0;
  for (ElementIterator element(group); !element.end(); ++element)
    r += sqr(residual(*element));

  return sqrt(r);
}
//-----------------------------------------------------------------------------
real Iteration::residual(Element& element)
{
  return fabs(element.computeElementResidual(f));
}
//-----------------------------------------------------------------------------
void Iteration::stabilize(const Residuals& r, real rho)
{
  // Take action depending on j, the remaining number of iterations
  // with small alpha.
  //
  //   j = 0 : increasing alpha (or alpha = 1)
  //   j = 1 : last stabilizing iteration
  //   j > 1 : still stabilizing

  cout << "Computing stabilization for rho = " << rho << endl;

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

  // Check if stabilization is needed
  if ( r.r2 > r.r1 && j == 0 )
  {
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
real Iteration::computeAlpha(real rho) const
{
  return gamma / (1.0 + rho);
}
//-----------------------------------------------------------------------------
unsigned int Iteration::computeSteps(real rho) const
{
  cout << "rho = " << rho << endl;
  cout << (1.0 + log(rho)) << endl;
  cout << (log(1.0/(1.0-gamma*gamma))) << endl;
  return ceil_int(1.0 + log(rho) / log(1.0/(1.0-gamma*gamma)));
}
//-----------------------------------------------------------------------------
void Iteration::initData(Values& values)
{
  // Reallocate data if necessary
  if ( datasize > values.size )
    values.init(datasize);

  // Reset offset
  values.offset = 0;
}
//-----------------------------------------------------------------------------
unsigned int Iteration::dataSize(ElementGroupList& list)
{
  // Compute total number of values
  int size = 0;
  for (ElementIterator element(list); !element.end(); ++element)
    size += element->size();
  
  return size;
}
//-----------------------------------------------------------------------------
unsigned int Iteration::dataSize(ElementGroup& group)
{
  // Compute total number of values
  int size = 0;  
  for (ElementIterator element(group); !element.end(); ++element)
    size += element->size();
  
  return size;
}
//-----------------------------------------------------------------------------
void Iteration::copyData(ElementGroupList& list, Values& values)
{
  // Copy data from group list
  unsigned int offset = 0;
  for (ElementIterator element(list); !element.end(); ++element)
  {
    // Copy values from element
    element->get(values.values + offset);

    // Increase offset
    offset += element->size();
  }
}
//-----------------------------------------------------------------------------
void Iteration::copyData(Values& values, ElementGroupList& list)
{
  // Copy data to group list
  unsigned int offset = 0;
  for (ElementIterator element(list); !element.end(); ++element)
  {
    // Copy values to element
    element->set(values.values + offset);

    // Increase offset
    offset += element->size();
  }
}
//-----------------------------------------------------------------------------
void Iteration::copyData(ElementGroup& group, Values& values)
{
  // Copy data from element group
  unsigned int offset = 0;
  for (ElementIterator element(group); !element.end(); ++element)
  {
    // Copy values from element
    element->get(values.values + offset);

    // Increase offset
    offset += element->size();
  }
}
//-----------------------------------------------------------------------------
void Iteration::copyData(Values& values, ElementGroup& group)
{
  // Copy data to element group
  unsigned int offset = 0;
  for (ElementIterator element(group); !element.end(); ++element)
  {
    // Copy values to element
    element->set(values.values + offset);

    // Increase offset
    offset += element->size();
  }
}
//-----------------------------------------------------------------------------
// Iteration::Values
//-----------------------------------------------------------------------------
Iteration::Values::Values() : values(0), size(0), offset(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Iteration::Values::~Values()
{
  if ( values )
    delete [] values;
  values = 0;
}
//-----------------------------------------------------------------------------
void Iteration::Values::init(unsigned int size)
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
