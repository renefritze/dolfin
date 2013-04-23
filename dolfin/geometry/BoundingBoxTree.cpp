// Copyright (C) 2013 Anders Logg
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
// First added:  2013-04-09
// Last changed: 2013-04-24

#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshGeometry.h>
#include <dolfin/mesh/MeshEntityIterator.h>
#include <dolfin/mesh/Vertex.h>
#include <dolfin/mesh/Point.h>
#include "BoundingBoxTree3D.h"
#include "BoundingBoxTree.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
BoundingBoxTree::BoundingBoxTree(const Mesh& mesh)
{
  _tree.reset(new BoundingBoxTree3D(mesh));
}
//-----------------------------------------------------------------------------
BoundingBoxTree::BoundingBoxTree(const Mesh& mesh, unsigned int dimension)
{
  _tree.reset(new BoundingBoxTree3D(mesh, dimension));
}
//-----------------------------------------------------------------------------
BoundingBoxTree::~BoundingBoxTree()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::vector<unsigned int> BoundingBoxTree::find(const Point& point) const
{
  dolfin_assert(_tree);
  return _tree->find(point);
}
//-----------------------------------------------------------------------------
