// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-05-08
// Last changed: 2006-09-18

#ifndef __NEW_MESH_H
#define __NEW_MESH_H

#include <string>
#include <dolfin/constants.h>
#include <dolfin/Variable.h>
#include <dolfin/NewMeshData.h>

namespace dolfin
{
  
  class MeshTopology;
  class MeshGeometry;
  class CellType;

  /// A Mesh consists of a set of connected and numbered mesh entities.
  ///
  /// Both the representation and the interface are dimension-independent,
  /// but a concrete interface is also provided for standard named mesh
  /// entities:
  ///
  ///     Entity  Dimension  Codimension
  ///
  ///     Vertex      0           -
  ///     Edge        1           -
  ///     Face        2           -
  ///
  ///     Facet       -           1
  ///     Cell        -           0
  ///
  /// When working with mesh iterators, all entities and connectivity
  /// are precomputed automatically the first time an iterator is
  /// created over any given topological dimension or connectivity.
  
  class NewMesh : public Variable
  {
  public:
    
    /// Create empty mesh
    NewMesh();

    /// Copy constructor
    NewMesh(const NewMesh& mesh);

    /// Create mesh from given file
    NewMesh(std::string filename);
    
    /// Destructor
    ~NewMesh();

    /// Assignment
    const NewMesh& operator=(const NewMesh& mesh);

    /// Return number of vertices
    inline uint numVertices() const { return data.topology.size(0); }

    /// Return number of cells
    inline uint numCells() const { return data.topology.size(data.topology.dim()); }

    /// Return coordinates of all vertices
    inline real* vertices() { return data.geometry.x(); }

    /// Return coordinates of all vertices
    inline const real* vertices() const { return data.geometry.x(); }

    /// Return connectivity for all cells
    inline uint* cells() { return data.topology(data.topology.dim(), 0)(); }

    /// Return connectivity for all cells
    inline const uint* cells() const { return data.topology(data.topology.dim(), 0)(); }

    /// Return topological dimension
    inline uint dim() const { return data.topology.dim(); }

    /// Return number of entities of given topological dimension
    inline uint size(uint dim) const { return data.topology.size(dim); }
    
    /// Return mesh topology
    inline MeshTopology& topology() { return data.topology; }

    /// Return mesh topology
    inline const MeshTopology& topology() const { return data.topology; }

    /// Return mesh geometry
    inline MeshGeometry& geometry() { return data.geometry; }

    /// Return mesh geometry
    inline const MeshGeometry& geometry() const { return data.geometry; }

    /// Return mesh cell type
    inline CellType& type() { dolfin_assert(data.cell_type); return *data.cell_type; }

    /// Return mesh cell type
    inline const CellType& type() const { dolfin_assert(data.cell_type); return *data.cell_type; }

    /// Compute entities of given topological dimension and return number of entities
    uint init(uint dim);

    /// Compute connectivity between given pair of dimensions
    void init(uint d0, uint d1);

    /// Compute all entities and connectivity
    void init();

    /// Refine mesh, either uniformly or according to cells marked for refinement
    void refine();

    /// Display mesh data
    void disp() const;
    
    /// Output
    friend LogStream& operator<< (LogStream& stream, const NewMesh& mesh);
    
  private:

    // Friends
    friend class MeshEditor;

    // Mesh data
    NewMeshData data;
    
  };

}

#endif
