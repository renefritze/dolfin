// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/ElementFunction.h>
#include <dolfin/FiniteElement.h>
#include <dolfin/Node.h>
#include <dolfin/Point.h>
#include <dolfin/NewPDE.h>
#include <dolfin/VectorExpressionFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
VectorExpressionFunction::VectorExpressionFunction(vfunction f, int dim, int size) :
  f(f), dim(dim), size(size)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
VectorExpressionFunction::~VectorExpressionFunction()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
real VectorExpressionFunction::operator() (const Node& n, real t)  const
{
  if ( f == 0 )
    return 0.0;
		 
  Point p = n.coord();
  return f(p.x, p.y, p.z, t, dim);
}
//-----------------------------------------------------------------------------
real VectorExpressionFunction::operator() (const Node& n, real t)
{
  if ( f == 0 )
    return 0.0;
		 
  Point p = n.coord();
  return f(p.x, p.y, p.z, t, dim);
}
//-----------------------------------------------------------------------------
real VectorExpressionFunction::operator() (const Point& p, real t) const
{
  if ( f == 0 )
    return 0.0;
  
  return f(p.x, p.y, p.z, t, dim);
}
//-----------------------------------------------------------------------------
real VectorExpressionFunction::operator() (const Point& p, real t)
{
  if ( f == 0 )
    return 0.0;
  
  return f(p.x, p.y, p.z, t, dim);
}
//-----------------------------------------------------------------------------
real VectorExpressionFunction::operator() (real x, real y, real z, real t) const
{
  if ( f == 0 )
    return 0.0;
  
  return f(x, y, z, t, dim);
}
//-----------------------------------------------------------------------------
real VectorExpressionFunction::operator() (real x, real y, real z, real t)
{
  if ( f == 0 )
    return 0.0;
  
  return f(x, y, z, t, dim);
}
//-----------------------------------------------------------------------------
void VectorExpressionFunction::update(FunctionSpace::ElementFunction& v,
				      const FiniteElement& element,
				      const Cell& cell, real t) const
{
  for (FiniteElement::TrialFunctionIterator phi(element); !phi.end(); ++phi)
    v.set(phi.index(), phi, phi.dof(cell, *this, t));
}
//-----------------------------------------------------------------------------
void VectorExpressionFunction::update(NewArray<real>& w, const Cell& cell, 
				      const NewPDE& pde) const
{
  // FIXME: time t and index ignored
  
  w.resize(pde.size());
  for (unsigned int i = 0; i < pde.size(); i++)
  {
    const Point& p = pde.coord(i, cell);
    w[i] = f(p.x, p.y, p.z, 0.0, 0);
  }
}
//-----------------------------------------------------------------------------
