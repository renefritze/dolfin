// Copyright (C) 2006-2010 Anders Logg
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
// First added:  2006-06-01
// Last changed: 2012-05-09

#ifndef __VERTEX_H
#define __VERTEX_H

#include "dolfin/common/types.h"
#include "Mesh.h"
#include "MeshEntity.h"
#include "MeshEntityIteratorBase.h"
#include "MeshFunction.h"
#include "Point.h"

namespace dolfin
{

  /// A Vertex is a MeshEntity of topological dimension 0.

  class Vertex : public MeshEntity
  {
  public:

    /// Create vertex on given mesh
    Vertex(const Mesh& mesh, uint index) : MeshEntity(mesh, 0, index) {}

    /// Create vertex from mesh entity
    Vertex(MeshEntity& entity) : MeshEntity(entity.mesh(), 0, entity.index()) {}

    /// Destructor
    ~Vertex() {}

    /// Return global index of vertex
    uint global_index() const
    { return _mesh->geometry().global_index(_local_index); }

    /// Return value of vertex coordinate i
    double x(uint i) const
    { return _mesh->geometry().x(_local_index, i); }

    /// Return vertex coordinates as a 3D point value
    Point point() const
    { return _mesh->geometry().point(_local_index); }

    /// Return array of vertex coordinates (const version)
    const double* x() const
    { return _mesh->geometry().x(_local_index); }

  };

  /// A VertexIterator is a MeshEntityIterator of topological dimension 0
  typedef MeshEntityIteratorBase<Vertex> VertexIterator;

  /// A VertexFunction is a MeshFunction of topological dimension 0.
  template <typename T> class VertexFunction : public MeshFunction<T>
  {
  public:

    VertexFunction(const Mesh& mesh) : MeshFunction<T>(mesh, 0) {}

    VertexFunction(const Mesh& mesh, const T& value)
        : MeshFunction<T>(mesh, 0, value) {}

  };

}

#endif
