// Copyright (C) 2008 Anders Logg
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
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN.  If not, see <http://www.gnu.org/licenses/>.
//
// First added:  2008-11-03
// Last changed: 2008-11-03

#ifndef __SUB_SPACE_H
#define __SUB_SPACE_H

#include <vector>
#include "FunctionSpace.h"

namespace dolfin
{

  /// This class represents a subspace (component) of a function space.
  ///
  /// The subspace is specified by an array of indices. For example,
  /// the array [3, 0, 2] specifies subspace 2 of subspace 0 of
  /// subspace 3.
  ///
  /// A typical example is the function space W = V x P for Stokes.
  /// Here, V = W[0] is the subspace for the velocity component and
  /// P = W[1] is the subspace for the pressure component. Furthermore,
  /// W[0][0] = V[0] is the first component of the velocity space etc.

  class SubSpace : public FunctionSpace
  {
  public:

    /// Create subspace for given component (one level)
    SubSpace(const FunctionSpace& V,
             uint component);

    /// Create subspace for given component (two levels)
    SubSpace(const FunctionSpace& V,
             uint component, uint sub_component);

    /// Create subspace for given component (n levels)
    SubSpace(const FunctionSpace& V,
             const std::vector<uint>& component);

  };

}

#endif
