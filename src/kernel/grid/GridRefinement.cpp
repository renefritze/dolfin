// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/Grid.h>
#include <dolfin/Cell.h>
#include <dolfin/Edge.h>
#include <dolfin/GridHierarchy.h>
#include <dolfin/GridIterator.h>
#include <dolfin/TriGridRefinement.h>
#include <dolfin/TetGridRefinement.h>
#include <dolfin/GridRefinement.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void GridRefinement::refine(GridHierarchy& grids)
{
  dolfin_start("Refining grids:");

  // Refine grid hierarchy
  globalRefinement(grids);

  dolfin_end();
}
//-----------------------------------------------------------------------------
void GridRefinement::globalRefinement(GridHierarchy& grids)
{
  // The global grid refinement algorithm working on the whole grid hierarchy.
  // This is algorithm GlobalRefinement() in Beys paper.

  // Phase I: Visit all grids top-down
  for (GridIterator grid(grids,last); !grid.end(); --grid) {
    cout << "--- Level " << grid.index() << ": evaluate marks" << endl;
    evaluateMarks(*grid);
    cout << "--- Level " << grid.index() << ": close grid" << endl;
    closeGrid(*grid);
  }
  
  // Phase II: Visit all grids bottom-up
  for (GridIterator grid(grids); !grid.end(); ++grid) {
    if (grid.index() > 0) {
      cout << "--- Level " << grid.index() << ": close grid" << endl;
      closeGrid(*grid);
    }
    cout << "--- Level " << grid.index() << ": unrefine grid" << endl;
    unrefineGrid(*grid, grids);
    cout << "--- Level " << grid.index() << ": refine grid" << endl;
    refineGrid(*grid);
  }

  // Update grid hierarchy
  dolfin_log(false);
  grids.init(grids.coarse());
  dolfin_log(true);
}
//-----------------------------------------------------------------------------
void GridRefinement::evaluateMarks(Grid& grid)
{
  // Evaluate and adjust marks for a grid.
  // This is algorithm EvaluateMarks() in Beys paper.

  for (CellIterator c(grid); !c.end(); ++c) {

    if ( c->status() == Cell::ref_reg && childrenMarkedForCoarsening(*c) )
      c->marker() = Cell::marked_for_no_ref;
    
    if ( c->status() == Cell::ref_irr ) {
      if ( edgeOfChildMarkedForRefinement(*c) )
	c->marker() = Cell::marked_for_reg_ref;
      else
      	c->marker() = Cell::marked_according_to_ref;
    }

  }
}
//-----------------------------------------------------------------------------
void GridRefinement::closeGrid(Grid& grid)
{
  // Perform the green closure on a grid.
  // This is algorithm CloseGrid() in Bey's paper.

  // Keep track of which cells are in the list
  Array<bool> closed(grid.noCells());
  closed = true;

  // Create a list of all elements that need to be closed
  List<Cell*> cells;
  for (CellIterator c(grid); !c.end(); ++c) {
    if ( c->status() == Cell::ref_reg || c->status() == Cell::unref ) {
      if ( edgeMarkedByOther(*c) ) {
	cells.add(c);
	closed(c->id()) = false;
      }
    }
  }

  // Repeat until the list of elements is empty
  while ( !cells.empty() ) {

    // Get first cell and remove it from the list
    Cell* cell = cells.pop();

    // Close cell
    closeCell(*cell, cells, closed);

  }

}
//-----------------------------------------------------------------------------
void GridRefinement::refineGrid(Grid& grid)
{
  // Refine a grid according to marks.
  // This is algorithm RefineGrid() in Bey's paper.

  // Change markers from marked_for_coarsening to marked_for_no_ref
  for (CellIterator c(grid); !c.end(); ++c)
    if ( c->marker() == Cell::marked_for_coarsening )
      c->marker() = Cell::marked_for_no_ref;

  // Refine cells which are not marked_according_to_ref
  for (CellIterator c(grid); !c.end(); ++c) {

    // Skip cells which are marked_according_to_ref
    if ( c->marker() == Cell::marked_according_to_ref )
      continue;

    // Refine according to refinement rule
    refine(*c, grid.child());
    
  }

  // Compute connectivity for child
  dolfin_log(false);
  grid.child().init();
  dolfin_log(true);

  cout << "Refined grid has " << grid.noCells() << " cells." << endl;
}
//-----------------------------------------------------------------------------
void GridRefinement::unrefineGrid(Grid& grid, const GridHierarchy& grids)
{
  // Unrefine a grid according to marks.
  // This is algorithm UnrefineGrid() in Bey's paper.

  // Get child grid or create a new child grid
  Grid* child = 0;
  if ( grid == grids.fine() )
    child = &grid.createChild();
  else
    child = &grid.child();

  // Mark all nodes in the child for not re-use
  Array<bool> reuse_node(child->noNodes());
  reuse_node = false;

  // Mark all cells in the child for not re-use
  Array<bool> reuse_cell(child->noCells());
  reuse_cell = false;

  // Mark nodes and cells for reuse
  for (CellIterator c(grid); !c.end(); ++c) {

    // Skip cells which are not marked according to refinement
    if ( c->marker() != Cell::marked_according_to_ref )
      continue;

    // Mark children of the cell for re-use
    for (int i = 0; i < c->noChildren(); i++) {
      reuse_cell(c->child(i)->id()) = true;
      for (NodeIterator n(*c->child(i)); !n.end(); ++n)
	reuse_node(n->id()) = true;
    }

  }

  // Remove all nodes in the child not marked for re-use
  for (NodeIterator n(*child); !n.end(); ++n)
    if ( !reuse_node(n->id()) )
      removeNode(*n,*child);
  
  // Remove all cells in the child not marked for re-use
  for (CellIterator c(*child); !c.end(); ++c)
    if ( !reuse_cell(c->id()) )
      removeCell(*c,*child);
}
//-----------------------------------------------------------------------------
void GridRefinement::closeCell(Cell& cell,
			       List<Cell*>& cells, Array<bool>& closed)
{
  // Close a cell, either by regular or irregular refinement. We check all
  // edges of the cell and try to find a matching refinement rule. This rule
  // is then assigned to the cell's marker for later refinement of the cell.
  // This is algorithm CloseElement() in Bey's paper.
  
  // First count the number of marked edges in the cell
  int no_marked_edges = noMarkedEdges(cell);

  // Check which rule should be applied
  if ( checkRule(cell, no_marked_edges) )
    return;

  // If we didn't find a matching refinement rule, mark cell for regular
  // refinement and add cells containing the previously unmarked edges
  // to the list of cells that need to be closed.

  for (EdgeIterator e(cell); !e.end(); ++e) {

    // Skip marked edges
    if ( e->marked() )
      continue;

    // Mark edge by this cell
    e->mark(cell);

    // Add neighbors to the list of cells that need to be closed
    for (CellIterator c(cell); !c.end(); ++c)
      if ( c->haveEdge(*e) && c->status() == Cell::ref_reg && c != cell && closed(cell.id()) )
	  cells.add(c);
  }

  // Mark cell for regular refinement
  cell.marker() = Cell::marked_for_reg_ref;

  // Remember that the cell has been closed, important since we don't want
  // to add cells which are not yet closed (and are already in the list).
  closed(cell.id()) = true;
}
//-----------------------------------------------------------------------------
bool GridRefinement::checkRule(Cell& cell, int no_marked_edges)
{
  switch ( cell.type() ) {
  case Cell::triangle:
    return TriGridRefinement::checkRule(cell, no_marked_edges);
    break;
  case Cell::tetrahedron:
    return TetGridRefinement::checkRule(cell, no_marked_edges);
    break;
  default:
    dolfin_error("Unknown cell type.");
  }

  return false;
}
//-----------------------------------------------------------------------------
void GridRefinement::refine(Cell& cell, Grid& grid)
{
  switch ( cell.type() ) {
  case Cell::triangle:
    TriGridRefinement::refine(cell, grid);
    break;
  case Cell::tetrahedron:
    TetGridRefinement::refine(cell, grid);
    break;
  default:
    dolfin_error("Unknown cell type.");
  }
}
//-----------------------------------------------------------------------------
bool GridRefinement::childrenMarkedForCoarsening(Cell& cell)
{
  for (int i = 0; i < cell.noChildren(); i++)
    if ( cell.child(i)->marker() != Cell::marked_for_coarsening )
      return false;
    
  return true;
}
//-----------------------------------------------------------------------------
bool GridRefinement::edgeOfChildMarkedForRefinement(Cell& cell)
{
  for (int i = 0; i < cell.noChildren(); i++)
    for (EdgeIterator e(*cell.child(i)); !e.end(); ++e)
      if ( e->marked() )
	return true;

  return false;
}
//-----------------------------------------------------------------------------
bool GridRefinement::edgeMarkedByOther(Cell& cell)
{
  for (EdgeIterator e(cell); !e.end(); ++e)
    if ( e->marked() )
      if ( !e->marked(cell) )
	return true;

  return false;
}
//-----------------------------------------------------------------------------
void GridRefinement::sortNodes(const Cell& cell, Array<Node*>& nodes)
{
  // Set the size of the list
  nodes.init(cell.noNodes());

  // Count the number of marked edges for each node
  Array<int> no_marked_edges(nodes.size());
  no_marked_edges = 0;
  for (EdgeIterator e(cell); !e.end(); ++e) {
    if ( e->marked() ) {
      no_marked_edges(nodeNumber(e->node(0), cell))++;
      no_marked_edges(nodeNumber(e->node(1), cell))++;
    }
  }

  // Sort the nodes according to the number of marked edges, the node
  // with the most number of edges is placed first.
  int max_edges = no_marked_edges.max();
  int pos = 0;
  for (int i = max_edges; i >= 0; i--) {
    for (int j = 0; j < nodes.size(); j++) {
      if ( no_marked_edges(j) >= i ) {
	nodes(pos++) = &cell.node(j);
	no_marked_edges(j) = -1;
      }
    }
  }
}
//-----------------------------------------------------------------------------
int GridRefinement::noMarkedEdges(const Cell& cell)
{
  int count = 0;
  for (EdgeIterator e(cell); !e.end(); ++e)
    if ( e->marked() )
      count++;
  return count;
}
//-----------------------------------------------------------------------------
int GridRefinement::nodeNumber(const Node& node, const Cell& cell)
{
  // Find the local node number for a given node within a cell
  for (NodeIterator n(cell); !n.end(); ++n)
    if ( n == node )
      return n.index();
  
  // Didn't find the node
  dolfin_error("Unable to find node within cell.");
  return -1;
}
//-----------------------------------------------------------------------------
Node& GridRefinement::createNode(Node& node, Grid& grid, const Cell& cell)
{
  // Create the node
  Node& n = createNode(node.coord(), grid, cell);

  // Set parent-child info
  n.setParent(node);
  node.setChild(n);

  return n;
}
//-----------------------------------------------------------------------------
Node& GridRefinement::createNode(const Point& p, Grid& grid, const Cell& cell)
{
  // First check with the children of the neighbors of the cell if the
  // node already exists. Note that it is not enough to only check
  // neighbors of the cell, since neighbors are defined as having a
  // common edge. We need to check all nodes within the cell and for
  // each node check the cell neighbors of that node.

  for (NodeIterator n(cell); !n.end(); ++n) {
    for (CellIterator c(n); !c.end(); ++c) {
      for (int i = 0; i < c->noChildren(); i++) {
	Node* new_node = c->child(i)->findNode(p);
	if ( new_node )
	  return *new_node;
      }
    }
  }

  // Create node if it doesn't exist
  return grid.createNode(p);
}
//-----------------------------------------------------------------------------
void GridRefinement::removeNode(Node& node, Grid& grid)
{
  // Update parent-child info for parent
  if ( node.parent() )
    node.parent()->removeChild();

  // Remove node
  grid.remove(node);
}
//-----------------------------------------------------------------------------
void GridRefinement::removeCell(Cell& cell, Grid& grid)
{
  // Update parent-child info for parent
  cell.parent()->removeChild(cell);

  // Update status 
  if ( cell.parent()->noChildren() == 0 )
    cell.parent()->status() = Cell::unref; 

  // Remove node
  grid.remove(cell);
}
//-----------------------------------------------------------------------------
