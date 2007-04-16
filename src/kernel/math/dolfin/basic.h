// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003-02-06
// Last changed: 2006-06-16

#ifndef __BASIC_H
#define __BASIC_H

#include <cmath>
#include <dolfin/constants.h>

namespace dolfin
{

  /// Return the square of x
  real sqr(real x);

  /// Return a to the power n
  uint ipow(uint a, uint n);

  /// Return a random number, uniformly distributed between [0.0, 1.0)
  real rand();

  /// Seed random number generator
  void seed(unsigned int s);

}

#endif
