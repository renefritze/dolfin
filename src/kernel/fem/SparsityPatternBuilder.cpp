// Copyright (C) 2007 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2007.
// Modified by Anders Logg, 2008.
//
// First added:  2007-05-24
// Last changed: 2007-12-07

#include <dolfin/dolfin_log.h>
#include <dolfin/DofMapSet.h>
#include <dolfin/Cell.h>
#include <dolfin/Facet.h>
#include <dolfin/Mesh.h>
#include <dolfin/GenericSparsityPattern.h>
#include <dolfin/SparsityPatternBuilder.h>
#include <dolfin/UFC.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void SparsityPatternBuilder::build(GenericSparsityPattern& sparsity_pattern,
				   Mesh& mesh, UFC& ufc, const DofMapSet& dof_map_set)
{
  // Initialise sparsity pattern
  if( dof_map_set.parallel() )
    sparsity_pattern.pinit(ufc.form.rank(), ufc.global_dimensions);
  else
    sparsity_pattern.init(ufc.form.rank(), ufc.global_dimensions);

  // Only build for rank >= 2 (matrices and higher order tensors)
  if (ufc.form.rank() < 2)
    return;

  // Build sparsity pattern for cell integrals
  if (ufc.form.num_cell_integrals() != 0)
  {
    for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      // Update to current cell
      ufc.update(*cell);
  
      // Tabulate dofs for each dimension
      for (uint i = 0; i < ufc.form.rank(); ++i)
        dof_map_set[i].tabulate_dofs(ufc.dofs[i], *cell);
 
      // Fill sparsity pattern.
      if( dof_map_set.parallel() )
        sparsity_pattern.pinsert(ufc.local_dimensions, ufc.dofs);
      else
        sparsity_pattern.insert(ufc.local_dimensions, ufc.dofs);
    }
  }

  // Note: no need to iterate over exterior facets since those dofs
  // are included when tabulating dofs on all cells

  // Build sparsity pattern for interior facet integrals
  if (ufc.form.num_interior_facet_integrals() != 0)
  {
    // Compute facets and facet - cell connectivity if not already computed
    mesh.init(mesh.topology().dim() - 1);
    mesh.init(mesh.topology().dim() - 1, mesh.topology().dim());
    mesh.order();

    for (FacetIterator facet(mesh); !facet.end(); ++facet)
    {
      // Check if we have an interior facet
      if (facet->numEntities(mesh.topology().dim()) != 2)
        continue;
      
      // Get cells incident with facet
      Cell cell0(mesh, facet->entities(mesh.topology().dim())[0]);
      Cell cell1(mesh, facet->entities(mesh.topology().dim())[1]);

      // Update to current pair of cells
      ufc.update(cell0, cell1);

      // Tabulate dofs for each dimension on macro element
      for (uint i = 0; i < ufc.form.rank(); ++i)
      {
        const uint offset = dof_map_set[i].local_dimension();
        dof_map_set[i].tabulate_dofs(ufc.macro_dofs[i], cell0);
        dof_map_set[i].tabulate_dofs(ufc.macro_dofs[i] + offset, cell1);
      }

      // Fill sparsity pattern.
      sparsity_pattern.insert(ufc.local_dimensions, ufc.macro_dofs);
    }
  }
  
  // Finalize sparsity pattern
  sparsity_pattern.apply();
}
//-----------------------------------------------------------------------------
