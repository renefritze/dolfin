// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Garth N. Wells, 2007.
//
// First added:  2007-01-17
// Last changed: 2007-03-14

#include <dolfin/dolfin_log.h>
#include <dolfin/Cell.h>
#include <dolfin/DofMap.h>
#include <dolfin/DofMaps.h>
#include <dolfin/Mesh.h>
#include <dolfin/SparsityPattern.h>
#include <dolfin/UFCCell.h>
#include <dolfin/UFCMesh.h>

#include <ufc.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
DofMaps::DofMaps()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
DofMaps::~DofMaps()
{
  // Delete all dof maps in the cache
  for (map_iterator it = dof_map_cache.begin(); it != dof_map_cache.end(); it++)
  {
    // Delete UFC dof map
    delete it->second.first;

    // Delete DOLFIN dof map
    delete it->second.second;
  }
}
//-----------------------------------------------------------------------------
void DofMaps::update(const ufc::form& form, Mesh& mesh)
{
  // Resize array of dof maps
  const uint num_arguments = form.rank() + form.num_coefficients();
  dof_maps.resize(num_arguments);

  // Create dof maps and reuse previously computed dof maps
  for (uint i = 0; i < num_arguments; i++)
  {
    // Create UFC dof map
    ufc::dof_map* ufc_dof_map = form.create_dof_map(i);
    dolfin_assert(ufc_dof_map);
    
    // Check if dof map is in cache
    map_iterator it = dof_map_cache.find(ufc_dof_map->signature());
    if ( it == dof_map_cache.end() )
    {
      cout << "Creating dof map (not in cache): " << ufc_dof_map->signature() << endl;

      // Create DOLFIN dof map
      DofMap* dolfin_dof_map = new DofMap(*ufc_dof_map, mesh);
      dolfin_assert(dolfin_dof_map);

      // Save pair of UFC and DOLFIN dof maps in cache
      std::pair<ufc::dof_map*, DofMap*> dof_map_pair(ufc_dof_map, dolfin_dof_map);
      dof_map_cache[ufc_dof_map->signature()] = dof_map_pair;
      
      // Set dof map for argument i
      dof_maps[i] = dolfin_dof_map;
    }
    else
    {
      cout << "Reusing dof map (already in cache): " << ufc_dof_map->signature() << endl;
      
      // Set dof map for argument i
      dof_maps[i] = it->second.second;
     
      // Delete UFC dof map (not used)
      delete ufc_dof_map;
    }
  }
}
//-----------------------------------------------------------------------------
dolfin::uint DofMaps::size() const
{
  return dof_maps.size();
}
//-----------------------------------------------------------------------------
const DofMap& DofMaps::operator[] (uint i) const
{
  dolfin_assert(i < dof_maps.size());
  return *dof_maps[i];
}
//-----------------------------------------------------------------------------
void DofMaps::sparsityPattern(SparsityPattern& sparsity_pattern, Mesh& mesh) const
{
  if( size() != 2)
    dolfin_error("Number of DOF maps in not equal to 2. Do not know how to build sparsity pattern.");
 
  DofMap* dof_map0 = dof_maps[0];
  DofMap* dof_map1 = dof_maps[1];

  // Get local dimensions 
  const unsigned int dim0 = dof_map0->local_dimension();
  const unsigned int dim1 = dof_map1->local_dimension();

  // Initialise sparsity pattern 
  sparsity_pattern.init(dof_map0->global_dimension());

  unsigned int* dof0 = new unsigned int[dim0];
  unsigned int* dof1 = new unsigned int[dim1];

  // Create UFC cell
  CellIterator cell(mesh);
  UFCCell ufc_cell(*cell);

  // Build sparsity pattern
  for ( ; !cell.end(); ++cell)
  {
    ufc_cell.update(*cell);

    dof_map0->dof_map(dof0, ufc_cell);
    dof_map1->dof_map(dof1, ufc_cell);

    // Building sparsity
    for (uint i = 0; i < dim0; ++i)
      for (uint j = 0; j < dim1; ++j)
        sparsity_pattern.insert(dof0[i], dof1[j] );
 }
  
 delete [] dof0;
 delete [] dof1;
}
//-----------------------------------------------------------------------------
