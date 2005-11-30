// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-11-26
// Last changed: 2005-11-29

#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H

#include <dolfin/GenericFunction.h>
#include <dolfin/FiniteElement.h>

namespace dolfin
{
  class Function;

  /// This class implements the functionality for a user-defined
  /// function defined by overloading the evaluation operator in
  /// the class Function.

  class UserFunction : public GenericFunction
  {
  public:

    /// Create user-defined function
    UserFunction(Function* f, uint vectordim);

    /// Destructor
    ~UserFunction();

    /// Evaluate function at given point
    real operator() (const Point& point, uint i);

    /// Evaluate function at given node
    real operator() (const Node& node, uint i);

    /// Compute interpolation of function onto local finite element space
    void interpolate(real coefficients[], AffineMap& map, FiniteElement& element);

    /// Return vector dimension of function
    uint vectordim() const;

    /// Calling this function generates an error (no vector associated)
    Vector& vector();

    /// Return mesh associated with function (if any)
    Mesh& mesh();

    /// Calling this function generates an error (no element associated)
    FiniteElement& element();

    /// Calling this function generates an error (no vector can be attached)
    void attach(Vector& x);

    /// Attach mesh to function
    void attach(Mesh& mesh);

    /// Calling this function generates an error (no element can be attached)
    void attach(FiniteElement& element);

  private:

    // Pointer to Function with overloaded evaluation operator
    Function* f;

    // Number of vector dimensions
    uint _vectordim;

    // Pointer to mesh associated with function (null if none)
    Mesh* _mesh;
    
  };

}

#endif
