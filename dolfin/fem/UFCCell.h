// Copyright (C) 2007-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2009.
// Modified by Garth N. Wells, 2010.
//
// First added:  2007-03-01
// Last changed: 2010-11-09

#ifndef __UFC_CELL_H
#define __UFC_CELL_H

#include <vector>
#include <dolfin/common/types.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/mesh/Cell.h>
#include <dolfin/mesh/MeshFunction.h>
#include <dolfin/main/MPI.h>
#include <dolfin/fem/ufcexp.h>

namespace dolfin
{

  /// This class is simple wrapper for a UFC cell and provides
  /// a layer between a DOLFIN cell and a UFC cell.

  class UFCCell : public ufcexp::cell
  {
  public:

    /// Create UFC cell from DOLFIN cell
    UFCCell(const Cell& cell) : ufcexp::cell(), num_vertices(0),
                                num_higher_order_vertices(0)
    {
      init(cell);
    }

    /// Create UFC cell for first DOLFIN cell in mesh
    UFCCell(const Mesh& mesh) : ufcexp::cell(), num_vertices(0),
                                num_higher_order_vertices(0)
    {
      CellIterator cell(mesh);
      init(*cell);
    }

    /// Destructor
    ~UFCCell()
    {
      clear();
    }

    /// Initialize UFC cell data
    void init(const Cell& cell)
    {
      // Clear old data
      clear();

      // Set cell shape
      switch (cell.type())
      {
      case CellType::interval:
        cell_shape = ufc::interval;
        num_vertices = 2;
        break;
      case CellType::triangle:
        cell_shape = ufc::triangle;
        num_vertices = 3;
        break;
      case CellType::tetrahedron:
        cell_shape = ufc::tetrahedron;
        num_vertices = 4;
        break;
      default:
        error("Unknown cell type.");
      }

      // Set topological dimension
      topological_dimension = cell.mesh().topology().dim();

      // Set geometric dimension
      geometric_dimension = cell.mesh().geometry().dim();

      // Allocate arrays for local entity indices
      entity_indices = new uint*[topological_dimension + 1];
      for (uint d = 0; d < topological_dimension; d++)
      {
        // Store number of cell entities allocated for (this can change between
        // init() and update() which is why it's stored)
        num_cell_entities.push_back(cell.num_entities(d));
        if (cell.num_entities(d) > 0)
          entity_indices[d] = new uint[cell.num_entities(d)];
        else
          entity_indices[d] = 0;
      }
      entity_indices[topological_dimension] = new uint[1];

      // Get global entity indices (if any)
      global_entities.resize(topological_dimension + 1);
      for (uint d = 0; d <= topological_dimension; ++d)
      {
        std::stringstream name;
        name << "global entity indices " << d;
        // This pointer may be zero, in which case local entity indices are used
        global_entities[d] = cell.mesh().data().mesh_function(name.str());
      }

      // Allocate vertex coordinates
      coordinates = new double*[num_vertices];

      // Allocate higher order vertex coordinates
      num_higher_order_vertices = cell.mesh().geometry().num_higher_order_vertices_per_cell();
      higher_order_coordinates = new double*[num_higher_order_vertices];

      // Update cell data
      update(cell);
    }

    // Clear UFC cell data
    void clear()
    {
      if (entity_indices)
      {
        for (uint d = 0; d <= topological_dimension; d++)
          delete [] entity_indices[d];
      }
      delete [] entity_indices;
      entity_indices = 0;

      global_entities.clear();

      delete [] coordinates;
      coordinates = 0;

      delete [] higher_order_coordinates;
      higher_order_coordinates = 0;

      cell_shape = ufc::interval;
      topological_dimension = 0;
      geometric_dimension = 0;
    }

    // Update cell entities and coordinates
    // Note: We use MeshEntity& rather than Cell& to avoid a gcc 4.4.1 warning
    void update(const MeshEntity& cell, int local_facet=-1)
    {
      assert(cell.dim() == topological_dimension);

      // Note handling of local and global mesh entity indices.
      // If mappings from local to global entities are available in
      // MeshData ("global entity indices %d") then those are used.
      // Otherwise, local entities are used. It is the responsibility
      // of the DofMap class to create the local-to-global mapping of
      // entity indices when running in parallel. In that sense, this
      // class is non parallel aware. It just uses the local-to-global
      // mapping when it is available.

      // Set mesh identifier
      mesh_identifier = cell.mesh_id();

      // Set local facet (-1 means no local facet set)
      this->local_facet = local_facet;

      // Copy local entity indices from mesh
      const uint D = topological_dimension;
      for (uint d = 0; d < D; ++d)
      {
        for (uint i = 0; i < num_cell_entities[d]; ++i)
          entity_indices[d][i] = cell.entities(d)[i];
      }

      // Set cell index
      entity_indices[D][0] = cell.index();
      index = cell.index();

      // Map to global entity indices (if any)
      for (uint d = 0; d < D; ++d)
      {
        if (global_entities[d])
        {
          for (uint i = 0; i < num_cell_entities[d]; ++i)
            entity_indices[d][i] = (*global_entities[d])[entity_indices[d][i]];
        }
      }
      if (global_entities[D])
        entity_indices[D][0] = (*global_entities[D])[entity_indices[D][0]];

      /// Set vertex coordinates
      const uint* vertices = cell.entities(0);
      for (uint i = 0; i < num_vertices; i++)
        coordinates[i] = const_cast<double*>(cell.mesh().geometry().x(vertices[i]));

      /// Set higher order vertex coordinates
      if (num_higher_order_vertices > 0)
      {
        const uint current_cell_index = cell.index();
        const uint* higher_order_vertex_indices = cell.mesh().geometry().higher_order_cell(current_cell_index);
        for (uint i = 0; i < num_higher_order_vertices; i++)
          higher_order_coordinates[i] = const_cast<double*>(cell.mesh().geometry().higher_order_x(higher_order_vertex_indices[i]));
      }
    }

  private:

    // Number of cell vertices
    uint num_vertices;

    // Number of higher order cell vertices
    uint num_higher_order_vertices;

    // Mappings from local to global entity indices (if any)
    std::vector<MeshFunction<uint>*> global_entities;

    // Number of cell entities of dimension d at initialisation
    std::vector<uint> num_cell_entities;

  };

}

#endif
