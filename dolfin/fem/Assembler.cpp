// Copyright (C) 2007-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2007-2009
// Modified by Ola Skavhaug, 2007-2009
// Modified by Kent-Andre Mardal, 2008
//
// First added:  2007-01-17
// Last changed: 2009-06-22

#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/Timer.h>
#include <dolfin/la/GenericMatrix.h>
#include <dolfin/la/GenericTensor.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/la/Scalar.h>
#include <dolfin/la/SparsityPattern.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/mesh/MeshData.h>
#include <dolfin/mesh/BoundaryMesh.h>
#include <dolfin/mesh/MeshFunction.h>
#include <dolfin/mesh/SubDomain.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/FunctionSpace.h>
#include "DofMap.h"
#include "Form.h"
#include "UFC.h"
#include "Assembler.h"
#include "SparsityPatternBuilder.h"
#include "DirichletBC.h"
#include "FiniteElement.h"

using namespace dolfin;

//----------------------------------------------------------------------------
void Assembler::assemble(GenericTensor& A,
                         const Form& a,
                         bool reset_tensor)
{
  // Extract boundary indicators (if any)
  MeshFunction<uint>* exterior_facet_domains
    = a.mesh().data().mesh_function("exterior facet domains"); 
  
  // Assemble
  assemble(A, a, 0, exterior_facet_domains, 0, reset_tensor);
}
//-----------------------------------------------------------------------------
void Assembler::assemble(GenericTensor& A,
                         const Form& a,
                         const SubDomain& sub_domain,
                         bool reset_tensor)
{
  // Extract mesh
  const Mesh& mesh = a.mesh();

  // Extract cell domains
  MeshFunction<uint>* cell_domains = 0;
  if (a.ufc_form().num_cell_integrals() > 0)
  {
    cell_domains = new MeshFunction<uint>(const_cast<Mesh&>(mesh), mesh.topology().dim());
    (*cell_domains) = 1;
    sub_domain.mark(*cell_domains, 0);
  }

  // Extract facet domains
  MeshFunction<uint>* facet_domains = 0;
  if (a.ufc_form().num_exterior_facet_integrals() > 0 ||
      a.ufc_form().num_interior_facet_integrals() > 0)
  {
    facet_domains = new MeshFunction<uint>(const_cast<Mesh&>(mesh), mesh.topology().dim() - 1);
    (*facet_domains) = 1;
    sub_domain.mark(*facet_domains, 0);
  }

  // Assemble
  assemble(A, a, cell_domains, facet_domains, facet_domains, reset_tensor);

  // Delete domains
  delete cell_domains;
  delete facet_domains;
}
//-----------------------------------------------------------------------------
void Assembler::assemble(GenericTensor& A,
                         const Form& a,
                         const MeshFunction<uint>* cell_domains,
                         const MeshFunction<uint>* exterior_facet_domains,
                         const MeshFunction<uint>* interior_facet_domains,
                         bool reset_tensor)
{
  // Note the importance of treating empty mesh functions as null pointers
  // for the PyDOLFIN interface.

  // Check form
  check(a);

  // Create data structure for local assembly data
  UFC ufc(a);

  // Initialize global tensor
  init_global_tensor(A, a, ufc, reset_tensor);

  // Assemble over cells
  assemble_cells(A, a, ufc, cell_domains, 0);

  // Assemble over exterior facets
  assemble_exterior_facets(A, a, ufc, exterior_facet_domains, 0);

  // Assemble over interior facets
  assemble_interior_facets(A, a, ufc, interior_facet_domains, 0);

  // Finalise assembly of global tensor
  A.apply();
}
//-----------------------------------------------------------------------------
void Assembler::assemble_cells(GenericTensor& A,
                               const Form& a,
                               UFC& ufc,
                               const MeshFunction<uint>* domains,
                               std::vector<double>* values)
{
  // Skip assembly if there are no cell integrals
  if (ufc.form.num_cell_integrals() == 0)
    return;
  Timer timer("Assemble cells");

  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();
  const std::vector<const Function*>& coefficients = a.coefficients();

  // Cell integral
  ufc::cell_integral* integral = ufc.cell_integrals[0];

  // Assemble over cells
  uint num_function_spaces = a.function_spaces().size();
  Progress p(progress_message(A.rank(), "cells"), mesh.num_cells());
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // FIXME: will move this check into a separate function.
    // Need to check the coefficients as well.
    bool compute_on_cell = true;
    for (uint i = 0; i < num_function_spaces; i++)
    {
      if (!a.function_space(i).is_inside_restriction(cell->index()))
        compute_on_cell = false;
    }
    if (!compute_on_cell)
      continue;

    // Get integral for sub domain (if any)
    if (domains && domains->size() > 0)
    {
      const uint domain = (*domains)(*cell);
      if (domain < ufc.form.num_cell_integrals())
        integral = ufc.cell_integrals[domain];
      else
        continue;
    }

    // Update to current cell
    ufc.update(*cell);

    // Interpolate coefficients on cell
    for (uint i = 0; i < coefficients.size(); i++)
      coefficients[i]->interpolate(ufc.w[i], ufc.cell, cell->index());

    // Tabulate dofs for each dimension
    for (uint i = 0; i < ufc.form.rank(); i++)
      a.function_space(i).dofmap().tabulate_dofs(ufc.dofs[i], ufc.cell, cell->index());

    // Tabulate cell tensor
    integral->tabulate_tensor(ufc.A, ufc.w, ufc.cell);

    // Add entries to global tensor
    if (values && ufc.form.rank() == 0)
      (*values)[cell->index()] = ufc.A[0];
    else
    {
      // Get local dimensions
      for (uint i = 0; i < a.rank(); i++)
        ufc.local_dimensions[i] = a.function_space(i).dofmap().local_dimension(ufc.cell);
      A.add(ufc.A, ufc.local_dimensions, ufc.dofs);
    }

    p++;
  }
}
//-----------------------------------------------------------------------------
void Assembler::assemble_exterior_facets(GenericTensor& A,
                                         const Form& a,
                                         UFC& ufc,
                                         const MeshFunction<uint>* domains,
                                         std::vector<double>* values)
{
  // Skip assembly if there are no exterior facet integrals
  if (ufc.form.num_exterior_facet_integrals() == 0)
    return;
  Timer timer("Assemble exterior facets");

  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();
  const std::vector<const Function*>& coefficients = a.coefficients();

  // Exterior facet integral
  ufc::exterior_facet_integral* integral = ufc.exterior_facet_integrals[0];

  // Create boundary mesh
  BoundaryMesh boundary(mesh);
  MeshFunction<uint>* cell_map = boundary.data().mesh_function("cell map");
  assert(cell_map);

  // Assemble over exterior facets (the cells of the boundary)
  Progress p(progress_message(A.rank(), "exterior facets"), boundary.num_cells());
  for (CellIterator boundary_cell(boundary); !boundary_cell.end(); ++boundary_cell)
  {
    // Get mesh facet corresponding to boundary cell
    Facet mesh_facet(mesh, (*cell_map)(*boundary_cell));

    // Get integral for sub domain (if any)
    if (domains && domains->size() > 0)
    {
      const uint domain = (*domains)(mesh_facet);
      if (domain < ufc.form.num_exterior_facet_integrals())
        integral = ufc.exterior_facet_integrals[domain];
      else
        continue;
    }

    // Get mesh cell to which mesh facet belongs (pick first, there is only one)
    assert(mesh_facet.num_entities(mesh.topology().dim()) == 1);
    Cell mesh_cell(mesh, mesh_facet.entities(mesh.topology().dim())[0]);

    // Get local index of facet with respect to the cell
    const uint local_facet = mesh_cell.index(mesh_facet);

    // Update to current cell
    ufc.update(mesh_cell);

    // Interpolate coefficients on cell
    for (uint i = 0; i < coefficients.size(); i++)
      coefficients[i]->interpolate(ufc.w[i], ufc.cell, mesh_cell.index(), local_facet);

    // Tabulate dofs for each dimension
    for (uint i = 0; i < ufc.form.rank(); i++)
      a.function_space(i).dofmap().tabulate_dofs(ufc.dofs[i], ufc.cell, mesh_cell.index());

    // Tabulate exterior facet tensor
    integral->tabulate_tensor(ufc.A, ufc.w, ufc.cell, local_facet);

    // Get local dimensions
    for (uint i = 0; i < a.rank(); i++)
      ufc.local_dimensions[i] = a.function_space(i).dofmap().local_dimension(ufc.cell);

    // Add entries to global tensor
    A.add(ufc.A, ufc.local_dimensions, ufc.dofs);

    p++;
  }
}
//-----------------------------------------------------------------------------
void Assembler::assemble_interior_facets(GenericTensor& A,
                                         const Form& a,
                                         UFC& ufc,
                                         const MeshFunction<uint>* domains,
                                         std::vector<double>* values)
{
  // Skip assembly if there are no interior facet integrals
  if (ufc.form.num_interior_facet_integrals() == 0)
    return;
  Timer timer("Assemble interior facets");

  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();
  const std::vector<const Function*>& coefficients = a.coefficients();

  // Interior facet integral
  ufc::interior_facet_integral* integral = ufc.interior_facet_integrals[0];

  // Compute facets and facet - cell connectivity if not already computed
  mesh.init(mesh.topology().dim() - 1);
  mesh.init(mesh.topology().dim() - 1, mesh.topology().dim());
  assert(mesh.ordered());

  // Assemble over interior facets (the facets of the mesh)
  Progress p(progress_message(A.rank(), "interior facets"), mesh.num_facets());
  for (FacetIterator facet(mesh); !facet.end(); ++facet)
  {
    // Check if we have an exterior facet
    if (facet->num_entities(mesh.topology().dim()) != 2)
    {
      p++;
      continue;
    }

    // Get integral for sub domain (if any)
    if (domains && domains->size() > 0)
    {
      const uint domain = (*domains)(*facet);
      if (domain < ufc.form.num_interior_facet_integrals())
        integral = ufc.interior_facet_integrals[domain];
      else
        continue;
    }

    // Get cells incident with facet
    Cell cell0(mesh, facet->entities(mesh.topology().dim())[0]);
    Cell cell1(mesh, facet->entities(mesh.topology().dim())[1]);

    // Get local index of facet with respect to each cell
    uint facet0 = cell0.index(*facet);
    uint facet1 = cell1.index(*facet);

    // Update to current pair of cells
    ufc.update(cell0, cell1);

    // Interpolate coefficients on cell
    for (uint i = 0; i < coefficients.size(); i++)
    {
      const uint offset = ufc.coefficient_elements[i]->space_dimension();
      coefficients[i]->interpolate(ufc.macro_w[i],          ufc.cell0, cell0.index(), facet0);
      coefficients[i]->interpolate(ufc.macro_w[i] + offset, ufc.cell1, cell1.index(), facet1);
    }

    // Tabulate dofs for each dimension on macro element
    for (uint i = 0; i < a.rank(); i++)
    {
      const uint offset = a.function_space(i).dofmap().local_dimension(ufc.cell0);
      a.function_space(i).dofmap().tabulate_dofs(ufc.macro_dofs[i],          ufc.cell0, cell0.index());
      a.function_space(i).dofmap().tabulate_dofs(ufc.macro_dofs[i] + offset, ufc.cell1, cell1.index());
    }

    // Tabulate exterior interior facet tensor on macro element
    integral->tabulate_tensor(ufc.macro_A, ufc.macro_w, ufc.cell0, ufc.cell1, facet0, facet1);

    // Get local dimensions
    for (uint i = 0; i < a.rank(); i++)
      ufc.macro_local_dimensions[i] = a.function_space(i).dofmap().local_dimension(ufc.cell0)
                                    + a.function_space(i).dofmap().local_dimension(ufc.cell1);

    // Add entries to global tensor
    A.add(ufc.macro_A, ufc.macro_local_dimensions, ufc.macro_dofs);

    p++;
  }
}
//-----------------------------------------------------------------------------
void Assembler::check(const Form& a)
{
  // Check the form
  a.check();

  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();
  const std::vector<const Function*>& coefficients = a.coefficients();

  // Check that we get the correct number of coefficients
  if (coefficients.size() != a.ufc_form().num_coefficients())
    error("Incorrect number of coefficients for form: %d given but %d required.",
          coefficients.size(), a.ufc_form().num_coefficients());

  // Check that all coefficients have valid value dimensions
  for (uint i = 0; i < coefficients.size(); ++i)
  {
    if(!coefficients[i])
      error("Got NULL Function as coefficient %d.", i);

    try
    {
      // auto_ptr deletes its object when it exits its scope
      std::auto_ptr<ufc::finite_element> fe( a.ufc_form().create_finite_element(i+a.rank()) );

      uint r = coefficients[i]->function_space().element().value_rank();
      uint fe_r = fe->value_rank();
      if (fe_r != r)
        warning("Invalid value rank of Function %d, got %d but expecting %d. \
You may need to provide the rank of a user defined Function.", i, r, fe_r);

      for (uint j = 0; j < r; ++j)
      {
        uint dim = coefficients[i]->function_space().element().value_dimension(j);
        uint fe_dim = fe->value_dimension(j);
        if (dim != fe_dim)
          warning("Invalid value dimension %d of Function %d, got %d but expecting %d. \
You may need to provide the dimension of a user defined Function.", j, i, dim, fe_dim);
      }
    }
    catch(std::exception & e)
    {
      warning("Function %d is invalid.", i);
    }
  }

  // Check that the cell dimension matches the mesh dimension
  if (a.ufc_form().rank() + a.ufc_form().num_coefficients() > 0)
  {
    ufc::finite_element* element = a.ufc_form().create_finite_element(0);
    assert(element);
    if (mesh.type().cell_type() == CellType::interval && element->cell_shape() != ufc::interval)
      error("Mesh cell type (intervals) does not match cell type of form.");
    if (mesh.type().cell_type() == CellType::triangle && element->cell_shape() != ufc::triangle)
      error("Mesh cell type (triangles) does not match cell type of form.");
    if (mesh.type().cell_type() == CellType::tetrahedron && element->cell_shape() != ufc::tetrahedron)
      error("Mesh cell type (tetrahedra) does not match cell type of form.");
    delete element;
  }

  // Check that the mesh is ordered
  if (!mesh.ordered())
    error("Unable to assemble, mesh is not correctly ordered (consider calling mesh.order()).");
}
//-----------------------------------------------------------------------------
void Assembler::init_global_tensor(GenericTensor& A,
                                   const Form& a,
                                   UFC& ufc, bool reset_tensor)
{
  if (reset_tensor)
  {
    // Build sparsity pattern
    Timer t0("Build sparsity");
    GenericSparsityPattern* sparsity_pattern = A.factory().create_pattern();
    if (sparsity_pattern)
    {
      std::vector<const DofMap*> dof_maps(0);
      for (uint i=0; i < a.rank(); ++i)
        dof_maps.push_back(&(a.function_space(i).dofmap()));
      SparsityPatternBuilder::build(*sparsity_pattern, a, ufc);
    }
    t0.stop();

    // Initialize tensor
    Timer t1("Init tensor");
    if (sparsity_pattern)
      A.init(*sparsity_pattern);
    else
    {
      A.resize(ufc.form.rank(), ufc.global_dimensions);
      A.zero();
    }
    t1.stop();

    // Delete sparsity pattern
    Timer t2("Delete sparsity");
    delete sparsity_pattern;
    t2.stop();
  }
  else
    A.zero();
}
//-----------------------------------------------------------------------------
std::string Assembler::progress_message(uint rank, std::string integral_type)
{
  std::stringstream s;
  s << "Assembling ";

  switch (rank)
  {
  case 0:
    s << "scalar value over ";
    break;
  case 1:
    s << "vector over ";
    break;
  case 2:
    s << "matrix over ";
    break;
  default:
    s << "rank " << rank << " tensor over ";
    break;
  }

  s << integral_type;

  return s.str();
}
//-----------------------------------------------------------------------------
