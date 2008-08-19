// Copyright (C) 2007 Magnus Vikstrom.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
//
// First added:  2007-02-12
// Last changed: 2008-08-19

#include <dolfin/log/dolfin_log.h>
#include "Graph.h"
#include "GraphEditor.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
GraphEditor::GraphEditor() : next_vertex(0), edge_count(0), graph(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
GraphEditor::~GraphEditor()
{
  clear();
}
//-----------------------------------------------------------------------------
void GraphEditor::open(Graph& graph, Graph::Type type)
{
  // Clear editor data
  clear();

  // Clear graph data
  graph.clear();

  this->graph = &graph;

  // Set graph type
  graph._type = type;
}
//-----------------------------------------------------------------------------
void GraphEditor::open(Graph& graph, std::string type)
{
  if ( type == "directed" )
    open(graph, Graph::directed);
  else if ( type == "undirected" )
    open(graph, Graph::undirected);
  else
    error("Unknown graph type \"%s\".", type.c_str());
}
//-----------------------------------------------------------------------------
void GraphEditor::initVertices(uint num_vertices)
{
  // Check if we are currently editing a graph
  if ( !graph )
    error("No graph opened, unable to edit.");
  
  // Initialize graph data
  graph->num_vertices = num_vertices;
  if(!graph->vertices && !graph->vertex_weights)
  {
    graph->vertices = new uint[num_vertices+1];
    graph->vertex_weights = new uint[num_vertices];
  }
  else
    error("Graph vertex data has already been allocated.");

  // Initialize vertex weights
  for(uint i = 0; i < num_vertices; ++i)
    graph->vertex_weights[i] = 1;
}
//-----------------------------------------------------------------------------
void GraphEditor::initEdges(uint num_edges)
{
  // Check if we are currently editing a graph
  if ( !graph )
    error("No graph opened, unable to edit.");
  
  // Check that vertex data has been inialised
  if( !graph->vertices )
    error("Vertex data has not been initialised.");

  // Initialize graph data
  graph->num_edges = num_edges;

  // Check if num_edges is consistent with edge_count
  if ( num_edges != edge_count )
    error("num_edges (%u) mismatch with sum of vertex edges (%u)", num_edges, edge_count);

  if(!graph->edges && !graph->edge_weights)
  {
    graph->edges        = new uint[graph->num_edges];
    graph->edge_weights = new uint[graph->num_edges];
  }
  else
    error("Graph edge data has already been allocated.");

  // Initialize arrays
  for(uint i=0; i < graph->num_edges; ++i)
  {
    graph->edges[i] = graph->num_vertices;
    graph->edge_weights[i] = 1;
  }

  // Update vertex array
  graph->vertices[graph->numVertices()] = graph->numEdges();
}
//-----------------------------------------------------------------------------
void GraphEditor::addVertex(uint u, uint num_edges)
{
  // Check if we are currently editing a graph
  if ( !graph )
    error("No graph opened, unable to edit.");

  // Check value of vertex index
  if ( u >= graph->num_vertices )
    error("Vertex index (%d) out of range [0, %d].", u, graph->num_vertices - 1);

  // Check if vertex added in correct order
  if ( u != next_vertex )
    error("Next vertex that can be added is %d.", next_vertex);
  
  // Set offset and step to next vertex
  graph->vertices[next_vertex++] = edge_count;

  // Keep track of total number of edges
  edge_count += num_edges;
}
//-----------------------------------------------------------------------------
void GraphEditor::addEdge(uint u, uint v)
{
  // Check value of to vertex index
  if ( v > next_vertex )
    error("Cannot create edge to undefined vertex (%d).", v);

  // Loop edges not allowed
  if ( u == v )
    error("Cannot create edge from vertex %d to itself.", v);

  // Check that vertex u is correctly specified
  if ( graph->vertices[u] < 0 || graph->vertices[u] > graph->num_edges )
    error("Vertex \"%u\" undefined or incorrectly defined.", u);

  uint u_next = graph->vertices[u];

  // If from vertex is last vertex stop at num_edges
  uint stop = (u == graph->num_vertices - 1) ? graph->num_edges : graph->vertices[u+1];
  while(u_next < stop && graph->edges[u_next] != graph->num_vertices)
    u_next++;

  // Check if vertex has room for edge
  if ( u_next == stop || u_next == graph->vertices[u+1] )
    error("Vertex %d does not have room for more edges.", u);

  graph->edges[u_next] = v;
}
//-----------------------------------------------------------------------------
void GraphEditor::close()
{
  for(uint i = 0; i < graph->num_vertices; ++i)
  {
    uint stop = graph->vertices[i] + graph->numEdges(i);
    for(uint j = graph->vertices[i]; j < stop; ++j)
    {
      if ( graph->edges[j] == graph->num_vertices )
        error("Cannot close, vertex %u has undefined edges", i);
    }
  }
  // Clear data
  clear();
}
//-----------------------------------------------------------------------------
void GraphEditor::clear()
{
  next_vertex = 0;
  edge_count = 0;
  graph = 0;
}
//-----------------------------------------------------------------------------

