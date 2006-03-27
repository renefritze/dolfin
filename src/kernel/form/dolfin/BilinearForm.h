// Copyright (C) 2004-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-05-28
// Last changed: 2006-03-27

#ifndef __BILINEAR_FORM_H
#define __BILINEAR_FORM_H

#include <dolfin/Form.h>

namespace dolfin
{

  /// BilinearForm represents a bilinear form a(v, u) with arguments v
  /// and u basis functions of the finite element space defined by a
  /// pair of finite elements (test and trial).

  class BilinearForm : public Form
  {
  public:
    
    /// Constructor
    BilinearForm(uint num_functions = 0);
    
    /// Destructor
    virtual ~BilinearForm();
    
    /// Compute element matrix (interior contribution)
    virtual void eval(real block[], const AffineMap& map) const = 0;
    
    /// Compute element matrix (boundary contribution)
    virtual void eval(real block[], const AffineMap& map, uint segment) const = 0;

    /// Return finite element defining the test space
    FiniteElement& test();

    /// Return finite element defining the trial space
    FiniteElement& trial();

  protected:

    // Finite element defining the test space
    FiniteElement* _test;

    // Finite element defining the trial space
    FiniteElement* _trial;
    
  };

}

#endif
