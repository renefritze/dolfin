// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Johan Jansson, 2005.

#ifndef __NEW_FUNCTION_H
#define __NEW_FUNCTION_H


#define HEJ 0.1

#include <dolfin/constants.h>
#include <dolfin/Point.h>
#include <dolfin/Variable.h>

namespace dolfin
{
  
  class Node;
  class Cell;
  class Mesh;
  class NewFiniteElement;
  class NewVector;
  
  /// This class represents a function defined on a mesh. The function
  /// is defined in terms of a mesh, a finite element and a vector
  /// containing the degrees of freedom of the function on the mesh.

  /// Vector functions are initialized by a vector x containing the 
  /// function values for all three vector components and no_comp, 
  /// the number of vector components.  
  /// The function values in x should be ordered as: 
  /// vector component j at dof i is listed at i*no_comp+j.   

  class NewFunction : public Variable
  {
  public:

    /// Create user-defined function
    NewFunction();

    /// Create a function in the function space defined by a finite element
    NewFunction(Mesh& mesh, NewVector& x, const NewFiniteElement& element);

    /// Create a piecewise linear function
    NewFunction(Mesh& mesh, NewVector& x);

    /// Destructor
    virtual ~NewFunction();

    /// Compute projection of function onto a given local finite element space
    void project(const Cell& cell, const NewFiniteElement& element, real c[]) const;

    /// Evaluate function at given node
    real operator() (const Node& node) const;

    /// Evaluate function at given point
    virtual real operator() (const Point& point) const;

    /// Evaluate vector-valued function at given point
    virtual real operator() (const Point& point, uint i) const;

    /// Return the mesh on which the function is defined
    Mesh& mesh();

    /// Return the finite element defining the function space
    const NewFiniteElement& element() const;

    /// Return current time
    real time() const;

    /// Set current time
    void set(real time);

  private:

    // Collect function data in one place
    class Data
    {
    public:
      Data(Mesh& mesh, NewVector& x, const NewFiniteElement& element)
	: mesh(mesh), x(x), element(element) {}
      Mesh& mesh;
      NewVector& x;
      const NewFiniteElement& element;
    };
    
    // Pointer to function data (null if not used)
    Data* data;

    // Current time
    real t;
    
  };

}

#endif
