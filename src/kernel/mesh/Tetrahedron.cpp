// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#include <cmath>

#include <dolfin/dolfin_log.h>
#include <dolfin/Array.h>
#include <dolfin/Node.h>
#include <dolfin/Edge.h>
#include <dolfin/Face.h>
#include <dolfin/Point.h>
#include <dolfin/Cell.h>
#include <dolfin/Tetrahedron.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Tetrahedron::Tetrahedron(Node& n0, Node& n1, Node& n2, Node& n3) : GenericCell()
{
  cn.init(noNodes());

  cn(0) = &n0;
  cn(1) = &n1;
  cn(2) = &n2;
  cn(3) = &n3;
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
Cell::Orientation Tetrahedron::orientation() const
{
  Point v01 = cn(1)->coord() - cn(0)->coord();
  Point v02 = cn(2)->coord() - cn(0)->coord();
  Point v03 = cn(3)->coord() - cn(0)->coord();
  Point n = v01.cross(v02);

  return ( n * v03 < 0.0 ? Cell::left : Cell::right );
}
//-----------------------------------------------------------------------------
real Tetrahedron::volume() const
{
  // Get the coordinates
  real x1 = coord(0).x; real y1 = coord(0).y; real z1 = coord(0).z;
  real x2 = coord(1).x; real y2 = coord(1).y; real z2 = coord(1).z;
  real x3 = coord(2).x; real y3 = coord(2).y; real z3 = coord(2).z;
  real x4 = coord(3).x; real y4 = coord(3).y; real z4 = coord(3).z;

  // Formula for volume from http://mathworld.wolfram.com
  real v = ( x1 * ( y2*z3 + y4*z2 + y3*z4 - y3*z2 - y2*z4 - y4*z3 ) -
	     x2 * ( y1*z3 + y4*z1 + y3*z4 - y3*z1 - y1*z4 - y4*z3 ) +
	     x3 * ( y1*z2 + y4*z1 + y2*z4 - y2*z1 - y1*z4 - y4*z2 ) -
	     x4 * ( y1*z2 + y2*z3 + y3*z1 - y2*z1 - y3*z2 - y1*z3 ) );

  return fabs(v);
}
//-----------------------------------------------------------------------------
real Tetrahedron::diameter() const
{
  // Compute side lengths
  real a  = coord(1).dist(coord(2));
  real b  = coord(0).dist(coord(2));
  real c  = coord(0).dist(coord(1));
  real aa = coord(0).dist(coord(3));
  real bb = coord(1).dist(coord(3));
  real cc = coord(2).dist(coord(3));
                                                                                                                          
  // Compute "area" of triangle with strange side lengths
  real l1   = a*aa;
  real l2   = b*bb;
  real l3   = c*cc;
  real s    = 0.5*(l1+l2+l3);
  real area = sqrt(s*(s-l1)*(s-l2)*(s-l3));
                                                                                                                          
  // Formula for diameter (2*circumradius) from http://mathworld.wolfram.com
  real d = area / ( 3.0*volume() );
                                                                                                                          
  return d;
}
//-----------------------------------------------------------------------------
bool Tetrahedron::edgeAligned(uint i) const
{
  dolfin_error("Not implemented.");
  return true;
}
//-----------------------------------------------------------------------------
bool Tetrahedron::faceAligned(uint i) const
{
  dolfin_error("Not implemented.");
  return true;
}
//-----------------------------------------------------------------------------
void Tetrahedron::createEdges()
{
  ce.init(6);
  ce.reset();

  createEdge(*cn(0), *cn(1));
  createEdge(*cn(1), *cn(2));
  createEdge(*cn(2), *cn(0));
  createEdge(*cn(0), *cn(3));
  createEdge(*cn(1), *cn(3));
  createEdge(*cn(2), *cn(3));
}
//-----------------------------------------------------------------------------
void Tetrahedron::createFaces()
{
  cf.init(4);
  cf.reset();

  createFace(*ce(0), *ce(1), *ce(2));
  createFace(*ce(0), *ce(4), *ce(3));
  createFace(*ce(1), *ce(5), *ce(4));
  createFace(*ce(2), *ce(5), *ce(3));
}
//-----------------------------------------------------------------------------
void Tetrahedron::sort()
{
  // Sort local mesh entities according to ordering used by FIAT,
  // see Mesh.h for a detailed description of the ordering used.

  // Soft the nodes to be right-oriented
  if ( orientation() == Cell::left )
  {
    Node* tmp = cn(2);
    cn(2) = cn(3);
    cn(3) = tmp;
  }
 
  // Sort the edges according to the ordering used by FIAT
  Array<Edge*> edges(6);
  edges[0] = findEdge(1, 2);
  edges[1] = findEdge(2, 3);
  edges[2] = findEdge(3, 1);
  edges[3] = findEdge(3, 0);
  edges[4] = findEdge(0, 2);
  edges[5] = findEdge(0, 1);
  for (uint i = 0; i < 6; i++)
    ce(i) = edges[i];

  // Sort the faces according to the ordering used by FIAT
  Array<Face*> faces(4);
  faces[0] = findFace(0);
  faces[1] = findFace(1);
  faces[2] = findFace(2);
  faces[3] = findFace(3);
  for (uint i = 0; i < 4; i++)
    cf(i) = faces[i];
}
//-----------------------------------------------------------------------------
Edge* Tetrahedron::findEdge(uint n0, uint n1) const
{
  // Find the edge containing both nodes n0 and n1
  const Node* node0 = cn(n0);
  const Node* node1 = cn(n1);
  for (uint i = 0; i < 6; i++)
  {
    Edge* edge = ce(i);
    if ( (edge->n0 == node0 && edge->n1 == node1) ||
	 (edge->n0 == node1 && edge->n1 == node0) )
      return edge;
  }

  dolfin_error2("Unable to find edge between nodes %d and %d.\n", n0, n1);
  return 0;
}
//-----------------------------------------------------------------------------
Face* Tetrahedron::findFace(uint n) const
{
  // Find the face not containing node n
  Node* node = cn(n);
  for (uint i = 0; i < 4; i++)
  {
    Face* face = cf(i);
    if ( !(face->contains(*node)) )
      return face;
  }

  dolfin_error1("Unable to find face opposite to node %d.", n);
  return 0;
}
//-----------------------------------------------------------------------------
