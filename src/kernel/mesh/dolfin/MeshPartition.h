// Copyright (C) 2007 Magnus Vikstrom
// Licensed under the GNU GPL Version 2.
//
// First added:  2007-04-24
// Last changed: 2007-04-25

#ifndef __MESH_PARTITIONING_H
#define __MESH_PARTITIONING_H

#include <dolfin/constants.h>

namespace dolfin
{
  /// This class provides a set of functions to partition a Mesh

  class MeshPartition
  {
  public:

    // FIXME: Change interface to and extract values inside cpp file after init()
    //static void partition(Mesh& mesh, uint num_partitions, MeshFunction<uint>& partitions);
    
    /// Partition a mesh into num_part partitions
    static void partition(Mesh& mesh, uint num_partitions, uint* vtx_part);

  };

}

#endif
