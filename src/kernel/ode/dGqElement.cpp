// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_math.h>
#include <dolfin/RHS.h>
#include <dolfin/dGqMethods.h>
#include <dolfin/dGqElement.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
dGqElement::dGqElement(unsigned int q, unsigned int index, real t0, real t1) : 
  Element(q, index, t0, t1)
{  
  dG.init(q);
}
//-----------------------------------------------------------------------------
dGqElement::~dGqElement()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Element::Type dGqElement::type() const
{
  return Element::dg;
}
//-----------------------------------------------------------------------------
real dGqElement::value(real t) const
{
  // Special case: initial value
  if ( t == t0 )
    return u0;

  real tau = (t - t0) / (t1 - t0);

  real sum = 0.0;
  for (unsigned int i = 0; i <= q; i++)
    sum += values[i] * dG(q).basis(i, tau);
  
  return sum;
}
//-----------------------------------------------------------------------------
real dGqElement::initval() const
{
  return u0;
}
//-----------------------------------------------------------------------------
real dGqElement::dx() const
{
  real dudx = 0.0;

  for (unsigned int i = 0; i <= q; i++)
    dudx += values[i] * dG(q).derivative(i);

  return dudx / (t1 - t0);
}
//-----------------------------------------------------------------------------
void dGqElement::update(real u0)
{
  this->u0 = u0;
}
//-----------------------------------------------------------------------------
void dGqElement::update(RHS& f)
{
  // Evaluate right-hand side
  feval(f);
  
  // Update nodal values
  for (unsigned int i = 0; i <= q; i++)
    values[i] = u0 + integral(i);
}
//-----------------------------------------------------------------------------
void dGqElement::update(RHS& f, real alpha)
{
  // Evaluate right-hand side
  feval(f);

  // Compute weight for old value
  real w0 = 1.0 - alpha;
 
  // Update nodal values
  for (unsigned int i = 0; i <= q; i++)
    values[i] = w0*values[i] + alpha*(u0 + integral(i));
}
//-----------------------------------------------------------------------------
void dGqElement::update(RHS& f, real alpha, real *newvalues)
{
  // Evaluate right-hand side
  feval(f);

  // Compute weight for old value
  real w0 = 1.0 - alpha;
 
  // Update nodal values
  for (unsigned int i = 0; i <= q; i++)
    newvalues[i] = w0*values[i] + alpha*(u0 + integral(i));
}
//-----------------------------------------------------------------------------
void dGqElement::reset(real u0)
{
  this->u0 = u0;
  for (unsigned int i = 0; i <= q; i++)
    values[i] = u0;
}
//-----------------------------------------------------------------------------
real dGqElement::computeTimeStep(real TOL, real r, real kmax) const
{
  // Compute new time step based on residual

  if ( abs(r) < DOLFIN_EPS )
    return kmax;

  // FIXME: Missing stability factor, interpolation constant, power

  return TOL / abs(r);
}
//-----------------------------------------------------------------------------
void dGqElement::feval(RHS& f)
{
  // See comment on cGqElement::feval()

  real k = t1 - t0;
  
  for (unsigned int i = 0; i <= q; i++)
    this->f(i) = f(_index, i, t0 + dG(q).point(i)*k);
}
//-----------------------------------------------------------------------------
real dGqElement::integral(unsigned int i) const
{
  real k = t1 - t0;

  real sum = 0.0;
  for (unsigned int j = 0; j <= q; j++)
    sum += dG(q).weight(i, j) * f(j);

  return k * sum;
}
//-----------------------------------------------------------------------------
real dGqElement::computeDiscreteResidual(RHS& f)
{
  // Evaluate right-hand side
  feval(f);

  /*
    cout << "    u0       = " << u0 << endl;
    cout << "    u1       = " << values[q] << endl;
    cout << "    u1 - u0  = " << values[q] - u0 << endl;
    cout << "    integral = " << integral(q) << endl;
    cout << "    timestep = " << timestep() << endl;
  */

  return (values[q] - u0 - integral(q)) / timestep();
}
//-----------------------------------------------------------------------------
