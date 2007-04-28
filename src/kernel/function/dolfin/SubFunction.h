// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2007-04-27
// Last changed: 2007-04-27

#ifndef __SUB_FUNCTION_H
#define __SUB_FUNCTION_H

#include <dolfin/constants.h>

namespace dolfin
{

  class DiscreteFunction;

  /// This class represents a sub function (view) of a (discrete function).
  /// It's purpose is to enable expressions like
  ///
  ///    Function w;
  ///    Function u = w[0];
  ///    Function p = w[1];
  ///
  /// without needing to create and destroy temporaries. No data is created
  /// until a Function is assigned to a SubFunction, at which point the data
  /// needed to represent the sub function is created.

  class SubFunction
  {
  public:

    /// Create sub function
    SubFunction(DiscreteFunction* f, uint i) : f(f), i(i) {}

    /// Destructor
    ~SubFunction() {}

    /// Friends
    friend class DiscreteFunction;

  private:

    // Pointer to discrete function
    DiscreteFunction* f;

    // Sub function index
    uint i;

  };

}

#endif
