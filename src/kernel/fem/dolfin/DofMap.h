// Copyright (C) 2007 Anders Logg and Garth N. Wells.
// Licensed under the GNU GPL Version 2.

// First added:  2007-03-01
// Last changed: 2007-03-15

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

    /// Return local dimension of dof map
    uint local_dimension() const
    {
      return ufc_dof_map.local_dimension();
    }

    /// Return global dimension of dof map
    uint global_dimension() const
    {
      return ufc_dof_map.global_dimension();
    }

    /// Compute dof map for a cell
    void dof_map(uint* dofs, const UFCCell& ufc_cell) const
    {
      ufc_dof_map.tabulate_dofs(dofs, ufc_mesh, ufc_cell);
    };

    /// Return DOFLIN mesh associated with map
    Mesh& mesh() const
    {
      return dolfin_mesh;
    };

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
