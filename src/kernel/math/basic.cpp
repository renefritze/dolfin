// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <time.h>
#include <stdlib.h>
#include <cmath>
#include <dolfin/basic.h>

using namespace dolfin;

// Seed only first time
bool rand_seeded = false;

//-----------------------------------------------------------------------------
real dolfin::sqr(real x)
{
  return x*x;
}
//-----------------------------------------------------------------------------
real dolfin::max(real x, real y)
{
  return ( x > y ? x : y );
}
//-----------------------------------------------------------------------------
real dolfin::min(real x, real y)
{
  return ( x < y ? x : y );
}
//-----------------------------------------------------------------------------
int dolfin::max(int x, int y)
{
  return ( x > y ? x : y );
}
//-----------------------------------------------------------------------------
int dolfin::min(int x, int y)
{
  return ( x < y ? x : y );
}
//-----------------------------------------------------------------------------
real dolfin::rand()
{
  if ( !rand_seeded ) {
    int seed = time(0);
    srand48(seed);
    rand_seeded = true;
  }

  return (real) drand48();
}
//-----------------------------------------------------------------------------
int dolfin::round_int(real x)
{
  return (int) (x + 0.5);
}
//-----------------------------------------------------------------------------
int dolfin::floor_int(real x)
{
  return (int) x;
}
//-----------------------------------------------------------------------------
int dolfin::ceil_int(real x)
{
  return (int) ceil(x);
}
//-----------------------------------------------------------------------------
