// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2009
// Modified by Garth N. Wells, 2009
//
// First added:  2007-01-17
// Last changed: 2009-08-03

#include <dolfin/common/types.h>
#include <dolfin/function/FunctionSpace.h>
#include "DofMap.h"
#include "FiniteElement.h"
#include "Form.h"
#include "UFC.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
UFC::UFC(const Form& form)
 : form(form.ufc_form()), cell(form.mesh()), cell0(form.mesh()), 
   cell1(form.mesh()), coefficients(form.coefficients())
{
  // Create finite elements
  finite_elements = new FiniteElement*[this->form.rank()];
  for (uint i = 0; i < this->form.rank(); i++)
  {
    boost::shared_ptr<ufc::finite_element> element(this->form.create_finite_element(i));
    finite_elements[i] = new FiniteElement(element);
  }
  // Create finite elements for coefficients
  coefficient_elements = new FiniteElement*[this->form.num_coefficients()];
  for (uint i = 0; i < this->form.num_coefficients(); i++)
  {
    boost::shared_ptr<ufc::finite_element> element(this->form.create_finite_element(this->form.rank() + i));
    coefficient_elements[i] = new FiniteElement(element);
  }
  // Create cell integrals
  cell_integrals = new ufc::cell_integral*[this->form.num_cell_integrals()];
  for (uint i = 0; i < this->form.num_cell_integrals(); i++)
    cell_integrals[i] = this->form.create_cell_integral(i);

  // Create exterior facet integrals
  exterior_facet_integrals = new ufc::exterior_facet_integral*[this->form.num_exterior_facet_integrals()];
  for (uint i = 0; i < this->form.num_exterior_facet_integrals(); i++)
    exterior_facet_integrals[i] = this->form.create_exterior_facet_integral(i);

  // Create interior facet integrals
  interior_facet_integrals = new ufc::interior_facet_integral*[this->form.num_interior_facet_integrals()];
  for (uint i = 0; i < this->form.num_interior_facet_integrals(); i++)
    interior_facet_integrals[i] = this->form.create_interior_facet_integral(i);

  // Initialize mesh
  this->mesh.init(form.mesh());

  // Get function spaces for arguments
  std::vector<const FunctionSpace*> V = form.function_spaces();

  // Initialize local tensor
  uint num_entries = 1;
  for (uint i = 0; i < this->form.rank(); i++)
    num_entries *= V[i]->dofmap().max_local_dimension();
  A = new double[num_entries];
  for (uint i = 0; i < num_entries; i++)
    A[i] = 0.0;

  // Initialize local tensor for macro element
  num_entries = 1;
  for (uint i = 0; i < this->form.rank(); i++)
    num_entries *= 2*V[i]->dofmap().max_local_dimension();
  macro_A = new double[num_entries];
  for (uint i = 0; i < num_entries; i++)
    macro_A[i] = 0.0;

  // Allocate memory for storing local dimensions
  local_dimensions = new uint[this->form.rank()];
  macro_local_dimensions = new uint[this->form.rank()];
  for (uint i = 0; i < this->form.rank(); i++)
  {
    local_dimensions[i] = V[i]->dofmap().max_local_dimension();
    macro_local_dimensions[i] = 2*V[i]->dofmap().max_local_dimension();
  }

  // Initialize global dimensions
  global_dimensions = new uint[this->form.rank()];
  for (uint i = 0; i < this->form.rank(); i++)
    global_dimensions[i] = V[i]->dofmap().global_dimension();

  // Initialize dofs
  dofs = new uint*[this->form.rank()];
  for (uint i = 0; i < this->form.rank(); i++)
  {
    dofs[i] = new uint[local_dimensions[i]];
    for (uint j = 0; j < local_dimensions[i]; j++)
      dofs[i][j] = 0;
  }

  // Initialize dofs on macro element
  macro_dofs = new uint*[this->form.rank()];
  for (uint i = 0; i < this->form.rank(); i++)
  {
    macro_dofs[i] = new uint[macro_local_dimensions[i]];
    for (uint j = 0; j < macro_local_dimensions[i]; j++)
      macro_dofs[i][j] = 0;
  }

  // Initialize coefficients
  w = new double*[this->form.num_coefficients()];
  for (uint i = 0; i < this->form.num_coefficients(); i++)
  {
    const uint n = coefficient_elements[i]->space_dimension();
    w[i] = new double[n];
    for (uint j = 0; j < n; j++)
      w[i][j] = 0.0;
  }

  // Initialize coefficients on macro element
  macro_w = new double*[this->form.num_coefficients()];
  for (uint i = 0; i < this->form.num_coefficients(); i++)
  {
    const uint n = 2*coefficient_elements[i]->space_dimension();
    macro_w[i] = new double[n];
    for (uint j = 0; j < n; j++)
      macro_w[i][j] = 0.0;
  }
}
//-----------------------------------------------------------------------------
UFC::~UFC()
{
  // Delete finite elements
  for (uint i = 0; i < this->form.rank(); i++)
    delete finite_elements[i];
  delete [] finite_elements;

  // Delete coefficient finite elements
  for (uint i = 0; i < this->form.num_coefficients(); i++)
    delete coefficient_elements[i];
  delete [] coefficient_elements;

  // Delete cell integrals
  for (uint i = 0; i < this->form.num_cell_integrals(); i++)
    delete cell_integrals[i];
  delete [] cell_integrals;

  // Delete exterior facet integrals
  for (uint i = 0; i < this->form.num_exterior_facet_integrals(); i++)
    delete exterior_facet_integrals[i];
  delete [] exterior_facet_integrals;

  // Delete interior facet integrals
  for (uint i = 0; i < this->form.num_interior_facet_integrals(); i++)
    delete interior_facet_integrals[i];
  delete [] interior_facet_integrals;

  // Delete local tensor
  delete [] A;

  // Delete local tensor for macro element
  delete [] macro_A;

  // Delete local dimensions
  delete [] local_dimensions;

  // Delete global dimensions
  delete [] global_dimensions;

  // Delete local dimensions for macro element
  delete [] macro_local_dimensions;

  // Delete dofs
  for (uint i = 0; i < this->form.rank(); i++)
    delete [] dofs[i];
  delete [] dofs;

  // Delete macro dofs
  for (uint i = 0; i < this->form.rank(); i++)
    delete [] macro_dofs[i];
  delete [] macro_dofs;

  // Delete coefficients
  for (uint i = 0; i < this->form.num_coefficients(); i++)
    delete [] w[i];
  delete [] w;

  // Delete macro coefficients
  for (uint i = 0; i < this->form.num_coefficients(); i++)
    delete [] macro_w[i];
  delete [] macro_w;
}
//-----------------------------------------------------------------------------
void UFC::update(const Cell& cell)
{
  // Update UFC cell
  this->cell.update(cell);

  // Interpolate coefficients on cell
  for (uint i = 0; i < coefficients.size(); i++)
    coefficients[i]->interpolate(w[i], this->cell, cell.index());
}
//-----------------------------------------------------------------------------
void UFC::update(const Cell& cell, uint local_facet)
{
  // Update UFC cell
  this->cell.update(cell);

  // Interpolate coefficients on facet
  for (uint i = 0; i < coefficients.size(); i++)
    coefficients[i]->interpolate(w[i], this->cell, cell.index(), local_facet);
}
//-----------------------------------------------------------------------------
void UFC::update(const Cell& cell0, uint local_facet0, 
                 const Cell& cell1, uint local_facet1)
{
  // Update UFC cells
  this->cell0.update(cell0);
  this->cell1.update(cell1);

  // Interpolate coefficients on cell
  for (uint i = 0; i < coefficients.size(); i++)
  {
    const uint offset = coefficient_elements[i]->space_dimension();
    coefficients[i]->interpolate(macro_w[i],          this->cell0, cell0.index(), local_facet0);
    coefficients[i]->interpolate(macro_w[i] + offset, this->cell1, cell1.index(), local_facet1);
  }
}
//-----------------------------------------------------------------------------
