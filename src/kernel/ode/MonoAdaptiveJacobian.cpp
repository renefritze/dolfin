// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/dolfin_math.h>
#include <dolfin/ODE.h>
#include <dolfin/NewVector.h>
#include <dolfin/NewMethod.h>
#include <dolfin/MonoAdaptiveTimeSlab.h>
#include <dolfin/MonoAdaptiveJacobian.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
MonoAdaptiveJacobian::MonoAdaptiveJacobian(MonoAdaptiveTimeSlab& timeslab)
  : TimeSlabJacobian(timeslab), ts(timeslab), z(0)
{
  z = new real[ts.N];
}
//-----------------------------------------------------------------------------
MonoAdaptiveJacobian::~MonoAdaptiveJacobian()
{
  if ( z ) delete [] z;
}
//-----------------------------------------------------------------------------
void MonoAdaptiveJacobian::mult(const NewVector& x, NewVector& y) const
{
  cout << "Computing product with mono-adaptive Jacobian" << endl;

  // Start with y = x, accounting for the derivative dF_j/dx_j = 1
  y = x;
  
  // Get data arrays (assumes uniprocessor case)
  const real* xx = x.array();
  real* yy = y.array();

  // Iterate over the stages
  for (uint n = 0; n < method.nsize(); n++)
  {
    const uint noffset = n * ts.N;

    // Compute z = df/du * x for current stage
    for (uint i = 0; i < ts.N; i++)
    {
      real sum = 0.0;
      const NewArray<uint>& deps = ode.dependencies[i];
      const uint Joffset = Jindices[i];
      for (uint pos = 0; pos < deps.size(); pos++)
      {
	const uint j = deps[pos];
	sum += Jvalues[Joffset + pos] * xx[noffset + j];
      }
      z[i] = sum;
    }

    // Add z with correct weights to y
    for (uint m = 0; m < method.nsize(); m++)
    {
      const uint moffset = m * ts.N;

      // Get correct weight
      real w = 0.0;
      if ( method.type() == NewMethod::cG )
	w = method.nweight(m, n + 1);
      else
	w = method.nweight(m, n);

      // Add w * z to y
      for (uint i = 0; i < ts.N; i++)
	yy[moffset + i] += w * z[i];
    }
  }

  // Restore data arrays
  x.restore(xx);
  y.restore(yy);
}
//-----------------------------------------------------------------------------
