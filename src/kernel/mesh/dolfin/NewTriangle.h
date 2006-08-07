// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-06-05
// Last changed: 2006-08-07

#ifndef __NEW_TRIANGLE_H
#define __NEW_TRIANGLE_H

#include <dolfin/CellType.h>

namespace dolfin
{

  /// This class implements functionality for triangular meshes.

  class NewTriangle : public CellType
  {
  public:

    /// Specify cell type and facet type
    NewTriangle() : CellType(triangle, interval) {}

    /// Return number of entitites of given topological dimension
    uint numEntities(uint dim) const;

    /// Return number of vertices for entity of given topological dimension
    uint numVertices(uint dim) const;

    /// Create entities e of given topological dimension from vertices v
    void createEntities(uint** e, uint dim, const uint v[]) const;

    /// Refine cell uniformly
    void refineCell(NewCell& cell, MeshEditor& editor, uint& current_cell) const;

    /// Return description of cell type
    std::string description() const;

  };

}

#endif
