// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-11-26
// Last changed: 2005-11-29

#ifndef __DISCRETE_FUNCTION_H
#define __DISCRETE_FUNCTION_H

#include <dolfin/GenericFunction.h>

namespace dolfin
{
  class Vector;
  class Mesh;
  
  /// This class implements the functionality for a discrete function
  /// of some given finite element space, defined by a vector of degrees
  /// of freedom, a mesh, and a finite element.

  class DiscreteFunction : public GenericFunction
  {
  public:

    /// Create discrete function from given data
    DiscreteFunction(Vector& x);

    /// Create discrete function from given data
    DiscreteFunction(Vector& x, Mesh& mesh);

    /// Create discrete function from given data
    DiscreteFunction(Vector& x, Mesh& mesh, FiniteElement& element);

    /// Destructor
    ~DiscreteFunction();

    /// Evaluate function at given point
    real operator() (const Point& point, uint i);

    /// Evaluate function at given node
    real operator() (const Node& node, uint i);

    /// Compute interpolation of function onto local finite element space
    void interpolate(real coefficients[], AffineMap& map, FiniteElement& element);

    /// Return vector dimension of function
    uint vectordim() const;

    /// Return vector associated with function (if any)
    Vector& vector();

    /// Return mesh associated with function (if any)
    Mesh& mesh();

    /// Return element associated with function (if any)
    FiniteElement& element();

    /// Attach vector to function
    void attach(Vector& x);

    /// Attach mesh to function
    void attach(Mesh& mesh);

    /// Attach finite element to function
    void attach(FiniteElement& element);\

  private:

    // Pointer to degrees of freedom
    Vector* _x;

    // Pointer to mesh
    Mesh* _mesh;

    // Pointer to finite element
    FiniteElement* _element;
    
  };

}

#endif
