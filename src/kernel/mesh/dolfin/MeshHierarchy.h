// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#ifndef __MESH_HIERARCHY
#define __MESH_HIERARCHY

#include <dolfin/PArray.h>

namespace dolfin
{

  class Mesh;

  class MeshHierarchy
  {
  public:

    /// Create empty mesh hierarchy
    MeshHierarchy();

    /// Create a mesh hierarchy from a given mesh
    MeshHierarchy(Mesh& mesh);

    /// Destructor
    ~MeshHierarchy();

    /// Compute mesh hierarchy from a given mesh
    void init(Mesh& mesh);

    /// Clear mesh hierarchy
    void clear();

    /// Add a mesh to the mesh hierarchy
    void add(Mesh& mesh);

    /// Return mesh at given level
    Mesh& operator() (int level) const;

    /// Return coarsest mesh (level 0)
    Mesh& coarse() const;

    /// Return finest mesh (highest level)
    Mesh& fine() const;

    /// Return number of levels
    int size() const;

    /// Check if mesh hierarchy is empty
    bool empty() const;

    /// Friends
    friend class MeshIterator;

  private:

    // An array of mesh pointers
    PArray<Mesh*> meshes;

  };

}

#endif
