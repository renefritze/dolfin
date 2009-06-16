// Copyright (C) 2002-2009 Johan Hoffman and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells 2005-2009.
// Modified by Haiko Etzel 2005.
// Modified by Magnus Vikstrom 2007.
// Modified by Nuno Lopes 2008.
// Modified by Niclas Jansson 2008.
// Modified by Ola Skavhaug 2009.
//
// First added:  2002-11-12
// Last changed: 2009-06-16

#include <string>
#include <dolfin/main/MPI.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/mesh/MeshFunction.h>
#include "File.h"
#include "GenericFile.h"
#include "XMLFile.h"
#include "MatlabFile.h"
#include "OctaveFile.h"
#include "PythonFile.h"
#include "PVTKFile.h"
#include "VTKFile.h"
#include "RAWFile.h"
#include "XYZFile.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
File::File(const std::string filename)
{
  // Choose file type base on suffix.

  // FIXME: Use correct function to find the suffix; using rfind() makes
  //        it essential that the suffixes are checked in the correct order.

  if (filename.rfind(".xml.gz") != filename.npos)
    file = new XMLFile(filename, true);
  else if (filename.rfind(".xml") != filename.npos)
    file = new XMLFile(filename, false);
  else if (filename.rfind(".m") != filename.npos)
    file = new OctaveFile(filename);
  else if (filename.rfind(".py") != filename.npos)
    file = new PythonFile(filename);
  else if (filename.rfind(".pvd") != filename.npos)
  {
    if (MPI::num_processes() > 1)
      file = new PVTKFile(filename);
    else
      file = new VTKFile(filename);
  }
  else if (filename.rfind(".raw") != filename.npos)
    file = new RAWFile(filename);
  else if (filename.rfind(".xyz") != filename.npos)
    file = new XYZFile(filename);
  else
  {
    file = 0;
    error("Unknown file type for \"%s\".", filename.c_str());
  }
}
//-----------------------------------------------------------------------------
File::File(const std::string filename, Type type)
{
  switch (type) 
  {
  case xml:
    file = new XMLFile(filename, false);
    break;
  case matlab:
    file = new MatlabFile(filename);
    break;
  case octave:
    file = new OctaveFile(filename);
    break;
  case vtk:
    file = new VTKFile(filename);
    break;
  case python:
    file = new PythonFile(filename);
    break;
  default:
    file = 0;
    error("Unknown file type for \"%s\".", filename.c_str());
  }
}
//-----------------------------------------------------------------------------
File::File(std::ostream& outstream)
{
  file = new XMLFile(outstream);
}
//-----------------------------------------------------------------------------
File::~File()
{
  delete file;
  file = 0;
}
//-----------------------------------------------------------------------------

