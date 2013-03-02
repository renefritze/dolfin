// Copyright (C) 2009 Anders Logg
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// First added:  2009-09-28
// Last changed: 2011-08-15

#ifndef __EXPRESSION_H
#define __EXPRESSION_H

#include <vector>
#include <ufc.h>
#include <dolfin/common/Array.h>
#include "GenericFunction.h"

namespace dolfin
{

  class Mesh;

  /// This class represents a user-defined expression. Expressions can
  /// be used as coefficients in variational forms or interpolated
  /// into finite element spaces.
  ///
  /// An expression is defined by overloading the eval() method. Users
  /// may choose to overload either a simple version of eval(), in the
  /// case of expressions only depending on the coordinate x, or an
  /// optional version for expressions depending on x and mesh data
  /// like cell indices or facet normals.
  ///
  /// The geometric dimension (the size of x) and the value rank and
  /// dimensions of an expression must supplied as arguments to the
  /// constructor.

  class Expression : public GenericFunction
  {

  public:

    /// Create scalar expression.
    Expression();

    /// Create vector-valued expression with given dimension.
    ///
    /// *Arguments*
    ///     dim (std::size_t)
    ///         Dimension of the vector-valued expression.
    explicit Expression(std::size_t dim);

    /// Create matrix-valued expression with given dimensions.
    ///
    /// *Arguments*
    ///     dim0 (std::size_t)
    ///         Dimension (rows).
    ///     dim1 (std::size_t)
    ///         Dimension (columns).
    Expression(std::size_t dim0, std::size_t dim1);

    /// Create tensor-valued expression with given shape.
    ///
    /// *Arguments*
    ///     value_shape (std::vector<std::size_t>)
    ///         Shape of expression.
    explicit Expression(std::vector<std::size_t> value_shape);

    /// Copy constructor
    ///
    /// *Arguments*
    ///     expression (_Expression_)
    ///         Object to be copied.
    Expression(const Expression& expression);

    /// Destructor
    virtual ~Expression();

    //--- Implementation of GenericFunction interface ---
    /// Note: The reimplementation of eval is needed for the Python interface.

    /// Evaluate at given point in given cell.
    ///
    /// *Arguments*
    ///     values (_Array_ <double>)
    ///         The values at the point.
    ///     x (_Array_ <double>)
    ///         The coordinates of the point.
    ///     cell (ufc::cell)
    ///         The cell which contains the given point.
    virtual void eval(Array<double>& values,
                      const Array<double>& x,
                      const ufc::cell& cell) const;

    /// Evaluate at given point.
    ///
    /// *Arguments*
    ///     values (_Array_ <double>)
    ///         The values at the point.
    ///     x (_Array_ <double>)
    ///         The coordinates of the point.
    virtual void eval(Array<double>& values, const Array<double>& x) const;

    /// Return value rank.
    ///
    /// *Returns*
    ///     std::size_t
    ///         The value rank.
    virtual std::size_t value_rank() const;

    /// Return value dimension for given axis.
    ///
    /// *Arguments*
    ///     i (std::size_t)
    ///         Integer denoting the axis to use.
    ///
    /// *Returns*
    ///     std::size_t
    ///         The value dimension (for the given axis).
    virtual std::size_t value_dimension(std::size_t i) const;

    /// Restrict function to local cell (compute expansion coefficients w).
    ///
    /// *Arguments*
    ///     w (list of doubles)
    ///         Expansion coefficients.
    ///     element (_FiniteElement_)
    ///         The element.
    ///     dolfin_cell (_Cell_)
    ///         The cell.
    ///     ufc_cell (ufc::cell)
    ///         The ufc::cell.
    virtual void restrict(double* w,
                          const FiniteElement& element,
                          const Cell& dolfin_cell,
                          const ufc::cell& ufc_cell) const;

    /// Compute values at all mesh vertices.
    ///
    /// *Arguments*
    ///     vertex_values (_Array_ <double>)
    ///         The values at all vertices.
    ///     mesh (_Mesh_)
    ///         The mesh.
    virtual void compute_vertex_values(std::vector<double>& vertex_values,
                                       const Mesh& mesh) const;

  protected:

    // Value shape
    std::vector<std::size_t> _value_shape;

  };

}

#endif
