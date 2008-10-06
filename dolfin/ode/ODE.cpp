// Copyright (C) 2003-2008 Johan Jansson and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-10-21
// Last changed: 2008-04-22

#include <dolfin/common/constants.h>
#include <dolfin/math/dolfin_math.h>
#include <dolfin/la/uBLASVector.h>
#include "ODESolver.h"
#include "ODE.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
ODE::ODE(uint N, double T)
  : N(N), T(T), dependencies(N), transpose(N), tmp(0),
    not_impl_f("Warning: consider implementing mono-adaptive ODE::f() to improve efficiency."),
    not_impl_M("Warning: multiplication with M not implemented, assuming identity."),
    not_impl_J("Warning: consider implementing Jacobian ODE::J() to improve efficiency."),
    not_impl_JT("Warning: consider implementing Jacobian transpose ODE::JT() to improve efficiency")
{
  message("Creating ODE of size %d.", N);
}
//-----------------------------------------------------------------------------
ODE::~ODE()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void ODE::f(const uBLASVector& u, double t, uBLASVector& y)
{
  // If a user of the mono-adaptive solver does not supply this function,
  // then call f_i() for each component.

  // Display a warning, more efficiently if implemented
  not_impl_f();

  // Call f for each component
  for (uint i = 0; i < N; i++)
    y[i] = this->f(u, t, i);
}
//-----------------------------------------------------------------------------
double ODE::f(const uBLASVector& u, double t, uint i)
{
  error("Right-hand side for ODE not supplied by user.");
  return 0.0;
}
//-----------------------------------------------------------------------------
void ODE::M(const uBLASVector& x, uBLASVector& y, const uBLASVector& u, double t)
{
  // Display a warning, implicit system but M is not implemented
  not_impl_M();

  // Assume M is the identity if not supplied by user: y = x
  y = x;
}
//-----------------------------------------------------------------------------
void ODE::J(const uBLASVector& x, uBLASVector& y, const uBLASVector& u, double t)
{
  // If a user does not supply J, then compute it by the approximation
  //
  //     Jx = ( f(u + hx) - f(u - hx) ) / 2h

  // FIXME: Maybe we should move this somewhere else?

  // Display a warning, more efficiently if implemented
  not_impl_J();

  // Small change in u
  double umax = 0.0;
  for (unsigned int i = 0; i < N; i++)
    umax = std::max(umax, std::abs(u[i]));
  double h = std::max(DOLFIN_SQRT_EPS, DOLFIN_SQRT_EPS * umax);

  // We are not allowed to change u, but we restore it afterwards,
  // so maybe we can cheat a little...
  uBLASVector& uu = const_cast<uBLASVector&>(u);

  // Initialize temporary array if necessary
  if ( tmp.size() != N )
  {
    tmp.resize(N);
    tmp.zero();
  }

  // Evaluate at u + hx
  ublas::noalias(uu.vec()) += h*x.vec();
  f(uu, t, y);

  // Evaluate at u - hx
  ublas::noalias(uu.vec()) -= 2.0*h*x.vec();
  f(uu, t, tmp);

  // Reset u
  ublas::noalias(uu.vec()) += h*x.vec();

  // Compute product y = Jx
  ublas::noalias(y.vec()) -= tmp.vec();
  y *= 0.5/h;
}
//------------------------------------------------------------------------

void ODE::JT(const uBLASVector& x, uBLASVector& y, const uBLASVector& u, double t) {

  //Display warning
  not_impl_JT();

  // Similar to J()
  double umax = 0.0;
  for (unsigned int i = 0; i < N; i++)
    umax = std::max(umax, std::abs(u[i]));
  double h = std::max(DOLFIN_SQRT_EPS, DOLFIN_SQRT_EPS * umax);

  uBLASVector& uu = const_cast<uBLASVector&>(u);

  if ( tmp.size() != N )
  {
    tmp.resize(N);
    tmp.zero();
  }  

  uBLASVector tmp2(N);

  for (uint i = 0; i < N; ++i) {
    uu[i] += h; //small change component i
    f(uu, t, tmp);
    
    uu[i] -= 2*h;
    f(uu, t, tmp2);

    //restore u
    uu[i] += h;

    tmp -= tmp2;
    tmp /= 2*h;

    y[i] = ublas::inner_prod(tmp.vec(), x.vec());
  }
}

//------------------------------------------------------------------------

double ODE::dfdu(const uBLASVector& u, double t, uint i, uint j)
{
  // Compute Jacobian numerically if dfdu() is not implemented by user
  
  // FIXME: Maybe we should move this somewhere else?

  // We are not allowed to change u, but we restore it afterwards,
  // so maybe we can cheat a little...
  uBLASVector& uu = const_cast<uBLASVector&>(u);

  // Save value of u_j
  double uj = uu[j];
  
  // Small change in u_j
  double h = std::max(DOLFIN_SQRT_EPS, DOLFIN_SQRT_EPS * std::abs(uj));
  
  // Compute F values
  uu[j] -= 0.5 * h;
  double f1 = f(uu, t, i);
  
  uu[j] = uj + 0.5*h;
  double f2 = f(uu, t, i);
         
  // Reset value of uj
  uu[j] = uj;

  // Compute derivative
  if ( std::abs(f1 - f2) < DOLFIN_EPS * std::max(std::abs(f1), std::abs(f2)) )
    return 0.0;

  return (f2 - f1) / h;
}
//-----------------------------------------------------------------------------
double ODE::timestep(double t, double k0) const
{
  // Keep old time step by default when "fixed time step" is set
  // and user has not overloaded this function
  return k0;
}
//-----------------------------------------------------------------------------
double ODE::timestep(double t, uint i, double k0) const
{
  // Keep old time step by default when "fixed time step" is set
  // and user has not overloaded this function
  return k0;
}
//-----------------------------------------------------------------------------
bool ODE::update(const uBLASVector& u, double t, bool end)
{
  return true;
}
//-----------------------------------------------------------------------------
void ODE::save(Sample& sample)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
double ODE::time(double t) const
{
  return t;
}
//-----------------------------------------------------------------------------
void ODE::sparse()
{
  dependencies.detect(*this);
}
//-----------------------------------------------------------------------------
void ODE::sparse(const uBLASSparseMatrix& A)
{
  dependencies.set(A);
}
//-----------------------------------------------------------------------------
dolfin::uint ODE::size() const
{
  return N;  
}
//-----------------------------------------------------------------------------
double ODE::endtime() const
{
  return T;
}
//-----------------------------------------------------------------------------
void ODE::solve()
{
  ODESolver::solve(*this);
}
//-----------------------------------------------------------------------------
