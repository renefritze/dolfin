// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Cell.h>
#include <dolfin/Edge.h>
#include <dolfin/Point.h>
#include <dolfin/Node.h>
#include <dolfin/P1TriMap.h>
#include <cmath>

using namespace dolfin;

//-----------------------------------------------------------------------------
P1TriMap::P1TriMap() : Map()
{
  // Set dimension
  dim = 2;
}
//-----------------------------------------------------------------------------
void P1TriMap::update(const Cell& cell)
{
  // Check that cell type is correct
  if ( cell.type() != Cell::triangle )
    dolfin_error("Wrong cell type for map (must be a triangle).");
  
  cell_ = &cell;

  // Reset values
  reset();
  
  // Get coordinates
  NodeIterator n(cell);
  Point p0 = n->coord(); ++n;
  Point p1 = n->coord(); ++n;
  Point p2 = n->coord();

  // Set values for Jacobian
  f11 = p1.x - p0.x; f12 = p2.x - p0.x;
  f21 = p1.y - p0.y; f22 = p2.y - p0.y;
  
  // Compute determinant
  d = f11 * f22 - f12 * f21;
  
  // Check determinant
  if ( fabs(d) < DOLFIN_EPS )
    dolfin_error("Map from reference element is singular.");
  
  // Compute inverse
  g11 =   f22 / d; g12 = - f12 / d;
  g21 = - f21 / d; g22 =   f11 / d;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction P1TriMap::ddx
(const FunctionSpace::ShapeFunction& v) const
{
  return g11*v.ddX() + g21*v.ddY();
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction P1TriMap::ddy
(const FunctionSpace::ShapeFunction& v) const
{
  return g12*v.ddX() + g22*v.ddY();
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction P1TriMap::ddz
(const FunctionSpace::ShapeFunction& v) const
{
  return v.ddZ();
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction P1TriMap::ddt
(const FunctionSpace::ShapeFunction& v) const
{
  return v.ddT();
}
//-----------------------------------------------------------------------------
void P1TriMap::update(const Edge& boundary)
{
  // Update map to interior of cell
  update(boundary.cell(0));

  // The determinant is given by the edge length
  bd = boundary.length();

  // Check determinant
  if ( fabs(bd) < DOLFIN_EPS )
    dolfin_error("Map to boundary of cell is singular.");
}
//-----------------------------------------------------------------------------
