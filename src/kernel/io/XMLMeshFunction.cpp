// Copyright (C) 2006 Ola Skavhaug.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-11-27
// Last changed: 2006-11-27

#include <dolfin/dolfin_log.h>
#include <dolfin/CellType.h>
#include <dolfin/Mesh.h>
#include <dolfin/MeshFunction.h>
#include <dolfin/XMLMeshFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<int>& meshfunction) 
: XMLObject(), 
  state(OUTSIDE), 
  mf_type(UNSET), 
  _imeshfunction(&meshfunction),
  _dmeshfunction(0),
  _bmeshfunction(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<double>& meshfunction)
: XMLObject(), 
  state(OUTSIDE),
  mf_type(UNSET), 
  _imeshfunction(0),
  _dmeshfunction(&meshfunction),
  _bmeshfunction(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<bool>& meshfunction)
: XMLObject(), 
  state(OUTSIDE),
  mf_type(UNSET), 
  _imeshfunction(0),
  _dmeshfunction(0),
  _bmeshfunction(&meshfunction)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMeshFunction::~XMLMeshFunction()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::startElement(const xmlChar *name, const xmlChar **attrs)
{
  switch ( state )
  {
  case OUTSIDE:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "meshfunction") == 0 )
    {
      readMeshFunction(name, attrs);
      state = INSIDE_MESHFUNCTION;
    }
    
    break;

  case INSIDE_MESHFUNCTION:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "entity") == 0 )
    {
      readEntities(name, attrs);
      state = INSIDE_ENTITY;
    }
  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::endElement(const xmlChar *name)
{
  switch ( state )
  {
  case INSIDE_MESHFUNCTION:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "meshfunction") == 0 )
    {
      state = DONE;
    }
    
    break;
    
  case INSIDE_ENTITY:
    
    if ( xmlStrcasecmp(name, (xmlChar *) "entity") == 0 )
      state = INSIDE_MESHFUNCTION;
    
    break;

  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::open(std::string filename)
{
  cout << "Reading mesh from file " << filename << "." << endl;
}
//-----------------------------------------------------------------------------
bool XMLMeshFunction::close()
{
  return state == DONE;
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::readMeshFunction(const xmlChar *name, const xmlChar **attrs)
{
  // Parse values
  std::string type = parseString(name, attrs, "type");
  uint tdim = parseUnsignedInt(name, attrs,   "dim");
  uint size = parseUnsignedInt(name, attrs,   "size");
  if (strcmp(type.c_str(), "int") == 0 ) 
  {
    if (_imeshfunction == 0) 
      dolfin_error("MeshFunction file of type \"int\", but MeshFunction<int> not initialized");
    if ( _imeshfunction->mesh().size(tdim) != size)
      dolfin_error4("In file: Size %d MeshEntities of dimension %d does not match underlying Mesh having size %d MeshEntities of dimension %d", size, tdim, _imeshfunction->mesh().size(tdim), tdim);
    _imeshfunction->init(tdim, size);
    mf_type = INT;
  }
  else if (strcmp(type.c_str(), "double") == 0 ) 
  {
    if (_dmeshfunction == 0) 
      dolfin_error("MeshFunction file of type \"double\", but MeshFunction<double> not initialized");
    if ( _dmeshfunction->mesh().size(tdim) != size)
      dolfin_error4("In file: Size %d MeshEntities of dimension %d does not match underlying Mesh having size %d MeshEntities of dimension %d", size, tdim, _dmeshfunction->mesh().size(tdim), tdim);
    _dmeshfunction->init(tdim, size);
    mf_type = DOUBLE;
  }
  else if (strcmp(type.c_str(), "bool") == 0 ) 
  {
    if (_bmeshfunction == 0) 
      dolfin_error("MeshFunction file of type \"bool\", but MeshFunction<bool> not initialized");
    if ( _bmeshfunction->mesh().size(tdim) != size)
      dolfin_error4("In file: Size %d MeshEntities of dimension %d does not match underlying Mesh having size %d MeshEntities of dimension %d", size, tdim, _bmeshfunction->mesh().size(tdim), tdim);
    _bmeshfunction->init(tdim, size);
    mf_type = BOOL;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::readEntities(const xmlChar *name, const xmlChar **attrs)
{
  // Read index
  uint i = parseUnsignedInt(name, attrs, "index");
  switch ( mf_type )
  {
  case INT:
    _imeshfunction->set(i, parseInt(name, attrs, "value"));
    break;

  case DOUBLE:
    _dmeshfunction->set(i, parseReal(name, attrs, "value"));
    break;

  case BOOL:
    _bmeshfunction->set(i, parseBool(name, attrs, "value"));
    break;

  default:
    dolfin_error("Could not determine the type of MeshFunction. Aborting");
  }
}
//-----------------------------------------------------------------------------
