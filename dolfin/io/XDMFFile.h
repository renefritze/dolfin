// Copyright (C) 2012-2015 Chris N. Richardson and Garth N. Wells
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

#ifndef __DOLFIN_XDMFFILE_H
#define __DOLFIN_XDMFFILE_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <dolfin/common/MPI.h>
#include <dolfin/common/Variable.h>

namespace dolfin
{

  // Forward declarations
  class Function;
#ifdef HAS_HDF5
  class HDF5File;
#endif
  class Mesh;
  template<typename T> class MeshFunction;
  template<typename T> class MeshValueCollection;
  class Point;

  /// This class supports the output of meshes and functions in XDMF
  /// (http://www.xdmf.org) format. It creates an XML file that
  /// describes the data and points to a HDF5 file that stores the
  /// actual problem data. Output of data in parallel is supported.
  ///
  /// XDMF is not suitable for checkpointing as it may decimate some
  /// data.

  class XDMFFile : public Variable
  {
  public:

    /// File encoding type
    enum class Encoding {HDF5, ASCII};

    /// Constructor
    XDMFFile(MPI_Comm comm, const std::string filename);

    /// Destructor
    ~XDMFFile();

    /// Save a mesh for visualisation, with e.g. ParaView. Creates a
    /// HDF5 file to store the mesh, and a related XDMF file with
    /// metadata.
    void write(const Mesh& mesh, Encoding encoding=Encoding::HDF5);

    /// Save a Function to XDMF/HDF5 files for visualisation.
    void write(const Function& u, Encoding encoding=Encoding::HDF5);

    /// Save Function + time stamp to file
    void write(const Function& u, double t, Encoding encoding=Encoding::HDF5);

    /// Save MeshFunction to file
    void write(const MeshFunction<bool>& meshfunction,
               Encoding encoding=Encoding::HDF5);
    void write(const MeshFunction<int>& meshfunction,
               Encoding encoding=Encoding::HDF5);
    void write(const MeshFunction<std::size_t>& meshfunction,
               Encoding encoding=Encoding::HDF5);
    void write(const MeshFunction<double>& meshfunction,
               Encoding encoding=Encoding::HDF5);

    /// Save a cloud of points to file
    void write(const std::vector<Point>& points,
               Encoding encoding=Encoding::HDF5);

    /// Save a cloud of points, with scalar values
    void write(const std::vector<Point>& points,
               const std::vector<double>& values,
               Encoding encoding=Encoding::HDF5);

    // Generic MeshFunction writer
    template<typename T>
    void write(const MeshFunction<T>& meshfunction,
               std::string format,
               Encoding encoding=Encoding::HDF5);

    // Write out mesh value collection (subset)
    void write(const MeshValueCollection<std::size_t>& mvc,
               Encoding encoding=Encoding::HDF5);


    /// Read in a mesh from the associated HDF5 file, optionally using
    /// stored partitioning, if possible when the same number of
    /// processes are being used.
    void read(Mesh& mesh, bool use_partition_from_file);

    /// Read first MeshFunction from file
    void read(MeshFunction<bool>& meshfunction);
    void read(MeshFunction<int>& meshfunction);
    void read(MeshFunction<std::size_t>& meshfunction);
    void read(MeshFunction<double>& meshfunction);

  private:

    // MPI communicator
    MPI_Comm _mpi_comm;

    // HDF5 data file
#ifdef HAS_HDF5
    std::unique_ptr<HDF5File> hdf5_file;
#endif

    // HDF5 filename
    std::string hdf5_filename;

    // HDF5 file mode (r/w)
    std::string hdf5_filemode;

    // Generic MeshFunction reader
    template<typename T>
    void read_mesh_function(MeshFunction<T>& meshfunction);

    // Generic MeshFunction writer
    template<typename T>
    void write_mesh_function(const MeshFunction<T>& meshfunction,
                             std::string format,
                             Encoding encoding);

    // Write XML description of point clouds, with value_size = 0, 1
    // or 3 (for either no point data, scalar, or vector)
    void write_point_xml(const std::string dataset_name,
                         const std::size_t num_global_points,
                         const unsigned int value_size,
                         Encoding encoding);

    // Get point data values for linear or quadratic mesh into
    // flattened 2D array in data_values with given width
    void get_point_data_values(std::vector<double>& data_values,
                               std::size_t width, const Function& u);

    // Write generic mesh func

    // Check whether the requested encoding is supported
    void check_encoding(Encoding encoding);

    // Generate the XDMF format string based on the Encoding enumeration
    std::string xdmf_format_str(Encoding encoding)
    { return (encoding == XDMFFile::Encoding::HDF5) ? "HDF" : "XML"; }

    // Generate the data string to insert in an xdmf file for the
    // mesh cell to node connectivity
    std::string generate_xdmf_ascii_mesh_topology_data(const Mesh& mesh);

    // Generate the data string to insert in an xdmf file for the
    // mesh for the topology of entitiy dimension edim -> 0.
    std::string generate_xdmf_ascii_mesh_topology_data(
        const Mesh& mesh,
        const std::size_t edim);

    // Generate the data string to insert in an xdmf file for the
    // mesh point cloud
    std::string generate_xdmf_ascii_mesh_geometry_data(const Mesh& mesh);

    // Generate the data string to insert in an xdmf file for vertex
    // data with output formatted according to format
    template<typename T>
    std::string generate_xdmf_ascii_vertex_data(
        const std::vector<T>& data, std::string format);

    // Determine the encoding of the data from the xml file.
    // E.g. "XML" or "HDF".
    Encoding get_file_encoding();
    Encoding get_file_encoding(std::string xdmf_format);

    // Most recent mesh name
    std::string current_mesh_name;

    // Cached filename
    const std::string _filename;

    // Counter for time series
    std::size_t counter;

  };
}

#endif
