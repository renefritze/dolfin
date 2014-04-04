// Copyright (C) 2007-2011 Anders Logg
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Garth N. Wells 2007-2009
// Modified by Ola Skavhaug 2007-2009
// Modified by Kent-Andre Mardal 2008
// Modified by Joachim B Haga 2012
// Modified by Martin Alnaes 2013
//
// First added:  2007-01-17
// Last changed: 2013-09-19

#include <boost/scoped_ptr.hpp>

#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/Timer.h>
#include <dolfin/parameter/GlobalParameters.h>
#include <dolfin/la/GenericTensor.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/Facet.h>
#include <dolfin/mesh/MeshData.h>
#include <dolfin/mesh/MeshFunction.h>
#include <dolfin/mesh/SubDomain.h>
#include <dolfin/function/GenericFunction.h>
#include <dolfin/function/FunctionSpace.h>
#include "GenericDofMap.h"
#include "Form.h"
#include "UFC.h"
#include "FiniteElement.h"
#include "OpenMpAssembler.h"
#include "AssemblerBase.h"
#include "Assembler.h"

#include <dolfin/la/GenericMatrix.h>

using namespace dolfin;

//----------------------------------------------------------------------------
void Assembler::assemble(GenericTensor& A, const Form& a)
{
  // All assembler functions above end up calling this function, which
  // in turn calls the assembler functions below to assemble over
  // cells, exterior and interior facets.

  // Check whether we should call the multi-core assembler
  #ifdef HAS_OPENMP
  const std::size_t num_threads = parameters["num_threads"];
  if (num_threads > 0)
  {
    OpenMpAssembler assembler;
    assembler.add_values = add_values;
    assembler.finalize_tensor = finalize_tensor;
    assembler.keep_diagonal = keep_diagonal;
    assembler.assemble(A, a);
    return;
  }
  #endif

  // Get cell domains
  const MeshFunction<std::size_t>* cell_domains = a.cell_domains().get();

  // Get exterior facet domains
  const MeshFunction<std::size_t>* exterior_facet_domains
      = a.exterior_facet_domains().get();

  // Get interior facet domains
  const MeshFunction<std::size_t>* interior_facet_domains
      = a.interior_facet_domains().get();

  // Check form
  AssemblerBase::check(a);

  // Create data structure for local assembly data
  UFC ufc(a);

  // Update off-process coefficients
  const std::vector<std::shared_ptr<const GenericFunction> >
    coefficients = a.coefficients();
  for (std::size_t i = 0; i < coefficients.size(); ++i)
    coefficients[i]->update();

  // Initialize global tensor
  init_global_tensor(A, a);

  // Assemble over cells
  assemble_cells(A, a, ufc, cell_domains, 0);

  // Assemble over exterior facets
  assemble_exterior_facets(A, a, ufc, exterior_facet_domains, 0);

  // Assemble over interior facets
  assemble_interior_facets(A, a, ufc, interior_facet_domains, 0);

  // Finalize assembly of global tensor
  if (finalize_tensor)
    A.apply("add");
}
//-----------------------------------------------------------------------------
void Assembler::assemble_cells(GenericTensor& A,
                               const Form& a,
                               UFC& ufc,
                               const MeshFunction<std::size_t>* domains,
                               std::vector<double>* values)
{
  // Skip assembly if there are no cell integrals
  if (!ufc.form.has_cell_integrals())
    return;

  // Set timer
  Timer timer("Assemble cells");

  // Extract mesh
  const Mesh& mesh = a.mesh();

  // Form rank
  const std::size_t form_rank = ufc.form.rank();

  // Collect pointers to dof maps
  std::vector<const GenericDofMap*> dofmaps;
  for (std::size_t i = 0; i < form_rank; ++i)
    dofmaps.push_back(a.function_space(i)->dofmap().get());

  // Vector to hold dof map for a cell
  std::vector<const std::vector<dolfin::la_index>* > dofs(form_rank);

  // Cell integral
  ufc::cell_integral* integral = ufc.default_cell_integral.get();

  // Check whether integral is domain-dependent
  bool use_domains = domains && !domains->empty();

  // Assemble over cells
  ufc::cell ufc_cell;
  std::vector<double> vertex_coordinates;
  Progress p(AssemblerBase::progress_message(A.rank(), "cells"),
             mesh.num_cells());
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Get integral for sub domain (if any)
    if (use_domains)
      integral = ufc.get_cell_integral((*domains)[*cell]);

    // Skip if no integral on current domain
    if (!integral)
      continue;

    // Update to current cell
    cell->get_cell_data(ufc_cell);
    cell->get_vertex_coordinates(vertex_coordinates);
    ufc.update(*cell, vertex_coordinates, ufc_cell);

    // Get local-to-global dof maps for cell
    bool empty_dofmap = false;
    for (std::size_t i = 0; i < form_rank; ++i)
    {
      dofs[i] = &(dofmaps[i]->cell_dofs(cell->index()));
      empty_dofmap = empty_dofmap || dofs[i]->size() == 0;
    }

    // Skip if at least one dofmap is empty
    if (empty_dofmap)
      continue;

    // Tabulate cell tensor
    integral->tabulate_tensor(ufc.A.data(), ufc.w(),
                              vertex_coordinates.data(),
                              ufc_cell.orientation);

    // Add entries to global tensor. Either store values cell-by-cell
    // (currently only available for functionals)
    if (values && ufc.form.rank() == 0)
      (*values)[cell->index()] = ufc.A[0];
    else
      add_to_global_tensor(A, ufc.A, dofs);

    p++;
  }
}
//-----------------------------------------------------------------------------
void Assembler::assemble_exterior_facets(GenericTensor& A,
                                         const Form& a,
                                         UFC& ufc,
                                         const MeshFunction<std::size_t>* domains,
                                         std::vector<double>* values)
{
  // Skip assembly if there are no exterior facet integrals
  if (!ufc.form.has_exterior_facet_integrals())
    return;

  // Set timer
  Timer timer("Assemble exterior facets");

  // Extract mesh
  const Mesh& mesh = a.mesh();

  // Form rank
  const std::size_t form_rank = ufc.form.rank();

  // Collect pointers to dof maps
  std::vector<const GenericDofMap*> dofmaps;
  for (std::size_t i = 0; i < form_rank; ++i)
    dofmaps.push_back(a.function_space(i)->dofmap().get());

  // Vector to hold dof map for a cell
  std::vector<const std::vector<dolfin::la_index>* > dofs(form_rank);

  // Exterior facet integral
  const ufc::exterior_facet_integral* integral
    = ufc.default_exterior_facet_integral.get();

  // Check whether integral is domain-dependent
  bool use_domains = domains && !domains->empty();

  // Compute facets and facet - cell connectivity if not already computed
  const std::size_t D = mesh.topology().dim();
  mesh.init(D - 1);
  mesh.init(D - 1, D);
  dolfin_assert(mesh.ordered());

  // Assemble over exterior facets (the cells of the boundary)
  ufc::cell ufc_cell;
  std::vector<double> vertex_coordinates;
  Progress p(AssemblerBase::progress_message(A.rank(), "exterior facets"),
             mesh.num_facets());
  for (FacetIterator facet(mesh); !facet.end(); ++facet)
  {
    // Only consider exterior facets
    if (!facet->exterior())
    {
      p++;
      continue;
    }

    // Get integral for sub domain (if any)
    if (use_domains)
      integral = ufc.get_exterior_facet_integral((*domains)[*facet]);

    // Skip integral if zero
    if (!integral)
      continue;

    // Get mesh cell to which mesh facet belongs (pick first, there is only one)
    dolfin_assert(facet->num_entities(D) == 1);
    Cell mesh_cell(mesh, facet->entities(D)[0]);

    // Get local index of facet with respect to the cell
    const std::size_t local_facet = mesh_cell.index(*facet);

    // Update UFC cell
    mesh_cell.get_cell_data(ufc_cell, local_facet);
    mesh_cell.get_vertex_coordinates(vertex_coordinates);

    // Update UFC object
    ufc.update(mesh_cell, vertex_coordinates, ufc_cell);

    // Get local-to-global dof maps for cell
    for (std::size_t i = 0; i < form_rank; ++i)
      dofs[i] = &(dofmaps[i]->cell_dofs(mesh_cell.index()));

    // Tabulate exterior facet tensor
    integral->tabulate_tensor(ufc.A.data(),
                              ufc.w(),
                              vertex_coordinates.data(),
                              local_facet,
                              ufc_cell.orientation);

    // Add entries to global tensor
    add_to_global_tensor(A, ufc.A, dofs);

    p++;
  }
}
//-----------------------------------------------------------------------------
void Assembler::assemble_interior_facets(GenericTensor& A, const Form& a,
                                      UFC& ufc,
                                      const MeshFunction<std::size_t>* domains,
                                      std::vector<double>* values)
{
  // Skip assembly if there are no interior facet integrals
  if (!ufc.form.has_interior_facet_integrals())
    return;

  not_working_in_parallel("Assembly over interior facets");

  // Set timer
  Timer timer("Assemble interior facets");

  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();

  // Form rank
  const std::size_t form_rank = ufc.form.rank();

  // Collect pointers to dof maps
  std::vector<const GenericDofMap*> dofmaps;
  for (std::size_t i = 0; i < form_rank; ++i)
    dofmaps.push_back(a.function_space(i)->dofmap().get());

  // Vector to hold dofs for cells, and a vector holding pointers to same
  std::vector<std::vector<dolfin::la_index> > macro_dofs(form_rank);
  std::vector<const std::vector<dolfin::la_index>* > macro_dof_ptrs(form_rank);
  for (std::size_t i = 0; i < form_rank; i++)
  {
    macro_dof_ptrs[i] = &macro_dofs[i];
  }

  // Interior facet integral
  const ufc::interior_facet_integral* integral
    = ufc.default_interior_facet_integral.get();

  // Check whether integral is domain-dependent
  bool use_domains = domains && !domains->empty();

  // Compute facets and facet - cell connectivity if not already computed
  const std::size_t D = mesh.topology().dim();
  mesh.init(D - 1);
  mesh.init(D - 1, D);
  dolfin_assert(mesh.ordered());

  // Get interior facet directions (if any)
  const std::vector<std::size_t>* facet_orientation = NULL;
  if (mesh.data().exists("facet_orientation", D - 1))
    facet_orientation = &(mesh.data().array("facet_orientation", D - 1));

  if (facet_orientation && facet_orientation->size() != mesh.num_facets())
  {
    dolfin_error("Assembler.cpp",
                 "assemble form over interior facets",
                 "Expecting facet orientation to be defined on facets");
  }

  // Assemble over interior facets (the facets of the mesh)
  ufc::cell ufc_cell[2];
  std::vector<double> vertex_coordinates[2];
  Progress p(AssemblerBase::progress_message(A.rank(), "interior facets"),
             mesh.num_facets());
  for (FacetIterator facet(mesh); !facet.end(); ++facet)
  {
    // Only consider interior facets
    if (facet->exterior())
    {
      p++;
      continue;
    }

    // Get integral for sub domain (if any)
    if (use_domains)
      integral = ufc.get_interior_facet_integral((*domains)[*facet]);

    // Skip integral if zero
    if (!integral)
      continue;

    // Get cells incident with facet
    std::pair<const Cell, const Cell>
      cells = facet->adjacent_cells(facet_orientation);
    const Cell& cell0 = cells.first;
    const Cell& cell1 = cells.second;

    // Get local index of facet with respect to each cell
    std::size_t local_facet0 = cell0.index(*facet);
    std::size_t local_facet1 = cell1.index(*facet);

    // Update to current pair of cells
    cell0.get_cell_data(ufc_cell[0], local_facet0);
    cell0.get_vertex_coordinates(vertex_coordinates[0]);
    cell1.get_cell_data(ufc_cell[1], local_facet1);
    cell1.get_vertex_coordinates(vertex_coordinates[1]);

    ufc.update(cell0, vertex_coordinates[0], ufc_cell[0],
               cell1, vertex_coordinates[1], ufc_cell[1]);

    // Tabulate dofs for each dimension on macro element
    for (std::size_t i = 0; i < form_rank; i++)
    {
      // Get dofs for each cell
      const std::vector<dolfin::la_index>& cell_dofs0
        = dofmaps[i]->cell_dofs(cell0.index());
      const std::vector<dolfin::la_index>& cell_dofs1
        = dofmaps[i]->cell_dofs(cell1.index());

      // Create space in macro dof vector
      macro_dofs[i].resize(cell_dofs0.size() + cell_dofs1.size());

      // Copy cell dofs into macro dof vector
      std::copy(cell_dofs0.begin(), cell_dofs0.end(),
                macro_dofs[i].begin());
      std::copy(cell_dofs1.begin(), cell_dofs1.end(),
                macro_dofs[i].begin() + cell_dofs0.size());
    }

    // Tabulate interior facet tensor on macro element
    integral->tabulate_tensor(ufc.macro_A.data(),
                              ufc.macro_w(),
                              vertex_coordinates[0].data(),
                              vertex_coordinates[1].data(),
                              local_facet0,
                              local_facet1,
                              ufc_cell[0].orientation,
                              ufc_cell[1].orientation);

    // Add entries to global tensor
    add_to_global_tensor(A, ufc.macro_A, macro_dof_ptrs);

    p++;
  }
}
//-----------------------------------------------------------------------------
void Assembler::add_to_global_tensor(GenericTensor& A,
                                     std::vector<double>& cell_tensor,
                                     std::vector<const std::vector<dolfin::la_index>* >& dofs)
{
  A.add(&cell_tensor[0], dofs);
}
//-----------------------------------------------------------------------------
