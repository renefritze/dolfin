// Copyright (C) 2006-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-09-19
// Last changed: 2008-12-13
//
// This demo program computes the value of the functional
//
//     M(v) = int v^2 + (grad v)^2 dx
//
// on the unit square for v = sin(x) + cos(y). The exact
// value of the functional is M(v) = 2 + 2*sin(1)*(1-cos(1))
//
// The functional M corresponds to the energy norm for a
// simple reaction-diffusion equation.

#include <dolfin.h>
#include "EnergyNorm.h"
  
using namespace dolfin;

int main()
{
  // The function v
  class MyFunction : public Function
  {
  public: 

    MyFunction(const FunctionSpace& V) : Function(V) {}

    void eval(double* values, const double* x) const
    {
      values[0] = sin(x[0]) + cos(x[1]);
    }
    
  };

  // Define functional
  UnitSquare mesh(16, 16);
  EnergyNormCoefficientSpace V(mesh);
  MyFunction v(V);
  EnergyNormFunctional M;
  M.v = v;

  // Evaluate functional
  double approximate_value = assemble(M);

  // Compute exact value
  double exact_value = 2.0 + 2.0*sin(1.0)*(1.0 - cos(1.0));

  message("The energy norm of v is: %.15g", approximate_value);
  message("It should be:            %.15g", exact_value);
  
  return 0;
}
