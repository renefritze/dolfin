// Copyright (C) 2003-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-10-21
// Last changed: 2006-06-22

#include <dolfin/log/dolfin_log.h>
#include <dolfin/mesh/CellType.h>
#include <dolfin/mesh/Mesh.h>
#include <dolfin/mesh/MeshData.h>
#include "XMLMesh.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLMesh::XMLMesh(Mesh& mesh) : XMLObject(), _mesh(mesh), state(OUTSIDE), f(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMesh::~XMLMesh()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void XMLMesh::startElement(const xmlChar *name, const xmlChar **attrs)
{
  switch ( state )
  {
  case OUTSIDE:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "mesh") == 0 )
    {
      readMesh(name, attrs);
      state = INSIDE_MESH;
    }
    
    break;

  case INSIDE_MESH:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "vertices") == 0 )
    {
      readVertices(name, attrs);
      state = INSIDE_VERTICES;
    }
    else if ( xmlStrcasecmp(name, (xmlChar *) "cells") == 0 )
    {
      readCells(name, attrs);
      state = INSIDE_CELLS;
    }
    else if ( xmlStrcasecmp(name, (xmlChar *) "data") == 0 )
      state = INSIDE_DATA;
    
    break;
    
  case INSIDE_VERTICES:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "vertex") == 0 )
      readVertex(name, attrs);

    break;
    
  case INSIDE_CELLS:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "interval") == 0 )
      readInterval(name, attrs);
    else if ( xmlStrcasecmp(name, (xmlChar *) "triangle") == 0 )
      readTriangle(name, attrs);
    else if ( xmlStrcasecmp(name, (xmlChar *) "tetrahedron") == 0 )
      readTetrahedron(name, attrs);
    
    break;

  case INSIDE_DATA:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "meshfunction") == 0 )
    {
      readMeshFunction(name, attrs);
      state = INSIDE_MESH_FUNCTION;
    }

    break;

  case INSIDE_MESH_FUNCTION:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "entity") == 0 )
      readEntity(name, attrs);

    break;

  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLMesh::endElement(const xmlChar *name)
{
  switch ( state )
  {
  case INSIDE_MESH:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "mesh") == 0 )
    {
      closeMesh();
      state = DONE;
    }
    
    break;
    
  case INSIDE_VERTICES:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "vertices") == 0 )
      state = INSIDE_MESH;
    
    break;

  case INSIDE_CELLS:
	 
    if ( xmlStrcasecmp(name, (xmlChar *) "cells") == 0 )
      state = INSIDE_MESH;
    
    break;

  case INSIDE_DATA:

    if ( xmlStrcasecmp(name, (xmlChar *) "data") == 0 )
      state = INSIDE_MESH;

    break;

  case INSIDE_MESH_FUNCTION:

    if ( xmlStrcasecmp(name, (xmlChar *) "meshfunction") == 0 )
      state = INSIDE_DATA;

    break;

  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLMesh::open(std::string filename)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
