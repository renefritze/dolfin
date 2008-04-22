// Copyright (C) 2005-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2006.
//
// First added:  2005
// Last changed: 2008-04-22

#include <stdio.h>
#include <limits>
#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/math/dolfin_math.h>
#include <dolfin/parameter/parameters.h>
#include "ComplexODE.h"
#include "HomotopyJacobian.h"
#include "HomotopyODE.h"
#include "Homotopy.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Homotopy::Homotopy(uint n)
  : tol(0), n(n), M(0), maxiter(0), maxpaths(0), maxdegree(0),
    divtol(0), monitor(false), random(false), 
    filename(""), mi(0), ci(0), tmp(0), x(2*n),
    degree_adjusted("Adjusting degree of equation, maximum reached.")
{
  message("Creating homotopy for system of size %d.", n);
  
  // We should not solve the dual problem
  dolfin_set("ODE solve dual problem", false);

  // System is implicit
  dolfin_set("ODE implicit", true);

  // Get divergence tolerance
  divtol = dolfin_get("homotopy divergence tolerance");

  // Check if we should monitor the homotopy
  monitor = dolfin_get("homotopy monitoring");

  // Get type of initial data (random or morgan)
  random = dolfin_get("homotopy randomize");
  if ( random )
    message("Using random initial system for homotopy.");

  // Get maximum number of iterations
  maxiter = dolfin_get("ODE maximum iterations");

  // Get maximum number of paths
  maxpaths = dolfin_get("homotopy maximum size");
  message("Maximum number of homotopy paths is %d.", maxpaths);

  // Get maximum degree for a single equation
  maxdegree = dolfin_get("homotopy maximum degree");
  message("Maximum degree for a single equations is %d.", maxdegree);

  // Get filename
  filename = static_cast<std::string>(dolfin_get("homotopy solution file name"));
  FILE* fp = fopen(filename.c_str(), "w");
  fclose(fp);

  // FIXME: Maybe this should be a parameter?
  tol = dolfin_get("homotopy solution tolerance");
  
  // Initialize array mi
  mi = new uint[n];
  for (uint i = 0; i < n; i++)
    mi[i] = 0;

  // Initialize array ci
  ci = new complex[n];
  for (uint i = 0; i < n; i++)
    ci[i] = 0.0;

  // Randomize vector ci
  randomize();
}
//-----------------------------------------------------------------------------
Homotopy::~Homotopy()
{
  if ( mi ) delete [] mi;
  if ( ci ) delete [] ci;
  if ( tmp ) delete [] tmp;
  
  for (unsigned int i = 0; i < zs.size(); i++)
    delete [] zs[i];
}
//-----------------------------------------------------------------------------
void Homotopy::solve()
{
  // Compute the total number of paths
  M = countPaths();
  message("Total number of paths is %d.", M);
  if ( M > maxpaths )
    message("Computing only %d paths as requested.", maxpaths);

  char filename[64];

  uint num_roots = 0;
  for (uint m = 0; m < M; m++)
  {
    message("\nComputing path number %d out of %d.", m + 1, M);

    // Change name of output file for each path
    sprintf(filename, "solution_%u.py", m);
    dolfin_set("ODE solution file name", filename);

    // Compute the component paths from global path number
    computePath(m);

    // Create and solve ODE
    // FIXME: what T should be used for HomotopyODE?
    HomotopyODE ode(*this, n, 1.0);
    ode.solve();

    // Use Newton's method to find the solution
    if ( ode.state() == HomotopyODE::endgame )
    {
      message("Homotopy path converged, using Newton's method to improve solution.");
      if ( computeSolution(ode) )
      {
	num_roots += 1;
	saveSolution();
      }
    }

    message("Number of solutions so far: %d (%d dropped).",
		zs.size(), num_roots - zs.size());

    // Check if we reached the maximum number of paths
    if ( m == (maxpaths - 1) )
    {
      message("Maximum number of paths reached, stopping.");
      break;
    }
  }

  message("\nTotal number of solutions found: %d (%d dropped).",
	      zs.size(), num_roots - zs.size());
}
//-----------------------------------------------------------------------------
const Array<complex*>& Homotopy::solutions() const
{
  return zs;
}
//-----------------------------------------------------------------------------
void Homotopy::z0(complex z[])
{
  for (uint i = 0; i < n; i++)
  {
    const real pp = static_cast<real>(adjustedDegree(i));
    const real mm = static_cast<real>(mi[i]);
    const complex c = ci[i];
    
    // Pick root number m of equation z_i^(p + 1) = c_i
    real r = std::pow(std::abs(c), 1.0/(pp + 1.0));
    real a = std::arg(c) / (pp + 1.0);
    z[i] = std::polar(r, a + mm/(pp + 1.0)*2.0*DOLFIN_PI);
  }
}
//-----------------------------------------------------------------------------
void Homotopy::G(const complex z[], complex y[])
{
  // Implement default starting system if not supplied by user

  // Compute G_i(z_i) = z_i^(p_i + 1) - c_i
  for (uint i = 0; i < n; i++)
  {
    const uint p = adjustedDegree(i);
    const complex zi = z[i];
    complex tmp = zi;
    for (uint j = 0; j < p; j++)
      tmp *= zi;

    y[i] = tmp - ci[i];
  }
}
//-----------------------------------------------------------------------------
void Homotopy::JG(const complex z[], const complex x[], complex y[])
{
  // Implement default starting system if not supplied by user
  
  // Compute (G'(z)*x)_i = (p_i + 1) z_i^p_i x_i
  for (uint i = 0; i < n; i++)
  {
    const uint p = adjustedDegree(i);
    const complex zi = z[i];
    complex tmp = static_cast<complex>(p + 1);
    for (uint j = 0; j < p ; j++)
      tmp *= zi;
    
    y[i] = tmp * x[i];
  }
}
//-----------------------------------------------------------------------------
void Homotopy::modify(complex z[])
{
  // Do nothing
}
//-----------------------------------------------------------------------------
bool Homotopy::verify(const complex z[])
{
  // Initialize temporary array
  if ( !tmp )
    tmp = new complex[n];

  // Evaluate y = F(z)
  F(z, tmp);

  // Check size of y
  real Fmax = 0.0;
  for (unsigned int i = 0; i < n; i++)
    Fmax = std::max(Fmax, std::abs(tmp[i]));
  
  return Fmax < 2.0*tol;
}
//-----------------------------------------------------------------------------
dolfin::uint Homotopy::adjustedDegree(uint i)
{
  uint q = degree(i);

  if ( q > maxdegree )
  {
    degree_adjusted();
    q = maxdegree;
  }

  return q;
}
//-----------------------------------------------------------------------------
dolfin::uint Homotopy::countPaths()
{
  uint product = 1;
  for (uint i = 0; i < n; i++)
  {
    if ( (adjustedDegree(i) + 1) * product <= product )
    {
      const int max_paths = std::numeric_limits<int>::max();
      error("Reached maximum number of homotopy paths (%d).", max_paths);
    }
    
    product *= (adjustedDegree(i) + 1);
  }

  return product;
}
//-----------------------------------------------------------------------------
void Homotopy::computePath(uint m)
{
  // The path number for each component can vary between 0 and p_i + 1,
  // and we need to compute the local path number for a given component
  // from the global path number which varies between 0 and the product of
  // all p_i + 1. This algorithm is copied from FFC (compiler.multiindex).
  
  uint posvalue = M;
  uint sum = m;
  for (uint i = 0; i < n; i++)
  {
    const uint dim = adjustedDegree(i) + 1;
    posvalue /= dim;
    const uint digit = sum / posvalue;
    mi[i] = digit;
    sum -= digit * posvalue;
  }
}
//-----------------------------------------------------------------------------
bool Homotopy::computeSolution(HomotopyODE& ode)
{
  // Create right-hand side and increment vector
  uBlasVector F(2*n), dx(2*n);

  // Create matrix-free Jacobian
  HomotopyJacobian J(ode, x);
  
  cout << "Starting point:     x = ";
  x.disp();

  // Solve system using Newton's method
  for (uint iter = 0; iter < maxiter; iter++)
  {
    // Evaluate right-hand side at current x
    feval(F, ode);

    // Check convergence
    real r = F.norm(linf);
    //cout << "r = " << r << ": x = "; x.disp();
    if ( r < tol )
    {
      cout << "Solution converged: x = ";
      x.disp();
      return true;
    }

    // Solve linear system
    solver.solve(J, dx, F);

    // Subtract increment
    x -= dx;
  }

  warning("Solution did not converge.");
  return false;
}
//-----------------------------------------------------------------------------
void Homotopy::saveSolution()
{
  // Copy values to complex array
  complex* z = new complex[n];
  for (uint i = 0; i < n; i++)
    z[i] = complex(x(2*i), x(2*i + 1));

  // Allow user to modify solution
  modify(z);

  // Check if solution is valid
  if ( !verify(z) )
  {
    message("Verification of solution failed, dropping solution.");
    delete [] z;
    return;
  }

  // Save solution
  message("Solution verified, keeping solution.");
  zs.push_back(z);

  // Save solution to file
  FILE* fp = fopen(filename.c_str(), "a");
  for (uint i = 0; i < n; i++)
    fprintf(fp, "%.15e %.15e ", z[i].real(), z[i].imag());
  fprintf(fp, "\n");
  fclose(fp);
}
//-----------------------------------------------------------------------------
void Homotopy::randomize()
{
  // Choose values for c
  for (uint i = 0; i < n; i++)
  {
    if ( random )
    {
      // Randomize each c in the unit circle
      const real r = rand();
      const real a = 2.0*DOLFIN_PI*rand();
      const complex c = std::polar(r, a);
      ci[i] = c;
    }
    else
    {
      // Choice from Morgan's paper
      const complex c(0.00143289 + static_cast<real>(i), 0.983727);
      ci[i] = c;
    }
  }

  //ci[0] = complex(-2.831791604946104e-02, -6.860112583567136e-01);
  //ci[1] = complex(8.464610889887528e-02, 7.660579271509420e-02);

  // Write to file
  FILE* fp = fopen("initialsystem.data", "w");
  for (uint i = 0; i < n; i++)
    fprintf(fp, "%.15e %.15e ", ci[i].real(), ci[i].imag());
  fprintf(fp, "\n");
  fclose(fp);
}
//-----------------------------------------------------------------------------
void Homotopy::feval(uBlasVector& F, ComplexODE& ode)
{
  // Reuse the right-hand side of the ODE so we don't have to reimplement
  // the mapping from complex to real numbers

  // Evaluate F at current x
  ode.f(x, 0.0, F);
}
//-----------------------------------------------------------------------------
