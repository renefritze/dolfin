// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <cmath>
#include <dolfin/Point.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Point::Point()
{
  x = 0.0;
  y = 0.0;
  z = 0.0;
}
//-----------------------------------------------------------------------------
Point::Point(real x)
{
  this->x = x;
  y = 0.0;
  z = 0.0;
}
//-----------------------------------------------------------------------------
Point::Point(real x, real y)
{
  this->x = x;
  this->y = y;
  z = 0.0;
}
//-----------------------------------------------------------------------------
Point::Point(real x, real y, real z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}
//-----------------------------------------------------------------------------
real Point::dist(Point p) const
{
  real dx = x - p.x;
  real dy = y - p.y;
  real dz = z - p.z;

  return sqrt( dx*dx + dy*dy + dz*dz );
}
//-----------------------------------------------------------------------------
Point Point::midpoint(Point p) const
{
  real mx = 0.5*(x + p.x);
  real my = 0.5*(y + p.y);
  real mz = 0.5*(z + p.z);

  Point mp(mx,my,mz);

  return mp;
}
//-----------------------------------------------------------------------------
// Additional operators
//-----------------------------------------------------------------------------
dolfin::LogStream& dolfin::operator<<(LogStream& stream, const Point& p)
{
  stream << "[ Point x = " << p.x << " y = " << p.y << " z = " << p.z << " ]";
  return stream;
}
//-----------------------------------------------------------------------------