bool XMLMesh::close()
{
  return state == DONE;
}
//-----------------------------------------------------------------------------
void XMLMesh::readMesh(const xmlChar *name, const xmlChar **attrs)
{
  // Parse values
  std::string type = parseString(name, attrs, "celltype");
  uint gdim = parseUnsignedInt(name, attrs, "dim");
  
  // Create cell type to get topological dimension
  CellType* cell_type = CellType::create(type);
  uint tdim = cell_type->dim();
  delete cell_type;

  // Open mesh for editing
  editor.open(_mesh, CellType::string2type(type), tdim, gdim);
}
//-----------------------------------------------------------------------------
void XMLMesh::readVertices(const xmlChar *name, const xmlChar **attrs)
{
  // Parse values
  uint num_vertices = parseUnsignedInt(name, attrs, "size");

  // Set number of vertices
  editor.initVertices(num_vertices);
}
//-----------------------------------------------------------------------------
void XMLMesh::readCells(const xmlChar *name, const xmlChar **attrs)
{
  // Parse values
  uint num_cells = parseUnsignedInt(name, attrs, "size");

  // Set number of vertices
  editor.initCells(num_cells);
}
//-----------------------------------------------------------------------------
void XMLMesh::readVertex(const xmlChar *name, const xmlChar **attrs)
{
  // Read index
  uint v = parseUnsignedInt(name, attrs, "index");
  
  // Handle differently depending on geometric dimension
  switch ( _mesh.geometry().dim() )
  {
  case 1:
    {
      real x = parseReal(name, attrs, "x");
      editor.addVertex(v, x);
    }
    break;
  case 2:
    {
      real x = parseReal(name, attrs, "x");
      real y = parseReal(name, attrs, "y");
      editor.addVertex(v, x, y);
    }
    break;
  case 3:
    {
      real x = parseReal(name, attrs, "x");
      real y = parseReal(name, attrs, "y");
      real z = parseReal(name, attrs, "z");
      editor.addVertex(v, x, y, z);
    }
    break;
  default:
    error("Dimension of mesh must be 1, 2 or 3.");
  }
}
//-----------------------------------------------------------------------------
void XMLMesh::readInterval(const xmlChar *name, const xmlChar **attrs)
{
  // Check dimension
  if ( _mesh.topology().dim() != 1 )
    error("Mesh entity (interval) does not match dimension of mesh (%d).",
		 _mesh.topology().dim());

  // Parse values
  uint c  = parseUnsignedInt(name, attrs, "index");
  uint v0 = parseUnsignedInt(name, attrs, "v0");
  uint v1 = parseUnsignedInt(name, attrs, "v1");
  
  // Add cell
  editor.addCell(c, v0, v1);
}
//-----------------------------------------------------------------------------
void XMLMesh::readTriangle(const xmlChar *name, const xmlChar **attrs)
{
  // Check dimension
  if ( _mesh.topology().dim() != 2 )
    error("Mesh entity (triangle) does not match dimension of mesh (%d).",
		 _mesh.topology().dim());

  // Parse values
  uint c  = parseUnsignedInt(name, attrs, "index");
  uint v0 = parseUnsignedInt(name, attrs, "v0");
  uint v1 = parseUnsignedInt(name, attrs, "v1");
  uint v2 = parseUnsignedInt(name, attrs, "v2");
  
  // Add cell
  editor.addCell(c, v0, v1, v2);
}
//-----------------------------------------------------------------------------
void XMLMesh::readTetrahedron(const xmlChar *name, const xmlChar **attrs)
{
  // Check dimension
  if ( _mesh.topology().dim() != 3 )
    error("Mesh entity (tetrahedron) does not match dimension of mesh (%d).",
		 _mesh.topology().dim());

  // Parse values
  uint c  = parseUnsignedInt(name, attrs, "index");
  uint v0 = parseUnsignedInt(name, attrs, "v0");
  uint v1 = parseUnsignedInt(name, attrs, "v1");
  uint v2 = parseUnsignedInt(name, attrs, "v2");
  uint v3 = parseUnsignedInt(name, attrs, "v3");
  
  // Add cell
  editor.addCell(c, v0, v1, v2, v3);
}
//-----------------------------------------------------------------------------
void XMLMesh::readMeshFunction(const xmlChar* name, const xmlChar** attrs)
{
  // Parse values
  const std::string id = parseString(name, attrs, "name");
  const std::string type = parseString(name, attrs, "type");
  const uint dim = parseUnsignedInt(name, attrs,   "dim");
  const uint size = parseUnsignedInt(name, attrs,   "size");

  // Only uint supported at this point
  if (strcmp(type.c_str(), "uint") != 0)
    error("Only uint-valued mesh data is currently supported.");

  // Register data
  f = _mesh.data().create(id, dim);
  
  // Set all values to zero
  *f = 0;

  // Check size
  if (size >= f->size())
    error("Wrong size of mesh data \"%s\", at most \"%d\" values may be specified.",
          id.c_str(), f->size());
}
//-----------------------------------------------------------------------------
void XMLMesh::readEntity(const xmlChar* name, const xmlChar** attrs)
{
  // Read index
  const uint index = parseUnsignedInt(name, attrs, "index");

  // Read and set value
  dolfin_assert(f);
  dolfin_assert(index < f->size());
  const uint value = parseUnsignedInt(name, attrs, "value");
  f->set(index, value);
}
//-----------------------------------------------------------------------------
void XMLMesh::closeMesh()
{
  editor.close();
}
//-----------------------------------------------------------------------------
