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
Cell::Cell(Node& n0, Node& n1, Node& n2)
{
  c = new Triangle(n0, n1, n2);
}
//-----------------------------------------------------------------------------
Cell::Cell(Node& n0, Node& n1, Node& n2, Node& n3)
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
Node& Cell::node(int i) const
{
  dolfin_assert(c);
  return c->node(i);
}
//-----------------------------------------------------------------------------
Edge& Cell::edge(int i) const
{
  dolfin_assert(c);
  return c->edge(i);
}
//-----------------------------------------------------------------------------
Face& Cell::face(int i) const
{
  dolfin_assert(c);
  return c->face(i);
}
//-----------------------------------------------------------------------------
Cell& Cell::neighbor(int i) const
{
  dolfin_assert(c);
  return c->neighbor(i);
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
Point& Cell::coord(int i) const
{ 
  dolfin_assert(c);
  return c->coord(i);
}
//-----------------------------------------------------------------------------
Point Cell::midpoint() const
{
  dolfin_assert(c);
  return c->midpoint();
}
//-----------------------------------------------------------------------------
int Cell::nodeID(int i) const
{
  if ( c )
    return c->nodeID(i);
  
  return -1;
}
//-----------------------------------------------------------------------------
real Cell::volume() const
{
  dolfin_assert(c);
  return c->volume();
}
//-----------------------------------------------------------------------------
real Cell::diameter() const
{
  dolfin_assert(c);
  return c->diameter();
}
//-----------------------------------------------------------------------------
bool Cell::operator==(const Cell& cell) const
{
  return this == &cell;
}
//-----------------------------------------------------------------------------
bool Cell::operator!=(const Cell& cell) const
{
  return this != &cell;
}
//-----------------------------------------------------------------------------
void Cell::mark()
{
  dolfin_assert(c);
  c->mark(*this);
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
int Cell::setID(int id, Grid& grid)
{
  dolfin_assert(c);
  return c->setID(id, grid);
}
//-----------------------------------------------------------------------------
void Cell::setGrid(Grid& grid)
{
  dolfin_assert(c);
  c->setGrid(grid);
}
//-----------------------------------------------------------------------------
void Cell::setParent(Cell& parent)
{
  // Set parent cell: a cell is parent if the current cell is created through 
  // refinement of the parent cell. 
  dolfin_assert(c);
  return c->setParent(parent);
}
//-----------------------------------------------------------------------------
void Cell::addChild(Cell& child)
{
  // Set the child if cell not already contains the child cell: a cell 
  // is child if it is created through refinement of the current cell.  
  dolfin_assert(c);
  return c->addChild(child);
}
//-----------------------------------------------------------------------------
void Cell::set(Node& n0, Node& n1, Node& n2)
{
  if ( c )
    delete c;
  
  c = new Triangle(n0, n1, n2);
}
//-----------------------------------------------------------------------------
void Cell::set(Node& n0, Node& n1, Node& n2, Node& n3)
{
  if ( c )
    delete c;

  c = new Tetrahedron(n0, n1, n2, n3);
}
//-----------------------------------------------------------------------------
bool Cell::neighbor(Cell& cell) const
{
  dolfin_assert(c);
  return c->neighbor(*cell.c);
}
//-----------------------------------------------------------------------------
bool Cell::haveNode(Node& node) const
{
  dolfin_assert(c);
  return c->haveNode(node);
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
void Cell::createEdge(Node& n0, Node& n1)
{
  dolfin_assert(c);
  c->createEdge(n0, n1);
}
//-----------------------------------------------------------------------------
void Cell::createFace(Edge& e0, Edge& e1, Edge& e2)
{
  dolfin_assert(c);
  c->createFace(e0, e1, e2);
}
//-----------------------------------------------------------------------------
Node* Cell::findNode(const Point& p) const
{
  dolfin_assert(c);
  return c->findNode(p);
}
//-----------------------------------------------------------------------------
Edge* Cell::findEdge(Node& n0, Node& n1)
{
  dolfin_assert(c);
  return c->findEdge(n0, n1);
}
//-----------------------------------------------------------------------------
Face* Cell::findFace(Edge& e0, Edge& e1, Edge& e2)
{
  dolfin_assert(c);
  return c->findFace(e0, e1, e2);
}
//-----------------------------------------------------------------------------
Face* Cell::findFace(Edge& e0, Edge& e1)
{
  dolfin_assert(c);
  return c->findFace(e0, e1);
}
//-----------------------------------------------------------------------------
Cell::Marker& Cell::marker()
{
  dolfin_assert(c);
  return c->marker();
}
//-----------------------------------------------------------------------------
Cell::Status& Cell::status()
{
  dolfin_assert(c);
  return c->status();
}
//-----------------------------------------------------------------------------
