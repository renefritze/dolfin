// Copyright (C) 2006 Johan Hoffman
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
// First added:  2006-12-20
// Last changed: 2011-11-15

#include "MeshHierarchy.h"
#include "Mesh.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
MeshHierarchy::MeshHierarchy(const Mesh& mesh)
  : meshes(0), num_meshes(0)
{
  // Allocate data
  num_meshes = 1;
  meshes = new Mesh[num_meshes];

  // Initialize mesh hierarchy
  meshes[0] = mesh;
}
//-----------------------------------------------------------------------------
MeshHierarchy::MeshHierarchy()
  : meshes(0), num_meshes(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MeshHierarchy::~MeshHierarchy()
{
  clear();
}
//-----------------------------------------------------------------------------
void MeshHierarchy::init(const Mesh& mesh)
{
  // Clear old data if any
  clear();

  // Allocate data
  num_meshes = 1;
  meshes = new Mesh[num_meshes];

  // Initialize mesh hierarchy
  meshes[0] = mesh;
}
//-----------------------------------------------------------------------------
void MeshHierarchy::clear()
{
  // Clear data
  num_meshes = 0;

  if ( meshes ){
    delete [] meshes;
    meshes = 0;
  }
}
//-----------------------------------------------------------------------------
int MeshHierarchy::size()
{
  // Return number of meshes
  return int(num_meshes);
}
//-----------------------------------------------------------------------------
void MeshHierarchy::add(const Mesh& mesh)
{
  // Add mesh to mesh hierarchy if non empty
  if ( num_meshes > 0 )
  {
    // Create temporary array
    Mesh* tmp_array;
    tmp_array = new Mesh[num_meshes];

    // Copy data
    for (uint i = 0; i < num_meshes; i++)
      tmp_array[i] = meshes[i];

    // Clear old mesh hierarchy
    delete [] meshes;

    // Add new mesh to mesh hierarchy
    num_meshes++;
    meshes = new Mesh[num_meshes];
    for (uint i = 0; i < num_meshes-1; i++)
      meshes[i] = tmp_array[i];
    meshes[num_meshes-1] = mesh;

    // Clear temporary array
    delete [] tmp_array;
  }
  else
  {
    init(mesh);
  }

}
//-----------------------------------------------------------------------------
void MeshHierarchy::remove()
{
  // Remove finest mesh from mesh hierarchy if non empty
  if ( num_meshes > 0 )
  {
    // Create temporary array
    Mesh* tmp_array;
    tmp_array = new Mesh[num_meshes-1];

    // Copy data
    for (uint i = 0; i < num_meshes-1; i++)
      tmp_array[i] = meshes[i];

    // Clear old mesh hierarchy
    delete [] meshes;

    // Remove finest mesh from mesh hierarchy
    num_meshes--;
    meshes = new Mesh[num_meshes];
    for (uint i = 0; i < num_meshes; i++)
      meshes[i] = tmp_array[i];

    // Clear temporary array
    delete [] tmp_array;
  }
  else
  {
    dolfin_error("MeshHierarchy.cpp",
                 "remove mesh from mesh hierarchy",
                 "Mesh hierarchy is empty");
  }

}
//-----------------------------------------------------------------------------
Mesh& MeshHierarchy::operator()(uint k) const
{
  // Return reduced mesh object on level k
  return meshes[k];
}
//-----------------------------------------------------------------------------
Mesh& MeshHierarchy::operator[](uint k) const
{
  // Return full (reconstructed) mesh object on level k
  dolfin_not_implemented();
  return meshes[k];
}
//-----------------------------------------------------------------------------


