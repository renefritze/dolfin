// Copyright (C) 2012 Anders Logg
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
// First added:  2012-08-20
// Last changed: 2012-08-23

#ifndef __GENERIC_LINEAR_OPERATOR_H
#define __GENERIC_LINEAR_OPERATOR_H

#include "LinearAlgebraObject.h"

namespace dolfin
{

  // Forward declarations
  class GenericVector;
  class GenericLinearAlgebraFactory;

  /// This class defines a common interface for linear operators,
  /// including actual matrices (class _GenericMatrix_) and linear
  /// operators only defined in terms of their action on vectors.
  ///
  /// This class is used internally by DOLFIN to define a class
  /// hierarchy of backend independent linear operators and solvers.
  /// Users should not interface to this class directly but instead
  /// use the _LinearOperator_ class.

  class GenericLinearOperator : public virtual LinearAlgebraObject
  {
  public:

    // Destructor
    virtual ~GenericLinearOperator() {}

    /// Compute matrix-vector product y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y) const = 0;

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const = 0;

    /// Return pointer to wrapper (const version)
    virtual const GenericLinearOperator* wrapper() const
    { return 0; }

    /// Return pointer to wrapper (const version)
    virtual GenericLinearOperator* wrapper()
    { return 0; }

  protected:

    friend class LinearOperator;

    // Initialize linear operator, needs to be implemented by
    // backend-specific implementation and called from the wrapper
    // class at the time of construction
    virtual void init(uint M, uint N, GenericLinearOperator* wrapper)
    {
      dolfin_error("GenericLinearOperator.h",
                   "initialize backend implementation of linear operator",
                   "Missing init() function");
    }

  };

}

#endif
