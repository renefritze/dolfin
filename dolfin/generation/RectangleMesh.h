// Copyright (C) 2005-2017 Anders Logg and Garth N. Wells
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

#ifndef __RECTANGLE_MESH_H
#define __RECTANGLE_MESH_H

#include <array>
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

    /// @param    p (std::array<_Point_, 2>)
    ///         Vertex points.
    /// @param    n (std::array<std::size_t, 2>)
    ///         Number of cells in each direction
    /// @param    diagonal (string)
    ///         Direction of diagonals: "left", "right", "left/right", "crossed"
    ///
    /// @code{.cpp}
    ///
    ///         // Mesh with 8 cells in each direction on the
    ///         // set [-1,2] x [-1,2]
    ///         Point p0(-1, -1);
    ///         Point p1(2, 2);
    ///         auto Mesh = RectangleMesh::create({p0, p1}, {8, 8});
    /// @endcode
    static Mesh create(const std::array<Point, 2>& p, std::array<std::size_t, 2> n,
                       std::string diagonal="right")
    { return create(MPI_COMM_WORLD, p, n); }


    /// @param    comm (MPI_Comm)
    ///         MPI communicator
    /// @param    p (std::array<_Point_, 2>)
    ///         Vertex points.
    /// @param    n (std::array<std::size_t, 2>)
    ///         Number of cells in each direction
    /// @param    diagonal (string)
    ///         Direction of diagonals: "left", "right", "left/right", "crossed"
    ///
    /// @code{.cpp}
    ///
    ///         // Mesh with 8 cells in each direction on the
    ///         // set [-1,2] x [-1,2]
    ///         Point p0(-1, -1);
    ///         Point p1(2, 2);
    ///         auto mesh = RectangleMesh::create(MPI_COMM_WORLD, {p0, p1}, {8, 8});
    /// @endcode
    static Mesh create(MPI_Comm comm, const std::array<Point, 2>& p,
                       std::array<std::size_t, 2> n,
                       std::string diagonal="right")
    { Mesh mesh(comm);
      build(mesh, p, n);
      return mesh;
    }

    /// @param    p0 (_Point_)
    ///         First point.
    /// @param    p1 (_Point_)
    ///         Second point.
    /// @param    nx (double)
    ///         Number of cells in :math:`x`-direction.
    /// @param    ny (double)
    ///         Number of cells in :math:`y`-direction.
    /// @param    diagonal (string)
    ///         Direction of diagonals: "left", "right", "left/right", "crossed"
    ///
    /// @code{.cpp}
    ///
    ///         // Mesh with 8 cells in each direction on the
    ///         // set [-1,2] x [-1,2]
    ///         Point p0(-1, -1);
    ///         Point p1(2, 2);
    ///         RectangleMesh mesh(p0, p1, 8, 8);
    /// @endcode
    RectangleMesh(const Point& p0, const Point& p1,
                  std::size_t nx, std::size_t ny,
                  std::string diagonal="right");

    /// @param    comm (MPI_Comm)
    ///         MPI communicator
    /// @param    p0 (_Point_)
    ///         First point.
    /// @param    p1 (_Point_)
    ///         Second point.
    /// @param    nx (double)
    ///         Number of cells in :math:`x`-direction.
    /// @param    ny (double)
    ///         Number of cells in :math:`y`-direction.
    /// @param    diagonal (string)
    ///         Direction of diagonals: "left", "right", "left/right", "crossed"
    ///
    /// @code{.cpp}
    ///
    ///         // Mesh with 8 cells in each direction on the
    ///         // set [-1,2] x [-1,2]
    ///         Point p0(-1, -1);
    ///         Point p1(2, 2);
    ///         RectangleMesh mesh(MPI_COMM_WORLD, p0, p1, 8, 8);
    /// @endcode
    RectangleMesh(MPI_Comm comm,
                  const Point& p0, const Point& p1,
                  std::size_t nx, std::size_t ny,
                  std::string diagonal="right");

  private:

    // Build mesh
    static void build(Mesh& mesh, const std::array<Point, 2>& p,
                      std::array<std::size_t, 2> n,
                      std::string diagonal="right");

  };

}

#endif
