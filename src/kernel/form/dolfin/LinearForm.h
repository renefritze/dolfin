// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2004-05-28
// Last changed: 2006-09-28

#ifndef __LINEAR_FORM_H
#define __LINEAR_FORM_H

#include <dolfin/Form.h>

namespace dolfin
{

  /// LinearForm represents a multilinear form of the type
  ///
  ///     L = L(v1, w1, w2, ..., wn)
  ///
  /// where the first argument v1 is a basis functions (the
  /// test function) and where w1, w2, ..., wn are any given
  /// functions.

  class LinearForm : public Form
  {
  public:
    
    /// Constructor
    LinearForm(uint num_functions = 0);
    
    /// Destructor
    virtual ~LinearForm();

    /// Check if there is a contribution from the interior
    virtual bool interior_contribution() const = 0;

    /// Compute element vector (interior contribution)
    virtual void eval(real block[], const AffineMap& map) const = 0;

    /// Check if there is a contribution from the boundary
    virtual bool boundary_contribution() const = 0;

    /// Compute element vector (boundary contribution)
    virtual void eval(real block[], const AffineMap& map, uint segment) const = 0;

    /// Update map to current cell
    void update(AffineMap& map);

    /// Return finite element defining the test space
    FiniteElement& test();

    /// Return finite element defining the test space
    const FiniteElement& test() const;

    /// Friends
    friend class FEM;

  protected:

    // Finite element defining the test space
    FiniteElement* _test;

    // Local-to-global mapping for test space
    int* test_nodes;

  };

}

#endif
