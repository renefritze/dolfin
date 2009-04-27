// Copyright (C) 2007 Magnus Vikstrom
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
//
// First added:  2007-03-19
// Last changed: 2008-08-19

#include <dolfin/log/dolfin_log.h>
#include "GraphEditor.h"
#include "Graph.h"
#include "UndirectedClique.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
UndirectedClique::UndirectedClique(uint num_vertices) : Graph()
{
  if ( num_vertices < 1 )
    error("Graph must have at least one vertex.");

  rename("graph", "Undirected clique");

  // Open graph for editing
  GraphEditor editor;
  editor.open(*this, Graph::undirected);

  // Create vertices
  editor.init_vertices(num_vertices);
  for (uint i = 0; i < num_vertices; ++i)
    editor.add_vertex(i, num_vertices - i - 1);

  // Create edges
  editor.init_edges(((num_vertices - 1) * num_vertices)/2);
  for (uint i = 0; i < num_vertices - 1; ++i)
  {
    for (uint j = i+1; j < num_vertices; ++j)
      editor.add_edge(i, j);
  }

  // Close graph editor
  editor.close();
}
//-----------------------------------------------------------------------------
