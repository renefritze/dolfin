// Copyright (C) 2010 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2010-11-16
// Last changed:

#include <boost/foreach.hpp>
#include "dolfin/log/log.h"
#include "dolfin/common/Timer.h"
#include "ZoltanInterface.h"

using namespace dolfin;

#ifdef HAS_TRILINOS
//-----------------------------------------------------------------------------
dolfin::uint ZoltanInterface::compute_local_vertex_coloring(const Graph& graph,
                                                            Array<uint>& colors)
{
  if (colors.size() != graph.size())
    error("ZoltanGraphColoring::compute_local_cell_coloring: colors array is of wrong length.");

  // Create Zoltan graph wrapper
  ZoltanGraphInterface zoltan_graph(graph);

  // Initialise Zoltan
  float version;
  int argc = 0;
  char** argv = NULL;
  Zoltan_Initialize(argc, argv, &version);

  // Create Zoltan object
  Zoltan zoltan;

  // Set parameters
  zoltan.Set_Param( "NUM_GID_ENTRIES", "1");  // global ID is single integer
  zoltan.Set_Param( "NUM_LID_ENTRIES", "1");  // local ID is single integer
  zoltan.Set_Param( "SUPERSTEP_SIZE", "1000");
  //zoltan.Set_Param( "COLORING_ORDER", "U");

  // Set call-back functions
  zoltan.Set_Num_Obj_Fn(ZoltanInterface::ZoltanGraphInterface::get_number_of_objects, &zoltan_graph);
  zoltan.Set_Obj_List_Fn(ZoltanInterface::ZoltanGraphInterface::get_object_list, &zoltan_graph);
  zoltan.Set_Num_Edges_Multi_Fn(ZoltanInterface::ZoltanGraphInterface::get_number_edges, &zoltan_graph);
  zoltan.Set_Edge_List_Multi_Fn(ZoltanInterface::ZoltanGraphInterface::get_all_edges, &zoltan_graph);

  // Create array for global ids that should be renumbered
  ZOLTAN_ID_PTR  global_ids = new ZOLTAN_ID_TYPE[graph.size()];
  for (uint i = 0; i < graph.size(); ++i)
    global_ids[i] = i;

  // Call Zoltan function to compute coloring
  int num_id = 1;
  int rc = zoltan.Color(num_id, graph.size(), global_ids,
                        reinterpret_cast<int*>(colors.data().get()));
  if (rc != ZOLTAN_OK)
    error("Zoltan coloring failed");

  // Clean up
  delete global_ids;

  // Compute number of colors
  boost::unordered_set<uint> colors_set;
  for (uint i = 0; i < colors.size(); ++i)
  {
    colors[i] = colors[i] - 1;
    colors_set.insert(colors[i]);
  }

  return colors_set.size();
}
//-----------------------------------------------------------------------------
#else
dolfin::uint ZoltanInterface::compute_local_vertex_coloring(const Graph& graph,
                                                    Array<uint>& colors)
{
  error("Trilinos (with Zoltan) must be enabled to use ZoltanInterface::compute_local_vertex_coloring.");
  return 0;
}
#endif
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#ifdef HAS_TRILINOS
ZoltanInterface::ZoltanGraphInterface::ZoltanGraphInterface(const Graph& graph)
    : graph(graph)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void ZoltanInterface::ZoltanGraphInterface::num_vertex_edges(uint* num_edges) const
{
  assert(num_edges);

  // Compute nunber of edges from each graph node
  for (uint i = 0; i < graph.size(); ++i)
    num_edges[i] = graph[i].size();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int ZoltanInterface::ZoltanGraphInterface::get_number_of_objects(void* data,
                                                                 int* ierr)
{
  ZoltanGraphInterface *objs = (ZoltanGraphInterface *)data;
  *ierr = ZOLTAN_OK;
  return objs->graph.size();
}
//-----------------------------------------------------------------------------
void ZoltanInterface::ZoltanGraphInterface::get_object_list(void *data,
                                   int sizeGID, int sizeLID,
                                   ZOLTAN_ID_PTR global_id,
                                   ZOLTAN_ID_PTR local_id, int wgt_dim,
                                   float* obj_wgts, int* ierr)
{
  ZoltanGraphInterface *objs = (ZoltanGraphInterface *)data;
  *ierr = ZOLTAN_OK;
  for (uint i = 0; i< objs->graph.size(); i++)
  {
    global_id[i] = i;
    local_id[i]  = i;
  }
}
//-----------------------------------------------------------------------------
void ZoltanInterface::ZoltanGraphInterface::get_number_edges(void *data,
                                    int num_gid_entries,
                                    int num_lid_entries,
                                    int num_obj, ZOLTAN_ID_PTR global_ids,
                                    ZOLTAN_ID_PTR local_ids, int *num_edges,
                                    int *ierr)
{
  ZoltanGraphInterface *objs = (ZoltanGraphInterface *)data;
  objs->num_vertex_edges(reinterpret_cast<uint*>(num_edges));
}
//-----------------------------------------------------------------------------
void ZoltanInterface::ZoltanGraphInterface::get_all_edges(void* data,
                                 int num_gid_entries,
                                 int num_lid_entries, int num_obj,
                                 ZOLTAN_ID_PTR global_ids,
                                 ZOLTAN_ID_PTR local_ids,
                                 int* num_edges,
                                 ZOLTAN_ID_PTR nbor_global_id,
                                 int* nbor_procs, int wgt_dim,
                                 float* ewgts, int* ierr)
{
  ZoltanGraphInterface *objs = (ZoltanGraphInterface *)data;

  // Get graph
  const Graph graph = objs->graph;

  uint entry = 0;
  for (uint i = 0; i < graph.size(); ++i)
  {
    assert(graph[i].size() == (uint) num_edges[i]);
    BOOST_FOREACH(boost::unordered_set<uint>::value_type edge, graph[i])
    {
      nbor_global_id[entry++] = edge;
    }
  }
}
//-----------------------------------------------------------------------------
#endif
