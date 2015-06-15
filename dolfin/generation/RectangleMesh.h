// Copyright (C) 2005-2011 Anders Logg
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
// Modified by Mikael Mortensen, 2014
//
// First added:  2005-12-02
// Last changed: 2015-06-15

#ifndef __RECTANGLE_MESH_H
#define __RECTANGLE_MESH_H

#include <string>
#include <dolfin/common/MPI.h>
#include <dolfin/mesh/Mesh.h>

namespace dolfin
{

  /// Triangular mesh of the 2D rectangle spanned by two points p0 and
  /// p1. Given the number of cells (nx, ny) in each direction, the
  /// total number of triangles will be 2*nx*ny and the total number
  /// of vertices will be (nx + 1)*(ny + 1).

  class RectangleMesh : public Mesh
  {
  public:

    /// *Arguments*
    ///     p0 (_Point_)
    ///         First point.
    ///     p1 (_Point_)
    ///         Second point.
    ///     nx (double)
    ///         Number of cells in :math:`x`-direction.
    ///     ny (double)
    ///         Number of cells in :math:`y`-direction.
    ///     diagonal (string)
    ///         Direction of diagonals: "left", "right", "left/right", "crossed"
    ///
    /// *Example*
    ///     .. code-block:: c++
    ///
    ///         // Mesh with 8 cells in each direction on the
    ///         // set [-1,2] x [-1,2]
    ///         Point p0(-1, -1);
    ///         Point p1(2, 2);
    ///         RectangleMesh mesh(p0, p1, 8, 8);
    ///
    RectangleMesh(const Point& p0, const Point& p1,
                  std::size_t nx, std::size_t ny,
                  std::string diagonal="right");

    /// *Arguments*
    ///     comm (MPI_Comm)
    ///         MPI communicator
    ///     p0 (_Point_)
    ///         First point.
    ///     p1 (_Point_)
    ///         Second point.
    ///     nx (double)
    ///         Number of cells in :math:`x`-direction.
    ///     ny (double)
    ///         Number of cells in :math:`y`-direction.
    ///     diagonal (string)
    ///         Direction of diagonals: "left", "right", "left/right", "crossed"
    ///
    /// *Example*
    ///     .. code-block:: c++
    ///
    ///         // Mesh with 8 cells in each direction on the
    ///         // set [-1,2] x [-1,2]
    ///         Point p0(-1, -1);
    ///         Point p1(2, 2);
    ///         RectangleMesh mesh(MPI_COMM_WORLD, p0, p1, 8, 8);
    ///
    RectangleMesh(MPI_Comm comm,
                  const Point& p0, const Point& p1,
                  std::size_t nx, std::size_t ny,
                  std::string diagonal="right");

  private:

    // Build mesh
    void build(const Point& p0, const Point& p1,
               std::size_t nx, std::size_t ny,
               std::string diagonal="right");

  };

}

#endif
