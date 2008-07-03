// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells 2005-2007.
// Modified by Martin Sandve Alnes 2008.
//
// First added:  2003-11-28
// Last changed: 2008-07-03
//
// The class Function serves as the envelope class and holds a pointer
// to a letter class that is a subclass of GenericFunction. All the
// functionality is handled by the specific implementation (subclass).

#include <dolfin/io/File.h>
#include "UserFunction.h"
#include "ConstantFunction.h"
#include "DiscreteFunction.h"
#include "UFCFunction.h"
#include "Function.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Function::Function()
  : Variable("u", "empty function"),
    f(0), _type(empty), _cell(0), _facet(-1)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh)
  : Variable("u", "user-defined function"),
    f(0), _type(user), _cell(0), _facet(-1)
{
  f = new UserFunction(mesh, this);
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, real value)
  : Variable("u", "constant function"),
    f(0), _type(constant), _cell(0), _facet(-1)
{
  f = new ConstantFunction(mesh, value);
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, uint size, real value)
  : Variable("u", "constant function"),
    f(0), _type(constant), _cell(0), _facet(-1)
{
  f = new ConstantFunction(mesh, size, value);
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, uint size, const real* values)
  : Variable("u", "constant function"),
    f(0), _type(constant), _cell(0), _facet(-1)
{
  f = new ConstantFunction(mesh, size, values);
}
//-----------------------------------------------------------------------------
//Function::Function(Mesh& mesh, const simple_array<real>& values)
//  : Variable("u", "constant function"),
//    f(0), _type(constant), _cell(0), _facet(-1)
//{
//  f = new ConstantFunction(mesh, values.size, values.data);
//}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, uint rank, const uint* shape, const real* values)
  : Variable("u", "constant function"),
    f(0), _type(constant), _cell(0), _facet(-1)
{
  f = new ConstantFunction(mesh, rank, shape, values);
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, const ufc::function& function, uint size)
  : Variable("u", "ufc function"),
    f(0), _type(ufc), _cell(0), _facet(-1)
{
  f = new UFCFunction(mesh, function, size);
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, GenericVector& x, Form& form, uint i)
  : Variable("u", "discrete function"),
    f(0), _type(discrete), _cell(0), _facet(-1)
{
  f = new DiscreteFunction(mesh, x, form, i);
}
//-----------------------------------------------------------------------------
Function::Function(Mesh& mesh, GenericVector& x, DofMap& dof_map, const ufc::form& form, uint i)
  : Variable("u", "discrete function"),
    f(0), _type(discrete), _cell(0), _facet(-1)
{
  f = new DiscreteFunction(mesh, x, dof_map, form, i);
}
//-----------------------------------------------------------------------------
Function::Function(const std::string filename)
  : Variable("u", "discrete function from data file"),
    f(0), _type(empty), _cell(0), _facet(-1)
{
  File file(filename);
  file >> *this;
}
//-----------------------------------------------------------------------------
Function::Function(SubFunction f)
  : Variable("u", "discrete function"),
    f(0), _type(discrete), _cell(0), _facet(-1)
{
  cout << "Extracting sub function." << endl;
  this->f = new DiscreteFunction(f);
}
//-----------------------------------------------------------------------------
Function::Function(const Function& f) : f(0), _type(f.type()), _cell(0), _facet(-1)
{
  if( f.type() == discrete ) 
  {
    this->f = new DiscreteFunction(*static_cast<DiscreteFunction*>(f.f));
    rename("x", "discrete function");
  }
  else if( f.type() == constant )
  {
    this->f = new ConstantFunction(*static_cast<ConstantFunction*>(f.f));
    rename("x", "constant function");
  }
  else
    error("Copy constructor works for discrete and constant functions only (so far).");
}
//-----------------------------------------------------------------------------
Function::~Function()
{
  if (f)
    delete f;
}
//-----------------------------------------------------------------------------
void Function::init(Mesh& mesh, GenericVector& x, Form& form, uint i)
{
  if (f)
    delete f;

  f = new DiscreteFunction(mesh, x, form, i);
  
  rename("u", "discrete function");
  _type = discrete;
}
//-----------------------------------------------------------------------------
void Function::init(Mesh& mesh, GenericVector& x, DofMap& dof_map, const ufc::form& form, uint i)
{
  if (f)
    delete f;

  f = new DiscreteFunction(mesh, x, dof_map, form, i);
  
  rename("u", "discrete function");
  _type = discrete;
}
//-----------------------------------------------------------------------------
Function::Type Function::type() const
{
  return _type;
}
//-----------------------------------------------------------------------------
dolfin::uint Function::rank() const
{
  if (!f)
    error("Function contains no data.");

  return f->rank();
}
//-----------------------------------------------------------------------------
dolfin::uint Function::dim(unsigned int i) const
{
  if (!f)
    error("Function contains no data.");

  return f->dim(i);
}
//-----------------------------------------------------------------------------
Mesh& Function::mesh() const
{
  if (!f)
    error("Function contains no data.");

  return f->mesh;
}
//-----------------------------------------------------------------------------
GenericVector& Function::vector() const
{
  if (!f)
    error("Function contains no data.");

  if (_type != discrete)
    error("A vector can only be extracted from discrete functions.");

  return (static_cast<DiscreteFunction*>(f))->vector();
}
//-----------------------------------------------------------------------------
dolfin::uint Function::numSubFunctions() const
{
  if (_type != discrete)
    error("Only discrete functions have sub functions.");

  return static_cast<DiscreteFunction*>(f)->numSubFunctions();
}
//-----------------------------------------------------------------------------
SubFunction Function::operator[] (uint i)
{
  if (_type != discrete)
    error("Sub functions can only be extracted from discrete functions.");

  SubFunction sub_function(static_cast<DiscreteFunction*>(f), i);
  return sub_function;
}
//-----------------------------------------------------------------------------
const Function& Function::operator= (Function& f)
{
  // FIXME: Handle other assignments
  if (f._type != discrete)
    error("Can only handle assignment from discrete functions (for now).");
  
  // Either create or copy discrete function
  if (_type == discrete)
  {
    *static_cast<DiscreteFunction*>(this->f) = *static_cast<DiscreteFunction*>(f.f);
  }
  else
  {
    delete this->f;
    this->f = new DiscreteFunction(*static_cast<DiscreteFunction*>(f.f));
    _type = discrete;
    rename(name(), "discrete function");
  }
  return *this;
}
//-----------------------------------------------------------------------------
const Function& Function::operator= (SubFunction sub_function)
{
  if (f)
    delete f;

  f = new DiscreteFunction(sub_function);
  
  rename("u", "discrete function");
  _type = discrete;

  return *this;
}
//-----------------------------------------------------------------------------
void Function::interpolate(real* values)
{
  if (!f)
    error("Function contains no data.");

  f->interpolate(values);
}
//-----------------------------------------------------------------------------
void Function::interpolate(real* coefficients,
                           const ufc::cell& ufc_cell,
                           const ufc::finite_element& finite_element,
                           Cell& cell, int facet)
{
  if (!f)
    error("Function contains no data.");

  // Make current cell and facet are available to user-defined function
  _cell = &cell;
  _facet = facet;

  // Interpolate function
  f->interpolate(coefficients, ufc_cell, finite_element);

  // Make cell and facet unavailable
  _cell = 0;
  _facet = -1;
}
//-----------------------------------------------------------------------------
void Function::eval(real* values, const real* x) const
{
  if (!f)
    error("Function contains no data.");
  
  // Try scalar version for user-defined function if not overloaded.
  // Otherwise, call eval() function in implementation. Note that we
  // must check if we have a user-defined function or we will go into
  // a loop between Function and UserFunction...

  if (_type == user)
    values[0] = eval(x);
  else
    f->eval(values, x);
}
//-----------------------------------------------------------------------------
dolfin::real Function::eval(const real* x) const
{
  // Try vector-version for non-user-defined function if not
  // overloaded. Otherwise, raise an exception. Note that we must
  // check that we *don't* have a user-defined function or we will go
  // into a loop between Function and UserFunction...

  if (_type != user)
  {
    real values[1] = {0.0};
    eval(values, x);
    return values[0];
  }
  
  error("Missing eval() for user-defined function (must be overloaded).");
  return 0.0;
}
//-----------------------------------------------------------------------------
const Cell& Function::cell() const
{
  if (!_cell)
    error("Current cell is unknown (only available during assembly).");
  return *_cell;
}
//-----------------------------------------------------------------------------
Point Function::normal() const
{
  return cell().normal(_facet);
}
//-----------------------------------------------------------------------------
int Function::facet() const
{
  return _facet;
}
//-----------------------------------------------------------------------------
