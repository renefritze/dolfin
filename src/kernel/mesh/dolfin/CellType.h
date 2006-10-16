// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2006-06-05
// Last changed: 2006-10-16

#ifndef __CELL_TYPE_H
#define __CELL_TYPE_H

#include <string>
#include <dolfin/dolfin_log.h>
#include <dolfin/constants.h>

namespace dolfin
{

  class Cell;
  class MeshEditor;

  /// This class provides a common interface for different cell types.
  /// Each cell type implements mesh functionality that is specific to
  /// a certain type of cell.

  class CellType
  {
  public:

    /// Enum for different cell types
    enum Type { point, interval, triangle, tetrahedron };

    /// Constructor
    CellType(Type cell_type, Type facet_type);

    /// Destructor
    virtual ~CellType();

    /// Create cell type from type (factory function)
    static CellType* create(Type type);

    /// Create cell type from string (factory function)
    static CellType* create(std::string type);

    /// Convert from string to cell type
    static Type string2type(std::string type);

    /// Convert from cell type to string
    static std::string type2string(Type type);

    /// Return type of cell
    inline Type cellType() const { return cell_type; }

    /// Return type of cell for facets
    inline Type facetType() const { return facet_type; }

    /// Return topological dimension of cell
    virtual uint dim() const = 0;
    
    /// Return number of entitites of given topological dimension
    virtual uint numEntities(uint dim) const = 0;

    /// Return number of vertices for entity of given topological dimension
    virtual uint numVertices(uint dim) const = 0;

    /// Create entities e of given topological dimension from vertices v
    virtual void createEntities(uint** e, uint dim, const uint v[]) const = 0;

    /// Refine cell uniformly
    virtual void refineCell(Cell& cell, MeshEditor& editor, uint& current_cell) const = 0;

    /// Return description of cell type
    virtual std::string description() const = 0;

  protected:

    Type cell_type;
    Type facet_type;
    
  };

}

#endif
