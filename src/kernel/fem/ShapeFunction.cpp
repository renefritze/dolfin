#include <iostream>

#include <dolfin/FunctionList.h>
#include <dolfin/Product.h>
#include <dolfin/ElementFunction.h>
#include <dolfin/ShapeFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
FunctionSpace::ShapeFunction::ShapeFunction()
{
  _id = 0; // Initialise to the zero function
}
//-----------------------------------------------------------------------------
FunctionSpace::ShapeFunction::ShapeFunction(int i)
{
  _id = 1; // Initialise to one
}
//-----------------------------------------------------------------------------
FunctionSpace::ShapeFunction::ShapeFunction(function f)
{
  _id = FunctionList::add(f);
}
//-----------------------------------------------------------------------------
FunctionSpace::ShapeFunction::ShapeFunction(const ShapeFunction &v)
{
  this->_id = v._id;
}
//-----------------------------------------------------------------------------
void FunctionSpace::ShapeFunction::set(ElementFunction dx,
													ElementFunction dy,
													ElementFunction dz,
													ElementFunction dt)
{
  FunctionList::set(_id, dx, dy, dz, dt);
}
//-----------------------------------------------------------------------------
int FunctionSpace::ShapeFunction::id() const
{
  return _id;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction FunctionSpace::ShapeFunction::dx() const
{
  return FunctionList::dx(_id);
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction FunctionSpace::ShapeFunction::dy() const
{
  return FunctionList::dy(_id);
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction FunctionSpace::ShapeFunction::dz() const
{
  return FunctionList::dz(_id);
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction FunctionSpace::ShapeFunction::dt() const
{
  return FunctionList::dt(_id);
}
//-----------------------------------------------------------------------------
real
FunctionSpace::ShapeFunction::operator() (real x, real y, real z, real t) const
{
  return FunctionList::eval(_id, x, y, z, t);
}
//-----------------------------------------------------------------------------
real FunctionSpace::ShapeFunction::operator() (Point p) const
{
  // Warning: value of t is ignored
  return FunctionList::eval(_id, p.x, p.y, p.z, 0.0);
}
//-----------------------------------------------------------------------------
FunctionSpace::ShapeFunction&
FunctionSpace::ShapeFunction::operator= (const ShapeFunction &v)
{
  this->_id = v._id;

  return *this;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator+ (const ShapeFunction &v) const
{
  ElementFunction w(1.0, *this, 1.0, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator+ (const FunctionSpace::Product &v) const
{
  ElementFunction w(1.0, *this, 1.0, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator+ (const ElementFunction &v) const
{
  ElementFunction w(1.0, *this, 1.0, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator- (const ShapeFunction &v) const
{
  ElementFunction w(1.0, *this, -1.0, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator- (const Product &v) const
{
  ElementFunction w(1.0, *this, -1.0, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator- (const ElementFunction &v) const
{
  ElementFunction w(1.0, *this, -1.0, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::Product
FunctionSpace::ShapeFunction::operator* (const ShapeFunction &v) const
{
  Product w(*this, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::Product
FunctionSpace::ShapeFunction::operator* (const Product &v) const
{
  Product w(*this, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator* (const ElementFunction &v) const
{
  ElementFunction w(*this, v);
  return w;
}
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction
FunctionSpace::ShapeFunction::operator* (real a) const
{
  ElementFunction w(a, *this);
  return w;
}
//-----------------------------------------------------------------------------
real FunctionSpace::ShapeFunction::operator* (Integral::Measure &dm) const
{
  return dm * (*this);
}
//-----------------------------------------------------------------------------
void FunctionSpace::ShapeFunction::operator= (int zero)
{
  // FIXME: Use logging system
  if ( zero != 0 ) {
	 cout << "Assignment to int must be zero." << endl;
	 exit(1);
  }
  
  _id = -1;
}
//-----------------------------------------------------------------------------
bool FunctionSpace::ShapeFunction::operator! () const
{
  return _id == -1;
}
//-----------------------------------------------------------------------------
// Additional operators
//-----------------------------------------------------------------------------
FunctionSpace::ElementFunction dolfin::operator*
(real a, const FunctionSpace::ShapeFunction &v)
{
  return v * a;
}
//-----------------------------------------------------------------------------
ostream& dolfin::operator << (ostream& output,
										const FunctionSpace::ShapeFunction &v)
{
  output << "[ ShapeFunction with id = " << v._id << " ]";
  
  return output;
}
//-----------------------------------------------------------------------------
