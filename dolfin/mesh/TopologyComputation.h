// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-06-02
// Last changed: 2006-06-21

#ifndef __TOPOLOGY_COMPUTATION_H
#define __TOPOLOGY_COMPUTATION_H

#include <dolfin/common/types.h>

namespace dolfin
{

  class Mesh;
  class MeshEntity;
  class MeshConnectivity;

  /// This class implements a set of basic algorithms that automate
  /// the computation of mesh entities and connectivity.

  class TopologyComputation
  {
  public:

    /// Compute mesh entities of given topological dimension
    static uint computeEntities(Mesh& mesh, uint dim);

    /// Compute connectivity for given pair of topological dimensions
    static void computeConnectivity(Mesh& mesh, uint d0, uint d1);

  private:

    /// Compute connectivity from transpose
    static void computeFromTranspose(Mesh& mesh, uint d0, uint d1);

    /// Compute connectivity from intersection
    static void computeFromIntersection(Mesh& mesh, uint d0, uint d1, uint d);

    /// Count how many of the given entities that are new
    static uint countEntities(Mesh& mesh, MeshEntity& cell, 
			      uint** vertices, uint m, uint n, uint dim);

    /// Add entities that are new
    static void addEntities(Mesh& mesh, MeshEntity& cell,
			    uint** vertices, uint m, uint n, uint dim,
			    MeshConnectivity& ce, MeshConnectivity& ev,
			    uint& current_entity);

    /// Check if mesh entity e0 contains mesh entity e1
    static bool contains(MeshEntity& e0, MeshEntity& e1);

    /// Check if array v0 contains array v1
    static bool contains(const uint* v0, uint n0, const uint* v1, uint n1);
    
  };

}

#endif
