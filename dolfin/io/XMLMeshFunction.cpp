// Copyright (C) 2009 Ola Skavhaug
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-03-02
// Last changed: 2009-09-08

#include <dolfin/log/dolfin_log.h>
#include <dolfin/mesh/MeshPartitioning.h>
#include "XMLSkipper.h"
#include "XMLIndent.h"
#include "XMLFile.h"
#include "XMLMeshFunction.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<int>& imf, XMLFile& parser)
  : XMLHandler(parser), imf(&imf), umf(0), dmf(0), xml_skipper(0), mesh(imf.mesh()),
    state(OUTSIDE_MESHFUNCTION), mf_type(INT), size(0), dim(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<uint>& umf, XMLFile& parser)
  : XMLHandler(parser), imf(0), umf(&umf), dmf(0), xml_skipper(0), mesh(umf.mesh()),
    state(OUTSIDE_MESHFUNCTION), mf_type(UINT), size(0), dim(0)
{
  info("Outside MeshFunction to start with.");
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<double>& dmf, XMLFile& parser)
  : XMLHandler(parser), imf(0), umf(0), dmf(&dmf), xml_skipper(0), mesh(dmf.mesh()),
    state(OUTSIDE_MESHFUNCTION), mf_type(DOUBLE), size(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<int>& imf, XMLFile& parser, uint size, uint dim)
  : XMLHandler(parser), imf(&imf), umf(0), dmf(0), xml_skipper(0), mesh(imf.mesh()),
    state(INSIDE_MESHFUNCTION), mf_type(INT), size(size), dim(dim)
{
  // Initialize mesh function
  this->imf->init(dim);

  // Set all values to zero
  *(this->imf) = 0;

  // Build global to local mapping for dimension
  if (MPI::num_processes() > 1)
    build_mapping(dim);
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<uint>& umf, XMLFile& parser, uint size, uint dim)
  : XMLHandler(parser), imf(0), umf(&umf), dmf(0), xml_skipper(0), mesh(umf.mesh()),
    state(INSIDE_MESHFUNCTION), mf_type(UINT), size(size), dim(dim)
{
  // Initialize mesh function
  this->umf->init(dim);

  // Set all values to zero
  *(this->umf) = 0;

  // Build global to local mapping for dimension
  if (MPI::num_processes() > 1)
    build_mapping(dim);
}
//-----------------------------------------------------------------------------
XMLMeshFunction::XMLMeshFunction(MeshFunction<double>& dmf, XMLFile& parser, uint size, uint dim)
  : XMLHandler(parser), imf(0), umf(0), dmf(&dmf), xml_skipper(0), mesh(dmf.mesh()),
    state(INSIDE_MESHFUNCTION), mf_type(DOUBLE), size(size), dim(dim)
{
  // Initialize mesh function
  this->dmf->init(dim);

  // Set all values to zero
  *(this->dmf) = 0;

  // Build global to local mapping for dimension
  if (MPI::num_processes() > 1)
    build_mapping(dim);
}
//-----------------------------------------------------------------------------
XMLMeshFunction::~XMLMeshFunction()
{
  delete xml_skipper;
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::start_element(const xmlChar *name, const xmlChar **attrs)
{
  switch ( state )
  {
  case OUTSIDE_MESHFUNCTION:

    if ( xmlStrcasecmp(name, (xmlChar *) "meshfunction") == 0 )
    {
      start_mesh_function(name, attrs);
      state = INSIDE_MESHFUNCTION;
    }
    else
    {
      delete xml_skipper;
      xml_skipper = new XMLSkipper(std::string((const char*)(name)), parser);
      xml_skipper->handle();
    }

    break;

  case INSIDE_MESHFUNCTION:

    if ( xmlStrcasecmp(name, (xmlChar *) "entity") == 0 )
      read_entity(name, attrs);

    break;

  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::end_element(const xmlChar *name)
{
  switch ( state )
  {
  case INSIDE_MESHFUNCTION:

    if ( xmlStrcasecmp(name, (xmlChar *) "meshfunction") == 0 )
    {
      state = DONE;
      release();
    }

    break;

  default:
    ;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::write(const MeshFunction<int>& mf, std::ostream& outfile, uint indentation_level, bool write_mesh)
{
  if (write_mesh)
    XMLMesh::write(mf.mesh(), outfile, indentation_level);
  XMLIndent indent(indentation_level);
  outfile << indent();
  outfile << "<meshfunction type=\"int\" dim=\"" << mf.dim() << "\" size=\"" << mf.size() << "\">" << std::endl;

  ++indent;
  for (uint i = 0; i < mf.size(); ++i)
  {
    outfile << indent();
    outfile << "<entity index=\"" << i << "\" value=\"" << mf.get(i) << "\"/>" << std::endl;
  }
  --indent;
  outfile << indent() << "</meshfunction>" << std::endl;
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::write(const MeshFunction<uint>& mf, std::ostream& outfile, uint indentation_level, bool write_mesh)
{
  if (write_mesh)
    XMLMesh::write(mf.mesh(), outfile, indentation_level);
  XMLIndent indent(indentation_level);
  outfile << indent();
  outfile << "<meshfunction type=\"uint\" dim=\"" << mf.dim() << "\" size=\"" << mf.size() << "\">" << std::endl;

  ++indent;
  for (uint i = 0; i < mf.size(); ++i)
  {
    outfile << indent();
    outfile << "<entity index=\"" << i << "\" value=\"" << mf.get(i) << "\"/>" << std::endl;
  }
  --indent;
  outfile << indent() << "</meshfunction>" << std::endl;
}

//-----------------------------------------------------------------------------
void XMLMeshFunction::write(const MeshFunction<double>& mf, std::ostream& outfile, uint indentation_level, bool write_mesh)
{
  if (write_mesh)
    XMLMesh::write(mf.mesh(), outfile, indentation_level);
  XMLIndent indent(indentation_level);
  outfile << indent();
  outfile << "<meshfunction type=\"double\" dim=\"" << mf.dim() << "\" size=\"" << mf.size() << "\">" << std::endl;

  ++indent;
  for (uint i = 0; i < mf.size(); ++i)
  {
    outfile << indent();
    outfile << "<entity index=\"" << i << "\" value=\"" << mf.get(i) << "\"/>" << std::endl;
  }
  --indent;
  outfile << indent() << "</meshfunction>" << std::endl;
}

//-----------------------------------------------------------------------------
void XMLMeshFunction::start_mesh_function(const xmlChar *name, const xmlChar **attrs)
{
  // Parse size of mesh function
  size = parse_uint(name, attrs, "size");

  // Parse type of mesh function
  std::string _type = parse_string(name, attrs, "type");

  // Parse dimension of mesh function

  uint dim = parse_uint(name, attrs, "dim");

  // Build global to local mapping for dimension
  if (MPI::num_processes() > 1)
    build_mapping(dim);

  // Initialize mesh function
  switch ( mf_type )
  {
    case INT:
      assert(imf);
      if ( _type.compare("int") != 0 )
        error("MeshFunction file of type '%s', expected 'int'.", _type.c_str());
      imf->init(dim);

      break;

    case UINT:
      assert(umf);
      if ( _type.compare("uint") != 0 )
        error("MeshFunction file of type '%s', expected 'uint'.", _type.c_str());
      umf->init(dim);

      break;

    case DOUBLE:
      assert(dmf);
      if ( _type.compare("double") != 0 )
        error("MeshFunction file of type '%s', expected 'double'.", _type.c_str());
      dmf->init(dim);

      break;

    default:
      ;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::read_entity(const xmlChar *name, const xmlChar **attrs)
{
  // Parse index
  uint index = parse_uint(name, attrs, "index");

  // Check values
  if (index >= size)
    error("Illegal XML data for MeshFunction: row index %d out of range (0 - %d)",
          index, size - 1);

  cout << "Reading global entity index " << index << endl;

  if (MPI::num_processes() > 1)
  {
    // Only read owned entities (belonging to local mesh)
    std::map<uint, uint>::const_iterator it = glob2loc.find(index);
    if (it != glob2loc.end())
      index = (*it).second;
    else
    {
      cout << "Skipping" << endl;
      return;
    }
  }
  cout << "Corresponding local entity number is " << index << endl;


  // Parse value and insert in array
  switch ( mf_type )
  {
    case INT:
      assert(imf);
      imf->set(index, parse_int(name, attrs, "value"));

      break;

     case UINT:
      assert(umf);
      cout << "Setting value to " << parse_uint(name, attrs, "value") << endl;
      umf->set(index, parse_uint(name, attrs, "value"));

      break;

     case DOUBLE:
      assert(dmf);
      dmf->set(index, parse_float(name, attrs, "value"));

      break;

     default:
      ;
  }
}
//-----------------------------------------------------------------------------
void XMLMeshFunction::build_mapping(uint entity_dimension)
{
  // Read global entity indices from mesh
  std::stringstream mesh_data_name;
  mesh_data_name << "global entity indices " << entity_dimension;
  MeshFunction<uint>* global_entity_indices = mesh.data().mesh_function(mesh_data_name.str());
  if (global_entity_indices == NULL)
  {
    MeshPartitioning::number_entities(mesh, entity_dimension);
    global_entity_indices = mesh.data().mesh_function(mesh_data_name.str());
  }
  assert(global_entity_indices);

  // Build global to local mapping
  glob2loc.clear();
  for (uint i = 0; i < global_entity_indices->size(); ++i)
    glob2loc[global_entity_indices->get(i)] = i;
}
//-----------------------------------------------------------------------------
