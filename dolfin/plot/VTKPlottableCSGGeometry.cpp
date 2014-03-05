// Copyright (C) 2012 Joachim B Haga
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
// Modified by Benjamin Kehlet, 2012
//
// First added:  2012-09-04
// Last changed: 2012-10-30

#ifdef HAS_VTK

#include <dolfin/mesh/BoundaryMesh.h>
#include <dolfin/generation/CSGGeometry.h>
#include <dolfin/generation/cgal_csg3d.h>
#include <dolfin/generation/GeometryToCGALConverter.h>
#include <dolfin/generation/CGALMeshBuilder.h>
#include <dolfin/generation/CSGCGALMeshGenerator2D.h>
#include "VTKPlottableCSGGeometry.h"

using namespace dolfin;

static std::shared_ptr<dolfin::Mesh> getBoundaryMesh(const CSGGeometry& geometry)
{
  #ifdef HAS_CGAL

  if (geometry.dim() == 3)
  {
    // Convert geometry to a CGAL polyhedron
    csg::Polyhedron_3 p;
    GeometryToCGALConverter::convert(geometry, p, false);

    std::shared_ptr<dolfin::Mesh> mesh(new Mesh);

    // copy the boundary of polyhedron to a dolfin mesh
    dolfin::cout << "Building surface mesh from cgal polyhedron" << dolfin::endl;
    CGALMeshBuilder::build_surface_mesh_poly(*mesh, p);
    dolfin::cout << "  Done" << dolfin::endl;
    return mesh;

  } else
  {
    dolfin_assert(geometry.dim() == 2);

    // We're cheating a bit here: Generate a mesh and extract the boundary.
    // TODO: Implement this properly by extracting the boundary from
    // the geometry.

    Mesh m;
    CSGCGALMeshGenerator2D generator(geometry);
    generator.generate(m);
    return std::shared_ptr<dolfin::Mesh>(new BoundaryMesh(m, "exterior"));
  }

  #else
    return std::shared_ptr<dolfin::Mesh>(new Mesh);
  #endif
}
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
VTKPlottableCSGGeometry::VTKPlottableCSGGeometry(std::shared_ptr<const CSGGeometry> geometry) :
  VTKPlottableMesh(getBoundaryMesh(*geometry)),
  _geometry(geometry)
{
  // Do nothing
}
//----------------------------------------------------------------------------
bool VTKPlottableCSGGeometry::is_compatible(const Variable &var) const
{
  return dynamic_cast<const CSGGeometry*>(&var);
}
//----------------------------------------------------------------------------
void VTKPlottableCSGGeometry::update(std::shared_ptr<const Variable> var, const Parameters& parameters, int framecounter)
{
  if (var)
  {
    _geometry = std::dynamic_pointer_cast<const CSGGeometry>(var);
    dolfin_assert(_geometry);
    std::shared_ptr<dolfin::Mesh> mesh = getBoundaryMesh(*_geometry);

    VTKPlottableMesh::update(mesh, parameters, framecounter);
  }
}
//----------------------------------------------------------------------------
VTKPlottableCSGGeometry *dolfin::CreateVTKPlottable(std::shared_ptr<const CSGGeometry> geometry)
{
  return new VTKPlottableCSGGeometry(geometry);
}
//----------------------------------------------------------------------------

#endif
