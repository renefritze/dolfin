// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

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
    Edge(Node *en1, Node *en2);
    ~Edge();
    
    void set(Node *en1, Node *en2);
    
    /// --- Accessor functions for stored data
  
      /// Get end node number i
      Node* node(int i);
      /// Get coordinates of end node number i
      Point coord(int i);
	
      /// --- Functions for mesh refinement
      void mark();
      void unmark();
      bool marked();
      
      /// --- Functions that require computation (every time!)
	  
      /// Compute and return the lenght of the edge
      real computeLength();
      /// Compute and return midpoint of the edge 
      Point computeMidpoint();
    
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

      // End nodes
      Node* _en0;
      Node* _en1;

      bool marked_for_refinement;

  };

}

#endif
