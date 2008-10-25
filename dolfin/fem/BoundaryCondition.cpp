// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells 2007, 2008.
//
// First added:  2008-06-18
// Last changed: 2008-10-14

#include <dolfin/fem/FiniteElement.h>
#include <dolfin/mesh/Mesh.h>
#include "DofMap.h"
#include "Form.h"
#include "SubSystem.h"
#include "BoundaryCondition.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
BoundaryCondition::BoundaryCondition()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
BoundaryCondition::~BoundaryCondition()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
BoundaryCondition::LocalData::LocalData(const ufc::form& form, const Mesh& mesh, 
                                        const DofMap& global_dof_map, 
                                        const SubSystem& sub_system)
  : ufc_mesh(mesh), finite_element(0), dof_map(0), dof_map_local(0), offset(0),
    w(0), cell_dofs(0), facet_dofs(0)
{
  // FIXME: Change behaviour of num_sub_elements() in FFC (return 0 when
  // FIXME: there are no nested elements

  // Check arity of form
  if (form.rank() != 2)
    error("Form must be bilinear for application of boundary conditions.");

  // Create finite element (second argument of form)
  finite_element = new FiniteElement(form.create_finite_element(1));
  
  // Extract sub element and sub dof map if we have a sub system
  if (sub_system.depth() > 0)
  {
    // Finite element
    FiniteElement* sub_finite_element = finite_element->extract_sub_element(sub_system.array());
    delete finite_element;
    finite_element = sub_finite_element;

    // Create sub dof map
    dof_map = global_dof_map.extract_sub_dofmap(sub_system.array(), offset);

    // Take responsibility for dof_map
    dof_map_local = dof_map;
  }
  else
    dof_map = &global_dof_map;

  // Create local data used to set boundary conditions
  w = new double[finite_element->space_dimension()];
  cell_dofs = new uint[finite_element->space_dimension()];
  for (uint i = 0; i < finite_element->space_dimension(); i++)
  {
    w[i] = 0.0;
    cell_dofs[i] = 0;
  }
  facet_dofs = new uint[dof_map->num_facet_dofs()];
  for (uint i = 0; i < dof_map->num_facet_dofs(); i++)
    facet_dofs[i] = 0;

  // Create local coordinate data
  coordinates = new double*[dof_map->local_dimension()];
  for (uint i = 0; i < dof_map->local_dimension(); i++)
  {
    coordinates[i] = new double[mesh.geometry().dim()];
    for (uint j = 0; j < mesh.geometry().dim(); j++)
      coordinates[i][j] = 0.0;
  }
}
//-----------------------------------------------------------------------------
BoundaryCondition::LocalData::~LocalData()
{
  if (coordinates)
  {
    for (uint i = 0; i < dof_map->local_dimension(); i++)
      delete [] coordinates[i];
    delete [] coordinates;
  }

  delete finite_element;
  delete dof_map_local;
  delete [] w;
  delete [] cell_dofs;
  delete [] facet_dofs;
}
//-----------------------------------------------------------------------------
