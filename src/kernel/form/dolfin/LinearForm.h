// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-05-28
// Last changed: 2006-05-15

#ifndef __LINEAR_FORM_H
#define __LINEAR_FORM_H

#include <dolfin/Form.h>

namespace dolfin
{

  /// LinearForm represents a linear form L(v) with argument v (the
  /// test function) a basis function of the finite element space
  /// defined by a finite element.

  class LinearForm : public Form
  {
  public:
    
    /// Constructor
    LinearForm(uint num_functions = 0);
    
    /// Destructor
    virtual ~LinearForm();

    /// Compute element vector (interior contribution)
    virtual void eval(real block[], const AffineMap& map) const = 0;

    /// Compute element vector (boundary contribution)
    virtual void eval(real block[], const AffineMap& map, uint segment) const = 0;

    /// Return finite element defining the test space
    FiniteElement& test();

    /// Return finite element defining the test space
    const FiniteElement& test() const;

  protected:

    // Finite element defining the test space
    FiniteElement* _test;

  };

}

#endif
