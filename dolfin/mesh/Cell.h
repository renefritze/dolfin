// Copyright (C) 2006-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Johan Hoffman 2006.
//
// First added:  2006-06-01
// Last changed: 2009-08-06

#ifndef __CELL_H
#define __CELL_H

#include "Point.h"
#include "CellType.h"
#include "MeshEntity.h"
#include "MeshEntityIterator.h"

namespace dolfin
{

  /// A Cell is a MeshEntity of topological codimension 0.

  class Cell : public MeshEntity
  {
  public:

    /// Constructor
    Cell(const Mesh& mesh, uint index) : MeshEntity(mesh, mesh.topology().dim(), index) {}

    /// Destructor
    ~Cell() {}

    /// Return type of cell
    inline CellType::Type type() const { return _mesh.type().cell_type(); }

    /// Compute orientation of cell (0 is right, 1 is left)
    inline double orientation() const
    { return _mesh.type().orientation(*this); }

    /// Compute (generalized) volume of cell
    inline double volume() const
    { return _mesh.type().volume(*this); }

    /// Compute diameter of cell
    inline double diameter() const
    { return _mesh.type().diameter(*this); }

    /// Compute midpoint of cell
    Point midpoint();

    /// Compute component i of normal of given facet with respect to the cell
    inline double normal(uint facet, uint i) const
    { return _mesh.type().normal(*this, facet, i); }

    /// Compute normal of given facet with respect to the cell
    inline Point normal(uint facet) const
    { return _mesh.type().normal(*this, facet); }

    /// Compute the area/length of given facet with respect to the cell
    inline double facet_area(uint facet) const
    { return _mesh.type().facet_area(*this, facet); }

    /// Order entities locally
    inline void order(MeshFunction<uint>* global_vertex_indices)
    { _mesh.type().order(*this, global_vertex_indices); }

    /// Check if entities are ordered
    inline bool ordered(MeshFunction<uint>* global_vertex_indices)
    { return _mesh.type().ordered(*this, global_vertex_indices); }

    /// Check for intersection with point
    inline bool intersects(const Point& p) const
    { return _mesh.type().intersects(*this, p); }

    /// Check for intersection with line defined by points
    inline bool intersects(const Point& p0, const Point& p1) const
    { return _mesh.type().intersects(*this, p0, p1); }

    /// Check for intersection with cell
    inline bool intersects(const Cell& cell)
    { return _mesh.type().intersects(*this, cell); }

  };

  /// A CellIterator is a MeshEntityIterator of topological codimension 0.

  class CellIterator : public MeshEntityIterator
  {
  public:

    CellIterator(const Mesh& mesh) : MeshEntityIterator(mesh, mesh.topology().dim()) {}
    CellIterator(const MeshEntity& entity) : MeshEntityIterator(entity, entity.mesh().topology().dim()) {}

    inline Cell& operator*() { return *operator->(); }
    inline Cell* operator->() { return static_cast<Cell*>(MeshEntityIterator::operator->()); }
  };

}

#endif
