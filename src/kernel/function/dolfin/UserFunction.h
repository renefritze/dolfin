// Copyright (C) 2005-2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-11-26
// Last changed: 2007-04-12

#ifndef __USER_FUNCTION_H
#define __USER_FUNCTION_H

#include <dolfin/GenericFunction.h>

namespace dolfin
{
  class Function;

  /// This class implements the functionality for a user-defined
  /// function defined by overloading the evaluation operator in
  /// the class Function.

  class UserFunction : public GenericFunction, public ufc::function
  {
  public:

    /// Create user-defined function
    UserFunction(Function* f);

    /// Destructor
    ~UserFunction();

    /// Return the rank of the value space
    uint rank() const;

    /// Return the dimension of the value space for axis i
    uint dim(uint i) const;

    /// Interpolate function to vertices of mesh
    void interpolate(real* values, Mesh& mesh);

    /// Interpolate function to finite element space on cell
    void interpolate(real coefficients[],
                     const ufc::cell& cell,
                     const ufc::finite_element& finite_element);

    /// Evaluate function at given point in cell (UFC function interface)
    void evaluate(real* values,
                  const real* coordinates,
                  const ufc::cell& cell) const;

  private:

    // Pointer to Function with overloaded evaluation operator
    Function* f;

    // Size of value (number of entries in tensor value)
    uint size;

  };

}

#endif
