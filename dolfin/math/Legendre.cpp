// Copyright (C) 2003-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-06-03
// Last changed: 2008-04-22

#include <cmath>
#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include "Legendre.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Legendre::Legendre(int n)
{
  if ( n < 0 )
    error("Degree for Legendre polynomial must be non-negative.");

  this->n = n;
}
//-----------------------------------------------------------------------------
double Legendre::operator() (double x)
{
  return eval(n, x);
}
//-----------------------------------------------------------------------------
double Legendre::ddx(double x)
{
  return ddx(n, x);
}
//-----------------------------------------------------------------------------
double Legendre::d2dx(double x)
{
  return d2dx(n, x);
}
//-----------------------------------------------------------------------------
double Legendre::eval(int n, double x)
{
  // Special case n = 0
  if ( n == 0 )
    return 1.0;

  // Special case n = 1
  if ( n == 1 )
    return x;
  
  // Recurrence, BETA page 254
  double nn = double(n);
  return ( (2.0*nn-1.0)*x*eval(n-1, x) - (nn-1.0)*eval(n-2, x) ) / nn;
}
//-----------------------------------------------------------------------------
double Legendre::ddx(int n, double x)
{
  // Special case n = 0
  if ( n == 0 )
    return 0.0;
  
  // Special case n = 1
  if ( n == 1 )
    return 1.0;
  
  // Avoid division by zero
  if ( fabs(x - 1.0) < DOLFIN_EPS )
    x -= 2.0*DOLFIN_EPS;
  if ( fabs(x + 1.0) < DOLFIN_EPS )
    x += 2.0*DOLFIN_EPS;
  
  // Formula, BETA page 254
  double nn = double(n);
  return nn * (x*eval(n, x) - eval(n-1, x)) / (x*x - 1.0);
}
//-----------------------------------------------------------------------------
double Legendre::d2dx(int, double x)
{
  // Special case n = 0
  if ( n == 0 )
    return 0.0;

  // Special case n = 1
  if ( n == 1 )
    return 0.0;

  // Avoid division by zero
  if ( fabs(x - 1.0) < DOLFIN_EPS )
    x -= 2.0*DOLFIN_EPS;
  if ( fabs(x + 1.0) < DOLFIN_EPS )
    x += 2.0*DOLFIN_EPS;

  // Formula, BETA page 254
  double nn = double(n);
  return ( 2.0*x*ddx(n, x) - nn*(nn+1)*eval(n, x) ) / (1.0-x*x);
}
//-----------------------------------------------------------------------------
