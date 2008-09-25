// Copyright (C) 2005-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-01-28
// Last changed: 2008-04-08

#include "ODE.h"
#include "Method.h"
#include "TimeSlab.h"
#include "TimeSlabJacobian.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeSlabJacobian::TimeSlabJacobian(TimeSlab& timeslab)
  : ode(timeslab.ode), method(*timeslab.method)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
TimeSlabJacobian::~TimeSlabJacobian()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void TimeSlabJacobian::init()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void TimeSlabJacobian::update()
{
  cout << "Recomputing Jacobian" << endl;

  // Initialize matrix if not already done
  const uint N = ode.size();
  A.resize(N, N);
  ej.resize(N);
  Aj.resize(N);

  // Reset unit vector
  ej.zero();

  // Compute columns of Jacobian
  for (uint j = 0; j < ode.size(); j++)
  {
    ej[j] = 1.0;

    //cout << ej << endl;
    //cout << Aj << endl;
    
    // Compute product Aj = Aej
    mult(ej, Aj);
    
    // Set column of A
    column(A.mat(), j) = Aj.vec();
    
    ej[j] = 0.0;
  }
}
//-----------------------------------------------------------------------------
const uBLASDenseMatrix& TimeSlabJacobian::matrix() const
{
  dolfin_assert(A.size(0) == ode.size());
  return A;
}
//-----------------------------------------------------------------------------
