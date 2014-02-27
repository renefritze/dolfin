// Copyright (C) 2012 Chris N. Richardson and Garth N. Wells
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

#ifdef HAS_HDF5

#include <ostream>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/assign.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "pugixml.hpp"

#include <dolfin/common/MPI.h>
#include <dolfin/function/Function.h>
#include <dolfin/function/FunctionSpace.h>
#include <dolfin/fem/GenericDofMap.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/DistributedMeshTools.h>
#include <dolfin/mesh/MeshEntityIterator.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/Vertex.h>
#include "HDF5File.h"
#include "HDF5Utility.h"
#include "XDMFFile.h"

using namespace dolfin;

//----------------------------------------------------------------------------
XDMFFile::XDMFFile(MPI_Comm comm, const std::string filename)
  : GenericFile(filename, "XDMF"), _mpi_comm(comm)
{
  // Make name for HDF5 file (used to store data)
  boost::filesystem::path p(filename);
  p.replace_extension(".h5");
  hdf5_filename = p.string();

  // File mode will be set when reading or writing
  hdf5_filemode = "";

  // Rewrite the mesh at every time step in a time series. Should be
  // turned off if the mesh remains constant.
  parameters.add("rewrite_function_mesh", true);

  // Flush datasets to disk at each timestep. Allows inspection of the
  // HDF5 file whilst running, at some performance cost.
  parameters.add("flush_output", false);
}
//----------------------------------------------------------------------------
XDMFFile::~XDMFFile()
{
  // Do nothing
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const Function& u)
{
  std::pair<const Function*, double> ut(&u, (double) counter);
  *this << ut;
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const std::pair<const Function*, double> ut)
{
  // Prepate HDF5 file
  if (hdf5_filemode != "w")
  {
    // Create HDF5 file (truncate)
    hdf5_file.reset(new HDF5File(_mpi_comm, hdf5_filename, "w"));
    hdf5_filemode = "w";
  }
  dolfin_assert(hdf5_file);

  // Access Function, Mesh, dofmap  and time step
  dolfin_assert(ut.first);
  const Function& u = *(ut.first);

  dolfin_assert(u.function_space()->mesh());
  const Mesh& mesh = *u.function_space()->mesh();

  dolfin_assert(u.function_space()->dofmap());
  const GenericDofMap& dofmap = *u.function_space()->dofmap();

  const double time_step = ut.second;

  // FIXME: Can we avoid this?
  // Update any ghost values
  u.update();

  // Geometric and topological dimension
  const std::size_t gdim = mesh.geometry().dim();
  const std::size_t tdim = mesh.topology().dim();

  // Get some Function and cell information
  const std::size_t value_rank = u.value_rank();
  const std::size_t value_size = u.value_size();

  std::size_t padded_value_size = value_size;

  // Test for cell-centred data
  std::size_t cell_based_dim = 1;
  for (std::size_t i = 0; i < value_rank; i++)
    cell_based_dim *= tdim;
  const bool vertex_data = !(dofmap.max_cell_dimension() == cell_based_dim);

  // Get number of local/global cells/vertices
  const std::size_t num_local_cells = mesh.num_cells();
  const std::size_t num_local_vertices = mesh.num_vertices();
  const std::size_t num_global_cells = mesh.size_global(tdim);

  // Get Function data at vertices/cell centres
  std::vector<double> data_values;

  if (vertex_data)
  {
    u.compute_vertex_values(data_values, mesh);

    // Interleave the values for vector or tensor fields and pad 2D
    // vectors and tensors to 3D
    if (value_rank > 0)
    {
      if (value_size == 2)
        padded_value_size = 3;
      if (value_size == 4)
        padded_value_size = 9;

      std::vector<double> _data_values(padded_value_size*num_local_vertices,
                                       0.0);
      for (std::size_t i = 0; i < num_local_vertices; i++)
      {
        for (std::size_t j = 0; j < value_size; j++)
        {
          std::size_t tensor_2d_offset = (j > 1 && value_size == 4) ? 1 : 0;
          _data_values[i*padded_value_size + j + tensor_2d_offset]
              = data_values[i + j*num_local_vertices];
        }
      }
      data_values = _data_values;
    }
  }
  else
  {
    dolfin_assert(u.function_space()->dofmap());
    dolfin_assert(u.vector());

    // Allocate memory for function values at cell centres
    const std::size_t size = num_local_cells*value_size;

    // Build lists of dofs and create map
    std::vector<dolfin::la_index> dof_set;
    std::vector<std::size_t> offset(size + 1);
    std::vector<std::size_t>::iterator cell_offset = offset.begin();
    for (CellIterator cell(mesh); !cell.end(); ++cell)
    {
      // Tabulate dofs
      const std::vector<dolfin::la_index>& dofs
        = dofmap.cell_dofs(cell->index());
      for (std::size_t i = 0; i < dofmap.cell_dimension(cell->index()); ++i)
        dof_set.push_back(dofs[i]);

      // Add local dimension to cell offset and increment
      *(cell_offset + 1) = *(cell_offset)
        + dofmap.cell_dimension(cell->index());
      ++cell_offset;
    }

    // Get  values
    data_values.resize(dof_set.size());
    dolfin_assert(u.vector());
    u.vector()->get_local(data_values.data(), dof_set.size(), dof_set.data());

    if (value_size == 2)
      padded_value_size = 3;
    if (value_size == 4)
      padded_value_size = 9;

    cell_offset = offset.begin();
    std::vector<double> _data_values(padded_value_size*num_local_cells, 0.0);
    std::size_t count = 0;
    if (value_rank == 1 && value_size == 2)
    {
      for (CellIterator cell(mesh); !cell.end(); ++cell)
      {
        _data_values[count++] = data_values[*cell_offset];
        _data_values[count++] = data_values[*cell_offset + 1];
        count++;
      }
      ++cell_offset;
    }
    else if (value_rank == 2 && value_size == 4)
    {
      // Pad with 0.0 to 2D tensors to make them 3D
      for (CellIterator cell(mesh); !cell.end(); ++cell)
      {
        for (std::size_t i = 0; i < 2; i++)
        {
          _data_values[count++] = data_values[*cell_offset + 2*i];
          _data_values[count++] = data_values[*cell_offset + 2*i + 1];
          count++;
        }
        count += 3;
        ++cell_offset;
      }
    }
    else
    {
      // Write all components
      for (CellIterator cell(mesh); !cell.end(); ++cell)
      {
        for (std::size_t i = 0; i < value_size; i++)
          _data_values[count++] = data_values[*cell_offset + i];
        ++cell_offset;
      }
    }
    data_values = _data_values;
  }

  // FIXME: Below is messy. Should query HDF5 file writer for existing
  //        mesh name
  // Write mesh to HDF5 file
  if (parameters["rewrite_function_mesh"] || counter == 0)
  {
      current_mesh_name = "/Mesh/" + boost::lexical_cast<std::string>(counter);
      hdf5_file->write(mesh, current_mesh_name);
  }

  // Remove duplicates for vertex-based data
  std::vector<std::size_t> global_size(2);
  global_size[1] = padded_value_size;
  if (vertex_data)
  {
    HDF5Utility::reorder_values_by_global_indices(mesh, data_values,
                                                  padded_value_size);
    global_size[0] = mesh.size_global(0);
  }
  else
    global_size[0] = mesh.size_global(tdim);

  // Save data values to HDF5 file.  Vertex/cell values are saved in
  // the hdf5 group /VisualisationVector as distinct from /Vector
  // which is used for solution vectors.
  const std::string dataset_name = "/VisualisationVector/"
    + boost::lexical_cast<std::string>(counter);

  const bool mpi_io = MPI::size(mesh.mpi_comm()) > 1 ? true : false;
  hdf5_file->write_data(dataset_name, data_values, global_size, mpi_io);

  // Flush file. Improves chances of recovering data if
  // interrupted. Also makes file somewhat readable between writes.
  if (parameters["flush_output"])
    hdf5_file->flush();

  // Write the XML meta description (see http://www.xdmf.org) on
  // process zero
  const std::size_t num_total_vertices = mesh.size_global(0);
  if (MPI::rank(mesh.mpi_comm()) == 0)
  {
    output_xml(time_step, vertex_data, tdim, num_global_cells, gdim,
               num_total_vertices, value_rank, padded_value_size,
               u.name(), dataset_name);
  }

  // Increment counter
  counter++;
}
//----------------------------------------------------------------------------
void XDMFFile::operator>> (Mesh& mesh)
{
  read(mesh, false);
}
//-----------------------------------------------------------------------------
void XDMFFile::read(Mesh& mesh, bool use_partition_from_file)
{
  // Prepare HDF5 file
  if (hdf5_filemode != "r")
  {
    hdf5_file.reset(new HDF5File(_mpi_comm, hdf5_filename, "r"));
    hdf5_filemode = "r";
  }
  dolfin_assert(hdf5_file);

  // Prepare XML file
  pugi::xml_document xml_doc;
  pugi::xml_parse_result result = xml_doc.load_file(_filename.c_str());
  if (!result)
  {
    dolfin_error("XDMFFile.cpp",
                 "read mesh from XDMF/H5 files",
                 "XML parsing error. XDMF file should contain only one mesh/dataset");
  }

  // Topology - check format and get dataset name
  pugi::xml_node xdmf_topology
    = xml_doc.child("Xdmf").child("Domain").child("Grid").child("Topology").child("DataItem");
  if (!xdmf_topology)
  {
    dolfin_error("XDMFFile.cpp",
                 "read mesh from XDMF/H5 files",
                 "XML parsing error. XDMF file should contain only one mesh/dataset");
  }

  const std::string
    topological_data_format(xdmf_topology.attribute("Format").value());
  if (topological_data_format != "HDF")
  {
    dolfin_error("XDMFFile.cpp",
                 "read mesh from XDMF/H5 files",
                 "XML parsing error. Wrong dataset format (not HDF5)");
  }

  const std::string topo_ref(xdmf_topology.first_child().value());
  std::vector<std::string> topo_bits;
  boost::split(topo_bits, topo_ref, boost::is_any_of(":/"));

  // Should have 5 elements "filename.h5", "", "Mesh", "meshname", "topology"
  dolfin_assert(topo_bits.size() == 5);
  // FIXME: get path() from filename to check
  //  dolfin_assert(topo_bits[0] == hdf5_filename);
  dolfin_assert(topo_bits[2] == "Mesh");
  dolfin_assert(topo_bits[4] == "topology");

  // Geometry - check format and get dataset name
  pugi::xml_node xdmf_geometry =
    xml_doc.child("Xdmf").child("Domain").child("Grid").child("Geometry").child("DataItem");
  dolfin_assert(xdmf_geometry);

  const std::string geom_fmt(xdmf_geometry.attribute("Format").value());
  dolfin_assert(geom_fmt == "HDF");

  const std::string geom_ref(xdmf_geometry.first_child().value());
  std::vector<std::string> geom_bits;
  boost::split(geom_bits, geom_ref, boost::is_any_of(":/"));

  // Should have 5 elements "filename.h5", "", "Mesh", "meshname",
  // "coordinates"
  dolfin_assert(geom_bits.size() == 5);
  //  dolfin_assert(geom_bits[0] == hdf5_filename);
  dolfin_assert(geom_bits[2] == "Mesh");
  dolfin_assert(geom_bits[3] == topo_bits[3]);
  dolfin_assert(geom_bits[4] == "coordinates");

  // Try to read the mesh from the associated HDF5 file
  hdf5_file->read(mesh, "/Mesh/" + geom_bits[3], use_partition_from_file);
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const Mesh& mesh)
{
  // Write Mesh to HDF5 file

  if (hdf5_filemode != "w")
  {
    // Create HDF5 file (truncate)
    hdf5_file.reset(new HDF5File(mesh.mpi_comm(), hdf5_filename, "w"));
    hdf5_filemode = "w";
  }

  // Output data name
  const std::string name = mesh.name();

  // Topological and geometric dimensions
  const std::size_t gdim = mesh.geometry().dim();
  const std::size_t cell_dim = mesh.topology().dim();

  // Make sure entities are numbered
  DistributedMeshTools::number_entities(mesh, cell_dim);

  // Get number of global cells and vertices
  const std::size_t num_global_cells   = mesh.size_global(cell_dim);
  const std::size_t num_total_vertices = mesh.size_global(0);

  // Write mesh to HDF5 file
  // The XML below will obliterate any existing XDMF file

  const std::string group_name = "/Mesh/" + name;
  hdf5_file->write(mesh, cell_dim, group_name);

  // FIXME: Names should be returned by HDF5::write_mesh
  // Mesh data set names
  const std::string mesh_topology_name = group_name + "/topology";
  const std::string mesh_coords_name = group_name + "/coordinates";

  // Write the XML meta description on process zero
  if (MPI::rank(mesh.mpi_comm()) == 0)
  {
    // Create XML document
    pugi::xml_document xml_doc;

    // XML headers
    xml_doc.append_child(pugi::node_doctype).set_value("Xdmf SYSTEM \"Xdmf.dtd\" []");
    pugi::xml_node xdmf = xml_doc.append_child("Xdmf");
    xdmf.append_attribute("Version") = "2.0";
    xdmf.append_attribute("xmlns:xi") = "http://www.w3.org/2001/XInclude";
    pugi::xml_node xdmf_domain = xdmf.append_child("Domain");
    pugi::xml_node xdmf_grid = xdmf_domain.append_child("Grid");
    xdmf_grid.append_attribute("Name") = name.c_str();
    xdmf_grid.append_attribute("GridType") = "Uniform";

    // Describe topological connectivity
    pugi::xml_node xdmf_topology = xdmf_grid.append_child("Topology");
    xml_mesh_topology(xdmf_topology, cell_dim, num_global_cells,
                      mesh_topology_name);

    // Describe geometric coordinates
    pugi::xml_node xdmf_geometry = xdmf_grid.append_child("Geometry");
    xml_mesh_geometry(xdmf_geometry, num_total_vertices, gdim,
                      mesh_coords_name);

    xml_doc.save_file(_filename.c_str(), "  ");
  }
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const MeshFunction<bool>& meshfunction)
{
  write_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const MeshFunction<int>& meshfunction)
{
  write_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const MeshFunction<std::size_t>& meshfunction)
{
  write_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
void XDMFFile::operator<< (const MeshFunction<double>& meshfunction)
{
  write_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
void XDMFFile::write(const std::vector<Point>& points)
{
  // Intialise HDF5 file
  if (hdf5_filemode != "w")
  {
    // Create HDF5 file (truncate)
    hdf5_file.reset(new HDF5File(_mpi_comm, hdf5_filename, "w"));
    hdf5_filemode = "w";
  }

  // Get number of points (global)
  const std::size_t num_global_points = MPI::sum(_mpi_comm, points.size());

  // Write HDF5 file
  const std::string group_name = "/Points";
  hdf5_file->write(points, group_name);

  // The XML created below will obliterate any existing XDMF file
  write_point_xml(group_name, num_global_points, 0);
}
//----------------------------------------------------------------------------
void XDMFFile::write(const std::vector<Point>& points,
                     const std::vector<double>& values)
{
  // Write clouds of points to XDMF/HDF5 with values

  dolfin_assert(points.size() == values.size());

  // Intialise HDF5 file
  if (hdf5_filemode != "w")
  {
    // Create HDF5 file (truncate)
    hdf5_file.reset(new HDF5File(_mpi_comm, hdf5_filename, "w"));
    hdf5_filemode = "w";
  }

  // Get number of points (global)
  const std::size_t num_global_points = MPI::sum(_mpi_comm, points.size());

  // Write HDF5 file
  const std::string group_name = "/Points";
  hdf5_file->write(points, group_name);

  const std::string values_name = group_name + "/values";
  hdf5_file->write(values, values_name);

  // The XML created will obliterate any existing XDMF file
  write_point_xml(group_name, num_global_points, 1);
}
//----------------------------------------------------------------------------
void XDMFFile::write_point_xml(const std::string group_name,
                               const std::size_t num_global_points,
                               const unsigned int value_size)
{
  // Write the XML meta description on process zero
  if (MPI::rank(_mpi_comm) == 0)
  {
    // Dataset names
    const std::string mesh_coords_name = group_name + "/coordinates";
    const std::string values_name = group_name + "/values";

    // Create XML document
    pugi::xml_document xml_doc;

    // XML headers
    xml_doc.append_child(pugi::node_doctype).set_value("Xdmf SYSTEM \"Xdmf.dtd\" []");
    pugi::xml_node xdmf = xml_doc.append_child("Xdmf");
    xdmf.append_attribute("Version") = "2.0";
    xdmf.append_attribute("xmlns:xi") = "http://www.w3.org/2001/XInclude";
    pugi::xml_node xdmf_domain = xdmf.append_child("Domain");
    pugi::xml_node xdmf_grid = xdmf_domain.append_child("Grid");
    xdmf_grid.append_attribute("Name") = "Point cloud";
    xdmf_grid.append_attribute("GridType") = "Uniform";


    // Describe topological connectivity
    pugi::xml_node xdmf_topology = xdmf_grid.append_child("Topology");
    xdmf_topology.append_attribute("NumberOfElements")
      = (unsigned int) num_global_points;
    xdmf_topology.append_attribute("TopologyType") = "PolyVertex";
    xdmf_topology.append_attribute("NodesPerElement") = "1";

    // Describe geometric coordinates
    pugi::xml_node xdmf_geometry = xdmf_grid.append_child("Geometry");
    xml_mesh_geometry(xdmf_geometry, num_global_points, 3,
                      mesh_coords_name);

    if(value_size != 0)
    {
      dolfin_assert(value_size == 1 || value_size == 3);

      // Grid/Attribute (value data)
      pugi::xml_node xdmf_values = xdmf_grid.append_child("Attribute");
      xdmf_values.append_attribute("Name") = "point_values";

      if(value_size == 1)
        xdmf_values.append_attribute("AttributeType") = "Scalar";
      else
        xdmf_values.append_attribute("AttributeType") = "Vector";

      xdmf_values.append_attribute("Center") = "Node";

      pugi::xml_node xdmf_data = xdmf_values.append_child("DataItem");
      xdmf_data.append_attribute("Format") = "HDF";

      std::string s
        = boost::lexical_cast<std::string>(num_global_points) + " "
        + boost::lexical_cast<std::string>(value_size);

      xdmf_data.append_attribute("Dimensions") = s.c_str();

      boost::filesystem::path p(hdf5_filename);
      s = p.filename().string() + ":" + values_name;
      xdmf_data.append_child(pugi::node_pcdata).set_value(s.c_str());
    }

    xml_doc.save_file(_filename.c_str(), "  ");
  }
}
//----------------------------------------------------------------------------
template<typename T>
void XDMFFile::write_mesh_function(const MeshFunction<T>& meshfunction)
{
  // Get mesh
  dolfin_assert(meshfunction.mesh());
  const Mesh& mesh = *meshfunction.mesh();

  if (hdf5_filemode != "w")
  {
    // Create HDF5 file (truncate)
    hdf5_file.reset(new HDF5File(mesh.mpi_comm(), hdf5_filename, "w"));
    hdf5_filemode = "w";
  }

  if (meshfunction.size() == 0)
  {
    dolfin_error("XDMFFile.cpp",
                 "save empty MeshFunction",
                 "No values in MeshFunction");
  }

  const std::size_t cell_dim = meshfunction.dim();
  dolfin_assert(cell_dim <= mesh.topology().dim());

  // Use HDF5 function to output MeshFunction
  current_mesh_name = "/Mesh/" + boost::lexical_cast<std::string>(counter);
  hdf5_file->write(meshfunction, current_mesh_name);

  // Saved MeshFunction values are in the /Mesh group
  const std::string dataset_name =  current_mesh_name + "/values";

  // Write the XML meta description (see http://www.xdmf.org) on
  // process zero
  if (MPI::rank(mesh.mpi_comm()) == 0)
  {
    output_xml((double)counter, false,
               cell_dim, mesh.size_global(cell_dim),
               mesh.geometry().dim(), mesh.size_global(0),
               0, 1, meshfunction.name(), dataset_name);
  }

  counter++;
}
//----------------------------------------------------------------------------
void XDMFFile::operator>> (MeshFunction<bool>& meshfunction)
{
  const Mesh& mesh = *meshfunction.mesh();
  const std::size_t cell_dim = meshfunction.dim();

  MeshFunction<std::size_t> mf(mesh, cell_dim);
  read_mesh_function(mf);

  for (MeshEntityIterator cell(mesh, cell_dim); !cell.end(); ++cell)
    meshfunction[cell->index()] = (mf[cell->index()] == 1);
}
//----------------------------------------------------------------------------
void XDMFFile::operator>> (MeshFunction<int>& meshfunction)
{
  read_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
void XDMFFile::operator>> (MeshFunction<std::size_t>& meshfunction)
{
  read_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
void XDMFFile::operator>> (MeshFunction<double>& meshfunction)
{
  read_mesh_function(meshfunction);
}
//----------------------------------------------------------------------------
template<typename T>
void XDMFFile::read_mesh_function(MeshFunction<T>& meshfunction)
{
  if (hdf5_filemode != "r")
  {
    hdf5_file.reset(new HDF5File(_mpi_comm, hdf5_filename, "r"));
    hdf5_filemode = "r";
  }

  dolfin_assert(hdf5_file);

  pugi::xml_document xml_doc;
  pugi::xml_parse_result result = xml_doc.load_file(_filename.c_str());
  if (!result)
  {
    dolfin_error("XDMFFile.cpp",
                 "read mesh from XDMF/H5 files",
                 "XML parsing error when reading from file");
  }

  // Topology - check format and get dataset name
  pugi::xml_node xdmf_topology = xml_doc.child("Xdmf").child("Domain").
    child("Grid").child("Grid").child("Topology").child("DataItem");
  dolfin_assert(xdmf_topology);

  const std::string topo_fmt(xdmf_topology.attribute("Format").value());
  dolfin_assert(topo_fmt == "HDF");

  const std::string topo_ref(xdmf_topology.first_child().value());
  std::vector<std::string> topo_bits;
  boost::split(topo_bits, topo_ref, boost::is_any_of(":/"));

  // Should have 5 elements "filename.h5", "", "Mesh", "meshname", "topology"
  dolfin_assert(topo_bits.size() == 5);
  //  dolfin_assert(topo_bits[0] == hdf5_filename);
  dolfin_assert(topo_bits[2] == "Mesh");
  dolfin_assert(topo_bits[4] == "topology");

  // Geometry - check format and get dataset name
  pugi::xml_node xdmf_geometry = xml_doc.child("Xdmf").child("Domain").
    child("Grid").child("Grid").child("Geometry").child("DataItem");
  dolfin_assert(xdmf_geometry);

  const std::string geom_fmt(xdmf_geometry.attribute("Format").value());
  dolfin_assert(geom_fmt == "HDF");

  const std::string geom_ref(xdmf_geometry.first_child().value());
  std::vector<std::string> geom_bits;
  boost::split(geom_bits, geom_ref, boost::is_any_of(":/"));

  // Should have 5 elements "filename.h5", "", "Mesh", "meshname",
  // "coordinates"
  dolfin_assert(geom_bits.size() == 5);
  //  dolfin_assert(geom_bits[0] == hdf5_filename);
  dolfin_assert(geom_bits[2] == "Mesh");
  dolfin_assert(geom_bits[3] == topo_bits[3]);
  dolfin_assert(geom_bits[4] == "coordinates");

  // Values - check format and get dataset name
  pugi::xml_node xdmf_values = xml_doc.child("Xdmf").child("Domain").
    child("Grid").child("Grid").child("Attribute").child("DataItem");
  dolfin_assert(xdmf_values);

  const std::string value_fmt(xdmf_values.attribute("Format").value());
  dolfin_assert(value_fmt == "HDF");

  const std::string value_ref(xdmf_values.first_child().value());
  std::vector<std::string> value_bits;
  boost::split(value_bits, value_ref, boost::is_any_of(":/"));
  dolfin_assert(value_bits.size() == 5);
  //  dolfin_assert(geom_bits[0] == hdf5_filename);
  dolfin_assert(value_bits[2] == "Mesh");
  dolfin_assert(value_bits[3] == topo_bits[3]);
  dolfin_assert(value_bits[4] == "values");

  // Try to read the meshfunction from the associated HDF5 file
  hdf5_file->read(meshfunction, "/Mesh/" + geom_bits[3]);
}
//----------------------------------------------------------------------------
void XDMFFile::xml_mesh_topology(pugi::xml_node &xdmf_topology,
                                 const std::size_t cell_dim,
                                 const std::size_t num_global_cells,
                                 const std::string topology_dataset_name) const
{
  xdmf_topology.append_attribute("NumberOfElements")
    = (unsigned int) num_global_cells;

  // Cell type
  if (cell_dim == 0)
  {
    xdmf_topology.append_attribute("TopologyType") = "PolyVertex";
    xdmf_topology.append_attribute("NodesPerElement") = "1";
  }
  else if (cell_dim == 1)
  {
    xdmf_topology.append_attribute("TopologyType") = "PolyLine";
    xdmf_topology.append_attribute("NodesPerElement") = "2";
  }
  else if (cell_dim == 2)
    xdmf_topology.append_attribute("TopologyType") = "Triangle";
  else if (cell_dim == 3)
    xdmf_topology.append_attribute("TopologyType") = "Tetrahedron";

  // Refer to all cells and dimensions
  pugi::xml_node xdmf_topology_data = xdmf_topology.append_child("DataItem");
  xdmf_topology_data.append_attribute("Format") = "HDF";
  const std::string cell_dims
    = boost::lexical_cast<std::string>(num_global_cells)
    + " " + boost::lexical_cast<std::string>(cell_dim + 1);
  xdmf_topology_data.append_attribute("Dimensions") = cell_dims.c_str();

  // For XDMF file need to remove path from filename so that xdmf
  // filenames such as "results/data.xdmf" correctly index h5 files in
  // the same directory
  boost::filesystem::path p(hdf5_filename);
  std::string topology_reference = p.filename().string() + ":"
    + topology_dataset_name;
  xdmf_topology_data.append_child(pugi::node_pcdata).set_value(topology_reference.c_str());
}
//----------------------------------------------------------------------------
void XDMFFile::xml_mesh_geometry(pugi::xml_node& xdmf_geometry,
                                 const std::size_t num_total_vertices,
                                 const std::size_t gdim,
                                 const std::string geometry_dataset_name) const
{
  dolfin_assert(0 < gdim && gdim <= 3);
  std::string geometry_type;
  if (gdim == 1)
  {
    // geometry "X" is not supported in XDMF
    geometry_type = "X_Y_Z";
  }
  else if (gdim == 2)
    geometry_type = "XY";
  else if (gdim == 3)
    geometry_type = "XYZ";

  xdmf_geometry.append_attribute("GeometryType") = geometry_type.c_str();
  pugi::xml_node xdmf_geom_data = xdmf_geometry.append_child("DataItem");

  xdmf_geom_data.append_attribute("Format") = "HDF";
  std::string geom_dim = boost::lexical_cast<std::string>(num_total_vertices)
    + " " + boost::lexical_cast<std::string>(gdim);
  xdmf_geom_data.append_attribute("Dimensions") = geom_dim.c_str();

  if (gdim == 1)
  {
    // FIXME: improve this workaround

    // When gdim==1, XDMF does not support a 1D geometry "X", so need
    // to provide some dummy Y and Z values.  Using the "X_Y_Z"
    // geometry the Y and Z values can be supplied as separate
    // datasets, here in plain text (though it could be done in HDF5
    // too).

    // Cannot write HDF5 here, as we are only running on rank 0, and
    // will deadlock.

    std::string dummy_zeros;
    dummy_zeros.reserve(2*num_total_vertices);
    for (std::size_t i = 0; i < num_total_vertices; ++i)
      dummy_zeros += "0 ";

    pugi::xml_node xdmf_geom_1 = xdmf_geometry.append_child("DataItem");
    xdmf_geom_1.append_attribute("Format") = "XML";
    geom_dim = boost::lexical_cast<std::string>(num_total_vertices) + " 1" ;
    xdmf_geom_1.append_attribute("Dimensions") = geom_dim.c_str();
    xdmf_geom_1.append_child(pugi::node_pcdata).set_value(dummy_zeros.c_str());

    pugi::xml_node xdmf_geom_2 = xdmf_geometry.append_child("DataItem");
    xdmf_geom_2.append_attribute("Format") = "XML";
    geom_dim = boost::lexical_cast<std::string>(num_total_vertices) + " 1" ;
    xdmf_geom_2.append_attribute("Dimensions") = geom_dim.c_str();
    xdmf_geom_2.append_child(pugi::node_pcdata).set_value(dummy_zeros.c_str());
  }

  boost::filesystem::path p(hdf5_filename);
  const std::string geometry_reference
    = p.filename().string() + ":" + geometry_dataset_name;
  xdmf_geom_data.append_child(pugi::node_pcdata).set_value(geometry_reference.c_str());
}
//----------------------------------------------------------------------------
void XDMFFile::output_xml(const double time_step, const bool vertex_data,
                          const std::size_t cell_dim,
                          const std::size_t num_global_cells,
                          const std::size_t gdim,
                          const std::size_t num_total_vertices,
                          const std::size_t value_rank,
                          const std::size_t padded_value_size,
                          const std::string name,
                          const std::string dataset_name) const
{
  // Working data structure for formatting XML file
  std::string s;
  pugi::xml_document xml_doc;
  pugi::xml_node xdmf_timegrid;
  pugi::xml_node xdmf_timedata;

  if (counter == 0)
  {
    // First time step - create document template, adding a mesh and
    // an empty time-series
    xml_doc.append_child(pugi::node_doctype).set_value("Xdmf SYSTEM \"Xdmf.dtd\" []");
    pugi::xml_node xdmf = xml_doc.append_child("Xdmf");
    xdmf.append_attribute("Version") = "2.0";
    xdmf.append_attribute("xmlns:xi") = "http://www.w3.org/2001/XInclude";
    pugi::xml_node xdmf_domain = xdmf.append_child("Domain");

    //  /Xdmf/Domain/Grid - actually a TimeSeries, not a spatial grid
    xdmf_timegrid = xdmf_domain.append_child("Grid");
    xdmf_timegrid.append_attribute("Name") = "TimeSeries";
    xdmf_timegrid.append_attribute("GridType") = "Collection";
    xdmf_timegrid.append_attribute("CollectionType") = "Temporal";

    //  /Xdmf/Domain/Grid/Time
    pugi::xml_node xdmf_time = xdmf_timegrid.append_child("Time");
    xdmf_time.append_attribute("TimeType") = "List";
    xdmf_timedata = xdmf_time.append_child("DataItem");
    xdmf_timedata.append_attribute("Format") = "XML";
    xdmf_timedata.append_attribute("Dimensions") = "1";
    xdmf_timedata.append_child(pugi::node_pcdata);
  }
  else
  {
    // Subsequent timestep - read in existing XDMF file
    pugi::xml_parse_result result = xml_doc.load_file(_filename.c_str());
    if (!result)
    {
      dolfin_error("XDMFFile.cpp",
                   "write data to XDMF file",
                   "XML parsing error when reading from existing file");
    }

    // Get data node
    xdmf_timegrid = xml_doc.child("Xdmf").child("Domain").child("Grid");
    dolfin_assert(xdmf_timegrid);

    // Get time series node
    xdmf_timedata = xdmf_timegrid.child("Time").child("DataItem");
    dolfin_assert(xdmf_timedata);
  }

  //  Add a time step to the TimeSeries List
  xdmf_timedata.attribute("Dimensions").set_value(static_cast<unsigned int>(counter + 1));
  s = boost::lexical_cast<std::string>(xdmf_timedata.first_child().value())
    + " " + boost::str((boost::format("%d") % time_step));
  xdmf_timedata.first_child().set_value(s.c_str());

  //   /Xdmf/Domain/Grid/Grid - the actual data for this timestep
  pugi::xml_node xdmf_grid = xdmf_timegrid.append_child("Grid");
  s = name + "_" + boost::lexical_cast<std::string>(counter);
  xdmf_grid.append_attribute("Name") = s.c_str();
  xdmf_grid.append_attribute("GridType") = "Uniform";

  // Grid/Topology
  pugi::xml_node xdmf_topology = xdmf_grid.append_child("Topology");
  xml_mesh_topology(xdmf_topology, cell_dim, num_global_cells,
                    current_mesh_name + "/topology");

  // Grid/Geometry
  pugi::xml_node xdmf_geometry = xdmf_grid.append_child("Geometry");
  xml_mesh_geometry(xdmf_geometry, num_total_vertices, gdim,
                    current_mesh_name + "/coordinates");

  // Grid/Attribute (Function value data)
  pugi::xml_node xdmf_values = xdmf_grid.append_child("Attribute");
  xdmf_values.append_attribute("Name") = name.c_str();

  if (value_rank == 0)
    xdmf_values.append_attribute("AttributeType") = "Scalar";
  else if (value_rank == 1)
    xdmf_values.append_attribute("AttributeType") = "Vector";
  else if (value_rank == 2)
    xdmf_values.append_attribute("AttributeType") = "Tensor";

  if (vertex_data)
    xdmf_values.append_attribute("Center") = "Node";
  else
    xdmf_values.append_attribute("Center") = "Cell";

  pugi::xml_node xdmf_data = xdmf_values.append_child("DataItem");
  xdmf_data.append_attribute("Format") = "HDF";

  const std::size_t num_total_entities
    = vertex_data ? num_total_vertices : num_global_cells;

  s = boost::lexical_cast<std::string>(num_total_entities) + " "
    + boost::lexical_cast<std::string>(padded_value_size);

  xdmf_data.append_attribute("Dimensions") = s.c_str();

  boost::filesystem::path p(hdf5_filename);
  s = p.filename().string() + ":" + dataset_name;
  xdmf_data.append_child(pugi::node_pcdata).set_value(s.c_str());

  // Write XML file
  xml_doc.save_file(_filename.c_str(), "  ");
}
//----------------------------------------------------------------------------
#endif
