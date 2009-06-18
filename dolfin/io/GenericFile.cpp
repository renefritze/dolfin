// Copyright (C) 2002-2008 Johan Hoffman and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Niclas Jansson, 2008.
// Modified by Ola Skavhaug 2009.
//
// First added:  2002-11-12
// Last changed: 2009-03-04

// FIXME: Use streams instead of stdio
#include <stdio.h>

#include <dolfin/main/MPI.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/plot/FunctionPlotData.h>
#include "GenericFile.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
GenericFile::GenericFile(const std::string filename) : filename(filename),
                                                       type("Unknown file type"),
                                                       opened_read(false),
                                                       opened_write(false),
                                                       check_header(false),
                                                       counter(0),
                                                       counter1(0),
                                                       counter2(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
GenericFile::~GenericFile()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (Mesh& mesh)
{
  read_not_impl("Mesh");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (GenericVector& x)
{
  read_not_impl("Vector");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (GenericMatrix& A)
{
  read_not_impl("Matrix");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (LocalMeshData& data)
{
  read_not_impl("LocalMeshData");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (MeshFunction<int>& meshfunction)
{
  read_not_impl("MeshFunction<int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (MeshFunction<unsigned int>& meshfunction)
{
  read_not_impl("MeshFunction<unsigned int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (MeshFunction<double>& meshfunction)
{
  read_not_impl("MeshFunction<double>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (MeshFunction<bool>& meshfunction)
{
  read_not_impl("MeshFunction<bool>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (Sample& sample)
{
  read_not_impl("Sample");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (ParameterList& parameters)
{
  read_not_impl("ParameterList");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (Graph& graph)
{
  read_not_impl("Graph");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (FunctionPlotData& data)
{
  read_not_impl("FunctionPlotData");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::vector<int>& x)
{
  read_not_impl("std::vector<int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::vector<uint>& x)
{
  read_not_impl("std::vector<uint>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::vector<double>& x)
{
  read_not_impl("std::vector<double>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::map<uint, int>& map)
{
  read_not_impl("std::map<uint, int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::map<uint, uint>& map)
{
  read_not_impl("std::map<uint, uint>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::map<uint, double>& map)
{
  read_not_impl("std::map<uint, double>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::map<uint, std::vector<int> >& array_map)
{
  read_not_impl("std::map<uint, std::vector<int> >");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::map<uint, std::vector<uint> >& array_map)
{
  read_not_impl("std::map<uint, std::vector<uint> >");
}
//-----------------------------------------------------------------------------
void GenericFile::operator>> (std::map<uint, std::vector<double> >& array_map)
{
  read_not_impl("std::map<uint, std::vector<double> >");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const Mesh & mesh)
{
  write_not_impl("Mesh");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const GenericVector& x)
{
  write_not_impl("Vector");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const GenericMatrix& A)
{
  write_not_impl("Matrix");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const LocalMeshData& data)
{
  write_not_impl("LocalMeshData");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const MeshFunction<int>& meshfunction)
{
  write_not_impl("MeshFunction<int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const MeshFunction<unsigned int>& meshfunction)
{
  write_not_impl("MeshFunction<unsigned int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const MeshFunction<double>& meshfunction)
{
  write_not_impl("MeshFunction<double>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const MeshFunction<bool>& meshfunction)
{
  write_not_impl("MeshFunction<bool>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const Function& u)
{
  write_not_impl("Function");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const Sample& sample)
{
  write_not_impl("Sample");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const ParameterList& parameters)
{
  write_not_impl("ParameterList");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const Graph& graph)
{
  write_not_impl("Graph");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const FunctionPlotData& data)
{
  write_not_impl("FunctionPlotData");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::vector<int>& x)
{
  read_not_impl("std::vector<int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::vector<uint>& x)
{
  read_not_impl("std::vector<uint>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::vector<double>& x)
{
  read_not_impl("std::vector<double>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::map<uint, int>& map)
{
  read_not_impl("std::map<uint, int>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::map<uint, uint>& map)
{
  read_not_impl("std::map<uint, uint>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::map<uint, double>& map)
{
  read_not_impl("std::map<uint, double>");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::map<uint, std::vector<int> >& array_map)
{
  read_not_impl("std::map<uint, std::vector<int> >");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::map<uint, std::vector<uint> >& array_map)
{
  read_not_impl("std::map<uint, std::vector<uint> >");
}
//-----------------------------------------------------------------------------
void GenericFile::operator<< (const std::map<uint, std::vector<double> >& array_map)
{
  read_not_impl("std::map<uint, std::vector<double> >");
}
//-----------------------------------------------------------------------------
void GenericFile::read()
{
  opened_read = true;
}
//-----------------------------------------------------------------------------
void GenericFile::write()
{
  //FIXME .pvd files should only be cleared by one processor
  if ( type == "VTK" && MPI::process_number() > 0)
    opened_write = true;

  if ( !opened_write ) {
    // Clear file
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp)
      error("Unable to open file %s", filename.c_str());
    fclose(fp);
  }

  opened_write = true;
}
//-----------------------------------------------------------------------------
void GenericFile::read_not_impl(const std::string object)
{
  error("Unable to read objects of type %s from %s files.",
        object.c_str(), type.c_str());
}
//-----------------------------------------------------------------------------
void GenericFile::write_not_impl(const std::string object)
{
  error("Unable to write objects of type %s to %s files.",
        object.c_str(), type.c_str());
}
//-----------------------------------------------------------------------------
