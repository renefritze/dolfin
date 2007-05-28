// Copyright (C) 2007 Magnus Vikstrom.
// Licensed under the GNU GPL Version 2.
//
// First added:  2007-04-03
// Last changed: 2007-04-24

#include <dolfin/Graph.h>
#include <dolfin/GraphPartition.h>
#include <dolfin/MeshPartition.h>
#include <dolfin/MeshFunction.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void MeshPartition::partition(Mesh& mesh, uint num_part, 
                              MeshFunction<uint>& partitions)
{
  dolfin_debug1("partitions.init(mesh, %d)", mesh.topology().dim());
  partitions.init(mesh, mesh.topology().dim());
  Graph graph(mesh);
  GraphPartition::partition(graph, num_part, partitions.values());
}
//-----------------------------------------------------------------------------
