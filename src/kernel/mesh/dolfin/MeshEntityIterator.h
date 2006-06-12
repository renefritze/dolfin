// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-05-09
// Last changed: 2006-06-31

#ifndef __MESH_ENTITY_ITERATOR_H
#define __MESH_ENTITY_ITERATOR_H

#include <dolfin/constants.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/NewMesh.h>
#include <dolfin/MeshEntity.h>


#include <dolfin/MeshAlgorithms.h>

namespace dolfin
{


  // FIXME: Move constructors and output to MeshEntityIterator.cpp

  // FIXME: Consistent use of incidence relations, connectivity, connections

  /// MeshEntityIterator provides a common iterator for mesh entities
  /// over meshes, boundaries and incidence relations. The basic use
  /// is illustrated below.
  ///
  /// The following example shows how to iterate over all mesh entities
  /// of a mesh of topological dimension dim:
  ///
  ///     for (MeshEntityIterator e(mesh, dim); !e.end(); ++e)
  ///     {
  ///       e->foo();
  ///     }
  ///
  /// The following example shows how to iterate over mesh entities of
  /// topological dimension dim connected (incident) to some mesh entity f:
  ///
  ///     for (MeshEntityIterator e(f, dim); !e.end(); ++e)
  ///     {
  ///       e->foo();
  ///     }
  ///
  /// In addition to the general iterator, a set of specific named iterators
  /// are provided for entities of type Vertex, Edge, Face, Facet and Cell.
  /// These iterators are defined along with their respective classes.

  class MeshEntityIterator
  {
  public:

    /// Create iterator for mesh entities over given topological dimension
    MeshEntityIterator(NewMesh& mesh, uint dim) 
      : entity(mesh, dim, 0), pos(0), pos_end(mesh.size(dim)), index(0)
    {
      // Compute entities if empty
      if ( pos_end == 0 )
      {
	MeshAlgorithms::computeEntities(mesh, dim);
	pos_end = mesh.size(dim);
      }
    }

    /// Create iterator for entities of given dimension connected to given entity    
    MeshEntityIterator(MeshEntity& entity, uint dim)
      : entity(entity.mesh(), dim, 0), pos(0)
    {
      // Get connectivity
      MeshConnectivity& c = entity.mesh().data.topology(entity.dim(), dim);
      
      // Compute connectivity if empty
      if ( c.size() == 0 )
	MeshAlgorithms::computeConnectivity(entity.mesh(), entity.dim(), dim);
      
      // Get size and index map
      if ( c.size() == 0 )
      {
	pos_end = 0;
	index = 0;
      }
      else
      {
	pos_end = c.size(entity.index());
	index = c(entity.index());
      }
    }

    /// Create iterator for entities of given dimension connected to given entity
    MeshEntityIterator(MeshEntityIterator& it, uint dim)
      : entity(it.entity.mesh(), dim, 0), pos(0)
    {
      // Get entity
      MeshEntity& entity = *it;
      
      // Get connectivity
      MeshConnectivity& c = entity.mesh().data.topology(entity.dim(), dim);
      
      // Compute connectivity if empty
      if ( c.size() == 0 )
	MeshAlgorithms::computeConnectivity(entity.mesh(), entity.dim(), dim);
      
      // Get size and index map
      if ( c.size() == 0 )
      {
	pos_end = 0;
	index = 0;
      }
      else
      {
	pos_end = c.size(entity.index());
	index = c(entity.index());
      }
    }

    /// Destructor
    virtual ~MeshEntityIterator() {}
    
    // FIXME: No bounds check, should be ok? Check what STL does.

    /// Step to next mesh entity
    MeshEntityIterator& operator++() { ++pos; return *this; }

    /// Check if iterator has reached the end
    inline bool end() const { return pos >= pos_end; }
    
    /// Dereference operator
    inline MeshEntity& operator*() { entity._index = (index ? index[pos] : pos); return entity; }

    /// Member access operator
    inline MeshEntity* operator->() { entity._index = (index ? index[pos] : pos); return &entity; }

    /// Member access operator
    //inline const MeshEntity* operator->() const { entity._index = (index ? index[pos] : pos); return &entity; }

    /// Output
    friend LogStream& operator<< (LogStream& stream, const MeshEntityIterator& it)
    {
      stream << "[ Mesh entity iterator at position "
	     << it.pos
	     << " stepping from 0 to "
	     << it.pos_end - 1
	     << " ]";
      return stream;
    }
    
  private:

    // Mesh entity
    MeshEntity entity;

    // Current position
    uint pos;

    // End position
    uint pos_end;

    // Mapping from pos to index (if any)
    uint* index;
    
  };

}

#endif
