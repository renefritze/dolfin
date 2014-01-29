// Copyright (C) 2013 Anders Logg
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
// First added:  2013-09-19
// Last changed: 2013-10-22

#ifndef __CCFEM_DOF_MAP_H
#define __CCFEM_DOF_MAP_H

#include "GenericDofMap.h"

namespace dolfin
{

  // Forward declarations
  class CCFEMFunctionSpace;

  /// This class handles the mapping of degrees of freedom for CCFEM
  /// function spaces.

  class CCFEMDofMap : public GenericDofMap
  {
  public:

    /// Constructor
    CCFEMDofMap();

    // Copy constructor
    CCFEMDofMap(const CCFEMDofMap& dofmap);

    /// Destructor
    ~CCFEMDofMap();

    /// Return the number dofmaps (parts) of the CCFEM dofmap
    ///
    /// *Returns*
    ///     std::size_t
    ///         The number of dofmaps (parts) of the CCFEM dofmap
    std::size_t num_parts() const;

    /// Return dofmap (part) number i
    ///
    /// *Returns*
    ///     _GenericDofMap_
    ///         Dofmap (part) number i
    boost::shared_ptr<const GenericDofMap> part(std::size_t i) const;

    /// Set current part. This will make the CCFEM dofmap act as a
    /// dofmap for the part of the CCFEM function space defined on the
    /// current part (mesh).
    ///
    /// *Arguments*
    ///     part (std::size_t)
    ///         The number of the part.
    void set_current_part(std::size_t part) const;

    /// Add dofmap (shared pointer version)
    ///
    /// *Arguments*
    ///     dofmap (_GenericDofMap_)
    ///         The dofmap.
    void add(boost::shared_ptr<const GenericDofMap> dofmap);

    /// Add dofmap (reference version)
    ///
    /// *Arguments*
    ///     dofmap (_DofMap_)
    ///         The dofmap.
    void add(const GenericDofMap& dofmap);

    /// Build CCFEM dofmap
    void build(const CCFEMFunctionSpace& function_space);

    /// Clear CCFEM dofmap
    void clear();

    //--- Implementation of GenericDofMap interface (incomplete) ---

    /// True if dof map is a view into another map (is a sub-dofmap)
    bool is_view() const;

    /// Return the dimension of the global finite element function
    /// space
    std::size_t global_dimension() const;

    /// Return the dimension of the local finite element function
    /// space on a cell
    std::size_t cell_dimension(std::size_t index) const;

    /// Return the maximum dimension of the local finite element
    /// function space
    std::size_t max_cell_dimension() const;

    /// Return the number of dofs for a given entity dimension
    std::size_t num_entity_dofs(std::size_t dim) const;

    /// Return the geometric dimension of the coordinates this dof map
    // provides
    std::size_t geometric_dimension() const;

    /// Return number of facet dofs
    std::size_t num_facet_dofs() const;

    /// Restriction if any. If the dofmap is not restricted, a null
    /// pointer is returned.
    boost::shared_ptr<const Restriction> restriction() const;

    /// Return the ownership range (dofs in this range are owned by
    /// this process)
    std::pair<std::size_t, std::size_t> ownership_range() const;

    /// Return map from nonlocal-dofs (that appear in local dof map)
    /// to owning process
    const boost::unordered_map<std::size_t, unsigned int>&
      off_process_owner() const;

    /// Local-to-global mapping of dofs on a cell
    const std::vector<dolfin::la_index>&
      cell_dofs(std::size_t cell_index) const;

    /// Tabulate local-local facet dofs
    void tabulate_facet_dofs(std::vector<std::size_t>& dofs,
                                     std::size_t local_facet) const;

    /// Tabulate the local-to-local mapping of dofs on entity
    /// (dim, local_entity)
    void tabulate_entity_dofs(std::vector<std::size_t>& dofs,
				      std::size_t dim,
                                      std::size_t local_entity) const;

    /// Return a map between vertices and dofs
    std::vector<dolfin::la_index>
      dof_to_vertex_map(const Mesh& mesh) const;

    /// Return a map between vertices and dofs
    std::vector<std::size_t>
      vertex_to_dof_map(const Mesh& mesh) const;

    /// Tabulate the coordinates of all dofs on a cell (UFC cell version)
    void tabulate_coordinates(boost::multi_array<double, 2>& coordinates,
                              const std::vector<double>& vertex_coordinates,
                              const Cell& cell) const;

    /// Tabulate the coordinates of all dofs owned by this
    /// process. This function is typically used by preconditioners
    /// that require the spatial coordinates of dofs, for example
    /// for re-partitioning or nullspace computations. The format for
    /// the return vector is [x0, y0, z0, x1, y1, z1, . . .].
    std::vector<double>
      tabulate_all_coordinates(const Mesh& mesh) const;

    /// Create a copy of the dof map
    boost::shared_ptr<GenericDofMap> copy() const;

    /// Create a new dof map on new mesh
    boost::shared_ptr<GenericDofMap>
      create(const Mesh& new_mesh) const;

    /// Extract sub dofmap component
    boost::shared_ptr<GenericDofMap>
        extract_sub_dofmap(const std::vector<std::size_t>& component,
                           const Mesh& mesh) const;

    /// Create a "collapsed" a dofmap (collapses from a sub-dofmap view)
    boost::shared_ptr<GenericDofMap>
        collapse(boost::unordered_map<std::size_t, std::size_t>& collapsed_map,
                 const Mesh& mesh) const;

    /// Return list of global dof indices on this process
    std::vector<dolfin::la_index> dofs() const;

    /// Set dof entries in vector to a specified value. Parallel
    /// layout of vector must be consistent with dof map range. This
    /// function is typically used to construct the null space of a
    /// matrix operator
    void set(GenericVector& x, double value) const;

    /// Set dof entries in vector to the value*x[i], where x[i] is the
    /// spatial coordinate of the dof. Parallel layout of vector must
    /// be consistent with dof map range. This function is typically
    /// used to construct the null space of a matrix operator, e.g. rigid
    /// body rotations.
    void set_x(GenericVector& x, double value, std::size_t component,
                       const Mesh& mesh) const;

    /// Return map from shared dofs to the processes (not including
    /// the current process) that share it.
    const boost::unordered_map<std::size_t,
      std::vector<unsigned int> >& shared_dofs() const;

    /// Return set of processes that share dofs with the this process
    const std::set<std::size_t>& neighbours() const;

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

  private:

    // Total global dimension (sum of parts)
    std::size_t _global_dimension;

    // List of dofmaps
    std::vector<boost::shared_ptr<const GenericDofMap> > _dofmaps;

    // Current part (mesh)
    mutable std::size_t _current_part;

    // Local-to-global dof map for all parts
    std::vector<std::vector<std::vector<dolfin::la_index> > > _dofmap;

  };

}

#endif
