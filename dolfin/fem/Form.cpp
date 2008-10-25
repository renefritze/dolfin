// Copyright (C) 2007-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008.
//
// First added:  2007-12-10
// Last changed: 2008-10-24

#include <ufc.h>
#include <dolfin/log/log.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/function/Function.h>
#include "Form.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Form::Form() : _ufc_form(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Form::~Form()
{
  delete _ufc_form;
}
//-----------------------------------------------------------------------------
dolfin::uint Form::rank() const
{ 
  dolfin_assert(_ufc_form); 
  return _ufc_form->rank(); 
}
//-----------------------------------------------------------------------------
const Mesh& Form::mesh() const
{
  const Mesh* _mesh = 0;

  // Check function spaces
  for (uint i = 0; i < _function_spaces.size(); i++)
  {
    const Mesh* m = &_function_spaces[i]->mesh();

    // Pick mesh
    if (!_mesh)
    {
      _mesh = m;
      continue;
    }

    // Check that meshes match
    if (_mesh != m)
      error("Unable to extract mesh from form, nonmatching meshes for function spaces.");
  }

  // Check coefficients
  for (uint i = 0; i < _coefficients.size(); i++)
  {
    const Mesh* m = &_coefficients[i]->function_space().mesh();

    // Pick mesh
    if (!_mesh)
    {
      _mesh = m;
      continue;
    }

    // Check that meshes match
    if (_mesh != m)
      error("Unable to extract mesh from form, nonmatching meshes for function spaces.");
  }

  // Check that we got a mesh
  if (!_mesh)
    error("Unable to extract mesh from form.");

  return *_mesh;
}
//-----------------------------------------------------------------------------
const FunctionSpace& Form::function_space(uint i) const
{
  dolfin_assert(i < _function_spaces.size());
  return *_function_spaces[i];
}
//-----------------------------------------------------------------------------
const std::vector<FunctionSpace*> Form::function_spaces() const
{
  std::vector<FunctionSpace*> V;
  for (uint i = 0; i < _function_spaces.size(); ++i)
    V.push_back(_function_spaces[i].get());

  return V;
}
//-----------------------------------------------------------------------------
const Function& Form::coefficient(uint i) const
{
  dolfin_assert(i < _coefficients.size());
  return *_coefficients[i];
}
//-----------------------------------------------------------------------------
const std::vector<Function*> Form::coefficients() const
{
  std::vector<Function*> V;
  for (uint i = 0; i < _coefficients.size(); ++i)
    V.push_back(_coefficients[i].get());

  return V;
}
//-----------------------------------------------------------------------------
const ufc::form& Form::ufc_form() const
{
  dolfin_assert(_ufc_form);
  return *_ufc_form;
}
//-----------------------------------------------------------------------------
void Form::check() const
{
  error("Form::check() not implemented.");
}
//-----------------------------------------------------------------------------
