// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_log.h>
#include <dolfin/Node.h>
#include <dolfin/Point.h>
#include <dolfin/Cell.h>
#include <dolfin/Tetrahedron.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Tetrahedron::Tetrahedron(Node* n0, Node* n1, Node* n2, Node* n3) : GenericCell()
{
  cn.init(noNodes());

  cn(0) = n0;
  cn(1) = n1;
  cn(2) = n2;
  cn(3) = n3;
}
//-----------------------------------------------------------------------------
int Tetrahedron::noNodes() const
{
  return 4;
}
//-----------------------------------------------------------------------------
int Tetrahedron::noEdges() const
{
  return 6;
}
//-----------------------------------------------------------------------------
int Tetrahedron::noFaces() const
{
  return 4;
}
//-----------------------------------------------------------------------------
int Tetrahedron::noBoundaries() const
{
  return noFaces();
}
//-----------------------------------------------------------------------------
Cell::Type Tetrahedron::type() const
{
  return Cell::tetrahedron;
}
//-----------------------------------------------------------------------------
bool Tetrahedron::neighbor(GenericCell* cell) const
{
  // Two tetrahedrons are neighbors if they have a common face or if they are
  // the same tetrahedron, i.e. if they have 3 or 4 common nodes.
  
  if ( !cell )
    return false;

  if ( cell->type() != Cell::tetrahedron )
    return false;
  
  int count = 0;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      if ( cn(i) == cell->cn(j) )
	count++;
  
  return count == 3 || count == 4;
}
//-----------------------------------------------------------------------------
void Tetrahedron::createEdges()
{
  ce.init(6);
  ce.reset();

  createEdge(cn(0), cn(1));
  createEdge(cn(1), cn(2));
  createEdge(cn(2), cn(0));
  createEdge(cn(0), cn(3));
  createEdge(cn(1), cn(3));
  createEdge(cn(2), cn(3));
}
//-----------------------------------------------------------------------------
void Tetrahedron::createFaces()
{
  cf.init(4);
  cf.reset();

  createFace(ce(0), ce(1), ce(2));
  createFace(ce(0), ce(4), ce(3));
  createFace(ce(1), ce(5), ce(4));
  createFace(ce(2), ce(5), ce(3));
}
//-----------------------------------------------------------------------------
