// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Cell.h>
#include <dolfin/Map.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Map::Map()
{
  reset();
}
//-----------------------------------------------------------------------------
const Cell* Map::cell() const 
{
  return cell_;
}
//-----------------------------------------------------------------------------
real Map::det() const
{
  return d;
}
//-----------------------------------------------------------------------------
real Map::bdet() const
{
  return bd;
}
//-----------------------------------------------------------------------------
void Map::update(const Edge& boundary)
{
  dolfin_error("Non-matching update of map to boundary of cell.");
}
//-----------------------------------------------------------------------------
void Map::update(const Face& boundary)
{
  dolfin_error("Non-matching update of map to boundary of cell.");
}
//-----------------------------------------------------------------------------
real Map::ddx(real a) const
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real Map::ddy(real a) const
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real Map::ddz(real a) const
{
  return 0.0;
}
//-----------------------------------------------------------------------------
real Map::ddt(real a) const
{
  return 0.0;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddx
(const FunctionSpace::Product& v) const
{
  dolfin_warning("Derivative of Product not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddy
(const FunctionSpace::Product& v) const
{
  dolfin_warning("Derivative of Product not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddz
(const FunctionSpace::Product& v) const
{
  dolfin_warning("Derivative of Product not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddt
(const FunctionSpace::Product& v) const
{
  dolfin_warning("Derivative of Product not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddx
(const FunctionSpace::ElementFunction& v) const
{
  dolfin_warning("Derivative of ElementFunction not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddy
(const FunctionSpace::ElementFunction& v) const
{
  dolfin_warning("Derivative of ElementFunction not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddz
(const FunctionSpace::ElementFunction& v) const
{
  dolfin_warning("Derivative of ElementFunction not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
const FunctionSpace::ElementFunction Map::ddt
(const FunctionSpace::ElementFunction& v) const
{
  dolfin_warning("Derivative of ElementFunction not implemented.");
  
  FunctionSpace::ElementFunction w;
  return w;
}
//-----------------------------------------------------------------------------
void Map::reset()
{
  f11 = f12 = f13 = 0.0;
  f21 = f22 = f23 = 0.0;
  f31 = f32 = f33 = 0.0;

  g11 = g12 = g13 = 0.0;
  g21 = g22 = g23 = 0.0;
  g31 = g32 = g33 = 0.0;
  
  d = 0.0;
}
//-----------------------------------------------------------------------------
