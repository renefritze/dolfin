// Copyright (C) 2006-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Kristoffer Selim, 2008.
//
// First added:  2006-06-21
// Last changed: 2008-10-08

#ifndef __GTS_INTERFACE_H
#define __GTS_INTERFACE_H

#ifdef HAS_GTS

#include <dolfin/common/types.h>

// Forward declarations
struct  _GtsBBox;
typedef _GtsBBox GtsBBox;
struct  _GNode;
typedef _GNode GNode;

namespace dolfin
{

  // Forward declarations
  class Mesh;
  class Point;
  class Cell;
  template <class T> class Array;

  class GTSInterface
  {
  public:

    /// Create GTS interface for mesh
    GTSInterface(const Mesh& mesh);

    /// Destructor
    ~GTSInterface();

    /// Compute cells overlapping point
    void intersection(const Point& p, Array<uint>& cells);

    /// Compute cells overlapping line defined by points
    void intersection(const Point& p0, const Point& p1, Array<uint>& cells);

    /// Compute cells overlapping cell
    void intersection(const Cell& cell, Array<uint>& cells);

  private:

    /// Create bounding box for a single point
    GtsBBox* createBox(const Point& p);

    /// Create bounding box for a pair of points
    GtsBBox* createBox(const Point& p0, const Point& p1);

    /// Create bounding box for cell
    GtsBBox* createBox(const Cell& cell);

    // Build tree (hierarchy) of bounding boxes
    void buildCellTree();

    // The mesh
    const Mesh& mesh;

    // GTS tree
    GNode* tree;

  };

}

#endif

#endif
