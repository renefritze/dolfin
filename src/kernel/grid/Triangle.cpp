// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Node.h>
#include <dolfin/Point.h>
#include <dolfin/Cell.h>
#include <dolfin/Triangle.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Triangle::Triangle(Node* n0, Node* n1, Node* n2) : GenericCell()
{
  cn.init(noNodes());

  cn(0) = n0;
  cn(1) = n1;
  cn(2) = n2;
}
//-----------------------------------------------------------------------------
int Triangle::noNodes() const
{
  return 3;
}
//-----------------------------------------------------------------------------
int Triangle::noEdges() const
{
  return 3;
}
//-----------------------------------------------------------------------------
int Triangle::noFaces() const
{
  return 1;
}
//-----------------------------------------------------------------------------
int Triangle::noBoundaries() const
{
  return noEdges();
}
//-----------------------------------------------------------------------------
Cell::Type Triangle::type() const
{
  return Cell::triangle;
}
//-----------------------------------------------------------------------------
bool Triangle::neighbor(GenericCell* cell) const
{
  // Two triangles are neighbors if they have a common edge or if they are
  // the same triangle, i.e. if they have 2 or 3 common nodes.

  if ( !cell )
    return false;

  if ( cell->type() != Cell::triangle )
    return false;
  
  int count = 0;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      if ( cn(i) == cell->cn(j) )
	count++;
  
  return count == 2 || count == 3;
}
//-----------------------------------------------------------------------------
void Triangle::createEdges()
{
  ce.init(3);
  ce.reset();

  createEdge(cn(0), cn(1));
  createEdge(cn(1), cn(2));
  createEdge(cn(2), cn(0));
}
//-----------------------------------------------------------------------------
void Triangle::createFaces()
{
  // A triangle has no faces
}
//-----------------------------------------------------------------------------
