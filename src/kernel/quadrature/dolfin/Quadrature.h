// Copyright (C) 2003 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#ifndef __QUADRATURE_H
#define __QUADRATURE_H

#include <dolfin/Point.h>

namespace dolfin {
  
  class Quadrature {
  public:
    
    Quadrature(int n);
    virtual ~Quadrature();
    
    /// Return number of quadrature points
    int size() const;

    /// Return quadrature point
    const Point& point(int i) const;

    /// Return quadrature weight
    real weight(int i) const;

    /// Return sum of weights (length, area, volume)
    real measure() const;
    
    /// Display quadrature data
    virtual void show() const = 0;

  protected:
    
    int n;         // Number of quadrature points
    Point* points; // Quadrature points
    real* weights; // Quadrature weights
    real m;        // Sum of weights
    
  };
  
}

#endif
