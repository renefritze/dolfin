// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/Node.h>
#include <dolfin/GenericCell.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Node::Node()
{
  clear();
}
//-----------------------------------------------------------------------------
Node::Node(real x)
{
  clear();
  p.x = x;
}
//-----------------------------------------------------------------------------
Node::Node(real x, real y)
{
  clear();
  p.x = x;
  p.y = y;
}
//-----------------------------------------------------------------------------
Node::Node(real x, real y, real z)
{
  clear();
  p.x = x;
  p.y = y;
  p.z = z;
}
//-----------------------------------------------------------------------------
Node::~Node()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void Node::clear()
{
  grid = 0;
  _id = -1;

  p.x = 0.0;
  p.y = 0.0;
  p.z = 0.0;

  nn.clear();
  nc.clear();
  ne.clear();

  _parent = 0;
  _child = 0;
}
//-----------------------------------------------------------------------------
int Node::id() const
{
  return _id;
}
//-----------------------------------------------------------------------------
int Node::noNodeNeighbors() const
{
  return nn.size();
}
//-----------------------------------------------------------------------------
int Node::noCellNeighbors() const
{
  return nc.size();
}
//-----------------------------------------------------------------------------
int Node::noEdgeNeighbors() const
{
  return ne.size();
}
//-----------------------------------------------------------------------------
Node& Node::node(int i) const
{
  return *nn(i);
}
//-----------------------------------------------------------------------------
Cell& Node::cell(int i) const
{
  return *nc(i);
}
//-----------------------------------------------------------------------------
Edge& Node::edge(int i) const
{
  return *ne(i);
}
//-----------------------------------------------------------------------------
Node* Node::parent() const 
{
  return _parent;
}
//-----------------------------------------------------------------------------
Node* Node::child() const
{
  return _child;
}
//-----------------------------------------------------------------------------
Point& Node::coord()
{
  return p;
}
//-----------------------------------------------------------------------------
Point Node::coord() const
{
  return p;
}
//-----------------------------------------------------------------------------
Point Node::midpoint(const Node& n) const
{
  return p.midpoint(n.p);
}
//-----------------------------------------------------------------------------
real Node::dist(const Node& n) const
{
  return p.dist(n.p);
}
//-----------------------------------------------------------------------------
real Node::dist(const Point& p) const
{
  return this->p.dist(p);
}
//-----------------------------------------------------------------------------
real Node::dist(real x, real y, real z) const
{
  return p.dist(x, y, z);
}
//-----------------------------------------------------------------------------
bool Node::neighbor(const Node& n) const
{
  for (NodeIterator neighbor(*this); !neighbor.end(); ++neighbor)
    if ( &n == neighbor )
      return true;
  
  return false;
}
//-----------------------------------------------------------------------------
bool Node::operator==(const Node& node) const
{
  return this == &node;
}
//-----------------------------------------------------------------------------
bool Node::operator!=(const Node& node) const
{
  return this != &node;
}
//-----------------------------------------------------------------------------
bool Node::operator== (int id) const
{
  return _id == id;
}
//-----------------------------------------------------------------------------
bool Node::operator< (int id) const
{
  return _id < id;
}
//-----------------------------------------------------------------------------
bool Node::operator<= (int id) const
{
  return _id <= id;
}
//-----------------------------------------------------------------------------
bool Node::operator> (int id) const
{
  return _id > id;
}
//-----------------------------------------------------------------------------
bool Node::operator>= (int id) const
{
  return _id >= id;
}
//-----------------------------------------------------------------------------
bool dolfin::operator== (int id, const Node& node)
{
  return node == id;
}
//-----------------------------------------------------------------------------
bool dolfin::operator< (int id, const Node& node)
{
  return node > id;
}
//-----------------------------------------------------------------------------
bool dolfin::operator<= (int id, const Node& node)
{
  return node >= id;
}
//-----------------------------------------------------------------------------
bool dolfin::operator> (int id, const Node& node)
{
  return node < id;
}
//-----------------------------------------------------------------------------
bool dolfin::operator>= (int id, const Node& node)
{
  return node <= id;
}
//-----------------------------------------------------------------------------
dolfin::LogStream& dolfin::operator<<(LogStream& stream, const Node& node)
{
  int id = node.id();
  const Point p = node.coord();
  
  stream << "[ Node: id = " << id
	 << " x = (" << p.x << "," << p.y << "," << p.z << ") ]";
  
  return stream;
}
//-----------------------------------------------------------------------------
int Node::setID(int id, Grid& grid)
{
  this->grid = &grid;
  return _id = id;
}
//-----------------------------------------------------------------------------
void Node::setGrid(Grid& grid)
{
  this->grid = &grid;
}
//-----------------------------------------------------------------------------
void Node::setParent(Node& parent)
{
  // Set parent node: a node is parent to if it has the same coordinates 
  // and is contained in the next coarser grid 
  this->_parent = &parent;
}
//-----------------------------------------------------------------------------
void Node::setChild(Node& child)
{
  // Set child node: a node is child to if it has the same coordinates 
  // and is contained in the next finer grid 
  this->_child = &child;
}
//-----------------------------------------------------------------------------
void Node::removeParent(Node& parent)
{
  // Remove parent
  this->_parent = 0;
}
//-----------------------------------------------------------------------------
void Node::removeChild()
{
  // Remove child 
  this->_child = 0;
}
//-----------------------------------------------------------------------------
void Node::set(real x, real y, real z)
{
  p.x = x;
  p.y = y;
  p.z = z;
}
//-----------------------------------------------------------------------------
