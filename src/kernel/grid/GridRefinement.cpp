// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/Grid.h>
#include <dolfin/Cell.h>
#include <dolfin/Edge.h>
#include <dolfin/GridInit.h>
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

  // Check pre-condition for all grids
  checkPreCondition(grids);

  // Mark edges
  updateEdgeMarks(grids);

  // Refine grid hierarchy
  globalRefinement(grids);

  // Check post-condition for all grids
  checkPostCondition(grids);

  dolfin_end();
}
//-----------------------------------------------------------------------------
void GridRefinement::updateEdgeMarks(GridHierarchy& grids)
{
  for (GridIterator grid(grids); !grid.end(); ++grid)
    updateEdgeMarks(*grid);
}
//-----------------------------------------------------------------------------
void GridRefinement::globalRefinement(GridHierarchy& grids)
{
  // The global grid refinement algorithm working on the whole grid hierarchy.
  // This is algorithm GlobalRefinement() in Beys paper.

  // Phase I: Visit all grids top-down
  for (GridIterator grid(grids,last); !grid.end(); --grid) {
    if ( *grid != grids.fine() ) {
      cout << "--- Evaluate marks at level " << grid.index() << endl;
      evaluateMarks(*grid);
    }
    cout << "--- Close grid at level " << grid.index() << endl;
    closeGrid(*grid);
  }

  cout << endl;

  // Phase II: Visit all grids bottom-up
  for (GridIterator grid(grids); !grid.end(); ++grid) {
    if ( *grid != grids.coarse() ) {
      cout << "--- Close grid at level " << grid.index() << endl;
      closeGrid(*grid);
    }
    cout << "--- Unrefine at level " << grid.index() << endl;
    unrefineGrid(*grid, grids);
    cout << "--- Refine at level " << grid.index() << endl;
    refineGrid(*grid);
  }

  // Update grid hierarchy
  dolfin_log(false);
  grids.init(grids.coarse());
  dolfin_log(true);
}
//-----------------------------------------------------------------------------
void GridRefinement::checkPreCondition(GridHierarchy& grids)
{
  // Marks should be marked_according_to_ref for all cells on coarser levels.
  for (GridIterator grid(grids); !grid.end(); ++grid)
    if ( *grid != grids.fine() )
      for (CellIterator c(grid); !c.end(); ++c)
	if ( c->marker() != Cell::marked_according_to_ref )
	  dolfin_error1("Grid %d does not satisfy pre-condition for grid refinement.",
			grid.index());
  
  // Check marks for finest grid
  for (CellIterator c(grids.fine()); !c.end(); ++c)
    if ( ( c->marker() != Cell::marked_for_reg_ref ) &&
	 ( c->marker() != Cell::marked_for_no_ref )  &&
	 ( c->marker() != Cell::marked_for_coarsening ) )
      dolfin_error("Finest grid does not satisfy pre-condition for grid refinement.");
  
  dolfin_debug("Checked pre-condition for grids.");
}
//-----------------------------------------------------------------------------
void GridRefinement::checkPostCondition(GridHierarchy& grids)
{
  // Marks should be marked_according_to_ref for all cells on coarser levels.
  for (GridIterator grid(grids); !grid.end(); ++grid)
    if ( *grid != grids.fine() )
      for (CellIterator c(grid); !c.end(); ++c)
	if ( c->marker() != Cell::marked_according_to_ref )
	  dolfin_error1("Grid %d does not satisfy post-condition for grid refinement.",
			grid.index());
  
  // Check marks for the new finest grid
  for (CellIterator c(grids.fine()); !c.end(); ++c)
    if ( c->marker() != Cell::marked_for_no_ref )
      dolfin_error("Finest grid does not satisfy post-condition for grid refinement.");

  dolfin_debug("Checked post-condition for grids.");
}
//-----------------------------------------------------------------------------
void GridRefinement::checkNumbering(GridHierarchy& grids)
{
  // Check numbering (IDs) for all objects
  for (GridIterator grid(grids); !grid.end(); ++grid) {

    // Check nodes
    for (NodeIterator n(grid); !n.end(); ++n)
      if ( n->id() < 0 || n->id() >= grid->noNodes() )
	dolfin_error1("Inconsistent node numbers at level %d.", grid.index());

    // Check cells
    for (CellIterator c(grid); !c.end(); ++c)
      if ( c->id() < 0 || c->id() >= grid->noCells() )
	dolfin_error1("Inconsistent cell numbers at level %d.", grid.index());

    // Check edges
    for (EdgeIterator e(grid); !e.end(); ++e)
      if ( e->id() < 0 || e->id() >= grid->noEdges() )
	dolfin_error1("Inconsistent edge numbers at level %d.", grid.index());

    // Check faces
    for (FaceIterator f(grid); !f.end(); ++f)
      if ( f->id() < 0 || f->id() >= grid->noFaces() )
	dolfin_error1("Inconsistent face numbers at level %d.", grid.index());

  }

  dolfin_debug("Object numbers are ok.");
}
//-----------------------------------------------------------------------------
void GridRefinement::updateEdgeMarks(Grid& grid)
{
  // Clear all edge marks
  for (EdgeIterator e(grid); !e.end(); ++e)
    e->clearMarks();

  // Mark edges of cells
  for (CellIterator c(grid); !c.end(); ++c)
    updateEdgeMarks(*c);
}
//-----------------------------------------------------------------------------
void GridRefinement::evaluateMarks(Grid& grid)
{
  // Evaluate and adjust marks for a grid.
  // This is algorithm EvaluateMarks() in Beys paper.

  for (CellIterator c(grid); !c.end(); ++c) {

    // Coarsening
    if ( c->status() == Cell::ref_reg && childrenMarkedForCoarsening(*c) )
      c->marker() = Cell::marked_for_no_ref;
    
    // Adjust marks for irregularly refined cells
    if ( c->status() == Cell::ref_irr ) {
      if ( edgeOfChildMarkedForRefinement(*c) )
	c->marker() = Cell::marked_for_reg_ref;
      else
      	c->marker() = Cell::marked_according_to_ref;
    }

    // Update edge marks
    updateEdgeMarks(*c);

  }

}
//-----------------------------------------------------------------------------
void GridRefinement::closeGrid(Grid& grid)
{
  // Perform the green closure on a grid.
  // This is algorithm CloseGrid() in Bey's paper.

  // Make sure that the numbers are correct since we use an array
  // of indices (IDs) to temporarily store data.
  GridInit::renumber(grid);

  // Keep track of which cells are in the list
  Array<bool> closed(grid.noCells());
  closed = true;

  // Create a list of all elements that need to be closed
  List<Cell*> cells;
  for (CellIterator c(grid); !c.end(); ++c) {
    
    if ( c->midpoint().dist(0.333, 0.333) < 0.01 ) {
      cout << "  Checking if cell needs to be closed: " << *c << endl;
      for (EdgeIterator e(c); !e.end(); ++e)
	cout << "    edge = " << *e << " marked = " << e->marked() 
	     << " midpoint = " << e->midpoint() << endl;
    }

    if ( c->status() == Cell::ref_reg || c->status() == Cell::unref ) {
      if ( edgeMarkedByOther(*c) ) {
	cells.add(c);
	closed(c->id()) = false;	

	if ( c->midpoint().dist(0.333, 0.333) < 0.01 )
	  cout << "  Needs to be closed!" << endl;
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

  // Update edge marks
  updateEdgeMarks(grid.child());

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

  // Make sure that the numbers are correct since we use arrays
  // of indices (IDs) to temporarily store data.
  GridInit::renumber(*child);

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

  if ( cell.midpoint().dist(0.333, 0.333) < 0.01 ) {
    cout << "    Closing cell with midpoint = " << cell.midpoint() << endl;
    cout << "    number of marked edges = " << no_marked_edges << endl;
  }

  // Check which rule should be applied
  if ( checkRule(cell, no_marked_edges) )
    return;

  if ( cell.midpoint().dist(0.333, 0.333) < 0.01 ) {
    cout << "    Unable to find refinement rule" << endl;
  }

  // If we didn't find a matching refinement rule, mark cell for regular
  // refinement and add cells containing the previously unmarked edges
  // to the list of cells that need to be closed.

  // Mark cell for regular refinement
  cell.marker() = Cell::marked_for_reg_ref;

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
void GridRefinement::updateEdgeMarks(Cell& cell)
{
  // Mark all edges of the cell if the cell is marked for regular refinement
  // or if the cell is refined regularly and marked_according_to_ref.

  if ( cell.marker() == Cell::marked_for_reg_ref ||
       (cell.marker() == Cell::marked_according_to_ref &&
	cell.status() == Cell::ref_reg) )
    for (EdgeIterator e(cell); !e.end(); ++e)
      e->mark(cell);
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
bool GridRefinement::leaf(Cell& cell)
{
  return cell.status() == Cell::unref;
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
  // Only leaf elements should be removed
  dolfin_assert(leaf(cell));

  // Update parent-child info for parent
  if ( cell.parent() )
    cell.parent()->removeChild(cell);

  // Remove children (leaf element can have 0 or 1 childs)
  if ( cell.noChildren() > 0 ) {
    dolfin_assert(cell.noChildren() == 1);
    removeCell(*cell.child(0), grid.child());
  }
  
  // Update status 
  if ( cell.parent()->noChildren() == 0 )
    cell.parent()->status() = Cell::unref; 
  
  // Remove cell
  grid.remove(cell);
}
//-----------------------------------------------------------------------------
