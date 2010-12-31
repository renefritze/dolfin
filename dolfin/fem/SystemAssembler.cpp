// Copyright (C) 2008-20010 Kent-Andre Mardal and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008-2009.
//
// First added:  2009-06-22
// Last changed: 2010-12-21

#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/Timer.h>
#include <dolfin/function/GenericFunction.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/la/GenericMatrix.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/mesh/MeshFunction.h>
#include <dolfin/mesh/SubDomain.h>
#include "AssemblerTools.h"
#include "DirichletBC.h"
#include "FiniteElement.h"
#include "Form.h"
#include "GenericDofMap.h"
#include "UFC.h"
#include "SystemAssembler.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void SystemAssembler::assemble(GenericMatrix& A, GenericVector& b,
                               const Form& a, const Form& L,
                               bool reset_sparsity, bool add_values)
{
  std::vector<const DirichletBC*> bcs;
  assemble(A, b, a, L, bcs, 0, 0, 0, 0, reset_sparsity, add_values);
}
//-----------------------------------------------------------------------------
void SystemAssembler::assemble(GenericMatrix& A, GenericVector& b,
                               const Form& a, const Form& L,
                               const DirichletBC& bc,
                               bool reset_sparsity, bool add_values)
{
  std::vector<const DirichletBC*> bcs;
  bcs.push_back(&bc);
  assemble(A, b, a, L, bcs, 0, 0, 0, 0, reset_sparsity, add_values);
}
//-----------------------------------------------------------------------------
void SystemAssembler::assemble(GenericMatrix& A, GenericVector& b,
                               const Form& a, const Form& L,
                               const std::vector<const DirichletBC*>& bcs,
                               bool reset_sparsity, bool add_values)
{
  assemble(A, b, a, L, bcs, 0, 0, 0, 0, reset_sparsity, add_values);
}
//-----------------------------------------------------------------------------
void SystemAssembler::assemble(GenericMatrix& A, GenericVector& b,
                               const Form& a, const Form& L,
                               const std::vector<const DirichletBC*>& bcs,
                               const MeshFunction<uint>* cell_domains,
                               const MeshFunction<uint>* exterior_facet_domains,
                               const MeshFunction<uint>* interior_facet_domains,
                               const GenericVector* x0,
                               bool reset_sparsity, bool add_values)
{
  Timer timer("Assemble system");
  info(PROGRESS, "Assembling linear system and applying boundary conditions...");

  const Mesh& mesh = a.mesh();

  // Compute facets and facet - cell connectivity if not already computed
  const uint D = mesh.topology().dim();
  mesh.init(D - 1);
  mesh.init(D - 1, D);
  assert(mesh.ordered());

  // FIXME: Some things can be simplified since we know it's a matrix and a vector

  // Sub domains not supported
  if (cell_domains || exterior_facet_domains || interior_facet_domains)
    error("SystemAssembler does not yet support subdomains.");

  // Check arguments
  AssemblerTools::check(a); AssemblerTools::check(L);

  // Check that we have a bilinear and a linear form
  assert(a.rank() == 2); assert(L.rank() == 1);

  // Check that forms share a function space
  assert(a.function_space(1).get() == L.function_space(0).get());


  // FIXME: This may gather coefficients twice. Checked for shared coefficients

  // Gather off-process coefficients for a
  std::vector<const GenericFunction*> coefficients = a.coefficients();
  for (uint i = 0; i < coefficients.size(); ++i)
    coefficients[i]->gather();

  // Gather off-process coefficients for L
  coefficients = L.coefficients();
  for (uint i = 0; i < coefficients.size(); ++i)
    coefficients[i]->gather();

  // Create data structure for local assembly data
  UFC A_ufc(a); UFC b_ufc(L);

  // Initialize global tensor
  AssemblerTools::init_global_tensor(A, a, reset_sparsity, add_values);
  AssemblerTools::init_global_tensor(b, L, reset_sparsity, add_values);

  // Allocate data
  Scratch data(a, L);

  // Get Dirichlet dofs and values for local mesh
  DirichletBC::Map boundary_values;
  for (uint i = 0; i < bcs.size(); ++i)
  {
    // Methods other than 'pointwise' are not robust in parallel since a vertex
    // can have a bc applied, but the partition might not have a facet on the boundary.
    if (MPI::num_processes() > 1 && bcs[i]->method() != "pointwise")
    {
      warning("Dirichlet boundary condition method '%s' is not robust in parallel with symmetric assembly. Using 'pointwise' instead.", bcs[i]->method().c_str());
      warning("Caution: 'on_boundary' does not work with 'pointwise' boundary conditions,");
      bcs[i]->get_boundary_values(boundary_values, "pointwise");
    }
    else
      bcs[i]->get_boundary_values(boundary_values);
  }

  // Modify boundary values for incremental (typically nonlinear) problems
  if (x0)
  {
    if (MPI::num_processes() > 1)
      warning("Parallel symmetric assembly over interior facets for nonlinear problems is untested");

    assert(x0->size() == a.function_space(1)->dofmap().global_dimension());

    const uint num_bc_dofs = boundary_values.size();
    std::vector<uint> bc_indices;
    std::vector<double> bc_values;
    bc_indices.reserve(num_bc_dofs);
    bc_values.reserve(num_bc_dofs);

    // Build list of boundary dofs and values
    DirichletBC::Map::const_iterator bv;
    for (bv = boundary_values.begin(); bv != boundary_values.end(); ++bv)
    {
      bc_indices.push_back(bv->first);
      bc_values.push_back(bv->second);
    }

    // Modify bc values
    std::vector<double> x0_values(num_bc_dofs);
    x0->get_local(&x0_values[0], num_bc_dofs, &bc_indices[0]);
    for (uint i = 0; i < num_bc_dofs; i++)
      bc_values[i] = x0_values[i] - bc_values[i];
  }

  if (A_ufc.form.num_interior_facet_integrals() == 0 && b_ufc.form.num_interior_facet_integrals() == 0)
  {
    // Assemble cell-wise (no interior facet integrals)
    cell_wise_assembly(A, b, a, L, A_ufc, b_ufc, data, boundary_values,
                       cell_domains, exterior_facet_domains);
  }
  else
  {
    not_working_in_parallel("Assembly over interior facets");

    // Assemble facet-wise (including cell assembly)
    facet_wise_assembly(A, b, a, L, A_ufc, b_ufc, data, boundary_values,
                        cell_domains, exterior_facet_domains,
                        interior_facet_domains);
  }

  // Finalise assembly
  A.apply("add");
  b.apply("add");
}
//-----------------------------------------------------------------------------
void SystemAssembler::cell_wise_assembly(GenericMatrix& A, GenericVector& b,
                              const Form& a, const Form& L,
                              UFC& A_ufc, UFC& b_ufc, Scratch& data,
                              const DirichletBC::Map& boundary_values,
                              const MeshFunction<uint>* cell_domains,
                              const MeshFunction<uint>* exterior_facet_domains)
{
  // FIXME: We can used some std::vectors or array pointers for the A and b
  // related terms to cut down on code repetition.

  const Mesh& mesh = a.mesh();

  // Compute facets and facet - cell connectivity if not already computed
  const uint D = mesh.topology().dim();
  mesh.init(D - 1);
  mesh.init(D - 1, D);
  assert(mesh.ordered());

  // Extract exterior (non shared) facets markers
  const MeshFunction<uint>* exterior_facets = mesh.data().mesh_function("exterior facets");

  // Form ranks
  const uint a_rank = a.rank();
  const uint L_rank = L.rank();

  // Collect pointers to dof maps
  std::vector<const GenericDofMap*> a_dof_maps;
  for (uint i = 0; i < a_rank; ++i)
    a_dof_maps.push_back(&a.function_space(i)->dofmap());

  std::vector<const GenericDofMap*> L_dof_maps;
  for (uint i = 0; i < L_rank; ++i)
    L_dof_maps.push_back(&L.function_space(i)->dofmap());

  // Vector to hold dof map for a cell
  std::vector<const std::vector<uint>* > a_dofs(a_rank);
  std::vector<const std::vector<uint>* > L_dofs(L_rank);

  // Cell integrals
  const ufc::cell_integral* A_integral = A_ufc.cell_integrals[0].get();
  const ufc::cell_integral* b_integral = b_ufc.cell_integrals[0].get();

  Progress p("Assembling system (cell-wise)", mesh.num_cells());
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update to current cell
    A_ufc.update(*cell);
    b_ufc.update(*cell);

    // Tabulate cell tensor (A)
    A_integral->tabulate_tensor(A_ufc.A.get(), A_ufc.w, A_ufc.cell);
    for (uint i = 0; i < data.Ae.size(); ++i)
      data.Ae[i] = A_ufc.A[i];

    // Tabulate cell tensor (b)
    b_integral->tabulate_tensor(b_ufc.A.get(), b_ufc.w, b_ufc.cell);
    for (uint i = 0; i < data.be.size(); ++i)
      data.be[i] = b_ufc.A[i];

    // FIXME: Is assembly over facets more efficient?
    // Compute exterior facet integral if present
    if (A_ufc.form.num_exterior_facet_integrals() > 0 || b_ufc.form.num_exterior_facet_integrals() > 0)
    {
      const uint D = mesh.topology().dim();
      for (FacetIterator facet(*cell); !facet.end(); ++facet)
      {
        // Assemble if we have an external facet
        const bool interior_facet = facet->num_entities(D) == 2 || (exterior_facets && !(*exterior_facets)[*facet]);
        if (!interior_facet)
        {
          const uint local_facet = cell->index(*facet);
          if (A_ufc.form.num_exterior_facet_integrals() > 0)
          {
            A_ufc.update(*cell, local_facet);

            const ufc::exterior_facet_integral* A_facet_integral = A_ufc.exterior_facet_integrals[0].get();
            A_facet_integral->tabulate_tensor(A_ufc.A.get(), A_ufc.w, A_ufc.cell, local_facet);
            for (uint i = 0; i < data.Ae.size(); i++)
              data.Ae[i] += A_ufc.A[i];
          }
          if (b_ufc.form.num_exterior_facet_integrals() > 0)
          {
            b_ufc.update(*cell, local_facet);

            const ufc::exterior_facet_integral* b_facet_integral = b_ufc.exterior_facet_integrals[0].get();
            b_facet_integral->tabulate_tensor(b_ufc.A.get(), b_ufc.w, b_ufc.cell, local_facet);
            for (uint i = 0; i < data.be.size(); i++)
              data.be[i] += b_ufc.A[i];
          }
        }
      }
    }

    // Get local-to-global dof maps for cell
    a_dofs[0] = &(a_dof_maps[0]->cell_dofs(cell->index()));
    a_dofs[1] = &(a_dof_maps[1]->cell_dofs(cell->index()));
    L_dofs[0] = &(L_dof_maps[0]->cell_dofs(cell->index()));

    assert(L_dofs[0] == a_dofs[1]);

    // Modify local matrix/element for Dirichlet boundary conditions
    apply_bc(&(data.Ae)[0], &(data.be)[0], boundary_values, a_dofs);

    // Add entries to global tensor
    A.add(&(data.Ae)[0], a_dofs);
    b.add(&(data.be)[0], L_dofs);

    p++;
  }
}
//-----------------------------------------------------------------------------
void SystemAssembler::facet_wise_assembly(GenericMatrix& A, GenericVector& b,
                              const Form& a, const Form& L,
                              UFC& A_ufc, UFC& b_ufc, Scratch& data,
                              const DirichletBC::Map& boundary_values,
                              const MeshFunction<uint>* cell_domains,
                              const MeshFunction<uint>* exterior_facet_domains,
                              const MeshFunction<uint>* interior_facet_domains)
{
  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();
  const std::vector<const GenericFunction*> A_coefficients = a.coefficients();
  const std::vector<const GenericFunction*> b_coefficients = L.coefficients();

  // Form ranks
  const uint a_rank = a.rank();
  const uint L_rank = L.rank();

  // Collect pointers to dof maps
  std::vector<const GenericDofMap*> a_dof_maps;
  for (uint i = 0; i < a_rank; ++i)
    a_dof_maps.push_back(&a.function_space(i)->dofmap());

  std::vector<const GenericDofMap*> L_dof_maps;
  for (uint i = 0; i < L_rank; ++i)
    L_dof_maps.push_back(&L.function_space(i)->dofmap());

  // Vector to hold dof map for a cell
  std::vector<const std::vector<uint>* > a_dofs(a_rank);
  std::vector<const std::vector<uint>* > L_dofs(L_rank);

  // Iterate over facets
  Progress p("Assembling system (facet-wise)", mesh.num_facets());
  for (FacetIterator facet(mesh); !facet.end(); ++facet)
  {
    // Interior facet
    if (facet->num_entities(mesh.topology().dim()) == 2)
    {
      // Get cells incident with facet and update UFC objects
      Cell cell0(mesh, facet->entities(mesh.topology().dim())[0]);
      Cell cell1(mesh, facet->entities(mesh.topology().dim())[1]);

      // Get local facet index
      const uint local_facet0 = cell0.index(*facet);
      const uint local_facet1 = cell1.index(*facet);

      A_ufc.update(cell0, local_facet0, cell1, local_facet1);
      b_ufc.update(cell0, local_facet0, cell1, local_facet1);

      // Reset some temp data
      const uint a_macro_dim = (a_dof_maps[0]->cell_dofs(cell0.index()).size()
                               + a_dof_maps[0]->cell_dofs(cell1.index()).size())
                              *(a_dof_maps[1]->cell_dofs(cell0.index()).size()
                               + a_dof_maps[1]->cell_dofs(cell1.index()).size());
      for (uint i = 0; i < a_macro_dim; ++i)
        A_ufc.macro_A[i] = 0.0;

      const uint L_macro_dim = L_dof_maps[0]->cell_dofs(cell0.index()).size()
                             + L_dof_maps[0]->cell_dofs(cell1.index()).size();
      for (uint i = 0; i < L_macro_dim; ++i)
        b_ufc.macro_A[i] = 0.0;

      // Assemble interior facet and neighbouring cells if needed
      assemble_interior_facet(A, b, A_ufc, b_ufc, a, L, cell0, cell1, *facet, data, boundary_values);
    }
    else // Exterior facet
    {
      // Get mesh cell to which mesh facet belongs (pick first, there is only one)
      Cell cell(mesh, facet->entities(mesh.topology().dim())[0]);

      // Get local index of facet with respect to the cell
      const uint local_facet = cell.index(*facet);

      A_ufc.update(cell, local_facet);
      b_ufc.update(cell, local_facet);

      // Reset some temp data
      const uint a_local_dim = (a_dof_maps[0]->cell_dofs(cell.index()).size())
                              *(a_dof_maps[1]->cell_dofs(cell.index()).size());
      for (uint i = 0; i < a_local_dim; i++)
        A_ufc.A[i] = 0.0;

      const uint L_local_dim = L_dof_maps[0]->cell_dofs(cell.index()).size();
      for (uint i = 0; i < L_local_dim; ++i)
        b_ufc.A[i] = 0.0;

      // Initialize macro element matrix/vector to zero
      data.zero_cell();

      // Assemble exterior facet and attached cells if needed
      assemble_exterior_facet(A, b, A_ufc, b_ufc, a, L, cell, *facet, data, boundary_values);
    }
    p++;
  }
}
//-----------------------------------------------------------------------------
void SystemAssembler::compute_tensor_on_one_interior_facet(const Form& a,
            UFC& ufc, const Cell& cell0, const Cell& cell1, const Facet& facet,
            const MeshFunction<uint>* interior_facet_domains)
{
  const std::vector<const GenericFunction*> coefficients = a.coefficients();

  // Facet integral
  ufc::interior_facet_integral* interior_facet_integral = ufc.interior_facet_integrals[0].get();

  // Get integral for sub domain (if any)
  if (interior_facet_domains && interior_facet_domains->size() > 0)
  {
    const uint domain = (*interior_facet_domains)[facet];
    if (domain < ufc.form.num_interior_facet_integrals())
      interior_facet_integral = ufc.interior_facet_integrals[domain].get();
  }

  // Get local index of facet with respect to each cell
  const uint local_facet0 = cell0.index(facet);
  const uint local_facet1 = cell1.index(facet);

  // Update to current pair of cells
  ufc.update(cell0, local_facet0, cell1, local_facet1);

  interior_facet_integral->tabulate_tensor(ufc.macro_A.get(), ufc.macro_w,
                                           ufc.cell0, ufc.cell1,
                                           local_facet0, local_facet1);
}
//-----------------------------------------------------------------------------
inline void SystemAssembler::apply_bc(double* A, double* b,
                      const DirichletBC::Map& boundary_values,
                      const std::vector<const std::vector<uint>* >& global_dofs)
{
  // Get local dimensions
  const uint m = (global_dofs[0])->size();
  const uint n = (global_dofs[1])->size();

  for (uint i = 0; i < n; ++i)
  {
    const uint ii = (*global_dofs[1])[i];
    DirichletBC::Map::const_iterator bc_value = boundary_values.find(ii);

    if (bc_value != boundary_values.end())
    {
      b[i] = bc_value->second;
      for (uint k = 0; k < n; ++k)
        A[k + i*n] = 0.0;
      for (uint j = 0; j < m; ++j)
      {
        b[j] -= A[i + j*n]*bc_value->second;
        A[i + j*n] = 0.0;
      }
      A[i + i*n] = 1.0;
    }
  }
}
//-----------------------------------------------------------------------------
void SystemAssembler::assemble_interior_facet(GenericMatrix& A, GenericVector& b,
                              UFC& A_ufc, UFC& b_ufc,
                              const Form& a, const Form& L,
                              const Cell& cell0, const Cell& cell1,
                              const Facet& facet, Scratch& data,
                              const DirichletBC::Map& boundary_values)
{
  // Facet orientation not supported
  if (cell0.mesh().data().mesh_function("facet orientation"))
    error("Facet orientation not supported by system assembler.");

  // Tabulate dofs
  const std::vector<uint>& a0_dofs0 = a.function_space(0)->dofmap().cell_dofs(cell0.index());
  const std::vector<uint>& a1_dofs0 = a.function_space(1)->dofmap().cell_dofs(cell0.index());
  const std::vector<uint>& L_dofs0  = L.function_space(0)->dofmap().cell_dofs(cell0.index());

  const std::vector<uint>& a0_dofs1 = a.function_space(0)->dofmap().cell_dofs(cell1.index());
  const std::vector<uint>& a1_dofs1 = a.function_space(1)->dofmap().cell_dofs(cell1.index());
  const std::vector<uint>& L_dofs1  = L.function_space(0)->dofmap().cell_dofs(cell1.index());

  // Cell integrals
  const ufc::cell_integral* A_cell_integral = A_ufc.cell_integrals[0].get();
  const ufc::cell_integral* b_cell_integral = b_ufc.cell_integrals[0].get();

  // Compute facet contribution to A
  if (A_ufc.form.num_interior_facet_integrals() > 0)
    compute_tensor_on_one_interior_facet(a, A_ufc, cell0, cell1, facet, 0);

  // Compute facet contribution to
  if (b_ufc.form.num_interior_facet_integrals() > 0)
    compute_tensor_on_one_interior_facet(L, b_ufc, cell0, cell1, facet, 0);

  // Get local facet index
  const uint facet0 = cell0.index(facet);
  const uint facet1 = cell1.index(facet);

  // If we have local facet 0, compute cell contribution
  if (facet0 == 0)
  {
    if (A_ufc.form.num_cell_integrals() > 0)
    {
      A_ufc.update(cell0);
      A_cell_integral->tabulate_tensor(A_ufc.A.get(), A_ufc.w, A_ufc.cell);
      const uint nn = a0_dofs0.size();
      const uint mm = a1_dofs0.size();
      for (uint i = 0; i < mm; i++)
        for (uint j = 0; j < nn; j++)
          A_ufc.macro_A[2*i*nn+j] += A_ufc.A[i*nn+j];
    }

    if (b_ufc.form.num_cell_integrals() > 0)
    {
      b_ufc.update(cell0);
      b_cell_integral->tabulate_tensor(b_ufc.A.get(), b_ufc.w, b_ufc.cell);
      for (uint i = 0; i < L_dofs0.size(); i++)
        b_ufc.macro_A[i] += b_ufc.A[i];
    }
  }

  // If we have local facet 0, compute and add cell contribution
  if (facet1 == 0)
  {
    if (A_ufc.form.num_cell_integrals() > 0)
    {
      A_ufc.update(cell1);

      A_cell_integral->tabulate_tensor(A_ufc.A.get(), A_ufc.w, A_ufc.cell);
      const uint nn = a0_dofs1.size();
      const uint mm = a1_dofs1.size();
      for (uint i=0; i < mm; i++)
        for (uint j=0; j < nn; j++)
          A_ufc.macro_A[2*nn*mm + 2*i*nn + nn + j] += A_ufc.A[i*nn+j];
    }

    if (b_ufc.form.num_cell_integrals() > 0)
    {
      b_ufc.update(cell1);

      b_cell_integral->tabulate_tensor(b_ufc.A.get(), b_ufc.w, b_ufc.cell);
      for (uint i=0; i < L_dofs0.size(); i++)
        b_ufc.macro_A[L_dofs0.size() + i] += b_ufc.A[i];
    }
  }

  // Vector to hold dofs for cells
  std::vector<std::vector<uint> > a_macro_dofs(2);
  std::vector<std::vector<uint> > L_macro_dofs(1);

  // Resize dof vector
  a_macro_dofs[0].resize(a0_dofs0.size() + a0_dofs1.size());
  a_macro_dofs[1].resize(a0_dofs1.size() + a1_dofs1.size());
  L_macro_dofs[0].resize(L_dofs0.size() + L_dofs1.size());

  // Tabulate dofs for each dimension on macro element
  std::copy(a0_dofs0.begin(), a0_dofs0.end(), a_macro_dofs[0].begin());
  std::copy(a0_dofs1.begin(), a0_dofs1.end(), a_macro_dofs[0].begin() + a0_dofs0.size());

  std::copy(a1_dofs0.begin(), a1_dofs0.end(), a_macro_dofs[1].begin());
  std::copy(a1_dofs1.begin(), a1_dofs1.end(), a_macro_dofs[1].begin() + a1_dofs0.size());

  std::copy(L_dofs0.begin(), L_dofs0.end(), L_macro_dofs[0].begin());
  std::copy(L_dofs1.begin(), L_dofs1.end(), L_macro_dofs[0].begin() + L_dofs0.size());

  // Modify local matrix/element for Dirichlet boundary conditions
  std::vector<const std::vector<uint>* > _a_macro_dofs(2);
  _a_macro_dofs[0] = &a_macro_dofs[0];
  _a_macro_dofs[1] = &a_macro_dofs[1];

  apply_bc(A_ufc.macro_A.get(), b_ufc.macro_A.get(), boundary_values, _a_macro_dofs);

  // Add entries to global tensor
  A.add(A_ufc.macro_A.get(), a_macro_dofs);
  b.add(b_ufc.macro_A.get(), L_macro_dofs);
}
//-----------------------------------------------------------------------------
void SystemAssembler::assemble_exterior_facet(GenericMatrix& A, GenericVector& b,
                               UFC& A_ufc, UFC& b_ufc,
                               const Form& a, const Form& L,
                               const Cell& cell, const Facet& facet,
                               Scratch& data,
                               const DirichletBC::Map& boundary_values)
{
  const uint local_facet = cell.index(facet);

  if (A_ufc.form.num_exterior_facet_integrals() > 0 )
  {
    ufc::exterior_facet_integral* A_facet_integral = A_ufc.exterior_facet_integrals[0].get();

    A_ufc.update(cell, local_facet);
    A_facet_integral->tabulate_tensor(A_ufc.A.get(), A_ufc.w, A_ufc.cell, local_facet);
    for (uint i = 0; i < data.Ae.size(); i++)
      data.Ae[i] += A_ufc.A[i];
  }
  if (b_ufc.form.num_exterior_facet_integrals() > 0 )
  {
    const ufc::exterior_facet_integral* b_facet_integral = b_ufc.exterior_facet_integrals[0].get();

    b_ufc.update(cell, local_facet);
    b_facet_integral->tabulate_tensor(b_ufc.A.get(), b_ufc.w, b_ufc.cell, local_facet);
    for (uint i = 0; i < data.be.size(); i++)
      data.be[i] += b_ufc.A[i];
  }

  // If we have local facet 0, assemble cell integral
  if (local_facet == 0)
  {
    if (A_ufc.form.num_cell_integrals() > 0 )
    {
      const ufc::cell_integral* A_cell_integral = A_ufc.cell_integrals[0].get();

      A_ufc.update(cell);
      A_cell_integral->tabulate_tensor(A_ufc.A.get(), A_ufc.w, A_ufc.cell);
      for (uint i = 0; i < data.Ae.size(); i++)
        data.Ae[i] += A_ufc.A[i];
    }

    if (b_ufc.form.num_cell_integrals() > 0 )
    {
      const ufc::cell_integral* b_cell_integral = b_ufc.cell_integrals[0].get();

      b_ufc.update(cell);
      b_cell_integral->tabulate_tensor(b_ufc.A.get(), b_ufc.w, b_ufc.cell);
      for (uint i = 0; i < data.be.size(); i++)
        data.be[i] += b_ufc.A[i];
    }
  }

  // Tabulate dofs
  std::vector<const std::vector<uint>* > a_dofs(2);
  std::vector<const std::vector<uint>* > L_dofs(1);
  a_dofs[0] = &(a.function_space(0)->dofmap().cell_dofs(cell.index()));
  a_dofs[1] = &(a.function_space(1)->dofmap().cell_dofs(cell.index()));
  L_dofs[0] = &(L.function_space(0)->dofmap().cell_dofs(cell.index()));

  // Modify local matrix/element for Dirichlet boundary conditions
  apply_bc(&(data.Ae)[0], &(data.be)[0], boundary_values, a_dofs);

  // Add entries to global tensor
  A.add(&(data.Ae)[0], a_dofs);
  b.add(&(data.be)[0], L_dofs);
}
//-----------------------------------------------------------------------------
SystemAssembler::Scratch::Scratch(const Form& a, const Form& L)
{
  uint A_num_entries  = a.function_space(0)->dofmap().max_local_dimension();
  A_num_entries      *= a.function_space(1)->dofmap().max_local_dimension();
  Ae.resize(A_num_entries);

  be.resize(L.function_space(0)->dofmap().max_local_dimension());
}
//-----------------------------------------------------------------------------
SystemAssembler::Scratch::~Scratch()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
inline void SystemAssembler::Scratch::zero_cell()
{
  std::fill(Ae.begin(), Ae.end(), 0.0);
  std::fill(be.begin(), be.end(), 0.0);
}
//-----------------------------------------------------------------------------
