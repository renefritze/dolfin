// Copyright (C) 2006-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Kristoffer Selim, 2008.
//
// First added:  2006-06-05
// Last changed: 2008-11-18

#include <dolfin/log/dolfin_log.h>
#include "PointCell.h"
#include "IntervalCell.h"
#include "TriangleCell.h"
#include "TetrahedronCell.h"
#include "Vertex.h"
#include "CellType.h"
#include "Cell.h"
#include "Point.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
CellType::CellType(Type cell_type, Type facet_type)
  : _cell_type(cell_type), _facet_type(facet_type)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
CellType::~CellType()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
CellType* CellType::create(Type type)
{
  switch ( type )
  {
  case point:
    return new PointCell();
  case interval:
    return new IntervalCell();
  case triangle:
    return new TriangleCell();
  case tetrahedron:
    return new TetrahedronCell();
  default:
    error("Unknown cell type: %d.", type);
  }

  return 0;
}
//-----------------------------------------------------------------------------
CellType* CellType::create(std::string type)
{
  return create(string2type(type));
}
//-----------------------------------------------------------------------------
CellType::Type CellType::string2type(std::string type)
{
  if ( type == "interval" )
    return interval;
  else if ( type == "triangle" )
    return triangle;
  else if ( type == "tetrahedron" )
    return tetrahedron;
  else
    error("Unknown cell type: \"%s\".", type.c_str());

  return interval;
}
//-----------------------------------------------------------------------------
std::string CellType::type2string(Type type)
{
  switch ( type )
  {
  case point:
    return "point";
  case interval:
    return "interval";
  case triangle:
    return "triangle";
  case tetrahedron:
    return "tetrahedron";
  default:
    error("Unknown cell type: %d.", type);
  }

  return "";
}
//-----------------------------------------------------------------------------
bool CellType::ordered(const Cell& cell) const
{
  // Get mesh topology
  const MeshTopology& topology = cell.mesh().topology();
  const uint dim = topology.dim();
  const uint c = cell.index();

  // Get vertices
  const uint num_vertices = topology(dim, 0).size(c);
  const uint* vertices = topology(dim, 0)(c);
  assert(vertices);

  // Check that vertices are in ascending order
  for (uint v = 1; v < num_vertices; v++)
    if (vertices[v - 1] >= vertices[v])
      return false;

  // Note the comparison below: d + 1 < dim, not d < dim - 1
  // Otherwise, d < dim - 1 will evaluate to true for dim = 0 with uint

  // Check numbering of entities of positive dimension and codimension
  for (uint d = 1; d + 1 < dim; d++)
  {
    // Check if entities exist, otherwise skip
    const MeshConnectivity& connectivity = topology(d, 0);
    if (connectivity.size() == 0) continue;

    // Get entities
    const uint num_entities = topology(dim, d).size(c);
    const uint* entities = topology(dim, d)(c);

    // Iterate over entities
    for (uint e = 1; e < num_entities; e++)
    {
      // Get vertices for first entity
      const uint  e0 = entities[e - 1];
      const uint  n0 = connectivity.size(e0);
      const uint* v0 = connectivity(e0);

      // Get vertices for second entity
      const uint  e1 = entities[e];
      const uint  n1 = connectivity.size(e1);
      const uint* v1 = connectivity(e1);

      // Check ordering of entities
      if (!increasing(n0, v0, n1, v1, num_vertices, vertices))
        return false;
    }
  }

  return true;
}
//-----------------------------------------------------------------------------
bool CellType::increasing(uint n0, const uint* v0,
                          uint n1, const uint* v1,
                          uint num_vertices, const uint* vertices)
{
  assert(n0 == n1);
  assert(num_vertices > n0);
  const uint num_non_incident = num_vertices - n0;

  // Compute non-incident vertices for first entity
  std::vector<uint> w0(num_non_incident);
  uint k = 0;
  for (uint i = 0; i < num_vertices; i++)
  {
    const uint v = vertices[i];
    bool incident = false;
    for (uint j = 0; j < n0; j++)
    {
      if (v0[j] == v)
      {
        incident = true;
        break;
      }
    }
    if (!incident)
      w0[k++] = v;
  }
  assert(k == num_non_incident);

  // Compute non-incident vertices for second entity
  std::vector<uint> w1(num_non_incident);
  k = 0;
  for (uint i = 0; i < num_vertices; i++)
  {
    const uint v = vertices[i];
    bool incident = false;
    for (uint j = 0; j < n1; j++)
    {
      if (v1[j] == v)
      {
        incident = true;
        break;
      }
    }
    if (!incident)
      w1[k++] = v;
  }
  assert(k == num_non_incident);

  // Compare lexicographic ordering of w0 and w1
  for (uint k = 0; k < num_non_incident; k++)
  {
    if (w0[k] < w1[k])
      return true;
    else if (w0[k] > w1[k])
      return false;
  }

  return true;
}
//-----------------------------------------------------------------------------
