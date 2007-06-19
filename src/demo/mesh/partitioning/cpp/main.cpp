// Copyright (C) 2007 Anders Logg and Magnus Vikström.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-05-08
// Last changed: 2007-06-01

#include <dolfin.h>

using namespace dolfin;

int main()
{
  // Create mesh
  UnitCube mesh(16, 16, 16);

  // Partition mesh
  MeshFunction<unsigned int> partitions;
  mesh.partition(20, partitions);

  // Plot mesh partition
  plot(partitions);

  return 0;
}
