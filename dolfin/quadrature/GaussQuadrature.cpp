// Copyright (C) 2003-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-06-03
// Last changed: 2009-08-11

#include <iomanip>
#include <cmath>
#include <dolfin/common/real.h>
#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/math/Legendre.h>
#include "GaussQuadrature.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
GaussQuadrature::GaussQuadrature(unsigned int n) : GaussianQuadrature(n)
{
  init();

  if ( !check(2*n-1) )
    error("Gauss quadrature not ok, check failed.");

  //info("Gauss quadrature computed for n = %d, check passed.", n);
}
//-----------------------------------------------------------------------------
std::string GaussQuadrature::str(bool verbose) const
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
    s << "<GaussQuadrature with " << n << " points on [-1, 1]>";
  }

  return s.str();
}
//----------------------------------------------------------------------------
void GaussQuadrature::compute_points()
{
  // Compute Gauss quadrature points on [-1,1] as the
  // as the zeroes of the Legendre polynomials using Newton's method

  // Special case n = 1
  if ( n == 1 )
  {
    points[0] = 0.0;
    return;
  }

  Legendre p(n);
  real x, dx;

  // Compute the points by Newton's method
  for (unsigned int i = 0; i <= ((n-1)/2); i++)
  {

    // Initial guess
    x = cos(DOLFIN_PI*(double(i+1)-0.25)/(double(n)+0.5));

    // Newton's method
    do
    {
      dx = - p(x) / p.ddx(x);
      x  = x + dx;
    } while (real_abs(dx) > real_epsilon());

    // Save the value using the symmetry of the points
    points[i] = - x;
    points[n-1-i] = x;
  }

  // Set middle node
  if ((n % 2) != 0)
    points[n/2] = 0.0;
}
//-----------------------------------------------------------------------------
