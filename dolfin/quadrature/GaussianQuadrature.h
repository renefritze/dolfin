// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-06-03
// Last changed: 2006-10-23

#ifndef __GAUSSIAN_QUADRATURE_H
#define __GAUSSIAN_QUADRATURE_H

#include "Quadrature.h"

namespace dolfin {
  
  /// Gaussian-type quadrature rule on the double line,
  /// including Gauss, Radau, and Lobatto quadrature.
  ///
  /// Points and weights are computed to be exact within a tolerance
  /// of DOLFIN_EPS. Comparing with known exact values for n <= 3 shows
  /// that we obtain full precision (16 digits, error less than 2e-16).

  class GaussianQuadrature : public Quadrature
  {
  public:
    
    GaussianQuadrature(unsigned int n);
    
  protected:
    
    // Compute points and weights
    void init();

    // Compute quadrature points
    virtual void compute_points() = 0;

    // Compute quadrature weights
    void compute_weights();

    // Check that quadrature is exact for given degree q
    bool check(unsigned int q) const;

    /// Display quadrature data
    virtual void disp() const = 0;
    
  };
  
}

#endif
