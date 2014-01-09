// Copyright (C) 2013 Garth N. Wells
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
// First added:  2013-07-07
// Last changed:

#ifndef __DOLFIN_IMPLICITDOMAINMESHGENERATOR_H
#define __DOLFIN_IMPLICITDOMAINMESHGENERATOR_H

#ifdef HAS_CGAL

#include <string>
#include <vector>

namespace dolfin
{

  // Forward declarations
  class ImplicitSurface;
  class Mesh;

  /// Generate surface or volume meshes from implicit representations
  /// of a domain modelled by the abstract dolfin::ImplicitSurface
  /// class. CGAL is used for the mesh generation.

  class ImplicitDomainMeshGenerator
  {
  public:

    /// Create volume mesh from implicit surface representation
    static void generate(Mesh& mesh, const ImplicitSurface& surface,
                         double cell_size);

    /// Create surface mesh from implicit surface representation. This
    /// function uses the CGAL 3D mesh genrator
    static void generate_surface(Mesh& mesh, const ImplicitSurface& surface,
                                 double cell_size);

  private:

    // Build CGAL triangulation
    template<typename X, typename Y>
      static X build_cgal_triangulation(const ImplicitSurface& surface,
                                        const Y& mesh_criteria);

  };

}

#endif
#endif
