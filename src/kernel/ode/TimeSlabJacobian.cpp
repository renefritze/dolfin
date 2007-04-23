// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-01-28
// Last changed: 2006-08-08

#include <dolfin/ODE.h>
#include <dolfin/Method.h>
#include <dolfin/TimeSlab.h>
#include <dolfin/TimeSlabJacobian.h>

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
  A.init(N, N);
  ej.init(N);
  Aj.init(N);

  // Reset unit vector
  ej = 0.0;

  // Compute columns of Jacobian
  for (uint j = 0; j < ode.size(); j++)
  {
    ej(j) = 1.0;

    //cout << ej << endl;
    //cout << Aj << endl;
    
    // Compute product Aj = Aej
    mult(ej, Aj);
    
    // Set column of A
    column(A, j) = Aj;
    
    ej(j) = 0.0;
  }
}
//-----------------------------------------------------------------------------
