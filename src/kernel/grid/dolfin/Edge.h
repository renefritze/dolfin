// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

// A couple of comments:
//
//   - More methods should be private?
//   - Why _en0, _en1 instead of just n0, n1?

#ifndef __EDGE_HH
#define __EDGE_HH

#include <dolfin/dolfin_log.h>
#include <dolfin/CellIterator.h>
#include <dolfin/NodeIterator.h>
#include <dolfin/EdgeIterator.h>

namespace dolfin {

  class Node;
  class Point;
  class Grid;

  class Edge{
  public:
    
    Edge();
    Edge(Node *en0, Node *en1);
    ~Edge();
    
    void set(Node *en0, Node *en1);
    
    /// --- Accessor functions for stored data
  
      /// Get end node number i
      Node* node(int i);
      /// Get coordinates of end node number i
      Point coord(int i);
	
      /// --- Functions for mesh refinement
      int  level() const;
      int  id() const;
      void setLevel(int level);
      void mark();
      void unmark();
      bool marked();
      int  refinedByCells();

      Cell* refinedByCell(int i);
      void  setRefinedByCell(Cell* c);

      void setMarkedForReUse(bool re_use);
      bool markedForReUse();

      /// --- Functions that require computation (every time!)
	  
      /// Compute and return the lenght of the edge
      real length();
      /// Compute and return midpoint of the edge 
      Point midpoint();
    
      // Friends
      friend class Grid;
      friend class Node;
      friend class GridData;
      friend class InitGrid;
      friend class NodeIterator::CellNodeIterator;
      friend class CellIterator::CellCellIterator;
      friend class EdgeIterator::CellEdgeIterator;
      friend class Triangle;
      friend class Tetrahedron;
	 
  protected:
  
  private:
	
      // Global edge number
      int _id;
      void setID(int id);

      // Refinement level in grid hierarchy, coarsest grid is level = 0
      int _level;

      // End nodes
      Node* _en0;
      Node* _en1;

      bool _marked_for_re_use;

      bool marked_for_refinement;

      ShortList<Cell *> refined_by_cell; 
      int _no_cells_refined;

  };

}

#endif
