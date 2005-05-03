// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <string>
#include <dolfin/dolfin_settings.h>
#include <dolfin/ODE.h>
#include <dolfin/cGqMethod.h>
#include <dolfin/dGqMethod.h>
#include <dolfin/TimeSlab.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
TimeSlab::TimeSlab(ODE& ode) : 
  N(ode.size()), _a(0.0), _b(0.0), ode(ode), method(0), u0(0)
{
  // Choose method
  std::string m = dolfin_get("method");
  int q = dolfin_get("order");
  if ( m == "cg" || m == "mcg" )
    method = new cGqMethod(q);
  else if ( m == "dg" || m == "mdg" )
    method = new dGqMethod(q);
  else
    dolfin_error1("Unknown ODE method: %s", m.c_str());

  // Initialize initial data
  u0 = new real[N];

  // Get initial data
  for (uint i = 0; i < N; i++)
  {
    u0[i] = ode.u0(i);
    //cout << "u0[" << i << "] = " << u0[i] << endl;
  }
}
//-----------------------------------------------------------------------------
TimeSlab::~TimeSlab()
{
  if ( method ) delete method;
  if ( u0 ) delete [] u0;
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
dolfin::LogStream& dolfin::operator<<(LogStream& stream,
				      const TimeSlab& timeslab)
{
  stream << "[ TimeSlab of length " << timeslab.length()
	 << " between a = " << timeslab.starttime()
	 << " and b = " << timeslab.endtime() << " ]";
  
  return stream;
}
//-----------------------------------------------------------------------------
