// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __NODE_HH
#define __NODE_HH

#include <iostream>
#include <dolfin/dolfin_constants.h>
#include <dolfin/Point.h>

namespace dolfin{

  class Cell;
  
  class Node{
  public:
	 
	 Node();
	 ~Node();
	 
	 void set(real x, real y, real z);
	 
	 
	 void Clear();
	 
	 /// --- Neigbor information
	 
	 /// Get number of cell neighbors
	 int GetNoCellNeighbors();
	 /// Get cell neighbor number i
	 int GetCellNeighbor(int i);
	 /// Get number of node neighbors
	 int GetNoNodeNeighbors();
	 /// Get node neighbor number i
	 int GetNodeNeighbor(int i);
	 
	 /// --- Accessor functions for stored data
	 
	 /// Set global node number
	 void SetNodeNo(int nn);  
	 /// Set coordinates
	 void SetCoord(float x, float y, float z);  
	 /// Get global node number
	 int GetNodeNo();  
	 /// Get coordinate i
	 real GetCoord(int i);
	 /// Get all coordinates
	 Point* GetCoord();
	 
	 /// Output
	 friend std::ostream &operator<<(std::ostream &os, const Node &node)
	 {
		os << "Node: x = [" << node.p.x << " " << node.p.y << " " << node.p.z << "]";
		return os;
	 }
	 
	 /// Friends
	 friend class Grid;
	 friend class Triangle;
	 friend class Tetrahedron;
	 friend class GridData;
	 
  protected:
	 
	 int setID(int id);
	 
	 /// Member functions used for computing neighbor information
	 
	 /// Allocate memory for list of neighbor cells
	 void AllocateForNeighborCells();
	 /// Check if this and the other node have a common cell neighbor
	 bool CommonCell(Node *n, int thiscell, int *cellnumber);
	 /// Check if this and the other two nodes have a common cell neighbor
	 bool CommonCell(Node *n1, Node *n2, int thiscell, int *cellnumber);
	 /// Return an upper bound for the number of node neighbors
	 int GetMaxNodeNeighbors(Cell **cell_list);
	 /// Compute node neighbors of the node
	 void ComputeNodeNeighbors(Cell **cell_list, int thisnode, int *tmp);
	 
  private:
	 
	 Point p;
	 
	 int id;
	 
	 int global_node_number;
	 
	 int *neighbor_nodes;
	 int *neighbor_cells;
	 int nn;
	 int nc;
	 
  };
  
}

#endif
