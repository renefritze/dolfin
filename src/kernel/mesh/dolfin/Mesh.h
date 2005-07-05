// Copyright (C) 2002-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2002
// Last changed: 2005

#ifndef __MESH_H
#define __MESH_H

#include <dolfin/dolfin_log.h>
#include <dolfin/Variable.h>
#include <dolfin/constants.h>
#include <dolfin/PList.h>
#include <dolfin/Point.h>
#include <dolfin/Node.h>
#include <dolfin/Cell.h>
#include <dolfin/Edge.h>
#include <dolfin/Face.h>
#include <dolfin/BoundaryData.h>
#include <dolfin/MeshData.h>

namespace dolfin
{

  class MeshData;

  /// A Mesh consists of Nodes, Cells, Edges, and Faces.
  /// The data of a Mesh is accessed through iterators:
  ///
  /// The Nodes of a Mesh is accessed through the class NodeIterator.
  /// The Cells of a Mesh is accessed through the class CellIterator.
  /// The Edges of a Mesh is accessed through the class EdgeIterator.
  /// The Faces of a Mesh is accessed through the class FaceIterator.
  ///
  /// A Cell can represent either a Triangle or a Tetrahedron depending
  /// on the type of the Mesh.
  
  class Mesh : public Variable
  {
  public:
    
    enum Type { triangles, tetrahedra };
    
    /// Create an empty mesh
    Mesh();

    /// Create mesh from given file
    Mesh(const char *filename);

    /// Copy constructor
    Mesh(const Mesh& mesh);

    /// Destructor
    ~Mesh();

    ///--- Basic functions

    /// Compute connectivity
    void init();

    /// Clear mesh
    void clear();

    /// Return number of nodes in the mesh
    int noNodes() const;

    /// Return number of cells in the mesh
    int noCells() const;

    /// Return number of edges in the mesh
    int noEdges() const;

    /// Return number of faces in the mesh
    int noFaces() const;

    // Create a new node at given position
    Node& createNode(Point p);
    Node& createNode(real x, real y, real z);

    // Create a new cell from the given nodes
    Cell& createCell(int n0, int n1, int n2);
    Cell& createCell(int n0, int n1, int n2, int n3);
    Cell& createCell(Node& n0, Node& n1, Node& n2);
    Cell& createCell(Node& n0, Node& n1, Node& n2, Node& n3);

    // Create a new edge from the given nodes
    Edge& createEdge(int n0, int n1);
    Edge& createEdge(Node& n0, Node& n1);

    // Create a new face from the given edges
    Face& createFace(int e0, int e1, int e2);
    Face& createFace(Edge& e0, Edge& e1, Edge& e2);
    
    // Remove node, cell, edge, face (use with care)
    void remove(Node& node);
    void remove(Cell& cell);
    void remove(Edge& edge);
    void remove(Face& face);

    /// Return type of mesh
    Type type() const;

    /// Return given node (can also use a node iterator)
    Node& node(uint id);

    /// Return given cell (can also use a cell iterator)
    Cell& cell(uint id);

    /// Return given edge (can also use an edge iterator)
    Edge& edge(uint id);

    /// Return given face (can also use a face iterator)
    Face& face(uint id);

    /// Return boundary 
    Boundary boundary();

    ///--- Mesh refinement ---

    /// Refine mesh
    void refine();

    /// Refine uniformly (all cells marked)
    void refineUniformly();
    void refineUniformly(int i);

    /// Return parent mesh
    Mesh& parent();

    /// Return child mesh
    Mesh& child();

    /// Comparison of two meshs
    bool operator==(const Mesh& mesh) const;

    /// Comparison of two meshs
    bool operator!=(const Mesh& mesh) const;
    
    ///--- Output ---

    /// Display mesh data
    void disp() const;

    /// Display condensed mesh data
    friend LogStream& operator<< (LogStream& stream, const Mesh& mesh);
    
    /// Friends
    friend class GenericCell;
    friend class Edge;
    friend class XMLMesh;
    friend class MeshInit;
    friend class MeshRefinement;
    friend class TriMeshRefinement;
    friend class TetMeshRefinement;
    friend class MeshHierarchy;
    friend class Boundary;
    friend class BoundaryInit;
    friend class NodeIterator::MeshNodeIterator;
    friend class NodeIterator::BoundaryNodeIterator;
    friend class CellIterator::MeshCellIterator;
    friend class EdgeIterator::MeshEdgeIterator;
    friend class EdgeIterator::BoundaryEdgeIterator;
    friend class FaceIterator::MeshFaceIterator;
    friend class FaceIterator::BoundaryFaceIterator;
    
  private:

    // Create a new mesh as a child to this mesh
    Mesh& createChild();
    

    /// Swap data with given mesh
    void swap(Mesh& mesh);

    //--- Mesh data ---

    // Mesh data
    MeshData* md;

    // Boundary data
    BoundaryData* bd;

    // Parent mesh
    Mesh* _parent;

    // Child mesh
    Mesh* _child;
    
    // Mesh type
    Type _type;

  };
  
}

#endif
