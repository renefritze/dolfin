// Copyright (C) 2005-2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-11-28
// Last changed: 2007-04-12

#include <dolfin/FunctionPointerFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
FunctionPointerFunction::FunctionPointerFunction(FunctionPointer f)
  : GenericFunction(), f(f)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
FunctionPointerFunction::~FunctionPointerFunction()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
dolfin::uint FunctionPointerFunction::rank() const
{
  // Just return 0 for now (might extend to vectors later)
  return 0;
}
//-----------------------------------------------------------------------------
dolfin::uint FunctionPointerFunction::dim(uint i) const
{
  // Just return 1 for now (might extend to vectors later)
  return 1;
}
//-----------------------------------------------------------------------------
void FunctionPointerFunction::interpolate(real* values, Mesh& mesh)
{
  // FIXME: Not implemented yet, should we remove this class?
}
//-----------------------------------------------------------------------------
void FunctionPointerFunction::interpolate(real* coefficients,
                                          const ufc::cell& cell,
                                          const ufc::finite_element& finite_element)
{
  // Evaluate each dof to get coefficients for nodal basis expansion
  for (uint i = 0; i < finite_element.space_dimension(); i++)
    coefficients[i] = finite_element.evaluate_dof(i, *this, cell);
}
//-----------------------------------------------------------------------------
void FunctionPointerFunction::evaluate(real* values,
                                       const real* coordinates,
                                       const ufc::cell& cell) const
{
  // Call function
  f(values, coordinates);
}
//-----------------------------------------------------------------------------
