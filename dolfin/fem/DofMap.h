// Copyright (C) 2007-2009 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Martin Alnes, 2008
// Modified by Kent-Andre Mardal, 2009
// Modified by Ola Skavhaug, 2009
//
// First added:  2007-03-01
// Last changed: 2009-11-10

#ifndef __DOF_MAP_H
#define __DOF_MAP_H

#include <map>
#include <memory>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <dolfin/common/types.h>
#include <dolfin/common/Variable.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshFunction.h>
#include "UFC.h"
#include "UFCCell.h"
#include "UFCMesh.h"

namespace dolfin
{

  class UFC;

  /// This class handles the mapping of degrees of freedom.
  /// It wraps a ufc::dof_map on a specific mesh and provides
  /// optional precomputation and reordering of dofs.

  class DofMap : public Variable
  {
  public:

    /// Create dof map on mesh
    DofMap(boost::shared_ptr<ufc::dof_map> ufc_dofmap,
           Mesh& dolfin_mesh);

    /// Create dof map on mesh (const mesh version)
    DofMap(boost::shared_ptr<ufc::dof_map> ufc_dofmap,
           const Mesh& dolfin_mesh);

  private:

    /// Create dof map on mesh with a std::vector dof map
    DofMap(std::auto_ptr<std::vector<dolfin::uint> > map,
           boost::shared_ptr<ufc::dof_map> ufc_dofmap,
           const Mesh& dolfin_mesh);

  public:

    /// Destructor
    ~DofMap();

    /// Return a string identifying the dof map
    std::string signature() const
    {
      if (!_map.get())
        return _ufc_dofmap->signature();
      else
      {
        error("DofMap has been re-ordered. Cannot return signature string.");
        return _ufc_dofmap->signature();
      }
    }

    /// Return true iff mesh entities of topological dimension d are needed
    bool needs_mesh_entities(unsigned int d) const
    { return _ufc_dofmap->needs_mesh_entities(d); }

    /// Return the dimension of the global finite element function space
    unsigned int global_dimension() const
    {
      assert(_ufc_dofmap->global_dimension() > 0);
      return _ufc_dofmap->global_dimension();
    }

    /// Return the dimension of the local finite element function space on a cell
    unsigned int local_dimension(const ufc::cell& cell) const
    { return _ufc_dofmap->local_dimension(cell); }

    /// Return the maximum dimension of the local finite element function space
    unsigned int max_local_dimension() const
    { return _ufc_dofmap->max_local_dimension(); }

    // Return the geometric dimension of the coordinates this dof map provides
    unsigned int geometric_dimension() const
    { return _ufc_dofmap->geometric_dimension(); }

    /// Return number of facet dofs
    unsigned int num_facet_dofs() const
    { return _ufc_dofmap->num_facet_dofs(); }

    /// Tabulate the local-to-global mapping of dofs on a cell (UFC cell version)
    void tabulate_dofs(uint* dofs, const ufc::cell& ufc_cell, uint cell_index) const;

    /// Tabulate the local-to-global mapping of dofs on a cell (DOLFIN cell version)
    void tabulate_dofs(uint* dofs, const Cell& cell) const;

    /// Tabulate local-local facet dofs
    void tabulate_facet_dofs(uint* dofs, uint local_facet) const;

    /// Tabulate the coordinates of all dofs on a cell (UFC cell version)
    void tabulate_coordinates(double** coordinates, const ufc::cell& ufc_cell) const
    { _ufc_dofmap->tabulate_coordinates(coordinates, ufc_cell); }

    /// Tabulate the coordinates of all dofs on a cell (DOLFIN cell version)
    void tabulate_coordinates(double** coordinates, const Cell& cell) const;

    /// Test whether dof map has been renumbered
    bool renumbered() const { return _map.get(); }

    /// Extract sub dofmap component
    DofMap* extract_sub_dofmap(const std::vector<uint>& component, const Mesh& dolfin_mesh) const;

    /// "Collapse" a sub dofmap
    DofMap* collapse(std::map<uint, uint>& collapsed_map, const Mesh& dolfin_mesh) const;

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

  private:

    /// Friends
    friend class DofMapBuilder;
    friend class FunctionSpace;
    friend class AdaptiveObjects;

    // Initialize
    void init(const Mesh& dolfin_mesh);

    /// Initialize the UFC mesh
    static void init_ufc_mesh(UFCMesh& ufc_mesh,
                              const Mesh& dolfin_mesh);

    /// Initialize the UFC dofmap
    static void init_ufc_dofmap(ufc::dof_map& dofmap,
                                const ufc::mesh ufc_mesh,
                                const Mesh& dolfin_mesh);

    // Recursively extract sub dofmap
    static ufc::dof_map* extract_sub_dofmap(const ufc::dof_map& ufc_dof_map,
                                            uint& offset,
                                            const std::vector<uint>& component,
                                            const ufc::mesh ufc_mesh,
                                            const Mesh& dolfin_mesh);

    // FIXME: Should this be a std::vector<std::vector<int> >,
    //        e.g. a std::vector for each cell?
    // FIXME: Document layout of map
    // Precomputed dof map
    std::auto_ptr<std::vector<dolfin::uint> > _map;

    // Map from UFC dofs to renumbered dof
    std::map<dolfin::uint, uint> _ufc_to_map;

    // UFC dof map
    boost::shared_ptr<ufc::dof_map> _ufc_dofmap;

    // UFC mesh
    UFCMesh _ufc_mesh;

    // UFC dof map offset into parent's vector of coefficients
    uint _ufc_offset;

    // True iff running in parallel
    bool _parallel;

  };

}

#endif
