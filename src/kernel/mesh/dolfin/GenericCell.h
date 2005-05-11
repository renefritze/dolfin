// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#ifndef __GENERIC_CELL_H
#define __GENERIC_CELL_H

#include <dolfin/constants.h>
#include <dolfin/Cell.h>
#include <dolfin/NodeIterator.h>
#include <dolfin/CellIterator.h>
#include <dolfin/EdgeIterator.h>
#include <dolfin/FaceIterator.h>
#include <dolfin/PArray.h>

namespace dolfin
{

  class Point;
  class Node;
  class Cell;
  class Mesh;
  class CellRefData;
  
  class GenericCell
  {
  public:
	 
    GenericCell();
    virtual ~GenericCell();

    int id() const;
    virtual Cell::Type type() const = 0;
    virtual Cell::Orientation orientation() const = 0;

    virtual int noNodes() const = 0;
    virtual int noEdges() const = 0;
    virtual int noFaces() const = 0;
    virtual int noBoundaries() const = 0;
    
    int noCellNeighbors() const;
    int noNodeNeighbors() const;
    int noChildren() const;

    Node& node(int i) const;
    Edge& edge(int i) const;
    Face& face(int i) const;
    Cell& neighbor(int i) const;
    Cell* parent() const;
    Cell* child(int i) const;
    Point& coord(int i) const;
    Point midpoint() const;
    int   nodeID(int i) const;
    virtual real volume() const = 0;
    virtual real diameter() const = 0;
    virtual bool edgeAligned(uint i) const = 0;
    virtual uint faceAlignment(uint i) const = 0;

    void mark();

    // Friends
    friend class Cell;
    friend class MeshRefinement;
    friend class Triangle;
    friend class Tetrahedron;
    friend class MeshInit;
    friend class NodeIterator::CellNodeIterator;
    friend class CellIterator::CellCellIterator;
    friend class EdgeIterator::CellEdgeIterator;
    friend class FaceIterator::CellFaceIterator;
    
  private:

    // Specify global cell number
    int setID(int id, Mesh& mesh);
    
    // Set mesh pointer
    void setMesh(Mesh& mesh);

    // Set parent cell
    void setParent(Cell& parent);

    // Remove parent cell
    void removeParent();

    // Set number of children
    void initChildren(int n);

    // Set child cell
    void addChild(Cell& child);

    // Remove child cell
    void removeChild(Cell& child);

    // Check if given cell is a neighbor
    bool neighbor(GenericCell& cell) const;

    // Check if given node is contained in the cell
    bool haveNode(Node& node) const;

    // Check if given edge is contained in the cell
    bool haveEdge(Edge& edge) const;

    // Create edges for the cell
    virtual void createEdges() = 0;

    // Create faces for the cell
    virtual void createFaces() = 0;

    // Create a given edge
    void createEdge(Node& n0, Node& n1);

    // Create a given face
    void createFace(Edge& e0, Edge& e1, Edge& e2);

    // Find node with given coordinates (null if not found)
    Node* findNode(const Point& p) const;

    // Find edge within cell (null if not found)
    Edge* findEdge(Node& n0, Node& n1);

    // Find face within cell (null if not found)
    Face* findFace(Edge& e0, Edge& e1, Edge& e2);
    Face* findFace(Edge& e0, Edge& e1);

    // Initialize marker (if not already done)
    void initMarker();

    // Return cell marker
    Cell::Marker& marker();

    // Return cell status
    Cell::Status& status();

    // Sort mesh entities locally
    virtual void sort() = 0;

    //--- Cell data ---

    // The mesh containing this cell
    Mesh* _mesh;

    // Global cell number
    int _id;

    // Connectivity
    PArray<Node*> cn;
    PArray<Cell*> cc;
    PArray<Edge*> ce;
    PArray<Face*> cf;

    // Parent-child info
    Cell* _parent;
    PArray<Cell*> children;

    // Mesh refinement data
    CellRefData* rd;
    
  };

}

#endif
