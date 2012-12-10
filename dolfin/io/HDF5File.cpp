// Copyright (C) 2012 Chris N Richardson
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
// Modified by Garth N. Wells, 2012
//
// First added:  2012-06-01
// Last changed: 2012-12-10

#ifdef HAS_HDF5

#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign.hpp>
#include <boost/multi_array.hpp>
#include <boost/bind.hpp>

#include <dolfin/common/types.h>
#include <dolfin/common/constants.h>
#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/Timer.h>
#include <dolfin/function/Function.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/log/log.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/LocalMeshData.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshPartitioning.h>
#include <dolfin/mesh/MeshEntityIterator.h>
#include <dolfin/mesh/MeshFunction.h>
#include <dolfin/mesh/Vertex.h>

#include "HDF5File.h"
#include "HDF5Interface.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
HDF5File::HDF5File(const std::string filename, const std::string file_mode, bool use_mpiio)
  : hdf5_file_open(false), hdf5_file_id(0),
    mpi_io(MPI::num_processes() > 1 && use_mpiio ? true : false)
{
  // HDF5 chunking
  parameters.add("chunking", false);

  // OPen HDF5 file
  hdf5_file_id = HDF5Interface::open_file(filename, file_mode, mpi_io);
  hdf5_file_open = true;
}
//-----------------------------------------------------------------------------
HDF5File::~HDF5File()
{
  // Close HDF5 file
  if (hdf5_file_open)
  {
    herr_t status = H5Fclose(hdf5_file_id);
    dolfin_assert(status != HDF5_FAIL);
  }
}
//-----------------------------------------------------------------------------
void HDF5File::flush()
{
  dolfin_assert(hdf5_file_open);
  HDF5Interface::flush_file(hdf5_file_id);
}
//-----------------------------------------------------------------------------
void HDF5File::write(const GenericVector& x, const std::string name)
{
  dolfin_assert(x.size() > 0);
  dolfin_assert(hdf5_file_open);

  // Create Vector group in HDF5 file
  if (!HDF5Interface::has_group(hdf5_file_id, "/Vector"))
    HDF5Interface::add_group(hdf5_file_id, "/Vector");

  // Get all local data
  std::vector<double> local_data;
  x.get_local(local_data);

  // Form HDF5 dataset tag
  const std::string dataset_name = "/Vector/" + name;

  // Write data to file
  std::pair<std::size_t, std::size_t> local_range = x.local_range();
  const bool chunking = parameters["chunking"];
  const std::vector<std::size_t> global_size(1, x.size());
  HDF5Interface::write_dataset(hdf5_file_id, dataset_name, local_data,
                               local_range, global_size, mpi_io, chunking);

  // Add partitioning attribute to dataset
  std::vector<std::size_t> partitions;
  MPI::gather(local_range.first, partitions);
  MPI::broadcast(partitions);

  HDF5Interface::add_attribute(hdf5_file_id, dataset_name, "partition",
                               partitions);
}
//-----------------------------------------------------------------------------
void HDF5File::write(const Mesh& mesh, const std::string name)
{
  write(mesh, mesh.topology().dim(), name);
}
//-----------------------------------------------------------------------------
void HDF5File::write(const Mesh& mesh, std::size_t cell_dim, const std::string name)
{
  dolfin_assert(hdf5_file_open);

  // Create Mesh group in HDF5 file
  if (!HDF5Interface::has_group(hdf5_file_id, "/Mesh"))
    HDF5Interface::add_group(hdf5_file_id, "/Mesh");

  CellType::Type _cell_type = mesh.type().cell_type();
  if (cell_dim == mesh.topology().dim())
    _cell_type = mesh.type().cell_type();
  else if (cell_dim == mesh.topology().dim() - 1)
    _cell_type = mesh.type().facet_type();
  else
  {
    dolfin_error("HDF5File.cpp",
                 "write mesh to file",
                 "Only Mesh for Mesh facets can be written to file");
  }

  const std::string cell_type = CellType::type2string(_cell_type);

  // ---------- Coordinates

  const std::vector<std::size_t>& global_indices = mesh.topology().global_indices(0);

  {
    // Write vertex data to HDF5 file
    const std::string coord_dataset = name + "/coordinates";

    // Copy coordinates and indices and remove off-process values
    std::vector<double> vertex_coords(mesh.coordinates());
    const std::size_t gdim = mesh.geometry().dim();
    reorder_vertices_by_global_indices(vertex_coords, gdim, global_indices);

    // Write coordinates out from each process
    std::vector<std::size_t> global_size(2);
    global_size[0] = MPI::sum(vertex_coords.size()/gdim); // reduced after reorder
    global_size[1] = gdim;
    write_data(coord_dataset, vertex_coords, global_size);
  }

  // ---------- Topology

  // Get/build topology data
  std::vector<std::size_t> topological_data;
  if (cell_dim == mesh.topology().dim())
  {
    topological_data.resize(mesh.cells().size());
    // reindex using global indices
    std::transform(mesh.cells().begin(), mesh.cells().end(),
         topological_data.begin(),
         boost::bind<const std::size_t &>(&std::vector<std::size_t>::at,
                                          &global_indices, _1));
  }
  else
  {
    topological_data.reserve(mesh.num_entities(cell_dim)*(cell_dim + 1));
    for (MeshEntityIterator c(mesh, cell_dim); !c.end(); ++c)
      for (VertexIterator v(*c); !v.end(); ++v)
        topological_data.push_back(v->global_index());
  }

  // Write topology data
  const std::string topology_dataset = name + "/topology";
  std::vector<std::size_t> global_size(2);
  global_size[0] = MPI::sum(topological_data.size()/(cell_dim + 1));
  global_size[1] = cell_dim + 1;
  write_data(topology_dataset, topological_data, global_size);

  HDF5Interface::add_attribute(hdf5_file_id, topology_dataset, "celltype",
                               cell_type);
}
//-----------------------------------------------------------------------------
void HDF5File::write_visualisation(const Mesh& mesh, const std::string name)
{
  write_visualisation(mesh, mesh.topology().dim(), name);
}
//-----------------------------------------------------------------------------
void HDF5File::write_visualisation(const Mesh& mesh, const std::size_t cell_dim,
                                   const std::string name)
{
  dolfin_assert(hdf5_file_open);

  // Create VisualisationMesh group in HDF5 file
  if (!HDF5Interface::has_group(hdf5_file_id, "/VisualisationMesh"))
    HDF5Interface::add_group(hdf5_file_id, "/VisualisationMesh");

  CellType::Type _cell_type = mesh.type().cell_type();
  if (cell_dim == mesh.topology().dim())
    _cell_type = mesh.type().cell_type();
  else if (cell_dim == mesh.topology().dim() - 1)
    _cell_type = mesh.type().facet_type();
  else
  {
    dolfin_error("HDF5File.cpp",
                 "write mesh to file",
                 "Only Mesh for Mesh facets can be written to file");
  }

  // Cell type string
  const std::string cell_type = CellType::type2string(_cell_type);

  // Vertex numbers, ranges and offsets
  const std::size_t num_local_vertices = mesh.num_vertices();
  const std::size_t vertex_offset = MPI::global_offset(num_local_vertices, true);

  // Write vertex data to HDF5 file
  const std::string coord_dataset = name + "/coordinates";
  {
    const std::size_t gdim = mesh.geometry().dim();
    const std::vector<double>& vertex_coords = mesh.coordinates();

    // Write coordinates contiguously from each process
    std::vector<std::size_t> global_size(2);
    global_size[0] = MPI::sum(num_local_vertices);
    global_size[1] = gdim;
    write_data(coord_dataset, vertex_coords, global_size);
  }

  // Write connectivity to HDF5 file (using local indices + offset)
  {
    // Get/build topology data
    std::vector<std::size_t> topological_data;
    if (cell_dim == mesh.topology().dim())
    {
      topological_data = mesh.cells();
      std::transform(topological_data.begin(), topological_data.end(),
                     topological_data.begin(),
                     std::bind2nd(std::plus<std::size_t>(), vertex_offset));
    }
    else
    {
      topological_data.reserve(mesh.num_entities(cell_dim)*(cell_dim + 1));
      for (MeshEntityIterator c(mesh, cell_dim); !c.end(); ++c)
        for (VertexIterator v(*c); !v.end(); ++v)
         topological_data.push_back(v->index() + vertex_offset);
    }

    // Write topology data
    const std::string topology_dataset = name + "/topology";
    std::vector<std::size_t> global_size(2);
    global_size[0] = MPI::sum(topological_data.size()/(cell_dim + 1));
    global_size[1] = cell_dim + 1;
    write_data(topology_dataset, topological_data, global_size);

    HDF5Interface::add_attribute(hdf5_file_id, topology_dataset, "celltype",
                                 cell_type);
  }
}
//-----------------------------------------------------------------------------
void HDF5File::read(GenericVector& x, const std::string dataset_name,
                    const bool use_partition_from_file)
{
  dolfin_assert(hdf5_file_open);

  const std::string _dataset_name = "/Vector/" + dataset_name;

  // Check for data set exists
  if (!HDF5Interface::has_group(hdf5_file_id, _dataset_name))
    error("Data set with name \"%s\" does not exist", _dataset_name.c_str());

  // Get dataset rank
  const std::size_t rank = HDF5Interface::dataset_rank(hdf5_file_id, _dataset_name);
  dolfin_assert(rank == 1);

  // Get global dataset size
  const std::vector<std::size_t> data_size
      = HDF5Interface::get_dataset_size(hdf5_file_id, _dataset_name);

  // Check that rank is 1
  dolfin_assert(data_size.size() == 1);

  // Check input vector, and re-size if not already sized
  if (x.size() == 0)
  {
    // Resize vector
    if (use_partition_from_file)
    {
      // Get partition from file
      std::vector<std::size_t> partitions;
      HDF5Interface::get_attribute(hdf5_file_id, _dataset_name, "partition", partitions);

      // Check that number of MPI processes matches partitioning
      if(MPI::num_processes() != partitions.size())
      {
        dolfin_error("HDF5File.cpp",
                     "read vector from file",
                     "Different number of processes used when writing. Cannot restore partitioning");
      }

      // Add global size at end of partition vectors
      partitions.push_back(data_size[0]);

      // Initialise vector
      const std::size_t process_num = MPI::process_number();
      const std::pair<std::size_t, std::size_t>
          local_range(partitions[process_num], partitions[process_num + 1]);
      x.resize(local_range);
    }
    else
      x.resize(data_size[0]);
  }
  else if (x.size() != data_size[0])
  {
    dolfin_error("HDF5File.cpp",
                 "read vector from file",
                 "Size mis-match between vector in file and input vector");
  }

  // Get local range
  const std::pair<std::size_t, std::size_t> local_range = x.local_range();

  // Read data from file
  std::vector<double> data;
  HDF5Interface::read_dataset(hdf5_file_id, _dataset_name, local_range, data);

  // Set data
  x.set_local(data);
}
//-----------------------------------------------------------------------------
std::string HDF5File::search_list(const std::vector<std::string>& list,
                                  const std::string& search_term)
{
  std::vector<std::string>::const_iterator it;
  for (it = list.begin(); it != list.end(); ++it)
  {
    if (it->find(search_term) != std::string::npos)
      return *it;
  }
  return std::string("");
}
//-----------------------------------------------------------------------------
void HDF5File::read(Mesh& input_mesh, const std::string name)
{
  dolfin_assert(hdf5_file_open);

  std::vector<std::string> _dataset_list =
    HDF5Interface::dataset_list(hdf5_file_id, name);

  std::string topology_name = search_list(_dataset_list,"topology");
  if (topology_name.size() == 0)
  {
    dolfin_error("HDF5File.cpp",
                 "read topology dataset",
                 "Dataset not found");
  }
  topology_name = name + "/" + topology_name;

  // Look for Coordinates dataset
  std::string coordinates_name=search_list(_dataset_list,"coordinates");
  if(coordinates_name.size()==0)
  {
    dolfin_error("HDF5File.cpp",
                 "read coordinates dataset",
                 "Dataset not found");
  }
  coordinates_name = name + "/" + coordinates_name;

  read_mesh_repartition(input_mesh, coordinates_name,
                                   topology_name);
}
//-----------------------------------------------------------------------------
void HDF5File::read_mesh_repartition(Mesh& input_mesh,
                                     const std::string coordinates_name,
                                     const std::string topology_name)
{
  Timer t("HDF5: ReadMesh");

  // Structure to store local mesh
  LocalMeshData mesh_data;
  mesh_data.clear();

  // --- Topology ---
  // Discover size of topology dataset
  std::vector<std::size_t> topology_dim
      = HDF5Interface::get_dataset_size(hdf5_file_id, topology_name);

  // Get total number of cells, as number of rows in topology dataset
  const std::size_t num_global_cells = topology_dim[0];
  mesh_data.num_global_cells = num_global_cells;

  // Set vertices-per-cell from number of columns
  const std::size_t num_vertices_per_cell = topology_dim[1];
  mesh_data.num_vertices_per_cell = num_vertices_per_cell;
  mesh_data.tdim = topology_dim[1] - 1;

  // Divide up cells ~equally between processes
  const std::pair<std::size_t,std::size_t> cell_range = MPI::local_range(num_global_cells);
  const std::size_t num_local_cells = cell_range.second - cell_range.first;

  // Read a block of cells
  std::vector<std::size_t> topology_data;
  topology_data.reserve(num_local_cells*num_vertices_per_cell);
  HDF5Interface::read_dataset(hdf5_file_id, topology_name, cell_range, topology_data);

  mesh_data.global_cell_indices.reserve(num_local_cells);
  for(std::size_t i = 0; i < num_local_cells; i++)
    mesh_data.global_cell_indices.push_back(cell_range.first + i);

  // Copy to boost::multi_array
  mesh_data.cell_vertices.resize(boost::extents[num_local_cells][num_vertices_per_cell]);
  std::copy(topology_data.begin(), topology_data.end(),
            mesh_data.cell_vertices.data());

  // --- Coordinates ---
  // Get dimensions of coordinate dataset
  std::vector<std::size_t> coords_dim
    = HDF5Interface::get_dataset_size(hdf5_file_id, coordinates_name);
  mesh_data.num_global_vertices = coords_dim[0];
  mesh_data.gdim = coords_dim[1];

  // Divide range into equal blocks for each process
  const std::pair<std::size_t, std::size_t> vertex_range =
                          MPI::local_range(mesh_data.num_global_vertices);
  const std::size_t num_local_vertices = vertex_range.second - vertex_range.first;

  // Read vertex data to temporary vector
  std::vector<double> coordinates_data;
  coordinates_data.reserve(num_local_vertices*mesh_data.gdim);
  HDF5Interface::read_dataset(hdf5_file_id, coordinates_name, vertex_range,
                              coordinates_data);

  // Copy to boost::multi_array
  mesh_data.vertex_coordinates.resize(boost::extents[num_local_vertices][mesh_data.gdim]);
  std::copy(coordinates_data.begin(), coordinates_data.end(),
            mesh_data.vertex_coordinates.data());

  // Fill vertex indices with values - not used in build_distributed_mesh
  mesh_data.vertex_indices.resize(num_local_vertices);
  for(std::size_t i = 0; i < mesh_data.vertex_coordinates.size(); ++i)
    mesh_data.vertex_indices[i] = vertex_range.first + i;

  // Build distributed mesh

  t.stop();

  if(MPI::num_processes() == 1)
    build_local_mesh(input_mesh, mesh_data);
  else
    MeshPartitioning::build_distributed_mesh(input_mesh, mesh_data);
}
//-----------------------------------------------------------------------------
void HDF5File::build_local_mesh(Mesh &mesh, const LocalMeshData& mesh_data)
{
  // Create mesh for editing
  MeshEditor editor;
  std::string cell_type_str;

  switch(mesh_data.tdim)
  {
  case 1:
    cell_type_str = "interval";
    break;
  case 2:
    cell_type_str = "triangle";
    break;
  case 3:
    cell_type_str = "tetrahedron";
    break;
  default:
    dolfin_error("HDF5File.cpp","resolve cell type","Topological dimension out of range");
  }

  editor.open(mesh, cell_type_str, mesh_data.tdim, mesh_data.gdim);
  editor.init_vertices(mesh_data.num_global_vertices);

  // Iterate over vertices and add to mesh
  for (std::size_t i = 0; i < mesh_data.num_global_vertices; ++i)
  {
    const std::size_t index = mesh_data.vertex_indices[i];
    const std::vector<double> coords(mesh_data.vertex_coordinates[i].begin(),
                                     mesh_data.vertex_coordinates[i].end());
    Point p(mesh_data.gdim, coords.data());
    editor.add_vertex(index, p);
  }

  editor.init_cells(mesh_data.num_global_cells);

  // Iterate over cells and add to mesh
  for (std::size_t i = 0; i < mesh_data.num_global_cells; ++i)
  {
    const std::size_t index = mesh_data.global_cell_indices[i];
    const std::vector<std::size_t> v(mesh_data.cell_vertices[i].begin(), mesh_data.cell_vertices[i].end());
    editor.add_cell(index, v);
  }

  // Close mesh editor
  editor.close();
}
//-----------------------------------------------------------------------------
bool HDF5File::has_dataset(const std::string dataset_name) const
{
  dolfin_assert(hdf5_file_open);
  return HDF5Interface::has_dataset(hdf5_file_id, dataset_name);
}
//-----------------------------------------------------------------------------
void HDF5File::reorder_vertices_by_global_indices(std::vector<double>& vertex_coords, std::size_t gdim,
                                                  const std::vector<std::size_t>& global_indices)
{
  Timer t("HDF5: reorder vertices");
  // FIXME: be more efficient with MPI

  dolfin_assert(gdim*global_indices.size() == vertex_coords.size());

  boost::multi_array_ref<double, 2> vertex_array(vertex_coords.data(),
                      boost::extents[vertex_coords.size()/gdim][gdim]);

  // Calculate size of overall global vector by finding max index value
  // anywhere
  const std::size_t global_vector_size
    = MPI::max(*std::max_element(global_indices.begin(), global_indices.end())) + 1;

  // Send unwanted values off process
  const std::size_t num_processes = MPI::num_processes();
  std::vector<std::vector<std::pair<std::size_t, std::vector<double> > > > values_to_send(num_processes);
  std::vector<std::size_t> destinations(num_processes);

  // Set up destination vector for communication with remote processes
  for(std::size_t process_j = 0; process_j < num_processes ; ++process_j)
    destinations[process_j] = process_j;

  // Go through local vector and append value to the appropriate list
  // to send to correct process
  for(std::size_t i = 0; i < vertex_array.shape()[0] ; ++i)
  {
    const std::size_t global_i = global_indices[i];
    const std::size_t process_i = MPI::index_owner(global_i, global_vector_size);
    const std::vector<double> v(vertex_array[i].begin(), vertex_array[i].end());
    values_to_send[process_i].push_back(make_pair(global_i, v));
  }

  // Redistribute the values to the appropriate process - including self
  // All values are "in the air" at this point, so local vector can be cleared
  std::vector<std::vector<std::pair<std::size_t,std::vector<double> > > > received_values;
  MPI::distribute(values_to_send, destinations, received_values);

  // When receiving, just go through all received values
  // and place them in the local partition of the global vector.
  std::pair<std::size_t, std::size_t> range = MPI::local_range(global_vector_size);
  vertex_coords.resize((range.second - range.first)*gdim);
  boost::multi_array_ref<double, 2> new_vertex_array(vertex_coords.data(),
                     boost::extents[range.second - range.first][gdim]);

  for(std::size_t i = 0; i < received_values.size(); ++i)
  {
    const std::vector<std::pair<std::size_t, std::vector<double> > >& received_global_data = received_values[i];
    for(std::size_t j = 0; j < received_global_data.size(); ++j)
    {
      const std::size_t global_i = received_global_data[j].first;
      dolfin_assert(global_i >= range.first && global_i < range.second);
      std::copy(received_global_data[j].second.begin(),
                received_global_data[j].second.end(),
                new_vertex_array[global_i - range.first].begin());
    }
  }
}
//-----------------------------------------------------------------------------

#endif
