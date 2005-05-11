// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include <dolfin/PArray.h>
#include <dolfin/GenericCell.h>

namespace dolfin
{

  class Node;
  class Cell;
  
  class Triangle : public GenericCell
  {
  public:
    
    Triangle(Node& n0, Node& n1, Node& n2);

    int noNodes() const;
    int noEdges() const;
    int noFaces() const;

    int noBoundaries() const;
    
    Cell::Type type() const;
    Cell::Orientation orientation() const;

    real volume() const;
    real diameter() const;

    bool edgeAligned(uint i) const;
    uint faceAlignment(uint i) const;
    
  private:

    void createEdges();
    void createFaces();
    void sort();
    
  };

}

#endif
