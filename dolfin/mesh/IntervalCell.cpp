// Copyright (C) 2006-2013 Anders Logg
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
// Modified by Kristian Oelgaard 2007
// Modified by Kristoffer Selim 2008
// Modified by Marie E. Rognes 2011
//
// First added:  2006-06-05
// Last changed: 2013-05-22

#include <algorithm>
#include <dolfin/log/log.h>
#include "Cell.h"
#include "MeshEditor.h"
#include "MeshEntity.h"
#include "MeshGeometry.h"
#include "IntervalCell.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
std::size_t IntervalCell::dim() const
{
  return 1;
}
//-----------------------------------------------------------------------------
std::size_t IntervalCell::num_entities(std::size_t dim) const
{
  switch (dim)
  {
  case 0:
    return 2; // vertices
  case 1:
    return 1; // cells
  default:
    dolfin_error("IntervalCell.cpp",
                 "access number of entities of interval cell",
                 "Illegal topological dimension (%d)", dim);
  }

  return 0;
}
//-----------------------------------------------------------------------------
std::size_t IntervalCell::num_vertices(std::size_t dim) const
{
  switch (dim)
  {
  case 0:
    return 1; // vertices
  case 1:
    return 2; // cells
  default:
    dolfin_error("IntervalCell.cpp",
                 "access number of vertices for subsimplex of interval cell",
                 "Illegal topological dimension (%d)", dim);
  }

  return 0;
}
//-----------------------------------------------------------------------------
std::size_t IntervalCell::orientation(const Cell& cell) const
{
  const Point up(0.0, 1.0);
  return cell.orientation(up);
}
//-----------------------------------------------------------------------------
void IntervalCell::create_entities(std::vector<std::vector<std::size_t> >& e,
                                   std::size_t dim, const unsigned int* v) const
{
  // We don't need to create any entities
  dolfin_error("IntervalCell.cpp",
               "create entities of interval cell",
               "Don't know how to create entities of topological dimension %d", dim);
}
//-----------------------------------------------------------------------------
void IntervalCell::refine_cell(Cell& cell, MeshEditor& editor,
                               std::size_t& current_cell) const
{
  // Get vertices
  const unsigned int* v = cell.entities(0);
  dolfin_assert(v);

  // Get offset for new vertex indices
  const std::size_t offset = cell.mesh().num_vertices();

  // Compute indices for the three new vertices
  const std::size_t v0 = v[0];
  const std::size_t v1 = v[1];
  const std::size_t e0 = offset + cell.index();

  // Add the two new cells
  std::vector<std::size_t> new_cell(2);

  new_cell[0] = v0; new_cell[1] = e0;
  editor.add_cell(current_cell++, new_cell);

  new_cell[0] = e0; new_cell[1] = v1;
  editor.add_cell(current_cell++, new_cell);
}
//-----------------------------------------------------------------------------
double IntervalCell::volume(const MeshEntity& interval) const
{
  // Check that we get an interval
  if (interval.dim() != 1)
  {
    dolfin_error("IntervalCell.cpp",
                 "compute volume (length) of interval cell",
                 "Illegal mesh entity, not an interval");
  }

  // Get mesh geometry
  const MeshGeometry& geometry = interval.mesh().geometry();

  // Get the coordinates of the two vertices
  const unsigned int* vertices = interval.entities(0);
  const double* x0 = geometry.x(vertices[0]);
  const double* x1 = geometry.x(vertices[1]);

  // Compute length of interval (line segment)
  double sum = 0.0;
  for (std::size_t i = 0; i < geometry.dim(); ++i)
  {
    const double dx = x1[i] - x0[i];
    sum += dx*dx;
  }

  return std::sqrt(sum);
}
//-----------------------------------------------------------------------------
double IntervalCell::diameter(const MeshEntity& interval) const
{
  // Check that we get an interval
  if (interval.dim() != 1)
  {
    dolfin_error("IntervalCell.cpp",
                 "compute diameter of interval cell",
                 "Illegal mesh entity, not an interval");
  }

  // Diameter is same as volume for interval (line segment)
  return volume(interval);
}
//-----------------------------------------------------------------------------
double IntervalCell::squared_distance(const Cell& cell, const Point& point) const
{
  // Note: assume that interval is embedded in 1D (only checking x-coordinate)
  //
  // Note: slightly inefficient since we compute the square distance and then
  // take the square root (in Cell::distance), but this is done to match the
  // implementation for triangles and tetrahedra, and speed is often not a
  // big issue in 1D.

  // Get the vertices as points
  const MeshGeometry& geometry = cell.mesh().geometry();
  const unsigned int* vertices = cell.entities(0);
  const Point p0 = geometry.point(vertices[0]);
  const Point p1 = geometry.point(vertices[1]);

  // Get x-coordinates of point
  const double x = point.x();
  const double x0 = p0.x();
  const double x1 = p1.x();

  // Compute min and max
  const double a = std::min(x0, x1);
  const double b = std::max(x0, x1);

  // Check if point is left of a
  if (x < a) return (x - a)*(x - a);

  // Check if point is right of b
  if (x > b) return (x - b)*(x - b);

  // Point is inside interval so distance is zero
  return 0.0;
}
//-----------------------------------------------------------------------------
double IntervalCell::normal(const Cell& cell, std::size_t facet, std::size_t i) const
{
  return normal(cell, facet)[i];
}
//-----------------------------------------------------------------------------
Point IntervalCell::normal(const Cell& cell, std::size_t facet) const
{
  // Get mesh geometry
  const MeshGeometry& geometry = cell.mesh().geometry();

  // Get the two vertices as points
  const unsigned int* vertices = cell.entities(0);
  Point p0 = geometry.point(vertices[0]);
  Point p1 = geometry.point(vertices[1]);

  // Compute normal
  Point n = p0 - p1;
  if (facet == 1)
    n *= -1.0;

  // Normalize
  n /= n.norm();

  return n;
}
//-----------------------------------------------------------------------------
Point IntervalCell::cell_normal(const Cell& cell) const
{
  // Get mesh geometry
  const MeshGeometry& geometry = cell.mesh().geometry();

  // Cell_normal only defined for gdim = 1, 2 for now
  const std::size_t gdim = geometry.dim();
  if (gdim > 2)
    dolfin_error("IntervalCell.cpp",
                 "compute cell normal",
                 "Illegal geometric dimension (%d)", gdim);

  // Get the two vertices as points
  const unsigned int* vertices = cell.entities(0);
  Point p0 = geometry.point(vertices[0]);
  Point p1 = geometry.point(vertices[1]);

  // Define normal by rotating tangent counterclockwise
  Point t = p1 - p0;
  Point n(-t.y(), t.x());

  // Normalize
  n /= n.norm();

  return n;
}
//-----------------------------------------------------------------------------
double IntervalCell::facet_area(const Cell& cell, std::size_t facet) const
{
  return 1.0;
}
//-----------------------------------------------------------------------------
void IntervalCell::order(Cell& cell,
                 const std::vector<std::size_t>& local_to_global_vertex_indices) const
{
  // Sort i - j for i > j: 1 - 0

  // Get mesh topology
  MeshTopology& topology = const_cast<MeshTopology&>(cell.mesh().topology());

  // Sort local vertices in ascending order, connectivity 1 - 0
  if (!topology(1, 0).empty())
  {
    unsigned int* cell_vertices = const_cast<unsigned int*>(cell.entities(0));
    sort_entities(2, cell_vertices, local_to_global_vertex_indices);
  }
}
//-----------------------------------------------------------------------------
bool IntervalCell::contains(const Cell& cell, const Point& point) const
{
  // Get coordinates
  const MeshGeometry& geometry = cell.mesh().geometry();
  const unsigned int* vertices = cell.entities(0);
  const double x0 = geometry.point(vertices[0])[0];
  const double x1 = geometry.point(vertices[1])[0];
  const double x = point.x();

  return ((x > x0 - DOLFIN_EPS && x < x1 + DOLFIN_EPS) ||
          (x > x1 - DOLFIN_EPS && x < x0 + DOLFIN_EPS));
}
//-----------------------------------------------------------------------------
std::string IntervalCell::description(bool plural) const
{
  if (plural)
    return "intervals";
  else
    return "interval";
}
//-----------------------------------------------------------------------------
