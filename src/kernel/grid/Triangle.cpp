// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Triangle.h>
#include <dolfin/Node.h>
#include <dolfin/Grid.h>
#include <utils.h>
#include <dolfin/Display.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Triangle::Triangle()
{

}
//-----------------------------------------------------------------------------
Triangle::~Triangle()
{

}
//-----------------------------------------------------------------------------
int Triangle::noNodes()
{
  return 3;
}
//-----------------------------------------------------------------------------
int Triangle::noEdges()
{
  return 3;
}
//-----------------------------------------------------------------------------
int Triangle::noFaces()
{
  return 1;
}
//-----------------------------------------------------------------------------
int Triangle::noBoundaries()
{
  return noEdges();
}
//-----------------------------------------------------------------------------
Cell::Type Triangle::type()
{
  return Cell::TRIANGLE;
}
//-----------------------------------------------------------------------------
void Triangle::set(Node *n0, Node *n1, Node *n2)
{

}
//-----------------------------------------------------------------------------
void Triangle::Set(Node *n1, Node *n2, Node *n3, int material)
{

}
//----------------------------------------------------------------------------
int Triangle::GetSize()
{
  return 3;
}
//-----------------------------------------------------------------------------
Node* Triangle::GetNode(int node)
{
  if ( (node<0) || (node>=3) ){
    int *tst;
    tst[2638231] = 0;
	 display->InternalError("Triangle::GetNode()","Illegal node: %d",node);
  }
  
  return nodes[node];
}
//-----------------------------------------------------------------------------
real Triangle::ComputeVolume(Grid *grid)
{
  // The volume of a triangle is the same as its area
  
  // Get the coordinates
  Point *A = nodes[0]->GetCoord();
  Point *B = nodes[1]->GetCoord();
  Point *C = nodes[2]->GetCoord();

  // Make sure we get full precision
  real x1, x2, x3, y1, y2, y3, z1, z2, z3;

  x1 = real(A->x); y1 = real(A->y); z1 = real(A->z);
  x2 = real(B->x); y2 = real(B->y); z2 = real(B->z);
  x3 = real(C->x); y3 = real(C->y); z3 = real(C->z);

  // Formula for volume from http://mathworld.wolfram.com
  real v1 = (y1*z2 + z1*y3 + y2*z3) - ( y3*z2 + z3*y1 + y2*z1 );
  real v2 = (z1*x2 + x1*z3 + z2*x3) - ( z3*x2 + x3*z1 + z2*x1 );
  real v3 = (x1*y2 + y1*x3 + x2*y3) - ( x3*y2 + y3*x1 + x2*y1 );
  
  real v = 0.5 * sqrt( v1*v1 + v2*v2 + v3*v3 );

  return ( v );
}
//-----------------------------------------------------------------------------
real Triangle::ComputeCircumRadius(Grid *grid)
{
  // Compute volume
  real volume = ComputeVolume(grid);

  // Return radius
  return ( ComputeCircumRadius(grid,volume) );
}
//-----------------------------------------------------------------------------
real Triangle::ComputeCircumRadius(Grid *grid, real volume)
{
  // Get the coordinates
  Point *A = nodes[0]->GetCoord();
  Point *B = nodes[1]->GetCoord();
  Point *C = nodes[2]->GetCoord();
  
  // Compute side lengths
  real a  = B->Distance(*C);
  real b  = A->Distance(*C);
  real c  = A->Distance(*B);

  // Formula for radius from http://mathworld.wolfram.com
  real R = 0.25*a*b*c/volume;

  return ( R );
}
//-----------------------------------------------------------------------------
void Triangle::CountCell(Node *node_list)
{
  node_list[nodes[0]->GetNodeNo()]._nc += 1;
  node_list[nodes[1]->GetNodeNo()]._nc += 1;
  node_list[nodes[2]->GetNodeNo()]._nc += 1;
}
//-----------------------------------------------------------------------------
void Triangle::AddCell(Node *node_list, int *current, int thiscell)
{
  int pos, n;
  n=nodes[0]->GetNodeNo(); pos=current[n]; node_list[n].neighbor_cells[pos]=thiscell; current[n]+=1;
  n=nodes[1]->GetNodeNo(); pos=current[n]; node_list[n].neighbor_cells[pos]=thiscell; current[n]+=1;
  n=nodes[2]->GetNodeNo(); pos=current[n]; node_list[n].neighbor_cells[pos]=thiscell; current[n]+=1;
}
//-----------------------------------------------------------------------------
void Triangle::AddNodes(int exclude_node, int *new_nodes, int *pos)
{
  int n;

  if ( (n = nodes[0]->GetNodeNo()) != exclude_node )
	 if ( !contains(new_nodes,*pos,n) ) new_nodes[(*pos)++] = n;
  if ( (n = nodes[1]->GetNodeNo()) != exclude_node )
	 if ( !contains(new_nodes,*pos,n) )	new_nodes[(*pos)++] = n;
  if ( (n = nodes[2]->GetNodeNo()) != exclude_node )
	 if ( !contains(new_nodes,*pos,n) )	new_nodes[(*pos)++] = n;
}
//-----------------------------------------------------------------------------
void Triangle::ComputeCellNeighbors(Node *node_list, int thiscell)
{
  // Although triangles on the boundary have only 2 neighbors, allocate
  // for 3 neighbors (data will be moved to another location anyway).
  if ( !neighbor_cells )
	 neighbor_cells = new int[3];

  int c;
  
  if ( node_list[nodes[0]->GetNodeNo()].CommonCell(&node_list[nodes[1]->GetNodeNo()],thiscell,&c) ){
	 neighbor_cells[_nc] = c;
	 _nc += 1;
  }
  if ( node_list[nodes[0]->GetNodeNo()].CommonCell(&node_list[nodes[2]->GetNodeNo()],thiscell,&c) ){
	 neighbor_cells[_nc] = c;
	 _nc += 1;
  }
  if ( node_list[nodes[1]->GetNodeNo()].CommonCell(&node_list[nodes[2]->GetNodeNo()],thiscell,&c) ){
	 neighbor_cells[_nc] = c;
	 _nc += 1;
  }

}
//-----------------------------------------------------------------------------
Node* Triangle::getNode(int i)
{
  return nodes[i];
}
//-----------------------------------------------------------------------------
bool Triangle::neighbor(ShortList<Node *> &cn, Cell &cell)
{
  // Two triangles are neighbors if they have a common edge or if they are
  // the same triangle, i.e. if they have 2 or 3 common nodes.

  if ( cell.type() != Cell::TRIANGLE )
	 return false;
  
  if ( !cell.c )
	 return false;

  int count = 0;
  for (int i = 0; i < 3; i++)
	 for (int j = 0; j < 3; j++)
		if ( cn(i) == cell.cn(j) )
		  count++;
  
  return count == 2 || count == 3;
}
//-----------------------------------------------------------------------------
namespace dolfin {

  //---------------------------------------------------------------------------
  std::ostream& operator << (std::ostream& output, const Triangle& t)
  {
	 output << "[ Triangle: id = " << t.id() << " " << "  nodes = ("
			  << t.nodes[0]->id() << ","
			  << t.nodes[1]->id() << ","
			  << t.nodes[2]->id() << ") ]";

	 return output;
  }
  //---------------------------------------------------------------------------

}
//-----------------------------------------------------------------------------
