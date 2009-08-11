// Copyright (C) 2003-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-06-03
// Last changed: 2009-08-11

#include <iomanip>
#include <dolfin/common/constants.h>
#include <dolfin/common/real.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/math/Legendre.h>
#include "RadauQuadrature.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
RadauQuadrature::RadauQuadrature(unsigned int n) : GaussianQuadrature(n)
{
  init();

  if ( !check(2*n-2) )
    error("Radau quadrature not ok, check failed.");

  //info("Radau quadrature computed for n = %d, check passed.", n);
}
//-----------------------------------------------------------------------------
std::string RadauQuadrature::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << str(false) << std::endl << std::endl;

    s << " i    points                   weights" << std::endl;
    s << "-----------------------------------------------------" << std::endl;

    s << std::setiosflags(std::ios::scientific) << std::setprecision(16);

    for (uint i = 0; i < n; i++)
    {
      s << i << " "
        << to_double(points[i]) << " "
        << to_double(weights[i]) << " "
        << std::endl;
    }
  }
  else
  {
    s << "<RadauQuadrature with " << n << " points on [-1, 1]>";
  }

  return s.str();
}
//-----------------------------------------------------------------------------
void RadauQuadrature::compute_points()
{
  // Compute the Radau quadrature points in [-1,1] as -1 and the zeros
  // of ( Pn-1(x) + Pn(x) ) / (1+x) where Pn is the n:th Legendre
  // polynomial. Computation is a little different than for Gauss and
  // Lobatto quadrature, since we don't know of any good initial
  // approximation for the Newton iterations.

  // Special case n = 1
  if ( n == 1 )
  {
    points[0] = -1.0;
    return;
  }

  Legendre p1(n-1), p2(n);
  real x, dx, step, sign;

  // Set size of stepping for seeking starting points
  step = 2.0 / ( double(n-1) * 10.0 );

  // Set the first nodal point which is -1
  points[0] = -1.0;

  // Start at -1 + step
  x = -1.0 + step;

  // Set the sign at -1 + epsilon
  sign = ( (p1(x) + p2(x)) > 0 ? 1.0 : -1.0 );

  // Compute the rest of the nodes by Newton's method
  for (unsigned int i = 1; i < n; i++) {

    // Step to a sign change
    while ( (p1(x) + p2(x))*sign > 0.0 )
      x += step;

    // Newton's method
    do
    {
      dx = - (p1(x) + p2(x)) / (p1.ddx(x) + p2.ddx(x));
      x  = x + dx;
    } while (real_abs(dx) > real_epsilon());

    // Set the node value
    points[i] = x;

    // Fix step so that it's not too large
    if ( step > (points[i] - points[i-1])/10.0 )
      step = (points[i] - points[i-1]) / 10.0;

    // Step forward
    sign = - sign;
    x += step;

  }

}
//-----------------------------------------------------------------------------
