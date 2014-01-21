// Copyright (C) 2012 Benjamin Kehlet
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
// First added:  2012-11-09
// Last changed: 2013-01-07

#ifndef __UNIT_INTERVAL_H
#define __UNIT_INTERVAL_H

#include "UnitIntervalMesh.h"
#include <dolfin/log/log.h>

namespace dolfin
{

  /// A mesh of the unit interval (0, 1) with a given number of cells
  /// (nx) in the axial direction. The total number of intervals will
  /// be nx and the total number of vertices will be (nx + 1).
  ///
  /// This class has been deprecated. Use _UnitIntervalMesh_.
  class UnitInterval : public UnitIntervalMesh
  {
  public:

    /// Create mesh of unit interval
    UnitInterval(std::size_t nx=1) : UnitIntervalMesh(nx)
    {
      deprecation("UnitInterval", "1.1.0", "1.4",
                  "The class UnitInterval has been replaced by UnitIntervalMesh.");
    }

  };
}

#endif
