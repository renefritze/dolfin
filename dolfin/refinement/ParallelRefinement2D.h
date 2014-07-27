// Copyright (C) 2012 Chris Richardson
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
//
// First Added: 2012-12-19
// Last Changed: 2013-01-03

#ifndef __PARALLEL_REFINEMENT2D_H
#define __PARALLEL_REFINEMENT2D_H

#include <utility>
#include <vector>

namespace dolfin
{
  class Mesh;
  template<typename T> class MeshFunction;

  class ParallelRefinement2D
  {
  public:

    /// Refine with marker
    static void refine(Mesh& new_mesh, const Mesh& mesh,
                       const MeshFunction<bool>& refinement_marker,
                       bool redistribute);

    /// Uniform refine
    static void refine(Mesh& new_mesh, const Mesh& mesh,
                       bool redistribute);
  private:

    /// Used to find longest edge of a cell, when working out reference edges
    static bool length_compare(std::pair<double, std::size_t> a,
                               std::pair<double, std::size_t> b);

    ///  Calculate which edges should be 'reference' edges for the RGB
    /// Carstensen type triangulation
    static void generate_reference_edges(const Mesh& mesh,
                                         std::vector<std::size_t>& ref_edge);

  };

}

#endif
