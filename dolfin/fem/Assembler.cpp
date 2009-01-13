// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2007,-2008
// Modified by Ola Skavhaug, 2007-2008
// Modified by Kent-Andre Mardal, 2008
//
// First added:  2007-01-17
// Last changed: 2009-01-06

#include <ufc.h>
#include <dolfin/main/MPI.h>
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

//-----------------------------------------------------------------------------
void Assembler::assemble(GenericTensor& A,
                         const Form& a,
                         bool reset_tensor)
{
  assemble(A, a, 0, 0, 0, reset_tensor);
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
void Assembler::assemble_system(GenericMatrix& A,
                                GenericVector& b,
                                const Form& a,
                                const Form& L,
                                const DirichletBC& bc,
                                bool reset_tensors)
{
  std::vector<const DirichletBC*> bcs;
  bcs.push_back(&bc);
  assemble_system(A, b, a, L, bcs, 0, 0, 0, 0, reset_tensors);
}
//-----------------------------------------------------------------------------
void Assembler::assemble_system(GenericMatrix& A,
                                GenericVector& b,
                                const Form& a,
                                const Form& L,
                                std::vector<const DirichletBC*>& bcs,
                                bool reset_tensors)
{
  assemble_system(A, b, a, L, bcs, 0, 0, 0, 0, reset_tensors);
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
  Progress p(progress_message(A.rank(), "cells"), mesh.numCells());
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // FIXME will move this check into a separate function. 
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
      A.add(ufc.A, ufc.local_dimensions, ufc.dofs);

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
  MeshFunction<uint>* cell_map = boundary.data().meshFunction("cell map");
  dolfin_assert(cell_map);

  // Assemble over exterior facets (the cells of the boundary)
  Progress p(progress_message(A.rank(), "exterior facets"), boundary.numCells());
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
    dolfin_assert(mesh_facet.numEntities(mesh.topology().dim()) == 1);
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
  dolfin_assert(mesh.ordered());
  // Assemble over interior facets (the facets of the mesh)
  Progress p(progress_message(A.rank(), "interior facets"), mesh.numFacets());
  for (FacetIterator facet(mesh); !facet.end(); ++facet)
  {
    // Check if we have an interior facet
    if ( facet->numEntities(mesh.topology().dim()) != 2 )
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
    for (uint i = 0; i < ufc.form.rank(); i++)
    {
      const uint offset = ufc.local_dimensions[i];
      a.function_space(i).dofmap().tabulate_dofs(ufc.macro_dofs[i],          ufc.cell0, cell0.index());
      a.function_space(i).dofmap().tabulate_dofs(ufc.macro_dofs[i] + offset, ufc.cell1, cell1.index());
    }

    // Tabulate exterior interior facet tensor on macro element
    integral->tabulate_tensor(ufc.macro_A, ufc.macro_w, ufc.cell0, ufc.cell1, facet0, facet1);

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
    dolfin_assert(element);
    if (mesh.type().cellType() == CellType::interval && element->cell_shape() != ufc::interval)
      error("Mesh cell type (intervals) does not match cell type of form.");
    if (mesh.type().cellType() == CellType::triangle && element->cell_shape() != ufc::triangle)
      error("Mesh cell type (triangles) does not match cell type of form.");
    if (mesh.type().cellType() == CellType::tetrahedron && element->cell_shape() != ufc::tetrahedron)
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
      for(uint i=0; i < a.rank(); ++i) 
        dof_maps.push_back(&(a.function_space(i).dofmap()));
//      SparsityPatternBuilder::build(*sparsity_pattern, a.mesh(), ufc, dof_maps);
      SparsityPatternBuilder::build(*sparsity_pattern, ufc, a);
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
void Assembler::assemble_system(GenericMatrix& A,
                                GenericVector& b,
                                const Form& a,
                                const Form& L,
                                std::vector<const DirichletBC*>& bcs,
                                const MeshFunction<uint>* cell_domains,
                                const MeshFunction<uint>* exterior_facet_domains,
                                const MeshFunction<uint>* interior_facet_domains,
                                const GenericVector* x0,
                                bool reset_tensors)
{
  Timer timer("Assemble system");
  message("Assembling linear system and applying boundary conditions...");

  // Note the importance of treating empty mesh functions as null pointers
  // for the PyDOLFIN interface.

  // FIXME: 1. Need consistency check between a and L
  // FIXME: 2. We assume that we get a bilinear and linear form, need to check this
  // FIXME: 3. Some things can be simplified since we know it's a matrix and a vector

  // Check arguments
  check(a);
  check(L);

  // Extract mesh and coefficients
  const Mesh& mesh = a.mesh();
  const std::vector<const Function*>& A_coefficients = a.coefficients();
  const std::vector<const Function*>& b_coefficients = L.coefficients();

  // Create data structure for local assembly data
  UFC A_ufc(a);
  UFC b_ufc(L);

  // Initialize global tensor
  init_global_tensor(A, a, A_ufc, reset_tensors);
  init_global_tensor(b, L, b_ufc, reset_tensors);

  // Pointers to element matrix and vector
  double* Ae = 0; 
  double* be = 0; 

  // Get boundary values (global) 
  const uint N = a.function_space(1).dofmap().global_dimension();  
  uint* indicators = new uint[N];
  double* g = new double[N];
  for (uint i = 0; i < N; i++) 
  {
    indicators[i] = 0; 
    g[i] = 0.0; 
  }
  for(uint i = 0; i < bcs.size(); ++i)
    bcs[i]->get_bc(indicators, g);

  // Modify boundary values for incremental (typically nonlinear) problems
  if (x0)
  {
    warning("Symmetric application of Dirichlet boundary conditions for incremental problems is untested.");
    dolfin_assert( x0->size() == N);
    double* x0_values = new double[N];
    x0->get(x0_values);
    for (uint i = 0; i < N; i++)
      g[i] = x0_values[i] - g[i];
    delete [] x0_values;
  }

  // If there are no interior facet integrals
  if (A_ufc.form.num_interior_facet_integrals() == 0 && b_ufc.form.num_interior_facet_integrals() == 0) 
  {
    // Allocate memory for Ae and be 
    uint A_num_entries = 1;
    for (uint i = 0; i < a.rank(); i++)
      A_num_entries *= a.function_space(i).dofmap().local_dimension();
    Ae = new double[A_num_entries];

    uint b_num_entries = 1;
    for (uint i = 0; i < L.rank(); i++)
      b_num_entries *= L.function_space(i).dofmap().local_dimension();
    be = new double[b_num_entries];

    for (CellIterator cell(mesh); !cell.end(); ++cell) 
    {
      for (uint i = 0; i < A_num_entries; i++) 
        Ae[i] = 0.0; 
      for (uint i = 0; i < b_num_entries; i++) 
        be[i] = 0.0; 

      // Update to current cell
      A_ufc.update(*cell);

      // Interpolate coefficients on cell
      for (uint i = 0; i < A_coefficients.size(); i++)
        A_coefficients[i]->interpolate(A_ufc.w[i], A_ufc.cell, cell->index());

      // Tabulate dofs for each dimension
      for (uint i = 0; i < A_ufc.form.rank(); i++)
        a.function_space(i).dofmap().tabulate_dofs(A_ufc.dofs[i], A_ufc.cell, cell->index());

      // Update to current cell
      b_ufc.update(*cell);

      // Interpolate coefficients on cell
      for (uint i = 0; i < b_coefficients.size(); i++)
        b_coefficients[i]->interpolate(b_ufc.w[i], b_ufc.cell, cell->index());

      // Tabulate dofs for each dimension
      for (uint i = 0; i < b_ufc.form.rank(); i++)
        L.function_space(i).dofmap().tabulate_dofs(b_ufc.dofs[i], b_ufc.cell, cell->index());

      // Compute cell integral for A 
      if (A_ufc.form.num_cell_integrals() > 0) 
      {
        ufc::cell_integral* A_cell_integral =  A_ufc.cell_integrals[0];
        if (cell_domains && cell_domains->size() > 0)
        {
          const uint cell_domain = (*cell_domains)(*cell);
          if (cell_domain < A_ufc.form.num_cell_integrals()) 
            A_cell_integral = A_ufc.cell_integrals[cell_domain];
          else 
            continue;
        }
        // Tabulate cell tensor
        A_cell_integral->tabulate_tensor(A_ufc.A, A_ufc.w, A_ufc.cell);
        for (uint i=0; i<A_num_entries; i++) 
          Ae[i] += A_ufc.A[i]; 
      }

      // Compute cell integral for b 
      if (b_ufc.form.num_cell_integrals() > 0) 
      {
        ufc::cell_integral* b_cell_integral = b_ufc.cell_integrals[0];
        if (cell_domains && cell_domains->size() > 0) 
        {
          const uint cell_domain = (*cell_domains)(*cell);
          if (cell_domain < b_ufc.form.num_cell_integrals()) 
            b_cell_integral = b_ufc.cell_integrals[cell_domain];
          else 
            continue;
        }
        // Tabulate cell tensor
        b_cell_integral->tabulate_tensor(b_ufc.A, b_ufc.w, b_ufc.cell);
        for (uint i=0; i<b_num_entries; i++) 
          be[i] += b_ufc.A[i]; 
      }

      // Compute exterior facet integral
      if (A_ufc.form.num_exterior_facet_integrals() > 0 || b_ufc.form.num_exterior_facet_integrals() > 0) 
      {
        const uint D = mesh.topology().dim(); 

        if (A_ufc.form.num_exterior_facet_integrals() > 0) 
        {
          for (FacetIterator facet(*cell); !facet.end(); ++facet)
          {
            if (facet->numEntities(D) != 2) 
            {
              ufc::exterior_facet_integral* A_integral = A_ufc.exterior_facet_integrals[0]; 
              if (exterior_facet_domains && exterior_facet_domains->size() > 0)
              {
                const uint exterior_facet_domain= (*exterior_facet_domains)(*facet);
                if (exterior_facet_domain < A_ufc.form.num_exterior_facet_integrals())
                  A_integral = A_ufc.exterior_facet_integrals[exterior_facet_domain];
                else
                  continue;
              }
              const uint local_facet = cell->index(*facet);
              for (uint i = 0; i < A_coefficients.size(); i++)
                A_coefficients[i]->interpolate(A_ufc.w[i], A_ufc.cell, cell->index(), local_facet);

              A_integral->tabulate_tensor(A_ufc.A, A_ufc.w, A_ufc.cell, local_facet);
              for (uint i=0; i<A_num_entries; i++) 
                Ae[i] += A_ufc.A[i]; 
            }
          }
        }

        if (b_ufc.form.num_exterior_facet_integrals() > 0) 
        {
          for (FacetIterator facet(*cell); !facet.end(); ++facet)
          {
            if (facet->numEntities(D) != 2) 
            {
              ufc::exterior_facet_integral* b_integral = b_ufc.exterior_facet_integrals[0]; 
              if (exterior_facet_domains && exterior_facet_domains->size() > 0)
              {
                const uint exterior_facet_domain= (*exterior_facet_domains)(*facet);
                if (exterior_facet_domain < b_ufc.form.num_exterior_facet_integrals())
                  b_integral = b_ufc.exterior_facet_integrals[exterior_facet_domain];
                else
                  continue;
              }
              const uint local_facet = cell->index(*facet);
              for (uint i = 0; i < b_coefficients.size(); i++)
                b_coefficients[i]->interpolate(b_ufc.w[i], b_ufc.cell, cell->index(), local_facet);

              b_integral->tabulate_tensor(b_ufc.A, b_ufc.w, b_ufc.cell, local_facet);
              for (uint i=0; i<b_num_entries; i++) 
                be[i] += b_ufc.A[i]; 
            }
          }
        }
      }

      // Enforce Dirichlet boundary conditions
      uint m = A_ufc.local_dimensions[0]; 
      uint n = A_ufc.local_dimensions[1]; 
      for (uint i=0; i<n; i++) 
      {  
        uint ii = A_ufc.dofs[1][i]; 
        if (indicators[ii] > 0) 
        {  
          be[i] = g[ii]; 
          for (uint k=0; k<n; k++) 
            Ae[k+i*n] = 0.0; 
          for (uint j=0; j<m; j++) 
          {
            be[j] -= Ae[i+j*n]*g[ii]; 
            Ae[i+j*n] = 0.0; 
          }
          Ae[i+i*n] = 1.0; 
        }
      }

      // Add entries to global tensor
      A.add(Ae, A_ufc.local_dimensions, A_ufc.dofs);
      b.add(be, b_ufc.local_dimensions, b_ufc.dofs);
    }
  }


  if (A_ufc.form.num_interior_facet_integrals() > 0 || b_ufc.form.num_interior_facet_integrals() > 0) 
  {
    // Create temporary storage for Ae, Ae_macro 
    uint A_num_entries = 1;
    for (uint i = 0; i < a.rank(); i++)
      A_num_entries *= a.function_space(i).dofmap().local_dimension();
    uint A_macro_num_entries = 4*A_num_entries; 
    Ae = new double[A_num_entries];
    double* Ae_macro = new double[A_macro_num_entries]; 

    // Create temporay storage for be, be_macro 
    uint b_num_entries = 1;
    for (uint i = 0; i < L.rank(); i++)
      b_num_entries *= L.function_space(i).dofmap().local_dimension();
    uint b_macro_num_entries = b_num_entries*2; 
    be = new double[b_num_entries];
    double* be_macro = new double[b_macro_num_entries]; 

    for (FacetIterator facet(mesh); !facet.end(); ++facet)
    {
      // Check if we have an interior facet
      if ( facet->numEntities(mesh.topology().dim()) == 2 ) 
      {
        for (uint i = 0; i < A_macro_num_entries; i++) 
          Ae_macro[i] = 0.0; 
        for (uint i = 0; i < b_macro_num_entries; i++) 
          be_macro[i] = 0.0; 

        // Get cells incident with facet
        Cell cell0(mesh, facet->entities(mesh.topology().dim())[0]);
        Cell cell1(mesh, facet->entities(mesh.topology().dim())[1]);

        // Update to current pair of cells
        A_ufc.update(cell0, cell1);
        b_ufc.update(cell0, cell1);

        // Get local index of facet with respect to each cell
        uint facet0 = cell0.index(*facet);
        uint facet1 = cell1.index(*facet);

        // Tabulate dofs for each dimension on macro element
        for (uint i = 0; i < A_ufc.form.rank(); i++)
        {
          const uint offset = A_ufc.local_dimensions[i];
          a.function_space(i).dofmap().tabulate_dofs(A_ufc.macro_dofs[i],
                                                     A_ufc.cell0, cell0.index());
          a.function_space(i).dofmap().tabulate_dofs(A_ufc.macro_dofs[i] + offset,
                                                     A_ufc.cell1, cell1.index());
        }

        // Tabulate dofs for each dimension on macro element
        for (uint i = 0; i < b_ufc.form.rank(); i++)
        {
          const uint offset = b_ufc.local_dimensions[i];
          L.function_space(i).dofmap().tabulate_dofs(b_ufc.macro_dofs[i],
                                                     b_ufc.cell0, cell0.index());
          L.function_space(i).dofmap().tabulate_dofs(b_ufc.macro_dofs[i] + offset,
                                                     b_ufc.cell1, cell1.index());
        }

        if ( A_ufc.form.num_interior_facet_integrals() ) 
        {  
          ufc::interior_facet_integral* interior_facet_integral = A_ufc.interior_facet_integrals[0];

          // Get integral for sub domain (if any)
          if (interior_facet_domains && interior_facet_domains->size() > 0)
          {
            const uint domain = (*interior_facet_domains)(*facet);
            if (domain < A_ufc.form.num_interior_facet_integrals())
              interior_facet_integral = A_ufc.interior_facet_integrals[domain];
            else
              continue;
          }


          // Interpolate coefficients on cell
          for (uint i = 0; i < A_coefficients.size(); i++)
          {
            const uint offset = A_ufc.coefficient_elements[i]->space_dimension();
            A_coefficients[i]->interpolate(A_ufc.macro_w[i],          A_ufc.cell0, cell0.index(), facet0);
            A_coefficients[i]->interpolate(A_ufc.macro_w[i] + offset, A_ufc.cell1, cell1.index(), facet1);
          }


          // Get integral for sub domain (if any)
          if (interior_facet_domains && interior_facet_domains->size() > 0)
          {
            const uint domain = (*interior_facet_domains)(*facet);
            if (domain < A_ufc.form.num_interior_facet_integrals())
              interior_facet_integral = A_ufc.interior_facet_integrals[domain];
            else
              continue;
          }

          // Tabulate interior facet tensor on macro element
          interior_facet_integral->tabulate_tensor(A_ufc.macro_A, A_ufc.macro_w, 
                                      A_ufc.cell0, A_ufc.cell1, facet0, facet1);
          for (uint i=0; i<A_macro_num_entries; i++) 
            Ae_macro[i] += A_ufc.macro_A[i]; 
        }

        if ( b_ufc.form.num_interior_facet_integrals() > 0 ) 
        {  
          ufc::interior_facet_integral* interior_facet_integral = b_ufc.interior_facet_integrals[0];

          b_ufc.update(cell0, cell1);

          // Interpolate coefficients on cell
          for (uint i = 0; i < b_coefficients.size(); i++)
          {
            const uint offset = b_ufc.coefficient_elements[i]->space_dimension();
            b_coefficients[i]->interpolate(b_ufc.macro_w[i],          b_ufc.cell0, cell0.index(), facet0);
            b_coefficients[i]->interpolate(b_ufc.macro_w[i] + offset, b_ufc.cell1, cell1.index(), facet1);
          }

          // Get integral for sub domain (if any)
          if (interior_facet_domains && interior_facet_domains->size() > 0)
          {
            const uint domain = (*interior_facet_domains)(*facet);
            if (domain < b_ufc.form.num_interior_facet_integrals())
              interior_facet_integral = b_ufc.interior_facet_integrals[domain];
            else
              continue;
          }

          interior_facet_integral->tabulate_tensor(b_ufc.macro_A, b_ufc.macro_w, b_ufc.cell0, 
                                                   b_ufc.cell1, facet0, facet1);

          for (uint i=0; i<b_macro_num_entries; i++) 
            be_macro[i] += b_ufc.macro_A[i]; 
        }

        if (facet0 == 0) 
        {
          if (A_ufc.form.num_cell_integrals() > 0) 
          {
            A_ufc.update(cell0);

            // Interpolate coefficients on cell
            for (uint i = 0; i < A_coefficients.size(); i++)
              A_coefficients[i]->interpolate(A_ufc.w[i], A_ufc.cell, cell0.index());

            // Tabulate dofs for each dimension
            for (uint i = 0; i < A_ufc.form.rank(); i++)
              a.function_space(i).dofmap().tabulate_dofs(A_ufc.dofs[i], A_ufc.cell, cell0.index());

            ufc::cell_integral* A_cell_integral =  A_ufc.cell_integrals[0];

            if (cell_domains && cell_domains->size() > 0)
            {
              const uint cell_domain = (*cell_domains)(cell0);
              if (cell_domain < A_ufc.form.num_cell_integrals()) 
                A_cell_integral = A_ufc.cell_integrals[cell_domain];
              else 
                continue;
            }
            // Tabulate cell tensor
            A_cell_integral->tabulate_tensor(A_ufc.A, A_ufc.w, A_ufc.cell0);

            uint nn = A_ufc.local_dimensions[0]; 
            uint mm = A_ufc.local_dimensions[1];
            for (uint i=0; i<mm; i++) 
              for (uint j=0; j<nn; j++) 
                Ae_macro[2*i*nn+j] += A_ufc.A[i*nn+j]; 
          }

          if (b_ufc.form.num_cell_integrals() > 0) 
          {
            // Update to cell0 
            b_ufc.update(cell0);

            // Interpolate coefficients on cell
            for (uint i = 0; i < b_coefficients.size(); i++)
              b_coefficients[i]->interpolate(b_ufc.w[i], b_ufc.cell, cell0.index());

            // Tabulate dofs for each dimension
            for (uint i = 0; i < b_ufc.form.rank(); i++)
              L.function_space(i).dofmap().tabulate_dofs(b_ufc.dofs[i], b_ufc.cell, cell0.index());

            ufc::cell_integral* b_cell_integral =  b_ufc.cell_integrals[0];

            if (cell_domains && cell_domains->size() > 0)
            {
              const uint cell_domain = (*cell_domains)(cell0);
              if (cell_domain < b_ufc.form.num_cell_integrals())
                b_cell_integral = b_ufc.cell_integrals[cell_domain];
              else 
                continue;
            }
            // Tabulate cell tensor
            b_cell_integral->tabulate_tensor(b_ufc.A, b_ufc.w, b_ufc.cell0);
            for (uint i=0; i<b_num_entries; i++) 
              be_macro[i] += b_ufc.A[i]; 
          }
        }

        if (facet1 == 0) 
        { 
          if (A_ufc.form.num_cell_integrals() > 0) 
          {
            A_ufc.update(cell1);

            // Interpolate coefficients on cell
            for (uint i = 0; i < A_coefficients.size(); i++)
              A_coefficients[i]->interpolate(A_ufc.w[i], A_ufc.cell, cell1.index());

            // Tabulate dofs for each dimension
            for (uint i = 0; i < A_ufc.form.rank(); i++)
              a.function_space(i).dofmap().tabulate_dofs(A_ufc.dofs[i], A_ufc.cell, cell1.index());

            ufc::cell_integral* A_cell_integral =  A_ufc.cell_integrals[0];

            if (cell_domains && cell_domains->size() > 0)
            {
              const uint cell_domain = (*cell_domains)(cell1);
              if (cell_domain < A_ufc.form.num_cell_integrals()) 
                A_cell_integral = A_ufc.cell_integrals[cell_domain];
              else 
                continue;
            }

            // Tabulate cell tensor 
            A_cell_integral->tabulate_tensor(A_ufc.A, A_ufc.w, A_ufc.cell1);
            uint nn = A_ufc.local_dimensions[0]; 
            uint mm = A_ufc.local_dimensions[1];
            for (uint i=0; i<mm; i++)
              for (uint j=0; j<nn; j++)
                Ae_macro[2*nn*mm + 2*i*nn + nn + j] += A_ufc.A[i*nn+j]; 
          }

          if (b_ufc.form.num_cell_integrals() > 0) 
          {
            b_ufc.update(cell1);

            // Interpolate coefficients on cell
            for (uint i = 0; i < b_coefficients.size(); i++)
              b_coefficients[i]->interpolate(b_ufc.w[i], b_ufc.cell, cell1.index());

            // Tabulate dofs for each dimension
            for (uint i = 0; i < b_ufc.form.rank(); i++)
              L.function_space(i).dofmap().tabulate_dofs(b_ufc.dofs[i], b_ufc.cell, cell1.index());

            ufc::cell_integral* b_cell_integral =  b_ufc.cell_integrals[0];

            if (cell_domains && cell_domains->size() > 0)
            {
              const uint cell_domain = (*cell_domains)(cell1);
              if (cell_domain < b_ufc.form.num_cell_integrals())
                b_cell_integral = b_ufc.cell_integrals[cell_domain];
              else 
                continue;
            }
            // Tabulate cell tensor
            b_cell_integral->tabulate_tensor(b_ufc.A, b_ufc.w, b_ufc.cell1);
            for (uint i=0; i<b_num_entries; i++) 
              be_macro[b_num_entries + i] += b_ufc.A[i]; 
          }
        }
        // enforce BC  ---------------------------------------

        const uint m = A_ufc.macro_local_dimensions[0]; 
        const uint n = A_ufc.macro_local_dimensions[1]; 

        for (uint i=0; i<n; i++) 
        {  
          uint ii = A_ufc.macro_dofs[1][i]; 
          if (indicators[ii] > 0) 
          {  
            be[i] = g[ii]; 
            for (uint k=0; k<n; k++) 
              Ae_macro[k+i*n] = 0.0; 
            for (uint j=0; j<m; j++) 
            {
              be_macro[j] -= Ae_macro[i+j*n]*g[ii]; 
              Ae_macro[i+j*n] = 0.0; 
            }
            Ae_macro[i+i*n] = 1.0; 
          }
        }
        // enforce BC done  ------------------------------------------

        // Add entries to global tensor
        A.add(Ae_macro, A_ufc.macro_local_dimensions, A_ufc.macro_dofs);
        b.add(be_macro, b_ufc.macro_local_dimensions, b_ufc.macro_dofs);
      }

      // Check if we have an exterior facet
      if ( facet->numEntities(mesh.topology().dim()) != 2 )  
      {
        // set element matrix and vector to zero 
        for (uint i=0; i<A_num_entries; i++) 
          Ae[i] = 0.0; 
        for (uint i=0; i<b_num_entries; i++) 
          be[i] = 0.0; 

        // Get mesh cell to which mesh facet belongs (pick first, there is only one)
        Cell cell(mesh, facet->entities(mesh.topology().dim())[0]);

        // Get local index of facet with respect to the cell
        const uint local_facet = cell.index(*facet);

        // Update to current cell
        A_ufc.update(cell);
        b_ufc.update(cell);

        // Interpolate coefficients on cell
        for (uint i = 0; i < A_coefficients.size(); i++)
          A_coefficients[i]->interpolate(A_ufc.w[i], A_ufc.cell, cell.index(), local_facet);

        // Interpolate coefficients on cell
        for (uint i = 0; i < b_coefficients.size(); i++)
          b_coefficients[i]->interpolate(b_ufc.w[i], b_ufc.cell, cell.index(), local_facet);

        // Tabulate dofs for each dimension
        for (uint i = 0; i < A_ufc.form.rank(); i++)
          a.function_space(i).dofmap().tabulate_dofs(A_ufc.dofs[i], A_ufc.cell, cell.index());

        // Tabulate dofs for each dimension
        for (uint i = 0; i < b_ufc.form.rank(); i++)
          L.function_space(i).dofmap().tabulate_dofs(b_ufc.dofs[i], b_ufc.cell, cell.index());

        if (local_facet == 0) 
        {
          // compute cell integrals ---------------------------------

          // compute cell for A integral ---------------------------- 
          if (A_ufc.form.num_cell_integrals() > 0) 
          {
            ufc::cell_integral* A_cell_integral =  A_ufc.cell_integrals[0];
            if (cell_domains && cell_domains->size() > 0)
            {
              const uint cell_domain = (*cell_domains)(cell);
              if (cell_domain < A_ufc.form.num_cell_integrals())
                A_cell_integral = A_ufc.cell_integrals[cell_domain];
              else 
                continue;
            }
            // Tabulate cell tensor
            A_cell_integral->tabulate_tensor(A_ufc.A, A_ufc.w, A_ufc.cell);
            for (uint i=0; i<A_num_entries; i++) 
              Ae[i] += A_ufc.A[i]; 
          }
          // compute cell A integral done ---------------------------- 

          // compute cell b integral ---------------------------- 
          if (b_ufc.form.num_cell_integrals() > 0) 
          {
            ufc::cell_integral* b_cell_integral = b_ufc.cell_integrals[0];
            if (cell_domains && cell_domains->size() > 0) 
            {
              const uint cell_domain = (*cell_domains)(cell);
              if (cell_domain < b_ufc.form.num_cell_integrals())
                b_cell_integral = b_ufc.cell_integrals[cell_domain];
              else 
                continue;
            }
            // Tabulate cell tensor
            b_cell_integral->tabulate_tensor(b_ufc.A, b_ufc.w, b_ufc.cell);
            for (uint i=0; i<b_num_entries; i++) 
              be[i] += b_ufc.A[i]; 
          }
          // compute cell b integral done ---------------------------- 

          // compute cell integral done ------------------------------ 
        }

        // compute exterior facet integral ------------------------- 

        if (A_ufc.form.num_exterior_facet_integrals() > 0 ) 
        {
          const uint D = mesh.topology().dim(); 
          if (facet->numEntities(D) != 2) 
          {
            ufc::exterior_facet_integral* A_integral = A_ufc.exterior_facet_integrals[0]; 

            if (exterior_facet_domains && exterior_facet_domains->size() > 0)
            {
              const uint exterior_facet_domain= (*exterior_facet_domains)(*facet);
              if (exterior_facet_domain < A_ufc.form.num_exterior_facet_integrals())
                A_integral = A_ufc.exterior_facet_integrals[exterior_facet_domain];
              else
                continue;
            }
            A_integral->tabulate_tensor(A_ufc.A, A_ufc.w, A_ufc.cell, local_facet);
            for (uint i=0; i<A_num_entries; i++) 
              Ae[i] += A_ufc.A[i]; 
          }
        }

        if (b_ufc.form.num_exterior_facet_integrals() > 0) 
        {
          const uint D = mesh.topology().dim(); 
          if (facet->numEntities(D) != 2) 
          {
            ufc::exterior_facet_integral* b_integral = b_ufc.exterior_facet_integrals[0]; 
            if (exterior_facet_domains && exterior_facet_domains->size() > 0)
            {
              const uint exterior_facet_domain= (*exterior_facet_domains)(*facet);
              if (exterior_facet_domain < b_ufc.form.num_exterior_facet_integrals())
                b_integral = b_ufc.exterior_facet_integrals[exterior_facet_domain];
              else
                continue;
            }
            const uint local_facet = cell.index(*facet);
            b_integral->tabulate_tensor(b_ufc.A, b_ufc.w, b_ufc.cell, local_facet);
            for (uint i=0; i<b_num_entries; i++) 
              be[i] += b_ufc.A[i]; 
          }
        }
        // enforce BC  ---------------------------------------

        uint m = A_ufc.local_dimensions[0]; 
        uint n = A_ufc.local_dimensions[1]; 

        for (uint i=0; i<n; i++) 
        {  
          uint ii = A_ufc.dofs[1][i]; 
          if (indicators[ii] > 0) 
          {  
            be[i] = g[ii]; 
            for (uint k=0; k<n; k++) 
              Ae[k+i*n] = 0.0; 
            for (uint j=0; j<m; j++) 
            {
              be[j] -= Ae[i+j*n]*g[ii]; 
              Ae[i+j*n] = 0.0; 
            }
            Ae[i+i*n] = 1.0; 
          }
        }

        // enforce BC done  ------------------------------------------

        // Add entries to global tensor
        A.add(Ae, A_ufc.local_dimensions, A_ufc.dofs);
        b.add(be, b_ufc.local_dimensions, b_ufc.dofs);
      }
    }
    delete [] Ae_macro;
    delete [] be_macro;
  }

  // -- Finalize tensors 
  A.apply();
  b.apply();

  delete [] Ae;
  delete [] be;
  delete [] g;
  delete [] indicators;
}
//-----------------------------------------------------------------------------
