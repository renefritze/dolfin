// Copyright (C) 2007 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.

// First added:  2007-03-01
// Last changed: 2007-04-03

#ifndef __DOF_MAP_H
#define __DOF_MAP_H

#include <ufc.h>
#include <dolfin/Mesh.h>
#include <dolfin/UFCCell.h>
#include <dolfin/UFCMesh.h>

namespace dolfin
{
  
  /// This class handles the mapping of degrees of freedom.
  /// It wraps a ufc::dof_map on a specific mesh and provides
  /// optional precomputation and reordering of dofs.

  class DofMap
  {
  public:

    /// Create dof map on mesh
    DofMap(ufc::dof_map& dof_map, Mesh& mesh);

    /// Destructor
    ~DofMap();

    /// Return a string identifying the dof map
    const char* signature() const
    {
      return ufc_dof_map.signature();
    }

    /// Return the dimension of the global finite element function space
    unsigned int global_dimension() const
    {
      return ufc_dof_map.global_dimension();
    }

    /// Return the dimension of the local finite element function space
    unsigned int local_dimension() const
    {
      return ufc_dof_map.local_dimension();
    }

    /// Tabulate the local-to-global mapping of dofs on a cell
    void tabulate_dofs(uint* dofs, const ufc::cell& cell) const
    {
      ufc_dof_map.tabulate_dofs(dofs, ufc_mesh, cell);
    }

    /// Return mesh associated with map
    Mesh& mesh() const
    {
      return dolfin_mesh;
    }

    /// Friends
    friend class UFC;
    
  private:

    // UFC dof map
    ufc::dof_map& ufc_dof_map;

    // UFC mesh
    UFCMesh ufc_mesh;

    // DOLFIN mesh
    Mesh& dolfin_mesh;

  };

}

#endif
