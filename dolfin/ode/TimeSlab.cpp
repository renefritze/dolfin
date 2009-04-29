// Copyright (C) 2005-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-05-02
// Last changed: 2008-10-06

#include <stdio.h>
#include <string>
#include <dolfin/parameter/parameters.h>
#include <dolfin/la/uBLASVector.h>
#include "ODE.h"
#include "cGqMethod.h"
#include "dGqMethod.h"
#include "TimeSlab.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeSlab::TimeSlab(ODE& ode) :
  N(ode.size()), _a(0.0), _b(0.0), ode(ode), method(0), u0(0),
  save_final(ode.get("ODE save final solution"))
{
  // Choose method
  std::string m = ode.get("ODE method");
  int q = ode.get("ODE order");
  if ( m == "cg" || m == "mcg" )
  {
    if ( q < 1 )
      error("Minimal order is q = 1 for continuous Galerkin.");
    method = new cGqMethod(q);
  }
  else if ( m == "dg" || m == "mdg" )
  {
    if ( q < 0 )
      error("Minimal order is q = 0 for discontinuous Galerkin.");
    method = new dGqMethod(q);
  }
  else
    error("Unknown ODE method: %s", m.c_str());

  // Get iinitial data
  u0 = new real[ode.size()];
  real_zero(ode.size(), u0);
  ode.u0(u0);
}
//-----------------------------------------------------------------------------
TimeSlab::~TimeSlab()
{
  delete method;
  delete [] u0;
}
//-----------------------------------------------------------------------------
dolfin::uint TimeSlab::size() const
{
  return N;
}
//-----------------------------------------------------------------------------
real TimeSlab::starttime() const
{
  return _a;
}
//-----------------------------------------------------------------------------
real TimeSlab::endtime() const
{
  return _b;
}
//-----------------------------------------------------------------------------
real TimeSlab::length() const
{
  return _b - _a;
}
//-----------------------------------------------------------------------------
void TimeSlab::set_state(const real* u)
{
  for (uint i = 0; i < N; i++)
    u0[i] = u[i];
}
//-----------------------------------------------------------------------------
void TimeSlab::get_state(real* u)
{
  for (uint i = 0; i < N; i++)
    u[i] = u0[i];
}
//-----------------------------------------------------------------------------
dolfin::LogStream& dolfin::operator<<(LogStream& stream,
				      const TimeSlab& timeslab)
{
  stream << "[ TimeSlab of length " << timeslab.length()
	 << " between a = " << timeslab.starttime()
	 << " and b = " << timeslab.endtime() << " ]";

  return stream;
}
//-----------------------------------------------------------------------------
void TimeSlab::write(uint N, const real* u)
{
  // FIXME: Make this a parameter?
  std::string filename = "solution.data";

  message("Saving solution at final time to file \"%s\".", filename.c_str());
  #ifdef HAS_GMP
    warning("Precision in solution file lost. Saving with double precision");
  #endif

  FILE* fp = fopen(filename.c_str(), "w");
  for (uint i = 0; i < N; i++)
  {
    fprintf(fp, "%.15e ", to_double(u[i]));
  }
  fprintf(fp, "\n");
  fclose(fp);
}
//-----------------------------------------------------------------------------
void TimeSlab::copy(const real x[], uint xoffset, real y[], uint yoffset, uint n)
{
  for (uint i = 0; i < n; i++)
    y[yoffset + i] = x[xoffset + i];
}
//-----------------------------------------------------------------------------
void TimeSlab::copy(const uBLASVector& x, uint xoffset, real y[], uint yoffset, uint n)
{
  for (uint i = 0; i < n; i++)
  {
    //Note: Precision lost if working with GMP
    y[yoffset + i] = x[xoffset + i];
  }
}
//-----------------------------------------------------------------------------
void TimeSlab::copy(const real x[], uint xoffset, uBLASVector& y, uint yoffset, uint n)
{
  for (uint i = 0; i < n; i++)
    y[yoffset + i] = to_double(x[xoffset + i]);

}
//-----------------------------------------------------------------------------
void TimeSlab::copy(const uBLASVector& x, uint xoffset, uBLASVector& y, uint yoffset, uint n)
{
  for (uint i = 0; i < n; i++)
    y[yoffset + i] = x[xoffset + i];
}
//-----------------------------------------------------------------------------
