// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-11-26
// Last changed: 2006-05-07

#ifndef __FUNCTION_POINTER_FUNCTION_H
#define __FUNCTION_POINTER_FUNCTION_H

#include <dolfin/FunctionPointer.h>
#include <dolfin/GenericFunction.h>

namespace dolfin
{
  /// This class implements the functionality for a user-defined
  /// function given by a function pointer.

  class FunctionPointerFunction : public GenericFunction
  {
  public:

    /// Create function from function pointer
    FunctionPointerFunction(FunctionPointer f, uint vectordim);

    /// Copy constructor
    FunctionPointerFunction(const FunctionPointerFunction& f);

    /// Destructor
    ~FunctionPointerFunction();

    /// Evaluate function at given point
    real operator() (const Point& point, uint i);

    /// Evaluate function at given vertex
    real operator() (const Vertex& vertex, uint i);

    // Restrict to sub function or component (if possible)
    void sub(uint i);

    /// Compute interpolation of function onto local finite element space
    void interpolate(real coefficients[], AffineMap& map, FiniteElement& element);

    /// Return vector dimension of function
    uint vectordim() const;

#ifdef HAVE_PETSC_H
    /// Calling this function generates an error (no vector associated)
    Vector& vector();
#endif

    /// Return mesh associated with function (if any)
    Mesh& mesh();

    /// Calling this function generates an error (no element associated)
    FiniteElement& element();

#ifdef HAVE_PETSC_H
    /// Calling this function generates an error (no vector can be attached)
    void attach(Vector& x, bool local);
#endif

    /// Attach mesh to function
    void attach(Mesh& mesh, bool local);

    /// Calling this function generates an error (no element can be attached)
    void attach(FiniteElement& element, bool local);

  private:
    
    // Function pointer to user-defined function
    FunctionPointer f;

    // Number of vector dimensions
    uint _vectordim;

    // Current component
    uint component;

    // Pointer to mesh associated with function (null if none)
    Mesh* _mesh;

    // True if mesh is local (not a reference to another mesh)
    bool mesh_local;
    
  };

}

#endif
