// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Sparsity.h>
#include <dolfin/Component.h>
#include <dolfin/ODE.h>
#include <dolfin/TimeSteppingData.h>
#include <dolfin/RHS.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
RHS::RHS(ODE& ode, TimeSteppingData& data) : ode(ode), data(data)
{
  // Initialize the solution vector
  u.init(ode.size());
  u = 0.0;
}
//-----------------------------------------------------------------------------
RHS::~RHS()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
real RHS::operator() (unsigned int index, unsigned int node, real t)
{
  // Update the solution vector
  update(index, node, t);
  
  // Evaluate right hand side for current component
  return ode.f(u, t, index);
}
//-----------------------------------------------------------------------------
void RHS::update(unsigned int index, unsigned int node, real t)
{
  // Update the solution vector for all components that influence the
  // current component.
  
  // FIXME: Use nodal values if possible

  for (Sparsity::Iterator i(index, ode.sparsity); !i.end(); ++i) 
    u(i) = data.u(i,t);
}
//-----------------------------------------------------------------------------
unsigned int RHS::size() const
{
  return ode.size();
}
//-----------------------------------------------------------------------------
