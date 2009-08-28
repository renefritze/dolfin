// Copyright (C) 2006-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Johan Hoffman, 2007.
// Modified by Magnus Vikstrøm, 2007.
// Modified by Garth N. Wells, 2007.
// Modified by Niclas Jansson, 2008.
// Modified by Kristoffer Selim, 2008.
//
// First added:  2006-05-08
// Last changed: 2009-08-27

#ifndef __MESH_H
#define __MESH_H

#include <string>
#include <dolfin/common/types.h>
#include <dolfin/common/Variable.h>
#include <dolfin/ale/ALEType.h>
#include "MeshTopology.h"
#include "MeshGeometry.h"
#include "MeshData.h"
#include "CellType.h"

namespace dolfin
{

  template <class T> class MeshFunction;
  class IntersectionDetector;
  class Function;
  class BoundaryMesh;
  class XMLMesh;

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
  ///
  /// Note that for efficiency, only entities of dimension zero
  /// (vertices) and entities of the maximal dimension (cells) exist
  /// when creating a Mesh. Other entities must be explicitly created
  /// by calling init(). For example, all edges in a mesh may be created
  /// by a call to mesh.init(1). Similarly, connectivities such as
  /// all edges connected to a given vertex must also be explicitly
  /// created (in this case by a call to mesh.init(0, 1)).

  class Mesh : public Variable
  {
  public:

    /// Create empty mesh
    Mesh();

    /// Copy constructor
    Mesh(const Mesh& mesh);

    /// Create mesh from data file
    explicit Mesh(std::string filename);

    /// Destructor
    ~Mesh();

    /// Assignment
    const Mesh& operator=(const Mesh& mesh);

    /// Return number of vertices
    inline uint num_vertices() const { return _topology.size(0); }

    /// Return number of edges
    inline uint num_edges() const { return _topology.size(1); }

    /// Return number of faces
    inline uint num_faces() const { return _topology.size(2); }

    /// Return number of facets
    inline uint num_facets() const { return _topology.size(_topology.dim() - 1); }

    /// Return number of cells
    inline uint num_cells() const { return _topology.size(_topology.dim()); }

    /// Return number of entities of dimension d
    inline uint num_entities(uint d) const { return _topology.size(d); }

    /// Return coordinates of all vertices
    inline double* coordinates() { return _geometry.x(); }

    /// Return coordinates of all vertices
    inline const double* coordinates() const { return _geometry.x(); }

    /// Return connectivity for all cells
    inline const uint* cells() const { return _topology(_topology.dim(), 0)(); }

    /// Return number of entities of given topological dimension
    inline uint size(uint dim) const { return _topology.size(dim); }

    /// Return mesh topology (non-const version)
    inline MeshTopology& topology() { return _topology; }

    /// Return mesh topology (const version)
    inline const MeshTopology& topology() const { return _topology; }

    /// Return mesh geometry (non-const version)
    inline MeshGeometry& geometry() { return _geometry; }

    /// Return mesh geometry (const version)
    inline const MeshGeometry& geometry() const { return _geometry; }

    /// Return mesh data (non-const version)
    MeshData& data() { return _data; }

    /// Return mesh data (const version)
    const MeshData& data() const { return _data; }

    /// Return mesh cell type
    inline CellType& type() { assert(_cell_type); return *_cell_type; }

    /// Return mesh cell type
    inline const CellType& type() const { assert(_cell_type); return *_cell_type; }

    /// Compute entities of given topological dimension and return number of entities
    uint init(uint dim) const;

    /// Compute connectivity between given pair of dimensions
    void init(uint d0, uint d1) const;

    /// Compute all entities and connectivity
    void init() const;

    /// Clear all mesh data
    void clear();

    /// Order all mesh entities (not needed if "mesh order entities" is set)
    void order();

    /// Return true iff topology is ordered according to the UFC numbering
    bool ordered() const;

    /// Refine mesh uniformly
    void refine();

    /// Refine mesh according to cells marked for refinement,
    void refine(MeshFunction<bool>& cell_markers);

    /// Coarsen mesh uniformly
    void coarsen();

    /// Coarsen mesh according to cells marked for coarsening
    void coarsen(MeshFunction<bool>& cell_markers, bool coarsen_boundary = false);

    /// Move coordinates of mesh according to new boundary coordinates
    void move(BoundaryMesh& boundary, dolfin::ALEType method=hermite);

    /// Move coordinates of mesh according to adjacent mesh with common global vertices
    void move(Mesh& mesh, dolfin::ALEType method=hermite);

    /// Move coordinates of mesh according to displacement function
    void move(const Function& displacement);

    /// Smooth mesh using Lagrangian mesh smoothing
    void smooth(uint num_smoothings=1);

    /// Compute cells intersecting point
    void intersection(const Point& p, std::vector<uint>& cells, bool fixed_mesh=true);

    /// Compute cells overlapping line defined by points
    void intersection(const Point& p1, const Point& p2, std::vector<uint>& cells, bool fixed_mesh=true);

    /// Compute cells overlapping cell
    void intersection(Cell& cell, std::vector<uint>& cells, bool fixed_mesh=true);

    /// Compute intersection with curve defined by points
    void intersection(std::vector<Point>& points, std::vector<uint>& intersection, bool fixed_mesh=true);

    /// Compute intersection with mesh
    void intersection(Mesh& mesh, std::vector<uint>& cells, bool fixed_mesh=true);

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose=false) const;

    /// Define XMLHandler for use in new XML reader/writer
    typedef XMLMesh XMLHandler;

  private:

    // Friends
    friend class MeshEditor;
    friend class TopologyComputation;
    friend class MeshOrdering;

    // Mesh topology
    MeshTopology _topology;

    // Mesh geometry
    MeshGeometry _geometry;

    // Auxiliary mesh data
    MeshData _data;

    // Cell type
    CellType* _cell_type;

    // Intersection detector
    IntersectionDetector* detector;

    // True if mesh has been ordered
    mutable bool _ordered;

  };

}

#endif
