// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __GENERIC_CELL_H
#define __GENERIC_CELL_H

// A cell is the geometric part of an element. An element contains
// basis functions, but a cell contains only the geometric information.
//
// Similarly to a Node, the a Cell should be small and simple to keep
// the total data size as small as possible.

#include <dolfin/dolfin_constants.h>
#include <dolfin/Cell.h>
#include <dolfin/NodeIterator.h>
#include <dolfin/ShortList.h>

namespace dolfin {

  class Node;
  class Cell;
  
  class GenericCell {
  public:
	 
	 GenericCell();
	 ~GenericCell();

	 virtual int noNodes() = 0;
	 virtual int noEdges() = 0;
	 virtual int noFaces() = 0;
	 virtual int noBoundaries() = 0;

	 int id() const;
	 virtual Cell::Type type() = 0;
	 
	 void Clear();
	 
	 /// --- Neighbor information
	 
	 /// Get number of cell neighbors
	 int GetNoGenericCellNeighbors();
	 /// Get cell neighbor number i
	 int GetGenericCellNeighbor(int i);
	 
	 /// --- Accessor functions for stored data
	 
	 /// Get material type
	 int GetMaterial();
	 /// Return the number of nodes in the cell
	 virtual int GetSize() = 0;
	 /// Return global node number of node number <node> in the cell
	 virtual Node* GetNode(int node) = 0;
	 
	 /// --- Functions that require computation (every time!)
	 
	 /// Compute and return the volume of the cell
	 //  virtual real ComputeVolume(Grid *grid) = 0;
	 /// Compute and return radius of circum-written circle
	 //virtual real ComputeCircumRadius(Grid *grid) = 0;
	 /// Compute and return radius of circum-written circle (faster version)
	 //virtual real ComputeCircumRadius(Grid *grid, real volume) = 0;
    
	 /// Give access to the special functions below
	 //friend class Grid;
	 friend class Node;
	 friend class Tetrahedron;
	 friend class Triangle;
	 friend class GridData;
	 friend class InitGrid;
	 friend class Cell;
	 friend class NodeIterator::CellNodeIterator;
	 friend class CellIterator::CellCellIterator;
	 
  protected:
	 
	 /// Member functions used for computing neighbor information
	 
	 /// Add this cell to the count of cell neighbors in all cell nodes
	 virtual void CountCell(Node *node_list) = 0;
	 /// Add this cell to the list of cell neighbors in all cell nodes
	 virtual void AddCell(Node *node_list, int *current, int thiscell) = 0;
	 /// Add *new* nodes from this to a list of nodes
	 virtual void AddNodes(int exclude_node, int *new_nodes, int *pos) = 0;
	 /// Compute all cell neighbors
	 virtual void ComputeCellNeighbors(Node *node_list, int thiscell) = 0;
	 
  private:

	 int  setID(int id);
	 void clear();
	 
	 int _id;
	 
	 int *neighbor_cells;
	 int _nc;
	 
	 int material;

	 virtual Node* getNode(int i) = 0;
	 virtual bool  neighbor(ShortList<Node *> &cn, Cell &cell) = 0;
	 
  };

}

#endif
