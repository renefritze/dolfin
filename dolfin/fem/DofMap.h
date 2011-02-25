// Copyright (C) 2007-2011 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Martin Alnes, 2008
// Modified by Kent-Andre Mardal, 2009
// Modified by Ola Skavhaug, 2009
//
// First added:  2007-03-01
// Last changed: 2011-02-23

#ifndef __DOF_MAP_H
#define __DOF_MAP_H

#include <map>
#include <memory>
#include <utility>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <ufc.h>

#include <dolfin/common/types.h>
#include <dolfin/mesh/Cell.h>
#include "GenericDofMap.h"

namespace dolfin
{

  class UFC;
  class UFCMesh;

  /// This class handles the mapping of degrees of freedom. It builds
  /// a dof map based on a ufc::dofmap on a specific mesh. It will
  /// reorder the dofs when running in parallel. Sub-dofmaps, both
  /// views and copies, are supported.

  class DofMap : public GenericDofMap
  {
  public:

    /// Create dof map on mesh (data is not shared)
    DofMap(boost::shared_ptr<const ufc::dofmap> ufc_dofmap, Mesh& mesh);

    /// Create dof map on mesh ((data is not shared), const mesh version)
    DofMap(boost::shared_ptr<const ufc::dofmap> ufc_dofmap, const Mesh& mesh);

    /// Copy constructor
    DofMap(const DofMap& dofmap);

  private:

    /// Create a sub-dofmap (a view) from parent_dofmap
    DofMap(const DofMap& parent_dofmap, const std::vector<uint>& component,
           const Mesh& mesh, bool distributed);

    /// Create a collapsed dofmap from parent_dofmap
    DofMap(boost::unordered_map<uint, uint>& collapsed_map, const DofMap& dofmap_view,
           const Mesh& mesh, bool distributed);

  public:

    /// Destructor
    ~DofMap();

    /// True if dof map is a view into another map (is a sub-dofmap)
    bool is_view() const
    { return _is_view; }

    /// Return true iff mesh entities of topological dimension d are needed
    bool needs_mesh_entities(unsigned int d) const;

    /// Return the dimension of the global finite element function space
    unsigned int global_dimension() const;

    /// Return the dimension of the local finite element function space on a
    // cell
    unsigned int cell_dimension(uint cell_index) const;

    /// Return the maximum dimension of the local finite element function space
    unsigned int max_cell_dimension() const;

    // Return the geometric dimension of the coordinates this dof map provides
    unsigned int geometric_dimension() const;

    /// Return number of facet dofs
    unsigned int num_facet_dofs() const;

    /// Return the ownership range (dofs in this range are owned by this process)
    std::pair<unsigned int, unsigned int> ownership_range() const;

    /// Return map from nonlocal-dofs that appear in local dof map to owning
    /// process
    const boost::unordered_map<unsigned int, unsigned int>& off_process_owner() const;

    /// Local-to-global mapping of dofs on a cell
    const std::vector<uint>& cell_dofs(uint cell_index) const
    {
      assert(cell_index < dofmap.size());
      return dofmap[cell_index];
    }

    /// Tabulate the local-to-global mapping of dofs on a cell
    void tabulate_dofs(uint* dofs, const Cell& cell) const
    {
      const uint cell_index = cell.index();
      assert(cell_index < dofmap.size());
      std::copy(dofmap[cell_index].begin(), dofmap[cell_index].end(), dofs);
    }

    /// Tabulate local-local facet dofs
    void tabulate_facet_dofs(uint* dofs, uint local_facet) const;

    /// Tabulate the coordinates of all dofs on a cell (UFC cell version)
    void tabulate_coordinates(double** coordinates,
                              const ufc::cell& ufc_cell) const
    { _ufc_dofmap->tabulate_coordinates(coordinates, ufc_cell); }

    /// Tabulate the coordinates of all dofs on a cell (DOLFIN cell version)
    void tabulate_coordinates(double** coordinates, const Cell& cell) const;

    /// Create a copy of the dof map
    DofMap* copy(const Mesh& mesh) const;

    /// Extract sub dofmap component
    DofMap* extract_sub_dofmap(const std::vector<uint>& component,
                               const Mesh& mesh) const;

    /// Create a "collapsed" dofmap (collapses a sub-dofmap)
    DofMap* collapse(boost::unordered_map<uint, uint>& collapsed_map,
                     const Mesh& mesh) const;

    /// Return the set of dof indices
    boost::unordered_set<dolfin::uint> dofs() const;

    // Renumber dofs
    void renumber(const std::vector<uint>& renumbering_map);

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

  private:

    // Friends
    friend class DofMapBuilder;

    // Recursively extract UFC sub-dofmap and compute offset
    static ufc::dofmap* extract_ufc_sub_dofmap(const ufc::dofmap& ufc_dofmap,
                                            uint& offset,
                                            const std::vector<uint>& component,
                                            const ufc::mesh ufc_mesh,
                                            const Mesh& dolfin_mesh);

    // Initialize the UFC dofmap
    static void init_ufc_dofmap(ufc::dofmap& dofmap, const ufc::mesh ufc_mesh,
                                const Mesh& dolfin_mesh);

    // Local-to-global dof map (dofs for cell dofmap[i])
    std::vector<std::vector<dolfin::uint> > dofmap;

    // UFC dof map
    boost::scoped_ptr<ufc::dofmap> _ufc_dofmap;

    // Map from UFC dof numbering to renumbered dof (ufc_dof, actual_dof)
    boost::unordered_map<dolfin::uint, uint> ufc_map_to_dofmap;

    // UFC dof map offset
    unsigned int ufc_offset;

    // Ownership range (dofs in this range are owned by this process)
    std::pair<uint, uint> _ownership_range;

    // Owner (process) of dofs in local dof map that do not belong to
    // this process
    boost::unordered_map<uint, uint> _off_process_owner;

    // True iff sub-dofmap (a view, i.e. not collapsed)
    bool _is_view;

    // True iff running in parallel
    bool _distributed;

  };

}

#endif
