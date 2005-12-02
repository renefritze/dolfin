// Copyright (C) 2003-2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Par Ingelstrom 2004.
//
// First added:  2003
// Last changed: 2005-12-01

#include <dolfin/dolfin_log.h>
#include <dolfin/dolfin_settings.h>
#include <dolfin/Mesh.h>
#include <dolfin/Cell.h>
#include <dolfin/Edge.h>
#include <dolfin/MeshInit.h>
#include <dolfin/MeshHierarchy.h>
#include <dolfin/MeshIterator.h>
#include <dolfin/TriMeshRefinement.h>
#include <dolfin/TetMeshRefinement.h>
#include <dolfin/MeshRefinement.h>
   
using namespace dolfin;

//-----------------------------------------------------------------------------
void MeshRefinement::refine(MeshHierarchy& meshes)
{
  dolfin_begin("Refining mesh:");

  // Check pre-condition for all meshes
  checkPreCondition(meshes);

  // Propagate markers for leaf elements
  propagateLeafMarks(meshes);

  // Mark edges
  updateEdgeMarks(meshes);

  // Refine mesh hierarchy
  globalRefinement(meshes);

  // Check post-condition for all meshes
  checkPostCondition(meshes);

  // FIXME: Unclear if sorting this affects the algorithm, since it is
  // FIXME: based on keeping the ordering

  // Sort all meshes locally
  sort(meshes);

  dolfin_end();
}
//-----------------------------------------------------------------------------
void MeshRefinement::propagateLeafMarks(MeshHierarchy& meshes)
{
  // We need to propagate the markers of leaf elements up through the
  // mesh hierarchy, since Bey's algorithm assumes that leaf elements
  // are not refined, whereas in our version leaf elements are copied
  // to the next level.  In Bey's algorithm, a leaf element is present
  // at all levels, starting at the level from which it is not further
  // refined, so we have to make sure that the copies of the leaf
  // elements in finer meshes are identical as far as the markers are
  // concerned.

  for (CellIterator c(meshes.fine()); !c.end(); ++c) {
    
    for (Cell* parent = c->parent(); parent; parent = parent->parent()) {

      if ( !leaf(*parent) )
	break;

      dolfin_assert(parent->noChildren() == 1);      
      parent->marker() = parent->child(0)->marker();

    }

  }
}
//-----------------------------------------------------------------------------
void MeshRefinement::updateEdgeMarks(MeshHierarchy& meshes)
{
  for (MeshIterator mesh(meshes); !mesh.end(); ++mesh)
    updateEdgeMarks(*mesh);
}
//-----------------------------------------------------------------------------
void MeshRefinement::globalRefinement(MeshHierarchy& meshes)
{
  // The global mesh refinement algorithm working on the whole mesh hierarchy.
  // This is algorithm GlobalRefinement() in Beys paper.

  // Phase I: Visit all meshes top-down
  for (MeshIterator mesh(meshes,last); !mesh.end(); --mesh) {

    // Evaluate marks for all levels but the finest
    if ( *mesh != meshes.fine() )
      evaluateMarks(*mesh);
    
    // Close mesh
    closeMesh(*mesh);
  }

  // Info message
  dolfin_info("Level 0: Initial mesh has %d cells",
	      meshes.coarse().noCells());

  // Phase II: Visit all meshes bottom-up
  for (MeshIterator mesh(meshes); !mesh.end(); ++mesh) {

    // Close mesh for all levels but the coarsest
    if ( *mesh != meshes.coarse() ) {
      updateEdgeMarks(*mesh);
      closeMesh(*mesh);
    }

    // Unrefine mesh
    unrefineMesh(*mesh, meshes);
    
    // Refine mesh
    refineMesh(*mesh);

    // Info message
    dolfin_info("Level %d: Refined mesh has %d cells",
		mesh.index() + 1, mesh->child().noCells());
  }

  // Update mesh hierarchy
  dolfin_log(false);
  meshes.init(meshes.coarse());
  dolfin_log(true);
}
//-----------------------------------------------------------------------------
void MeshRefinement::checkPreCondition(MeshHierarchy& meshes)
{
  // Marks should be marked_according_to_ref for all cells on coarser levels.
  for (MeshIterator mesh(meshes); !mesh.end(); ++mesh)
    if ( *mesh != meshes.fine() )
      for (CellIterator c(mesh); !c.end(); ++c)
	if ( c->marker() != Cell::marked_according_to_ref )
	  dolfin_error1("Mesh %d does not satisfy pre-condition for mesh refinement.",
			mesh.index());
  
  // Check marks for finest mesh
  for (CellIterator c(meshes.fine()); !c.end(); ++c)
    if ( ( c->marker() != Cell::marked_for_reg_ref ) &&
	 ( c->marker() != Cell::marked_for_no_ref )  &&
	 ( c->marker() != Cell::marked_for_coarsening ) )
      dolfin_error("Finest mesh does not satisfy pre-condition for mesh refinement.");
}
//-----------------------------------------------------------------------------
void MeshRefinement::checkPostCondition(MeshHierarchy& meshes)
{
  // Marks should be marked_according_to_ref for all cells on coarser levels.
  for (MeshIterator mesh(meshes); !mesh.end(); ++mesh)
    if ( *mesh != meshes.fine() )
      for (CellIterator c(mesh); !c.end(); ++c)
	if ( c->marker() != Cell::marked_according_to_ref )
	  dolfin_error1("Mesh %d does not satisfy post-condition for mesh refinement.",
			mesh.index());
  
  // Check marks for the new finest mesh
  for (CellIterator c(meshes.fine()); !c.end(); ++c)
    if ( c->marker() != Cell::marked_for_no_ref )
      dolfin_error("Finest mesh does not satisfy post-condition for mesh refinement.");
}
//-----------------------------------------------------------------------------
void MeshRefinement::checkNumbering(MeshHierarchy& meshes)
{
  // Check numbering (IDs) for all objects
  for (MeshIterator mesh(meshes); !mesh.end(); ++mesh) {

    // Check vertices
    for (VertexIterator n(mesh); !n.end(); ++n)
      if ( n->id() < 0 || n->id() >= mesh->noVertices() )
	dolfin_error1("Inconsistent vertex numbers at level %d.", mesh.index());

    // Check cells
    for (CellIterator c(mesh); !c.end(); ++c)
      if ( c->id() < 0 || c->id() >= mesh->noCells() )
	dolfin_error1("Inconsistent cell numbers at level %d.", mesh.index());

    // Check edges
    for (EdgeIterator e(mesh); !e.end(); ++e)
      if ( e->id() < 0 || e->id() >= mesh->noEdges() )
	dolfin_error1("Inconsistent edge numbers at level %d.", mesh.index());

    // Check faces
    for (FaceIterator f(mesh); !f.end(); ++f)
      if ( f->id() < 0 || f->id() >= mesh->noFaces() )
	dolfin_error1("Inconsistent face numbers at level %d.", mesh.index());

  }

  dolfin_debug("Object numbers are ok.");
}
//-----------------------------------------------------------------------------
void MeshRefinement::sort(MeshHierarchy& meshes)
{
  for (MeshIterator mesh(meshes); !mesh.end(); ++mesh)
  {
    MeshInit::sort(*mesh);
  }
}
//-----------------------------------------------------------------------------
void MeshRefinement::updateEdgeMarks(Mesh& mesh)
{
  // Clear all edge marks
  for (EdgeIterator e(mesh); !e.end(); ++e)
    e->clearMarks();

  // Mark edges of cells
  for (CellIterator c(mesh); !c.end(); ++c)
    updateEdgeMarks(*c);
}
//-----------------------------------------------------------------------------
void MeshRefinement::evaluateMarks(Mesh& mesh)
{
  // Evaluate and adjust marks for a mesh.
  // This is algorithm EvaluateMarks() in Beys paper.

  for (CellIterator c(mesh); !c.end(); ++c) {

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
void MeshRefinement::closeMesh(Mesh& mesh)
{
  // Perform the green closure on a mesh.
  // This is algorithm CloseMesh() in Bey's paper.

  // Make sure that the numbers are correct since we use an array
  // of indices (IDs) to temporarily store data.
  MeshInit::renumber(mesh);

  // Keep track of which cells are in the list
  PArray<bool> closed(mesh.noCells());
  closed = true;
  
  // Create a list of all elements that need to be closed
  PList<Cell*> cells;
  for (CellIterator c(mesh); !c.end(); ++c) {

    // Remove marks for irregular refinement
    if (c->marker() == Cell::marked_for_irr_ref_1 ||
	c->marker() == Cell::marked_for_irr_ref_2 ||
	c->marker() == Cell::marked_for_irr_ref_3 ||
	c->marker() == Cell::marked_for_irr_ref_4)
      c->marker() = Cell::marked_for_no_ref;
    
    if ( edgeMarkedByOther(*c) && okToRefine(*c) ) {
      cells.add(c);
      closed(c->id()) = false;
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
void MeshRefinement::refineMesh(Mesh& mesh)
{
  // Refine a mesh according to marks.
  // This is algorithm RefineMesh() in Bey's paper.

  // Change markers from marked_for_coarsening to marked_for_no_ref
  for (CellIterator c(mesh); !c.end(); ++c)
    if ( c->marker() == Cell::marked_for_coarsening )
      c->marker() = Cell::marked_for_no_ref;

  // Refine cells which are not marked_according_to_ref
  for (CellIterator c(mesh); !c.end(); ++c) {

    // Skip cells which are marked_according_to_ref
    if ( c->marker() == Cell::marked_according_to_ref )
      continue;

    // Refine according to refinement rule
    refine(*c, mesh.child());
    
  }

  // Compute connectivity for child
  dolfin_log(false);
  mesh.child().init();
  dolfin_log(true);

  // Update edge marks
  updateEdgeMarks(mesh.child());
}
//-----------------------------------------------------------------------------
void MeshRefinement::unrefineMesh(Mesh& mesh, const MeshHierarchy& meshes)
{
  // Unrefine a mesh according to marks.
  // This is algorithm UnrefineMesh() in Bey's paper.

  // Get child mesh or create a new child mesh
  Mesh* child = 0;
  if ( mesh == meshes.fine() )
    child = &mesh.createChild();
  else
    child = &mesh.child();

  // Make sure that the numbers are correct since we use arrays
  // of indices (IDs) to temporarily store data.
  MeshInit::renumber(*child);

  // Mark all vertices in the child for not re-use
  PArray<bool> reuse_vertex(child->noVertices());
  reuse_vertex = false;

  // Mark all cells in the child for not re-use
  PArray<bool> reuse_cell(child->noCells());
  reuse_cell = false;

  // Mark vertices and cells for reuse
  for (CellIterator c(mesh); !c.end(); ++c) {

    // To avoid removing the entire fine mesh
    if ( (c->marker() == Cell::marked_for_no_ref) &&
	 ( c->status() == Cell::unref ) &&
	 ( c->noChildren() == 1) )
      c->marker() = Cell::marked_according_to_ref;

    if ( (c->marker() == Cell::marked_for_reg_ref) &&
	 ( c->status() == Cell::ref_reg ) &&
	 ( c->noChildren() == 8 ) )
      c->marker() = Cell::marked_according_to_ref;

    // Skip cells which are not marked according to refinement
    if ( c->marker() != Cell::marked_according_to_ref )
      continue;

    // Mark children of the cell for re-use
    for (int i = 0; i < c->noChildren(); i++) {
      reuse_cell(c->child(i)->id()) = true;
      for (VertexIterator n(*c->child(i)); !n.end(); ++n)
	reuse_vertex(n->id()) = true;
    }
  }

  // Remove all vertices in the child not marked for re-use
  for (VertexIterator n(*child); !n.end(); ++n)
    if ( !reuse_vertex(n->id()) )
      removeVertex(*n, *child);
  
  // Remove all cells in the child not marked for re-use
  for (CellIterator c(*child); !c.end(); ++c)
    if ( !reuse_cell(c->id()) )
      removeCell(*c, *child);
}
//-----------------------------------------------------------------------------
void MeshRefinement::closeCell(Cell& cell,
			       PList<Cell*>& cells, PArray<bool>& closed)
{
  // Close a cell, either by regular or irregular refinement. We check all
  // edges of the cell and try to find a matching refinement rule. This rule
  // is then assigned to the cell's marker for later refinement of the cell.
  // This is algorithm CloseElement() in Bey's paper.
  
  // First count the number of marked edges in the cell
  int no_marked_edges = noMarkedEdges(cell);

  // Check which rule should be applied
  if ( checkRule(cell, no_marked_edges) )
  {
    closed(cell.id()) = true;
    return;
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
      if ( c->haveEdge(*e) && okToRefine(*c) && c != cell && closed(c->id()) )
	cells.add(c);
  }

  // Remember that the cell has been closed, important since we don't want
  // to add cells which are not yet closed (and are already in the list).
  closed(cell.id()) = true;
}
//-----------------------------------------------------------------------------
bool MeshRefinement::checkRule(Cell& cell, int no_marked_edges)
{
  switch ( cell.type() ) {
  case Cell::triangle:
    return TriMeshRefinement::checkRule(cell, no_marked_edges);
    break;
  case Cell::tetrahedron:
    return TetMeshRefinement::checkRule(cell, no_marked_edges);
    break;
  default:
    dolfin_error("Unknown cell type.");
  }

  return false;
}
//-----------------------------------------------------------------------------
void MeshRefinement::refine(Cell& cell, Mesh& mesh)
{
  switch ( cell.type() ) {
  case Cell::triangle:
    TriMeshRefinement::refine(cell, mesh);
    break;
  case Cell::tetrahedron:
    TetMeshRefinement::refine(cell, mesh);
    break;
  default:
    dolfin_error("Unknown cell type.");
  }
}
//-----------------------------------------------------------------------------
void MeshRefinement::updateEdgeMarks(Cell& cell)
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
bool MeshRefinement::childrenMarkedForCoarsening(Cell& cell)
{
  for (int i = 0; i < cell.noChildren(); i++)
    if ( cell.child(i)->marker() != Cell::marked_for_coarsening )
      return false;
    
  return true;
}
//-----------------------------------------------------------------------------
bool MeshRefinement::edgeOfChildMarkedForRefinement(Cell& cell)
{
  for (int i = 0; i < cell.noChildren(); i++)
    for (EdgeIterator e(*cell.child(i)); !e.end(); ++e)
      if ( e->marked() )
	return true;

  return false;
}
//-----------------------------------------------------------------------------
bool MeshRefinement::edgeMarkedByOther(Cell& cell)
{
  for (EdgeIterator e(cell); !e.end(); ++e)
    if ( e->marked() ) 
      if ( !e->marked(cell) )
	return true;

  return false;
}
//-----------------------------------------------------------------------------
void MeshRefinement::sortVertices(const Cell& cell, PArray<Vertex*>& vertices)
{
  // Set the size of the list
  vertices.init(cell.noVertices());

  // Count the number of marked edges for each vertex
  PArray<int> no_marked_edges(vertices.size());
  no_marked_edges = 0;
  for (EdgeIterator e(cell); !e.end(); ++e) {
    if ( e->marked() ) {
      no_marked_edges(vertexNumber(e->vertex(0), cell))++;
      no_marked_edges(vertexNumber(e->vertex(1), cell))++;
    }
  }

  // Sort the vertices according to the number of marked edges, the vertex
  // with the most number of edges is placed first.
  int max_edges = no_marked_edges.max();
  int pos = 0;
  for (int i = max_edges; i >= 0; i--) {
    for (int j = 0; j < vertices.size(); j++) {
      if ( no_marked_edges(j) >= i ) {
	vertices(pos++) = &cell.vertex(j);
	no_marked_edges(j) = -1;
      }
    }
  }
}
//-----------------------------------------------------------------------------
int MeshRefinement::noMarkedEdges(const Cell& cell)
{
  int count = 0;
  for (EdgeIterator e(cell); !e.end(); ++e)
    if ( e->marked() )
      count++;
  return count;
}
//-----------------------------------------------------------------------------
int MeshRefinement::vertexNumber(const Vertex& vertex, const Cell& cell)
{
  // Find the local vertex number for a given vertex within a cell
  for (VertexIterator n(cell); !n.end(); ++n)
    if ( n == vertex )
      return n.index();
  
  // Didn't find the vertex
  dolfin_error("Unable to find vertex within cell.");
  return -1;
}
//-----------------------------------------------------------------------------
bool MeshRefinement::leaf(Cell& cell)
{
  return cell.status() == Cell::unref;
}
//-----------------------------------------------------------------------------
bool MeshRefinement::okToRefine(Cell& cell)
{
  // Get the cell's parent
  Cell* parent = cell.parent();

  // Can be refined if it has no parent (belongs to coarsest mesh)
  if ( !parent )
    return true;

  // If cell has a parent, the parent can not be irregularly refined
  return parent->status() != Cell::ref_irr;
}
//-----------------------------------------------------------------------------
Vertex& MeshRefinement::createVertex(Vertex& vertex, Mesh& mesh, const Cell& cell)
{
  // Create the vertex
  Vertex& n = createVertex(vertex.coord(), mesh, cell);

  // Set parent-child info
  n.setParent(vertex);
  vertex.setChild(n);

  return n;
}
//-----------------------------------------------------------------------------
Vertex& MeshRefinement::createVertex(const Point& p, Mesh& mesh, const Cell& cell)
{
  // First check with the children of the neighbors of the cell if the
  // vertex already exists. Note that it is not enough to only check
  // neighbors of the cell, since neighbors are defined as having a
  // common edge. We need to check all vertices within the cell and for
  // each vertex check the cell neighbors of that vertex.

  for (VertexIterator n(cell); !n.end(); ++n) {
    for (CellIterator c(n); !c.end(); ++c) {
      for (int i = 0; i < c->noChildren(); i++) {

	// FIXME: No children should be null!
	Cell* child = c->child(i);
	if ( !child )
	  continue;

	Vertex* new_vertex = child->findVertex(p);
	if ( new_vertex )
	  return *new_vertex;
	
      }
    }
  }

  // Create vertex if it doesn't exist
  return mesh.createVertex(p);
}
//-----------------------------------------------------------------------------
void MeshRefinement::removeVertex(Vertex& vertex, Mesh& mesh)
{
  // Update parent-child info for parent
  if ( vertex.parent() )
    vertex.parent()->removeChild();

  // Remove vertex
  mesh.remove(vertex);
}
//-----------------------------------------------------------------------------
void MeshRefinement::removeCell(Cell& cell, Mesh& mesh)
{
  // Only leaf elements should be removed
  dolfin_assert(leaf(cell));

  // Update parent-child info for parent
  if ( cell.parent() )
    cell.parent()->removeChild(cell);

  // Remove children
  for (int i = 0; i < cell.noChildren(); i++)
    removeCell(*cell.child(i), mesh.child());
  
  // Update status 
  if ( cell.parent()->noChildren() == 0 )
    cell.parent()->status() = Cell::unref; 
  
  // Remove cell
  mesh.remove(cell);
}
//-----------------------------------------------------------------------------

