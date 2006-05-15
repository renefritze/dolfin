// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-11-28
// Last changed: 2006-05-07

#ifndef __GENERIC_FUNCTION_H
#define __GENERIC_FUNCTION_H

#include <dolfin/constants.h>
#include <dolfin/Vector.h>
#include <dolfin/LocalFunctionData.h>

namespace dolfin
{
  
  class Point;
  class Vertex;
  class Mesh;
  class AffineMap;
  class FiniteElement;

  /// This class serves as a base class/interface for implementations
  /// of specific function representations.

  class GenericFunction
  {
  public:

    /// Constructor
    GenericFunction();

    /// Destructor
    virtual ~GenericFunction();

    /// Evaluate function at given point
    virtual real operator() (const Point& point, uint i) = 0;

    /// Evaluate function at given vertex
    virtual real operator() (const Vertex& vertex, uint i) = 0;

    // Restrict to sub function or component (if possible)
    virtual void sub(uint i) = 0;

    /// Compute interpolation of function onto local finite element space
    virtual void interpolate(real coefficients[], AffineMap& map, FiniteElement& element) = 0;

    /// Return vector dimension of function
    virtual uint vectordim() const = 0;

#ifdef HAVE_PETSC_H
    /// Return vector associated with function (if any)
    virtual Vector& vector() = 0;
#endif

    /// Return mesh associated with function (if any)
    virtual Mesh& mesh() = 0;

    /// Return element associated with function (if any)
    virtual FiniteElement& element() = 0;

#ifdef HAVE_PETSC_H
    /// Attach vector to function
    virtual void attach(Vector& x, bool local) = 0;
#endif

    /// Attach mesh to function
    virtual void attach(Mesh& mesh, bool local) = 0;

    /// Attach finite element to function
    virtual void attach(FiniteElement& element, bool local) = 0;

  protected:

    // Local storage for interpolation and evaluation
    LocalFunctionData local;

  };

}

#endif
