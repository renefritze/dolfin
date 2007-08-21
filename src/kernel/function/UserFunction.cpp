// Copyright (C) 2005-2007 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-11-26
// Last changed: 2007-08-20
//
// Note: this breaks the standard envelope-letter idiom slightly,
// since we call the envelope class from one of the letter classes.

#include <dolfin/dolfin_log.h>
#include <dolfin/Mesh.h>
#include <dolfin/Vertex.h>
#include <dolfin/Function.h>
#include <dolfin/UserFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
UserFunction::UserFunction(Mesh& mesh, Function* f)
  : GenericFunction(mesh), ufc::function(), f(f), size(1)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
UserFunction::~UserFunction()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
dolfin::uint UserFunction::rank() const
{
  // Just return 0 for now (might extend to vectors later)
  return 0;
}
//-----------------------------------------------------------------------------
dolfin::uint UserFunction::dim(uint i) const
{
  // Just return 1 for now (might extend to vectors later)
  return 1;
}
//-----------------------------------------------------------------------------
void UserFunction::interpolate(real* values)
{
  dolfin_assert(values);
  dolfin_assert(f);

  // Compute size of value (number of entries in tensor value)
  //uint size = 1;
  //for (uint i = 0; i < finite_element->value_rank(); i++)
  //  size *= finite_element->value_dimension(i);

  // Call overloaded eval function at each vertex
  array local_values(size, new real[size]);
  
  for (VertexIterator vertex(mesh); !vertex.end(); ++vertex)
  {
    // Evaluate at function at vertex
    array x(mesh.geometry().dim(), vertex->x());
    f->eval(local_values, x);

    // Copy values to array of vertex values
    for (uint i = 0; i < size; i++)
      values[i*mesh.numVertices() + vertex->index()] = local_values[i];
  }
  delete [] local_values.data;
}
//-----------------------------------------------------------------------------
void UserFunction::interpolate(real* coefficients,
                               const ufc::cell& cell,
                               const ufc::finite_element& finite_element)
{
  dolfin_assert(coefficients);

  // Evaluate each dof to get coefficients for nodal basis expansion
  for (uint i = 0; i < finite_element.space_dimension(); i++)
    coefficients[i] = finite_element.evaluate_dof(i, *this, cell);
}
//-----------------------------------------------------------------------------
void UserFunction::evaluate(real* values,
                            const real* coordinates,
                            const ufc::cell& cell) const
{
  dolfin_assert(values);
  dolfin_assert(coordinates);
  dolfin_assert(f);

  // Call overloaded eval function
  array v(size, values);
  array x(cell.geometric_dimension, const_cast<real*>(coordinates));
  f->eval(v, x);
}
//-----------------------------------------------------------------------------
