// Copyright (C) 2008 Ola Skavhaug
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
// First added:  2008-11-28
// Last changed: 2011-05-30
//
// Modified by Anders Logg, 2008.
// Modified by Kent-Andre Mardal, 2011.

#include <boost/assign/list_of.hpp>
#include <boost/shared_ptr.hpp>
#include <dolfin/log/log.h>
#include <dolfin/common/MPI.h>
#include <dolfin/mesh/CellType.h>
#include "XMLLocalMeshDataDistributed.h"
#include <dolfin/common/constants.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLLocalMeshDataDistributed::XMLLocalMeshDataDistributed(LocalMeshData& mesh_data,
  const std::string filename) : state(OUTSIDE), mesh_data(mesh_data), filename(filename)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void XMLLocalMeshDataDistributed::read()
{
  std::cout << "Inside new read function" << std::endl;

  // Create handler
  xmlSAXHandler sax_handler;

  sax_handler.startDocument = sax_start_document;
  sax_handler.endDocument   = sax_end_document;

  sax_handler.startElement  = XMLLocalMeshDataDistributed::sax_start_element;
  sax_handler.endElement    = sax_end_element;

  //xmlSAXUserParseFile(&sax_handler, (void *) this, filename.c_str());
  xmlSAXParseFile(&sax_handler, filename.c_str(), 1);
  std::cout << "End parse" << std::endl;
  //
  /*

  saxHandler.initialized = XML_SAX2_MAGIC;
  */
}
//-----------------------------------------------------------------------------
void XMLLocalMeshDataDistributed::start_element(const xmlChar *name, const xmlChar **attrs)
{
  std::cout << "In class function" << std::endl;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void dolfin::sax_start_document(void *ctx)
{
  std::cout << "Call back sax_start_document" << std::endl;
  // Do nothing
}
//-----------------------------------------------------------------------------
void dolfin::sax_end_document(void *ctx)
{
  std::cout << "Call back sax_end_document" << std::endl;
  // Do nothing
}
//-----------------------------------------------------------------------------
void XMLLocalMeshDataDistributed::sax_start_element(void *ctx, const xmlChar *name, const xmlChar **attrs)
{
  std::cout << "***sax_start element" << std::endl;
  ((XMLLocalMeshDataDistributed*) ctx)->start_element(name, attrs);
//( (XMLFile*) ctx )->start_element(name, attrs);
  /*
  switch (state)
  {
  case OUTSIDE:
    if (xmlStrcasecmp(name, (xmlChar* ) "mesh") == 0)
    {
      //read_mesh(name, attrs);
      state = INSIDE_MESH;
    }
    break;

  case INSIDE_MESH:
    if (xmlStrcasecmp(name, (xmlChar* ) "vertices") == 0)
    {
      //read_vertices(name, attrs);
      state = INSIDE_VERTICES;
    }
    else if (xmlStrcasecmp(name, (xmlChar* ) "cells") == 0)
    {
      //read_cells(name, attrs);
      state = INSIDE_CELLS;
    }
    else if (xmlStrcasecmp(name, (xmlChar* ) "data") == 0)
    {
      state = INSIDE_DATA;
      //read_mesh_data(name, attrs);
    }
    break;

  case INSIDE_VERTICES:
    if (xmlStrcasecmp(name, (xmlChar* ) "vertex") == 0)
    {
      //read_vertex(name, attrs);
    }
    break;

  case INSIDE_CELLS:
    if (xmlStrcasecmp(name, (xmlChar* ) "interval") == 0)
    {
      read_interval(name, attrs);
    }
    else if (xmlStrcasecmp(name, (xmlChar* ) "triangle") == 0)
    {
      read_triangle(name, attrs);
    }
    else if (xmlStrcasecmp(name, (xmlChar* ) "tetrahedron") == 0)
    {
      read_tetrahedron(name, attrs);
    }
    break;

  case INSIDE_DATA:
    if (xmlStrcasecmp(name, (xmlChar* ) "meshfunction") == 0)
    {
      //read_mesh_function(name, attrs);
      state = INSIDE_MESH_FUNCTION;
    }
    else if (xmlStrcasecmp(name, (xmlChar* ) "array") == 0)
    {
      //read_array(name, attrs);
      state = INSIDE_ARRAY;
    }
    else if (xmlStrcasecmp(name, (xmlChar* ) "data_entry") == 0)
    {
      //read_data_entry(name, attrs);
      state = INSIDE_DATA_ENTRY;
    }
    break;

  case INSIDE_DATA_ENTRY:
    if (xmlStrcasecmp(name, (xmlChar* ) "array") == 0)
    {
      read_array(name, attrs);
      state = INSIDE_ARRAY;
    }
    break;

  default:
    error("Inconsistent state in XML reader: %d.", state);
  }
  */
}
//-----------------------------------------------------------------------------
void dolfin::sax_end_element(void *ctx, const xmlChar *name)
{
  std::cout << "***sax_end element" << std::endl;
  /*
  switch (state)
  {
  case INSIDE_MESH:
    if (xmlStrcasecmp(name, (xmlChar* ) "mesh") == 0)
    {
      state = DONE;
      //release();
    }
    break;

  case INSIDE_VERTICES:
    if (xmlStrcasecmp(name, (xmlChar* ) "vertices") == 0)
    {
      state = INSIDE_MESH;
    }
    break;

  case INSIDE_CELLS:
    if (xmlStrcasecmp(name, (xmlChar* ) "cells") == 0)
    {
      state = INSIDE_MESH;
    }
    break;

  case INSIDE_DATA:
    if (xmlStrcasecmp(name, (xmlChar* ) "data") == 0)
    {
      state = INSIDE_MESH;
    }
    break;

  case INSIDE_MESH_FUNCTION:
    if (xmlStrcasecmp(name, (xmlChar* ) "meshfunction") == 0)
    {
      state = INSIDE_DATA;
    }
    break;

  case INSIDE_DATA_ENTRY:
    if (xmlStrcasecmp(name, (xmlChar* ) "data_entry") == 0)
    {
      state = INSIDE_DATA;
    }

  case INSIDE_ARRAY:
    if (xmlStrcasecmp(name, (xmlChar* ) "array") == 0)
    {
      state = INSIDE_DATA_ENTRY;
    }

    if (xmlStrcasecmp(name, (xmlChar* ) "data_entry") == 0)
    {
      state = INSIDE_DATA;
    }
    break;

  default:
    error("Closing XML tag '%s', but state is %d.", name, state);
  }
  */
}
//-----------------------------------------------------------------------------
void dolfin::sax_warning(void *ctx, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  char buffer[DOLFIN_LINELENGTH];
  vsnprintf(buffer, DOLFIN_LINELENGTH, msg, args);
  warning("Incomplete XML data: " + std::string(buffer));
  va_end(args);
}
//-----------------------------------------------------------------------------
void dolfin::sax_error(void *ctx, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  char buffer[DOLFIN_LINELENGTH];
  vsnprintf(buffer, DOLFIN_LINELENGTH, msg, args);
  error("Illegal XML data: " + std::string(buffer));
  va_end(args);
}
//-----------------------------------------------------------------------------
void dolfin::sax_fatal_error(void *ctx, const char *msg, ...)
{
  va_list args;
  va_start(args, msg);
  char buffer[DOLFIN_LINELENGTH];
  vsnprintf(buffer, DOLFIN_LINELENGTH, msg, args);
  error("Illegal XML data: " + std::string(buffer));
  va_end(args);
}
//-----------------------------------------------------------------------------
