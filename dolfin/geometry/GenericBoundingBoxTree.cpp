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
// First added:  2013-05-02
// Last changed: 2013-05-02

#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Point.h>
#include <dolfin/mesh/MeshEntity.h>
#include <dolfin/mesh/MeshEntityIterator.h>
#include "GenericBoundingBoxTree.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
GenericBoundingBoxTree::GenericBoundingBoxTree()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void GenericBoundingBoxTree::build(const Mesh& mesh)
{
  build(mesh, mesh.topology().dim());
}
//-----------------------------------------------------------------------------
void GenericBoundingBoxTree::build(const Mesh& mesh, unsigned int dimension)
{
  // Check dimension
  if (dimension < 1 or dimension > mesh.topology().dim())
  {
    dolfin_error("GenericBoundingBoxTree.cpp",
                 "compute bounding box tree",
                 "dimension must be a number between 1 and %d",
                 mesh.topology().dim());
  }

  // Initialize entities of given dimension if they don't exist
  mesh.init(dimension);

  // Clear existing data if any
  bboxes.clear();

  // Initialize bounding boxes for leaves
  const unsigned int gdim = mesh.geometry().dim();
  const unsigned int num_leaves = mesh.num_entities(dimension);
  std::vector<double> leaf_bboxes(2*gdim*num_leaves);
  for (MeshEntityIterator it(mesh, dimension); !it.end(); ++it)
    compute_bbox_of_entity(leaf_bboxes.data() + 2*gdim*it->index(), *it, gdim);

  // Initialize leaf partition (to be sorted)
  std::vector<unsigned int> leaf_partition(num_leaves);
  for (unsigned int i = 0; i < num_leaves; ++i)
    leaf_partition[i] = i;

  // Recursively build the bounding box tree from the leaves
  build(leaf_bboxes, leaf_partition.begin(), leaf_partition.end(), gdim);

  info("Computed bounding box tree with %d nodes for %d entities.",
       bboxes.size(), num_leaves);
}
//-----------------------------------------------------------------------------
std::vector<unsigned int> GenericBoundingBoxTree::find(const Point& point) const
{
  // Call recursive find function
  std::vector<unsigned int> entities;
  find(point.coordinates(), bboxes.size() - 1, entities);

  return entities;
}
//-----------------------------------------------------------------------------
void GenericBoundingBoxTree::find(const double* x,
                                  unsigned int node,
                                  std::vector<unsigned int>& entities) const
{
  // Three cases: either the point is not contained (so skip branch),
  // or it's contained in a leaf (so add it) or it's contained in the
  // bounding box (so search the two children).

  const BBox& bbox = bboxes[node];

  if (!point_in_bbox(x, node))
    return;
  else if (is_leaf(bbox, node))
    entities.push_back(bbox.child_1);
  else
  {
    find(x, bbox.child_0, entities);
    find(x, bbox.child_1, entities);
  }
}
//-----------------------------------------------------------------------------
void GenericBoundingBoxTree::compute_bbox_of_entity(double* b,
                                                    const MeshEntity& entity,
                                                    unsigned int gdim) const
{
  // Get bounding box coordinates
  double* xmin = b;
  double* xmax = b + gdim;

  // Get mesh entity data
  const MeshGeometry& geometry = entity.mesh().geometry();
  const size_t num_vertices = entity.num_entities(0);
  const unsigned int* vertices = entity.entities(0);
  dolfin_assert(num_vertices >= 2);

  // Get coordinates for first vertex
  const double* x = geometry.x(vertices[0]);
  for (unsigned int j = 0; j < gdim; ++j)
    xmin[j] = xmax[j] = x[j];

  // Compute min and max over remaining vertices
  for (unsigned int i = 1; i < num_vertices; ++i)
  {
    const double* x = geometry.x(vertices[i]);
    for (unsigned int j = 0; j < gdim; ++j)
    {
      xmin[j] = std::min(xmin[j], x[j]);
      xmax[j] = std::max(xmax[j], x[j]);
    }
  }
}
//-----------------------------------------------------------------------------
