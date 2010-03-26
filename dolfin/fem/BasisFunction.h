// Copyright (C) 2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-01-01
// Last changed: 2009-12-14

#ifndef __BASIS_FUNCTION_H
#define __BASIS_FUNCTION_H

#include <ufc.h>
#include <dolfin/common/types.h>
#include <dolfin/fem/FiniteElement.h>

namespace dolfin
{

  /// This class represents a finite element basis function. It can be
  /// used for computation of basis function values and derivatives.
  ///
  /// Evaluation of basis functions is also possible through the use
  /// of the functions evaluate_basis and evaluate_basis_derivatives
  /// available in the FiniteElement class. The BasisFunction class
  /// relies on these functions for evaluation but also implements the
  /// ufc::function interface which allows evaluate_dof to be
  /// evaluated for a basis function (on a possibly different
  /// element).

  class BasisFunction : public ufc::function
  {
  public:

    /// Create basis function with given index on element on given cell
    BasisFunction(uint index, const FiniteElement& element, const ufc::cell& cell)
      : index(index), element(element), cell(cell) {}

    /// Destructor
    ~BasisFunction() {}

    /// Evaluate basis function at given point
    void eval(double* values, const double* x) const
    { element.evaluate_basis(index, values, x, cell); }

    /// Evaluate all order n derivatives at given point
    void eval_derivatives(double* values, const double* x, uint n) const
    { element.evaluate_basis_derivatives(index, n, values, x, cell); }

    //--- Implementation of ufc::function interface ---

    /// Evaluate function at given point in cell
    void evaluate(double* values,
                  const double* coordinates,
                  const ufc::cell& cell) const
    { eval(values, coordinates); }

  private:

    /// The index
    uint index;

    /// The finite element
    const FiniteElement& element;

    /// The (UFC) cell
    const ufc::cell& cell;

  };

}

#endif
