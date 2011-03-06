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
#include "LobattoQuadrature.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
LobattoQuadrature::LobattoQuadrature(unsigned int n) : GaussianQuadrature(n)
{
  if (n < 2)
    error("Lobatto quadrature requires at least 2 points.");

  init();

  if (!check(2*n - 3))
    error("Lobatto quadrature not ok, check failed.");
}
//-----------------------------------------------------------------------------
std::string LobattoQuadrature::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << str(false) << std::endl << std::endl;

    s << " i    points                   weights" << std::endl;
    s << "-----------------------------------------------------" << std::endl;

    s << std::setiosflags(std::ios::scientific) << std::setprecision(16);

    for (uint i = 0; i < points.size(); i++)
    {
      s << i << " "
        << to_double(points[i]) << " "
        << to_double(weights[i]) << " "
        << std::endl;
    }
  }
  else
    s << "<LobattoQuadrature with " << points.size() << " points on [-1, 1]>";

  return s.str();
}
//----------------------------------------------------------------------------
void LobattoQuadrature::compute_points()
{
  // Compute the Lobatto quadrature points in [-1,1] as the enpoints
  // and the zeroes of the derivatives of the Legendre polynomials
  // using Newton's method

  const uint n = points.size();

  // Special case n = 1 (should not be used)
  if (n == 1)
  {
    points[0] = 0.0;
    return;
  }

  // Special case n = 2
  if (n == 2)
  {
    points[0] = -1.0;
    points[1] = 1.0;
    return;
  }

  Legendre p(n - 1);
  real x, dx;

  // Set the first and last nodal points which are 0 and 1
  points[0] = -1.0;
  points[n - 1] = 1.0;

  // Compute the rest of the nodes by Newton's method
  for (unsigned int i = 1; i <= ((n-1)/2); i++)
  {

    // Initial guess
    x = cos(3.1415926*double(i)/double(n - 1));

    // Newton's method
    do
    {
      dx = -p.ddx(x)/p.d2dx(x);
      x  = x + dx;
    } while (real_abs(dx) > real_epsilon());

    // Save the value using the symmetry of the points
    points[i] = -x;
    points[n - 1 - i] = x;

  }

  // Fix the middle node
  if ((n % 2) != 0)
    points[n/2] = 0.0;
}
//----------------------------------------------------------------------------
