// Copyright (C) 2007 Anders Logg and Garth N. Wells.
// Licensed under the GNU GPL Version 2.

// First added:  2007-03-01
// Last changed: 2007-03-01

#ifndef __DOF_MAP_H
#define __DOF_MAP_H

#include <ufc.h>
#include <dolfin/UFCMesh.h>

namespace dolfin
{

  class Mesh;
  
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

    /// Friends
    friend class UFC;
    
  private:

    // UFC dof map
    ufc::dof_map& ufc_dof_map;

    // UFC mesh
    UFCMesh ufc_mesh;

  };

}

#endif
