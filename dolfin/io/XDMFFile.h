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

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#ifdef HAS_HDF5
#include <hdf5.h>
#else
typedef int hid_t;
#endif

#include <dolfin/common/MPI.h>
#include <dolfin/common/Variable.h>

namespace boost
{
  namespace filesystem
  {
    class path;
  }
}

namespace pugi
{
  class xml_node;
}

namespace dolfin
{

  // Forward declarations
  class Function;
#ifdef HAS_HDF5
  class HDF5File;
#endif
  class LocalMeshData;
  class Mesh;
  template<typename T> class MeshFunction;
  template<typename T> class MeshValueCollection;
  class Point;
  class XDMFxml;

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

    /// Re-use any partition stored in file
    enum class UseFilePartition : bool {yes=true, no=false};

    /// Constructor
    XDMFFile(const std::string filename)
      : XDMFFile(MPI_COMM_WORLD, filename) {}

    /// Constructor
    XDMFFile(MPI_Comm comm, const std::string filename);

    /// Destructor
    ~XDMFFile();

    /// Save a mesh to XDMF format, either using an associated HDF5
    /// file, or storing the data inline as XML Create function on
    /// given function space
    ///
    /// *Arguments*
    ///     mesh (_Mesh_)
    ///         A mesh to save.
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const Mesh& mesh, Encoding encoding=Encoding::HDF5);

    /// Save a Function to XDMF file for visualisation, using an
    /// associated HDF5 file, or storing the data inline as XML.
    ///
    /// *Arguments*
    ///     u (_Function_)
    ///         A function to save.
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const Function& u, Encoding encoding=Encoding::HDF5);

    /// Save a Function with timestamp to XDMF file for visualisation,
    /// using an associated HDF5 file, or storing the data inline as
    /// XML.
    ///
    /// *Arguments*
    ///     u (_Function_)
    ///         A function to save.
    ///     t (_double_)
    ///         Timestep
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const Function& u, double t, Encoding encoding=Encoding::HDF5);

    /// Save MeshFunction to file using an associated HDF5 file, or
    /// storing the data inline as XML.
    ///
    /// *Arguments*
    ///     meshfunction (_MeshFunction_)
    ///         A meshfunction to save.
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const MeshFunction<bool>& meshfunction,
               Encoding encoding=Encoding::HDF5);
    void write(const MeshFunction<int>& meshfunction,
               Encoding encoding=Encoding::HDF5);
    void write(const MeshFunction<std::size_t>& meshfunction,
               Encoding encoding=Encoding::HDF5);
    void write(const MeshFunction<double>& meshfunction,
               Encoding encoding=Encoding::HDF5);

    /// Write out mesh value collection (subset) using an associated
    /// HDF5 file, or storing the data inline as XML.
    ///
    /// *Arguments*
    ///     mvc (_MeshValueCollection<std::size_t>_)
    ///         A list of points to save.
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const MeshValueCollection<std::size_t>& mvc,
               Encoding encoding=Encoding::HDF5);

    /// Save a cloud of points to file using an associated HDF5 file,
    /// or storing the data inline as XML.
    ///
    /// *Arguments*
    ///     points (_std::vector<Point>_)
    ///         A list of points to save.
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const std::vector<Point>& points,
               Encoding encoding=Encoding::HDF5);

    /// Save a cloud of points, with scalar values using an associated
    /// HDF5 file, or storing the data inline as XML.
    ///
    /// *Arguments*
    ///     points (_std::vector<Point>_)
    ///         A list of points to save.
    ///     values (_std::vector<double>_)
    ///         A list of values at each point.
    ///     encoding (_Encoding_)
    ///         Encoding to use: HDF5 or ASCII
    ///
    void write(const std::vector<Point>& points,
               const std::vector<double>& values,
               Encoding encoding=Encoding::HDF5);

    /// Read in a mesh from the associated HDF5 file, optionally using
    /// stored partitioning, if possible when the same number of
    /// processes are being used.
    ///
    /// *Arguments*
    ///     mesh (_Mesh_)
    ///
    ///     use_partition_from_file (_UseFilePartition_)
    ///         Use the existing partition information in HDF5 file
    ///
    void read(Mesh& mesh,
              UseFilePartition use_file_partition=UseFilePartition::no);

    /// Read first MeshFunction from file
    void read(MeshFunction<bool>& meshfunction);
    void read(MeshFunction<int>& meshfunction);
    void read(MeshFunction<std::size_t>& meshfunction);
    void read(MeshFunction<double>& meshfunction);

    // Write mesh
    void write_new(const Mesh& mesh, Encoding encoding=Encoding::HDF5) const;

    // Read mesh
    void read_new(Mesh& mesh) const;

  private:

    // Build mesh (serial)
    static void build_mesh(Mesh& mesh, std::string cell_type_str,
                           std::int64_t num_points, std::int64_t num_cells,
                           int num_points_per_cell,
                           int tdim, int gdim,
                           const pugi::xml_node& topology_dataset_node,
                           const pugi::xml_node& geometry_dataset_node,
                           const boost::filesystem::path& parent_path);

   // Build local mesh data structure
   static void
   build_local_mesh_data (LocalMeshData& local_mesh_data,
                          const CellType& cell_type,
                          const std::int64_t num_points,
                          const std::int64_t num_cells,
                          const int num_points_per_cell,
                          const int tdim, const int gdim,
                          const pugi::xml_node& topology_dataset_node,
                          const pugi::xml_node& geometry_dataset_node,
                          const boost::filesystem::path& parent_path);

    // Add topology node to xml_node (includes writing data to XML or  HDF5
    // file)
    template<typename T>
    static void add_topology_data(MPI_Comm comm, pugi::xml_node& xml_node,
                                  hid_t h5_id, const std::string path_prefix,
                                  const Mesh& mesh);

    // Add geometry node and data to xml_node
    static void add_geometry_data(MPI_Comm comm, pugi::xml_node& xml_node,
                                  hid_t h5_id, const std::string path_prefix,
                                  const Mesh& mesh);

    // Add DataItem node to an XML node. If HDF5 is open (h5_id > 0) the data is
    // written to the HDFF5 file with the path 'h5_path'. Otherwise, data is
    // witten to the XML node and 'h5_path' is ignored
    template<typename T>
    static void add_data_item(MPI_Comm comm, pugi::xml_node& xml_node,
                              hid_t h5_id, const std::string h5_path, const T& x,
                              const std::vector<std::int64_t> dimensions,
                              const std::string number_type="");

    // Return topology data on this process as a flat vector
    template<typename T>
    static std::vector<T> compute_topology_data(const Mesh& mesh, int cell_dim);

    // Get DOLFIN cell type string from XML topology node
    static std::string get_cell_type(const pugi::xml_node& topology_node);

    // Get dimensions from an XML DataSet node
    static std::vector<std::int64_t>
      get_dataset_shape(const pugi::xml_node& dataset_node);

    // Get number of cells from an XML Topology node
    static std::int64_t get_num_cells(const pugi::xml_node& topology_node);

    // Return data associated with a data set node
    template <typename T>
    static std::vector<T> get_dataset(MPI_Comm comm,
                                      const pugi::xml_node& dataset_node,
                                      const boost::filesystem::path& parent_path);

    // Return (0) HDF5 filename and (1) path in HDF5 file from a DataItem node
    std::array<std::string, 2> get_hdf5_paths(const pugi::xml_node& dataitem_node);

    static std::string get_hdf5_filename(std::string xdmf_filename);

    // Generic MeshFunction reader
    template<typename T>
    void read_mesh_function(MeshFunction<T>& meshfunction);

    // Generic MeshFunction writer
    template<typename T>
    void write_mesh_function(const MeshFunction<T>& meshfunction,
                             std::string format, Encoding encoding);

    // Write XML description of point clouds, with value_size = 0, 1
    // or 3 (for either no point data, scalar, or vector)
    void write_point_xml(const std::string dataset_name,
                         const std::size_t num_global_points,
                         const unsigned int value_size, Encoding encoding);

    // Get point data values for linear or quadratic mesh into
    // flattened 2D array in data_values with given width
    void get_point_data_values(std::vector<double>& data_values,
                               std::size_t width, const Function& u) const;

    // Check whether the requested encoding is supported
    void check_encoding(Encoding encoding) const;

    // Generate the XDMF format string based on the Encoding
    // enumeration
    static std::string xdmf_format_str(Encoding encoding)
    { return (encoding == XDMFFile::Encoding::HDF5) ? "HDF" : "XML"; }

    // Generate the data string to insert in an xdmf file for the mesh
    // cell to node connectivity
    static std::string generate_xdmf_ascii_mesh_topology_data(const Mesh& mesh);

    // Generate the data string to insert in an xdmf file for the mesh
    // for the topology of entitiy dimension edim -> 0
    static std::string
    generate_xdmf_ascii_mesh_topology_data(const Mesh& mesh,
                                           const std::size_t edim);

    // Generate the data string to insert in an xdmf file for the mesh
    // point cloud
    static std::string generate_xdmf_ascii_mesh_geometry_data(const Mesh& mesh);

    // Generate a string of the vertex data numeric values contained
    // in the data argument which can then be inserted into an xdmf
    // file. The numeric data is formatted according to the format
    // argument. E.g:
    //    generate_xdmf_ascii_data(data_values, "%.15e")
    template<typename T>
    static std::string generate_xdmf_ascii_data(const T& data,
                                                std::string format);

    // As XDMFFile::generate_xdmf_ascii_data, using
    // boost::lexical_cast to format the numeric data as a string.
    template<typename T>
    static std::string generate_xdmf_ascii_data(const T& data);

    // Determine the encoding of the data from the xml file.
    Encoding get_file_encoding() const;

    // Determine the encoding enumeration value from the xdmf format
    // string E.g. "XML" or "HDF" See XDMFFile::xdmf_format_str
    static Encoding get_file_encoding(std::string xdmf_format);

    // Write MVC to ascii string to store in XDMF XML file
    template <typename T>
    void write_ascii_mesh_value_collection(const MeshValueCollection<T>& mesh_values,
                                             std::string data_name);

    static std::string vtk_cell_type_str(CellType::Type cell_type, int order);

    // Return a string of the form "x y"
    template <typename X, typename Y>
    static std::string to_string(X x, Y y);

    // Return a vector of numerical values from a vector of stringstream
    template <typename T>
    static std::vector<T> string_to_vector(const std::vector<std::string>& x_str);

    // MPI communicator
    MPI_Comm _mpi_comm;

    // HDF5 data file
#ifdef HAS_HDF5
    std::unique_ptr<HDF5File> _hdf5_file;
#endif

    // HDF5 file mode (r/w)
    std::string _hdf5_filemode;

    // Most recent mesh name
    std::string _current_mesh_name;

    // Cached filename
    const std::string _filename;

    // Counter for time series
    std::size_t _counter;

    // The xml document of the XDMF file
    std::unique_ptr<XDMFxml> _xml;

  };

}

#endif
