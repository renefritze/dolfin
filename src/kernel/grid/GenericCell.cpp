// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Point.h>
#include <dolfin/Grid.h>
#include <dolfin/Cell.h>
#include <dolfin/GenericCell.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
GenericCell::GenericCell()
{
  grid = 0;
  _id = -1;
}
//-----------------------------------------------------------------------------
GenericCell::~GenericCell()
{

}
//-----------------------------------------------------------------------------
int GenericCell::id() const
{
  return _id;
}
//-----------------------------------------------------------------------------
int GenericCell::noCellNeighbors() const
{
  return cc.size();
}
//-----------------------------------------------------------------------------
int GenericCell::noNodeNeighbors() const
{
  return cn.size();
}
//-----------------------------------------------------------------------------
Node* GenericCell::node(int i) const
{
  return cn(i);
}
//-----------------------------------------------------------------------------
Edge* GenericCell::edge(int i) const
{
  return ce(i);
}
//-----------------------------------------------------------------------------
Cell* GenericCell::neighbor(int i) const
{
  return cc(i);
}
//-----------------------------------------------------------------------------
Point GenericCell::coord(int i) const
{
  return cn(i)->coord();
}
//-----------------------------------------------------------------------------
Point GenericCell::midpoint() const
{
  Point p;

  for (Array<Node*>::Iterator n(cn); !n.end(); ++n)
    p += (*n)->coord();

  p /= real(cn.size());

  return p;
}
//-----------------------------------------------------------------------------
int GenericCell::nodeID(int i) const
{
  return cn(i)->id();
}
//-----------------------------------------------------------------------------
void GenericCell::mark(Cell* cell)
{
  if ( !grid )
    dolfin_error("You cannot mark a cell that does not belong to grid.");
 
  grid->mark(cell);
}
//-----------------------------------------------------------------------------
int GenericCell::setID(int id, Grid* grid)
{
  this->grid = grid;
  return _id = id;
}
//-----------------------------------------------------------------------------
void GenericCell::createEdge(Node* n0, Node* n1)
{
  Edge* edge = 0;

  // Check neighbor cells if an edge already exists between the two nodes
  for (Array<Cell*>::Iterator c(cc); !c.end(); ++c) {
    edge = (*c)->findEdge(n0, n1);
    if ( edge )
      break;
  }

  // Create the new edge if it doesn't exist
  if ( !edge )
    edge = grid->createEdge(n0, n1);

  // Add the edge at the first empty position
  ce.add(edge);
}
//-----------------------------------------------------------------------------
void GenericCell::createFace(Edge* e0, Edge* e1, Edge* e2)
{
  Face* face = 0;
  
  // Check neighbor cells if the face already exists
  for (Array<Cell*>::Iterator c(cc); !c.end(); ++c) {
    face = (*c)->findFace(e0, e1, e2);
    if ( face )
      break;
  }

  // Create the new face if it doesn't exist
  if ( !face )
    face = grid->createFace(e0, e1, e2);

  // Add the face at the first empty position
  cf.add(face);
}
//-----------------------------------------------------------------------------
Edge* GenericCell::findEdge(Node* n0, Node* n1)
{
  for (Array<Edge*>::Iterator e(ce); !e.end(); ++e)
    if ( *e )
      if ( (*e)->equals(n0, n1) )
	return *e;

  return 0;
}
//-----------------------------------------------------------------------------
Face* GenericCell::findFace(Edge* e0, Edge* e1, Edge* e2)
{
  for (Array<Face*>::Iterator f(cf); !f.end(); ++f)
    if ( *f )
      if ( (*f)->equals(e0, e1, e2) )
	return *f;

  return 0;
}
//-----------------------------------------------------------------------------
