// Copyright (C) 2008-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Niclas Jansson, 2008.
//
// First added:  2008-05-19
// Last changed: 2009-06-18

#include <sstream>

#include "MeshFunction.h"
#include "MeshData.h"

using namespace dolfin;

typedef std::map<std::string, MeshFunction<dolfin::uint>*>::iterator mf_iterator;
typedef std::map<std::string, MeshFunction<dolfin::uint>*>::const_iterator mf_const_iterator;

typedef std::map<std::string, std::vector<dolfin::uint>*>::iterator a_iterator;
typedef std::map<std::string, std::vector<dolfin::uint>*>::const_iterator a_const_iterator;

typedef std::map<std::string, std::map<dolfin::uint, dolfin::uint>*>::iterator m_iterator;
typedef std::map<std::string, std::map<dolfin::uint, dolfin::uint>*>::const_iterator m_const_iterator;

typedef std::map<std::string, std::map<dolfin::uint, std::vector<dolfin::uint> > *>::iterator mvec_iterator;
typedef std::map<std::string, std::map<dolfin::uint, std::vector<dolfin::uint> > *>::const_iterator mvec_const_iterator;

//-----------------------------------------------------------------------------
MeshData::MeshData(Mesh& mesh) : mesh(mesh)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MeshData::~MeshData()
{
  clear();
}
//-----------------------------------------------------------------------------
const MeshData& MeshData::operator= (const MeshData& data)
{
  // Clear all data
  clear();

  // Copy MeshFunctions
  for (mf_const_iterator it = data.mesh_functions.begin(); it != data.mesh_functions.end(); ++it)
  {
    MeshFunction<uint> *f = create_mesh_function(it->first, it->second->dim());
    *f = *it->second;
  }

  // Copy arrays
  for (a_const_iterator it = data.arrays.begin(); it != data.arrays.end(); ++it)
  {
    std::vector<uint>* a = create_array( it->first, static_cast<uint>(it->second->size()) );
    *a = *it->second;
  }

  // Copy mappings
  for (m_const_iterator it = data.mappings.begin(); it != data.mappings.end(); ++it)
  {
    std::map<uint, uint>* m = create_mapping(it->first);
    *m = *it->second;
  }

  // Copy vector mappings
  for (mvec_const_iterator it = data.vector_mappings.begin(); it != data.vector_mappings.end(); ++it)
  {
    std::map<uint, std::vector<uint> >* m = create_vector_mapping(it->first);
    *m = *it->second;
  }

  return *this;
}
//-----------------------------------------------------------------------------
void MeshData::clear()
{
  for (mf_iterator it = mesh_functions.begin(); it != mesh_functions.end(); ++it)
    delete it->second;
  mesh_functions.clear();

  for (a_iterator it = arrays.begin(); it != arrays.end(); ++it)
    delete it->second;
  arrays.clear();

  for (m_iterator it = mappings.begin(); it != mappings.end(); ++it)
    delete it->second;
  mappings.clear();

  for (mvec_iterator it = vector_mappings.begin(); it != vector_mappings.end(); ++it)
    delete it->second;
  vector_mappings.clear();
}
//-----------------------------------------------------------------------------
MeshFunction<dolfin::uint>* MeshData::create_mesh_function(std::string name)
{
  // Check if data already exists
  mf_iterator it = mesh_functions.find(name);
  if (it != mesh_functions.end())
  {
    warning("Mesh data named \"%s\" already exists.", name.c_str());
    return it->second;
  }

  // Create new data
  MeshFunction<uint>* f = new MeshFunction<uint>(mesh);
  assert(f);

  // Add to map
  mesh_functions[name] = f;

  return f;
}
//-----------------------------------------------------------------------------
MeshFunction<dolfin::uint>* MeshData::create_mesh_function(std::string name, uint dim)
{
  MeshFunction<uint>* f = create_mesh_function(name);
  f->init(dim);

  return f;
}
//-----------------------------------------------------------------------------
std::vector<dolfin::uint>* MeshData::create_array(std::string name, uint size)
{
  // Check if data already exists
  a_iterator it = arrays.find(name);
  if (it != arrays.end())
  {
    warning("Mesh data named \"%s\" already exists.", name.c_str());
    return it->second;
  }

  // Create new data
  std::vector<uint>* a = new std::vector<uint>(size);
  std::fill(a->begin(), a->end(), 0);

  // Add to map
  arrays[name] = a;

  return a;
}
//-----------------------------------------------------------------------------
std::map<dolfin::uint, dolfin::uint>* MeshData::create_mapping(std::string name)
{
  // Check if data already exists
  m_iterator it = mappings.find(name);
  if (it != mappings.end())
  {
    warning("Mesh data named \"%s\" already exists.", name.c_str());
    return it->second;
  }

  // Create new data
  std::map<uint, uint>* m = new std::map<uint, uint>;

  // Add to map
  mappings[name] = m;

  return m;
}
//-----------------------------------------------------------------------------
std::map<dolfin::uint, std::vector<dolfin::uint> >* MeshData::create_vector_mapping(std::string name)
{
  // Check if data already exists
  mvec_iterator it = vector_mappings.find(name);
  if (it != vector_mappings.end())
  {
    warning("Mesh data named \"%s\" already exists.", name.c_str());
    return it->second;
  }

  // Create new data
  std::map<uint, std::vector<uint> >* m = new std::map<uint, std::vector<uint> >;

  // Add to map
  vector_mappings[name] = m;

  return m;
}
//-----------------------------------------------------------------------------
MeshFunction<dolfin::uint>* MeshData::mesh_function(const std::string name) const
{
  // Check if data exists
  mf_const_iterator it = mesh_functions.find(name);
  if (it == mesh_functions.end())
    return 0;

  return it->second;
}
//-----------------------------------------------------------------------------
std::vector<dolfin::uint>* MeshData::array(const std::string name) const
{
  // Check if data exists
  a_const_iterator it = arrays.find(name);
  if (it == arrays.end())
    return 0;

  return it->second;
}
//-----------------------------------------------------------------------------
std::map<dolfin::uint, dolfin::uint>* MeshData::mapping(const std::string name) const
{
  // Check if data exists
  m_const_iterator it = mappings.find(name);
  if (it == mappings.end())
    return 0;

  return it->second;
}
//-----------------------------------------------------------------------------
std::map<dolfin::uint, std::vector<dolfin::uint> >* MeshData::vector_mapping(const std::string name) const
{
  // Check if data exists
  mvec_const_iterator it = vector_mappings.find(name);
  if (it == vector_mappings.end())
    return 0;

  return it->second;
}
//-----------------------------------------------------------------------------
void MeshData::erase_mesh_function(const std::string name)
{
  mf_iterator it = mesh_functions.find(name);
  if (it != mesh_functions.end())
  {
    delete it->second;
    mesh_functions.erase(it);
  }
  else
  {
    warning("Mesh data named \"%s\" doesn't exist.", name.c_str());
  }
}
//-----------------------------------------------------------------------------
void MeshData::erase_array(const std::string name)
{
  a_iterator it = arrays.find(name);
  if (it != arrays.end())
  {
    delete it->second;
    arrays.erase(it);
  }
  else
  {
    warning("Mesh data named \"%s\" doesn't exist.", name.c_str());
  }
}
//-----------------------------------------------------------------------------
void MeshData::erase_mapping(const std::string name)
{
  m_iterator it = mappings.find(name);
  if (it != mappings.end())
  {
    delete it->second;
    mappings.erase(it);
  }
  else
  {
    warning("Mesh data named \"%s\" doesn't exist.", name.c_str());
  }
}
//-----------------------------------------------------------------------------
void MeshData::erase_vector_mapping(const std::string name)
{
  mvec_iterator it = vector_mappings.find(name);
  if (it != vector_mappings.end())
  {
    delete it->second;
    vector_mappings.erase(it);
  }
  else
  {
    warning("Mesh data named \"%s\" doesn't exist.", name.c_str());
  }
}
//-----------------------------------------------------------------------------
void MeshData::disp() const
{
  // FIXME: Remove this function and use str() instead

  // Begin indentation
  begin("Auxiliary mesh data");

  // Print mesh functions
  for (mf_const_iterator it = mesh_functions.begin(); it != mesh_functions.end(); ++it)
  {
    cout << "MeshFunction<uint> of size "
         << it->second->size()
         << " on entities of topological dimension "
         << it->second->dim()
         << ": \"" << it->first << "\"" << endl;
  }

  // Print arrays
  for (a_const_iterator it = arrays.begin(); it != arrays.end(); ++it)
    cout << "std::vector<uint> of size " << static_cast<uint>(it->second->size())
         << ": \"" << it->first << "\"" << endl;

  // Print mappings
  for (m_const_iterator it = mappings.begin(); it != mappings.end(); ++it)
    cout << "map<uint, uint> of size " << static_cast<uint>(it->second->size())
         << ": \"" << it->first << "\"" << endl;

  // Print vector mappings
  for (mvec_const_iterator it = vector_mappings.begin(); it != vector_mappings.end(); ++it)
    cout << "map<uint, vector<uint> > of size " << static_cast<uint>(it->second->size())
         << ": \"" << it->first << "\"" << endl;

  // End indentation
  end();
}
//-----------------------------------------------------------------------------
std::string MeshData::str() const
{
  std::stringstream s;

  // Header
  s << "Mesh data" << std::endl;
  s << "---------" << std::endl << std::endl;

  // Mesh functions
  s << "  MeshFunction<uint>" << std::endl;
  s << "  ------------------" << std::endl;
  for (mf_const_iterator it = mesh_functions.begin(); it != mesh_functions.end(); ++it)
    s << "  " << it->first << " (size = " << it->second->size() << ")" << std::endl;
  s << std::endl;

  // Arrays
  s << "  std::vector<uint>" << std::endl;
  s << "  -----------------" << std::endl;
  for (a_const_iterator it = arrays.begin(); it != arrays.end(); ++it)
    s << "  " << it->first << " (size = " << it->second->size() << ")" << std::endl;
  s << std::endl;

  // Mappings
  s << "  std::map<uint, uint>" << std::endl;
  s << "  --------------------" << std::endl;
  for (m_const_iterator it = mappings.begin(); it != mappings.end(); ++it)
    s << "  " << it->first << " (size = " << it->second->size() << ")" << std::endl;
  s << std::endl;

  // Vector mappings
  s << "  std::map<uint, std::vector<uint>" << std::endl;
  s << "  --------------------------------" << std::endl;
  for (mvec_const_iterator it = vector_mappings.begin(); it != vector_mappings.end(); ++it)
    s << "  " << it->first << " (size = " << it->second->size() << ")" << std::endl;
  s << std::endl;

  return s.str();
}
//-----------------------------------------------------------------------------
