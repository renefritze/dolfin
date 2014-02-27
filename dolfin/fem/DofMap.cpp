// Copyright (C) 2007-2013 Anders Logg and Garth N. Wells
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
// Modified by Martin Alnes, 2008, 2013
// Modified by Kent-Andre Mardal, 2009
// Modified by Ola Skavhaug, 2009
// Modified by Niclas Jansson, 2009
// Modified by Joachim B Haga, 2012
// Modified by Mikael Mortensen, 2012
// Modified by Jan Blechta, 2013
//
// First added:  2007-03-01
// Last changed: 2013-09-19

#include <boost/unordered_map.hpp>

#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/types.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/log/LogStream.h>
#include <dolfin/mesh/PeriodicBoundaryComputation.h>
#include <dolfin/mesh/Restriction.h>
#include <dolfin/mesh/Vertex.h>
#include "DofMapBuilder.h"
#include "DofMap.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
DofMap::DofMap(std::shared_ptr<const ufc::dofmap> ufc_dofmap,
               const Mesh& mesh)
  : _ufc_dofmap(ufc_dofmap), _is_view(false), _global_dimension(0),
    _ufc_offset(0)
{
  dolfin_assert(_ufc_dofmap);

  // Call dofmap builder
  DofMapBuilder::build(*this, mesh, slave_master_mesh_entities, _restriction);
}
//-----------------------------------------------------------------------------
DofMap::DofMap(std::shared_ptr<const ufc::dofmap> ufc_dofmap,
               const Mesh& mesh,
               std::shared_ptr<const SubDomain> constrained_domain)
  : _ufc_dofmap(ufc_dofmap), _is_view(false), _global_dimension(0),
    _ufc_offset(0)
{
  dolfin_assert(_ufc_dofmap);

  this->constrained_domain = constrained_domain;

  // Compute slave-master pairs
  dolfin_assert(constrained_domain);
  const std::size_t D = mesh.topology().dim();
  slave_master_mesh_entities.reset(new std::map<unsigned int, std::map<unsigned int, std::pair<unsigned int, unsigned int> > >());
  dolfin_assert(slave_master_mesh_entities);
  for (std::size_t d = 0; d <= D; ++d)
  {
    if (_ufc_dofmap->needs_mesh_entities(d))
    {
      slave_master_mesh_entities->insert(std::make_pair(d,
           PeriodicBoundaryComputation::compute_periodic_pairs(mesh, *constrained_domain,
                                                               d)));
    }
  }

  // Call dofmap builder
  DofMapBuilder::build(*this, mesh, slave_master_mesh_entities, _restriction);
}
//-----------------------------------------------------------------------------
DofMap::DofMap(std::shared_ptr<const ufc::dofmap> ufc_dofmap,
               std::shared_ptr<const Restriction> restriction)
  : _ufc_dofmap(ufc_dofmap), _restriction(restriction), _is_view(false),
    _global_dimension(0), _ufc_offset(0)
{
  dolfin_assert(_ufc_dofmap);
  dolfin_assert(_restriction);

  // Get mesh
  const dolfin::Mesh& mesh = restriction->mesh();

  // Check that we get cell markers, extend later
  if (restriction->dim() != mesh.topology().dim())
  {
    dolfin_error("DofMap.cpp",
                 "create mapping of degrees of freedom",
                 "Only cell-based restriction of function spaces are currently supported. ");
  }

  // Call dofmap builder
  DofMapBuilder::build(*this, mesh, slave_master_mesh_entities, restriction);
}
//-----------------------------------------------------------------------------
DofMap::DofMap(const DofMap& parent_dofmap,
  const std::vector<std::size_t>& component, const Mesh& mesh)
  : _is_view(true), _global_dimension(0), _ufc_offset(0),
    _ownership_range(parent_dofmap._ownership_range)
{

  // Share slave-master map with parent
  this->slave_master_mesh_entities = parent_dofmap.slave_master_mesh_entities;

  // Build sub-dofmap
  DofMapBuilder::build_sub_map(*this, parent_dofmap, component, mesh);
}
//-----------------------------------------------------------------------------
DofMap::DofMap(boost::unordered_map<std::size_t, std::size_t>& collapsed_map,
               const DofMap& dofmap_view, const Mesh& mesh)
  :  _ufc_dofmap(dofmap_view._ufc_dofmap), _is_view(false),
     _global_dimension(0), _ufc_offset(0)
{
  dolfin_assert(_ufc_dofmap);

  // Check for dimensional consistency between the dofmap and mesh
  check_dimensional_consistency(*_ufc_dofmap, mesh);

  // Check that mesh has been ordered
  if (!mesh.ordered())
  {
     dolfin_error("DofMap.cpp",
                  "create mapping of degrees of freedom",
                  "Mesh is not ordered according to the UFC numbering convention. "
                  "Consider calling mesh.order()");
  }

  // Check dimensional consistency between UFC dofmap and the mesh
  check_provided_entities(*_ufc_dofmap, mesh);

  // Copy slave-master map (copy or share?)
  if (dofmap_view.slave_master_mesh_entities)
    slave_master_mesh_entities.reset(new std::map<unsigned int, std::map<unsigned int, std::pair<unsigned int, unsigned int> > >(*dofmap_view.slave_master_mesh_entities));

  // Build new dof map
  DofMapBuilder::build(*this, mesh, slave_master_mesh_entities, _restriction);

  // Dimension sanity checks
  dolfin_assert(dofmap_view._dofmap.size() == mesh.num_cells());
  dolfin_assert(global_dimension() == dofmap_view.global_dimension());
  dolfin_assert(_dofmap.size() == mesh.num_cells());

  // FIXME: Could we use a std::vector instead of std::map if the
  //        collapsed dof map is contiguous (0, . . . , n)?

  // Build map from collapsed dof index to original dof index
  collapsed_map.clear();
  for (std::size_t i = 0; i < mesh.num_cells(); ++i)
  {
    const std::vector<dolfin::la_index>& view_cell_dofs
      = dofmap_view._dofmap[i];
    const std::vector<dolfin::la_index>& cell_dofs = _dofmap[i];
    dolfin_assert(view_cell_dofs.size() == cell_dofs.size());

    for (std::size_t j = 0; j < view_cell_dofs.size(); ++j)
      collapsed_map[cell_dofs[j]] = view_cell_dofs[j];
  }
}
//-----------------------------------------------------------------------------
DofMap::DofMap(const DofMap& dofmap)
{
  // Copy data
  _dofmap = dofmap._dofmap;
  _ufc_dofmap = dofmap._ufc_dofmap;
  ufc_map_to_dofmap = dofmap.ufc_map_to_dofmap;
  _is_view = dofmap._is_view;
  _global_dimension = dofmap._global_dimension;
  _ufc_offset = dofmap._ufc_offset;
  _ownership_range = dofmap._ownership_range;
  _off_process_owner = dofmap._off_process_owner;
  _shared_dofs = dofmap._shared_dofs;
  _neighbours = dofmap._neighbours;
  slave_master_mesh_entities = dofmap.slave_master_mesh_entities;
}
//-----------------------------------------------------------------------------
DofMap::~DofMap()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::size_t DofMap::global_dimension() const
{
  return _global_dimension;
}
//-----------------------------------------------------------------------------
std::size_t DofMap::cell_dimension(std::size_t cell_index) const
{
  dolfin_assert(cell_index < _dofmap.size());
  return _dofmap[cell_index].size();
}
//-----------------------------------------------------------------------------
std::size_t DofMap::max_cell_dimension() const
{
  dolfin_assert(_ufc_dofmap);
  return _ufc_dofmap->local_dimension();
}
//-----------------------------------------------------------------------------
std::size_t DofMap::num_entity_dofs(std::size_t dim) const
{
  dolfin_assert(_ufc_dofmap);
  return _ufc_dofmap->num_entity_dofs(dim);
}
//-----------------------------------------------------------------------------
std::size_t DofMap::geometric_dimension() const
{
  dolfin_assert(_ufc_dofmap);
  return _ufc_dofmap->geometric_dimension();
}
//-----------------------------------------------------------------------------
std::size_t DofMap::num_facet_dofs() const
{
  dolfin_assert(_ufc_dofmap);
  return _ufc_dofmap->num_facet_dofs();
}
//-----------------------------------------------------------------------------
std::shared_ptr<const dolfin::Restriction> DofMap::restriction() const
{
  return _restriction;
}
//-----------------------------------------------------------------------------
std::pair<std::size_t, std::size_t> DofMap::ownership_range() const
{
  return _ownership_range;
}
//-----------------------------------------------------------------------------
const boost::unordered_map<std::size_t, unsigned int>&
DofMap::off_process_owner() const
{
  return _off_process_owner;
}
//-----------------------------------------------------------------------------
const boost::unordered_map<std::size_t, std::vector<unsigned int> >&
DofMap::shared_dofs() const
{
  return _shared_dofs;
}
//-----------------------------------------------------------------------------
const std::set<std::size_t>& DofMap::neighbours() const
{
  return _neighbours;
}
//-----------------------------------------------------------------------------
void DofMap::tabulate_entity_dofs(std::vector<std::size_t>& dofs,
				  std::size_t dim, std::size_t local_entity) const
{
  dolfin_assert(_ufc_dofmap);
  if (_ufc_dofmap->num_entity_dofs(dim)==0)
    return ;

  dofs.resize(_ufc_dofmap->num_entity_dofs(dim));
  _ufc_dofmap->tabulate_entity_dofs(&dofs[0], dim, local_entity);
}
//-----------------------------------------------------------------------------
void DofMap::tabulate_facet_dofs(std::vector<std::size_t>& dofs,
                                 std::size_t local_facet) const
{
  dolfin_assert(_ufc_dofmap);
  dofs.resize(_ufc_dofmap->num_facet_dofs());
  _ufc_dofmap->tabulate_facet_dofs(dofs.data(), local_facet);
}
//-----------------------------------------------------------------------------
void DofMap::tabulate_coordinates(boost::multi_array<double, 2>& coordinates,
                                  const std::vector<double>& vertex_coordinates,
                                  const Cell& cell) const
{
  // FIXME: This is a hack because UFC wants a double pointer for coordinates
  dolfin_assert(_ufc_dofmap);

  // Check dimensions
  if (coordinates.shape()[0] != cell_dimension(cell.index()) ||
      coordinates.shape()[1] != _ufc_dofmap->geometric_dimension())
  {
    boost::multi_array<double, 2>::extent_gen extents;
    const std::size_t cell_dim = cell_dimension(cell.index());
    coordinates.resize(extents[cell_dim][_ufc_dofmap->geometric_dimension()]);
  }

  // Set vertex coordinates
  const std::size_t num_points = coordinates.size();
  std::vector<double*> coords(num_points);
  for (std::size_t i = 0; i < num_points; ++i)
    coords[i] = &(coordinates[i][0]);

  // Tabulate coordinates
  _ufc_dofmap->tabulate_coordinates(coords.data(), vertex_coordinates.data());
}
//-----------------------------------------------------------------------------
std::vector<double> DofMap::tabulate_all_coordinates(const Mesh& mesh) const
{
  // Geometric dimension
  const std::size_t gdim = _ufc_dofmap->geometric_dimension();
  dolfin_assert(gdim == mesh.geometry().dim());

  if (_is_view)
  {
    dolfin_error("DofMap.cpp",
                 "tabulate_all_coordinates",
                 "Cannot tabulate coordinates for a DofMap that is a view.");
  }

  // Local offset
  const std::size_t offset = ownership_range().first;

  // Number of local dofs (dofs owned by this process)
  const std::size_t local_size
    = ownership_range().second - ownership_range().first;

  // Vector to hold coordinates and return
  std::vector<double> x(gdim*local_size);

  // Loop over cells and tabulate dofs
  boost::multi_array<double, 2> coordinates;
  std::vector<double> vertex_coordinates;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update UFC cell
    cell->get_vertex_coordinates(vertex_coordinates);

    // Get local-to-global map
    const std::vector<dolfin::la_index>& dofs = cell_dofs(cell->index());

    // Tabulate dof coordinates on cell
    tabulate_coordinates(coordinates, vertex_coordinates, *cell);

    // Copy dof coordinates into vector
    for (std::size_t i = 0; i < dofs.size(); ++i)
    {
      const std::size_t dof = dofs[i];
      if (dof >=  _ownership_range.first && dof < _ownership_range.second)
      {
        const std::size_t local_index = dof - offset;
        for (std::size_t j = 0; j < gdim; ++j)
        {
          dolfin_assert(gdim*local_index + j < x.size());
          x[gdim*local_index + j] = coordinates[i][j];
        }
      }
    }
  }

  return x;
}
//-----------------------------------------------------------------------------
std::vector<dolfin::la_index> DofMap::dof_to_vertex_map(const Mesh& mesh) const
{
  deprecation("dof_to_vertex_map", "1.3.0", "1.4",
	      "DofMap::dof_to_vertex_map has been replaced by the free "
	      "function vertex_to_dof_map.");

  if (_is_view)
  {
    dolfin_error("DofMap.cpp",
                 "tabulate dof to vertex map",
                 "Cannot tabulate dof_to_vertex map for a DofMap that is a view");
  }

  // Check that we only have dofs living on vertices
  assert(_ufc_dofmap);

  // Initialize vertex to cell connections
  const std::size_t top_dim = mesh.topology().dim();
  mesh.init(0, top_dim);

  // Num dofs per vertex
  const std::size_t dofs_per_vertex = _ufc_dofmap->num_entity_dofs(0);
  const std::size_t vert_per_cell = mesh.topology()(top_dim, 0).size(0);

  if (vert_per_cell*dofs_per_vertex != _ufc_dofmap->local_dimension())
  {
    dolfin_error("DofMap.cpp",
                 "tabulate dof to vertex map",
                 "Can only tabulate dofs on vertices");
  }

  // Allocate data for tabulating local to local map
  std::vector<std::size_t> local_to_local_map(dofs_per_vertex);

  // Offset of local to global dof numbering
  const dolfin::la_index n0 = _ownership_range.first;

  // Create return data structure
  std::vector<dolfin::la_index> dof_map(dofs_per_vertex*mesh.num_entities(0));

  // Iterate over vertices
  std::size_t local_vertex_ind = 0;
  dolfin::la_index global_dof;
  for (VertexIterator vertex(mesh); !vertex.end(); ++vertex)
  {
    // Get the first cell connected to the vertex
    const Cell cell(mesh, vertex->entities(top_dim)[0]);

    // Find local vertex number
    for (std::size_t i = 0; i < cell.num_entities(0); i++)
    {
      if (cell.entities(0)[i] == vertex->index())
      {
        local_vertex_ind = i;
        break;
      }
    }

    // Get all cell dofs
    const std::vector<dolfin::la_index>& _cell_dofs = cell_dofs(cell.index());

    // Tabulate local to local map of dofs on local vertex
    _ufc_dofmap->tabulate_entity_dofs(local_to_local_map.data(), 0,
                                      local_vertex_ind);

    // Fill local dofs for the vertex
    for (std::size_t local_dof = 0; local_dof < dofs_per_vertex; local_dof++)
    {
      global_dof = _cell_dofs[local_to_local_map[local_dof]];
      dof_map[dofs_per_vertex*vertex->index() + local_dof] = global_dof - n0;
    }
  }

  // Return the map
  return dof_map;
}
//-----------------------------------------------------------------------------
std::vector<std::size_t> DofMap::vertex_to_dof_map(const Mesh& mesh) const
{
  deprecation("vertex_to_dof_map", "1.3.0", "1.4",
	      "DofMap::vertex_to_dof_map has been replaced by the "
	      "free function dof_to_vertex_map.");

  if (_is_view)
  {
    dolfin_error("DofMap.cpp",
                 "tabulate vertex to dof map",
                 "Cannot tabulate vertex_to_dof map for a DofMap that is a view");
  }

  // Get dof to vertex map
  const std::vector<dolfin::la_index> dof_map = dof_to_vertex_map(mesh);

  // Create return data structure
  const dolfin::la_index num_dofs = _ownership_range.second
                                  - _ownership_range.first;
  std::vector<std::size_t> vertex_map(num_dofs);

  // Invert dof_map
  dolfin::la_index dof;
  for (std::size_t i = 0; i < dof_map.size(); i++)
  {
    dof = dof_map[i];

    // Skip ghost dofs
    if (dof >= 0 && dof < num_dofs)
      vertex_map[dof] = i;
  }

  // Return the map
  return vertex_map;
}
//-----------------------------------------------------------------------------
std::shared_ptr<GenericDofMap> DofMap::copy() const
{
  return std::shared_ptr<GenericDofMap>(new DofMap(*this));
}
//-----------------------------------------------------------------------------
std::shared_ptr<GenericDofMap> DofMap::create(const Mesh& new_mesh) const
{
  // Get underlying UFC dof map
  std::shared_ptr<const ufc::dofmap> ufc_dof_map(_ufc_dofmap);
  return std::shared_ptr<GenericDofMap>(new DofMap(ufc_dof_map, new_mesh));
}
//-----------------------------------------------------------------------------
std::shared_ptr<GenericDofMap>
  DofMap::extract_sub_dofmap(const std::vector<std::size_t>& component,
                             const Mesh& mesh) const
{
  return std::shared_ptr<GenericDofMap>(new DofMap(*this, component, mesh));
}
//-----------------------------------------------------------------------------
std::shared_ptr<GenericDofMap>
  DofMap::collapse(boost::unordered_map<std::size_t, std::size_t>&
                   collapsed_map,
                   const Mesh& mesh) const
{
  return std::shared_ptr<GenericDofMap>(new DofMap(collapsed_map,
                                                     *this, mesh));
}
//-----------------------------------------------------------------------------
std::vector<dolfin::la_index> DofMap::dofs() const
{
  // Ownership range
  const std::size_t r0 = _ownership_range.first;
  const std::size_t r1 = _ownership_range.second;

  // Create vector to hold dofs
  std::vector<la_index> _dofs;
  _dofs.reserve(_dofmap.size()*max_cell_dimension());

  // Insert all dofs into a vector (will contain duplicates)
  std::vector<std::vector<dolfin::la_index> >::const_iterator cell_dofs;
  for (cell_dofs = _dofmap.begin(); cell_dofs != _dofmap.end(); ++cell_dofs)
  {
    for (std::size_t i = 0; i < cell_dofs->size(); ++i)
    {
      const std::size_t dof = (*cell_dofs)[i];
      if (dof >= r0 && dof < r1)
        _dofs.push_back(dof);
    }
  }

  // Sort dofs (required to later remove duplicates)
  std::sort(_dofs.begin(), _dofs.end());

  // Remove duplicates
  _dofs.erase(std::unique(_dofs.begin(), _dofs.end() ), _dofs.end());

  return _dofs;
}
//-----------------------------------------------------------------------------
void DofMap::set(GenericVector& x, double value) const
{
  std::vector<double> _value;
  std::vector<std::vector<dolfin::la_index> >::const_iterator cell_dofs;
  for (cell_dofs = _dofmap.begin(); cell_dofs != _dofmap.end(); ++cell_dofs)
  {
    _value.resize(cell_dofs->size(), value);
    x.set(_value.data(), cell_dofs->size(), cell_dofs->data());
  }
  x.apply("insert");
}
//-----------------------------------------------------------------------------
void DofMap::set_x(GenericVector& x, double value, std::size_t component,
                   const Mesh& mesh) const
{
  std::vector<double> x_values;
  boost::multi_array<double, 2> coordinates;
  std::vector<double> vertex_coordinates;
  for (CellIterator cell(mesh); !cell.end(); ++cell)
  {
    // Update UFC cell
    cell->get_vertex_coordinates(vertex_coordinates);

    // Get local-to-global map
    const std::vector<dolfin::la_index>& dofs = cell_dofs(cell->index());

    // Tabulate dof coordinates
    tabulate_coordinates(coordinates, vertex_coordinates, *cell);
    dolfin_assert(coordinates.shape()[0] == dofs.size());
    dolfin_assert(component < coordinates.shape()[1]);

    // Copy coordinate (it may be possible to avoid this)
    x_values.resize(dofs.size());
    for (std::size_t i = 0; i < coordinates.shape()[0]; ++i)
      x_values[i] = value*coordinates[i][component];

    // Set x[component] values in vector
    x.set(x_values.data(), dofs.size(), dofs.data());
  }
}
//-----------------------------------------------------------------------------
void DofMap::check_provided_entities(const ufc::dofmap& dofmap,
                                     const Mesh& mesh)
{
  // Check that we have all mesh entities
  for (std::size_t d = 0; d <= mesh.topology().dim(); ++d)
  {
    if (dofmap.needs_mesh_entities(d) && mesh.num_entities(d) == 0)
    {
      dolfin_error("DofMap.cpp",
                   "initialize mapping of degrees of freedom",
                   "Missing entities of dimension %d. Try calling mesh.init(%d)", d, d);
    }
  }
}
//-----------------------------------------------------------------------------
std::string DofMap::str(bool verbose) const
{
  // TODO: Display information on parallel stuff

  // Prefix with process number if running in parallel
  std::stringstream prefix;
  //if (MPI::size() > 1)
  //  prefix << "Process " << MPI::rank() << ": ";

  std::stringstream s;
  s << prefix.str() << "<DofMap of global dimension " << global_dimension()
    << ">" << std::endl;
  if (verbose)
  {
    // Cell loop
    for (std::size_t i = 0; i < _dofmap.size(); ++i)
    {
      s << prefix.str() << "Local cell index, cell dofmap dimension: " << i
        << ", " << _dofmap[i].size() << std::endl;

      // Local dof loop
      for (std::size_t j = 0; j < _dofmap[i].size(); ++j)
      {
        s << prefix.str() <<  "  " << "Local, global dof indices: " << j
          << ", " << _dofmap[i][j] << std::endl;
      }
    }
  }

  return s.str();
}
//-----------------------------------------------------------------------------
void DofMap::check_dimensional_consistency(const ufc::dofmap& dofmap,
                                            const Mesh& mesh)
{
  // Check geometric dimension
  if (dofmap.geometric_dimension() != mesh.geometry().dim())
  {
    dolfin_error("DofMap.cpp",
                 "create mapping of degrees of freedom",
                 "Geometric dimension of the UFC dofmap (dim = %d) and the mesh (dim = %d) do not match",
                 dofmap.geometric_dimension(),
                 mesh.geometry().dim());
  }

  // Check topological dimension
  if (dofmap.topological_dimension() != mesh.topology().dim())
  {
    dolfin_error("DofMap.cpp",
                 "create mapping of degrees of freedom",
                 "Topological dimension of the UFC dofmap (dim = %d) and the mesh (dim = %d) do not match",
                 dofmap.topological_dimension(),
                 mesh.topology().dim());
  }
}
//-----------------------------------------------------------------------------
