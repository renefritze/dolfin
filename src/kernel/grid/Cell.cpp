// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Node.h>
#include <dolfin/Edge.h>
#include <dolfin/GenericCell.h>
#include <dolfin/Triangle.h>
#include <dolfin/Tetrahedron.h>
#include <dolfin/Cell.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Cell::Cell()
{
  c = 0;
}
//-----------------------------------------------------------------------------
Cell::Cell(Node* n0, Node* n1, Node* n2)
{
  c = new Triangle(n0, n1, n2);
}
//-----------------------------------------------------------------------------
Cell::Cell(Node* n0, Node* n1, Node* n2, Node* n3)
{
  c = new Tetrahedron(n0, n1, n2, n3);
}
//-----------------------------------------------------------------------------
Cell::~Cell()
{
  if ( c )
    delete c;
}
//-----------------------------------------------------------------------------
int Cell::id() const
{
  if ( c )
    return c->id();

  return -1;
}
//-----------------------------------------------------------------------------
Cell::Type Cell::type() const
{
  if ( c )
    return c->type();

  return none;
}
//-----------------------------------------------------------------------------
int Cell::noNodes() const
{
  if ( c )
    return c->noNodes();

  return 0;
}
//-----------------------------------------------------------------------------
int Cell::noEdges() const
{
  if ( c )
    return c->noEdges();

  return 0;
}
//-----------------------------------------------------------------------------
int Cell::noFaces() const
{
  if ( c )
    return c->noFaces();

  return 0;
}
//-----------------------------------------------------------------------------
int Cell::noBoundaries() const
{
  if ( c )
    return c->noBoundaries();

  return 0;
}
//-----------------------------------------------------------------------------
int Cell::noCellNeighbors() const
{
  if ( c )
    return c->noCellNeighbors();

  return 0;
}
//-----------------------------------------------------------------------------
int Cell::noNodeNeighbors() const
{
  if ( c )
    return c->noNodeNeighbors();

  return 0;
}
//-----------------------------------------------------------------------------
int Cell::noChildren() const
{
  if ( c )
    return c->noChildren();

  return 0;
}
//-----------------------------------------------------------------------------
Node* Cell::node(int i) const
{
  if ( c )
    return c->node(i);

  return 0;
}
//-----------------------------------------------------------------------------
Edge* Cell::edge(int i) const
{
  if ( c )
    return c->edge(i);

  return 0;
}
//-----------------------------------------------------------------------------
Cell* Cell::neighbor(int i) const
{
  if ( c )
    return c->neighbor(i);

  return 0;
}
//-----------------------------------------------------------------------------
Cell* Cell::parent() const
{
  dolfin_assert(c);
  return c->parent();
}
//-----------------------------------------------------------------------------
Cell* Cell::child(int i) const
{
  dolfin_assert(c);
  return c->child(i);
}
//-----------------------------------------------------------------------------
Point Cell::coord(int i) const
{ 
  if ( c )
    return c->coord(i);

  Point p;
  return p;
}
//-----------------------------------------------------------------------------
Point Cell::midpoint() const
{
  if ( c )
    return c->midpoint();

  Point p;
  return p;
}
//-----------------------------------------------------------------------------
int Cell::nodeID(int i) const
{
  if ( c )
    return c->nodeID(i);
  
  return -1;
}
//-----------------------------------------------------------------------------
void Cell::mark()
{
  if ( !c )
    dolfin_error("You cannot mark an unspecified cell.");
  
  c->mark(this);
}
//-----------------------------------------------------------------------------
dolfin::LogStream& dolfin::operator<<(LogStream& stream, const Cell& cell)
{
  switch ( cell.type() ){
  case Cell::triangle:
    stream << "[Cell (triangle): id = " << cell.id() << " nodes = ( ";
    for (NodeIterator n(cell); !n.end(); ++n)
      stream << n->id() << " ";
    stream << ") ]";
    break;
  case Cell::tetrahedron:
    stream << "[Cell (tetrahedron): id = " << cell.id() << " nodes = ( ";
    for (NodeIterator n(cell); !n.end(); ++n)
      stream << n->id() << " ";
    stream << ") ]";
    break;
  default:
    dolfin_error("Unknown cell type");
  }	 
  
  return stream;
}
//-----------------------------------------------------------------------------
int Cell::setID(int id, Grid* grid)
{
  dolfin_assert(c);
  return c->setID(id, grid);
}
//-----------------------------------------------------------------------------
void Cell::setParent(Cell* parent)
{
  // Set parent cell: a cell is parent if the current cell is created through 
  // refinement of the parent cell. 
  dolfin_assert(c);
  return c->setParent(parent);
}
//-----------------------------------------------------------------------------
void Cell::addChild(Cell* child)
{
  // Set the child if cell not already contains the child cell: a cell 
  // is child if it is created through refinement of the current cell.  
  dolfin_assert(c);
  return c->addChild(child);
}
//-----------------------------------------------------------------------------
void Cell::set(Node* n0, Node* n1, Node* n2)
{
  if ( c )
    delete c;
  
  c = new Triangle(n0, n1, n2);
}
//-----------------------------------------------------------------------------
void Cell::set(Node* n0, Node* n1, Node* n2, Node* n3)
{
  if ( c )
    delete c;

  c = new Tetrahedron(n0, n1, n2, n3);
}
//-----------------------------------------------------------------------------
bool Cell::neighbor(Cell& cell) const
{
  if ( c )
    return c->neighbor(cell.c);
  
  return false;
}
//-----------------------------------------------------------------------------
bool Cell::haveEdge(Edge& edge) const
{
  dolfin_assert(c);
  return c->haveEdge(edge);
}
//-----------------------------------------------------------------------------
void Cell::createEdges()
{
  dolfin_assert(c);
  c->createEdges();
}
//-----------------------------------------------------------------------------
void Cell::createFaces()
{
  dolfin_assert(c);
  c->createFaces();
}
//-----------------------------------------------------------------------------
void Cell::createEdge(Node* n0, Node* n1)
{
  dolfin_assert(c);
  c->createEdge(n0, n1);
}
//-----------------------------------------------------------------------------
void Cell::createFace(Edge* e0, Edge* e1, Edge* e2)
{
  dolfin_assert(c);
  c->createFace(e0, e1, e2);
}
//-----------------------------------------------------------------------------
Edge* Cell::findEdge(Node* n0, Node* n1)
{
  dolfin_assert(c);
  return c->findEdge(n0, n1);
}
//-----------------------------------------------------------------------------
Face* Cell::findFace(Edge* e0, Edge* e1, Edge* e2)
{
  dolfin_assert(c);
  return c->findFace(e0, e1, e2);
}
//-----------------------------------------------------------------------------
void Cell::initMarker()
{
  dolfin_assert(c);
  c->initMarker();
}
//-----------------------------------------------------------------------------
CellMarker& Cell::marker()
{
  dolfin_assert(c);
  return c->marker();
}
//-----------------------------------------------------------------------------
CellStatus& Cell::status()
{
  dolfin_assert(c);
  return c->status();
}
//-----------------------------------------------------------------------------
bool& Cell::closed()
{
  dolfin_assert(c);
  return c->closed();
}
//-----------------------------------------------------------------------------
