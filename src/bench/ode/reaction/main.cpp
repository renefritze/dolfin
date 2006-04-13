// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-10-14
// Last changed: 2006-04-12

#include <dolfin.h>

using namespace dolfin;

/// Test problem taken from "Multirate time stepping for parabolic PDEs"
/// by Savcenco, Hundsdorfer and Verwer:
///
///    u' - epsilon u'' = gamma u^2 (1 - u)  in (0, L) x (0, T).
///
/// The solution is a reaction front sweeping across the domain.

class Reaction : public ODE
{
public:

  /// Constructor
  Reaction(unsigned int N, real T, real L, real epsilon, real gamma)
    : ODE(N, T), L(L), epsilon(epsilon), gamma(gamma)
  {
    // Compute parameters
    h = L / static_cast<real>(N - 1);
    lambda = 0.5*sqrt(2.0*gamma/epsilon);
    v = 0.5*sqrt(2.0*gamma*epsilon);
    
    // Set sparse dependency pattern
    for (unsigned int i = 0; i < N; i++)
    {
      dependencies.setsize(i, 3);
      if ( i > 0 ) dependencies.set(i, i - 1);
      dependencies.set(i, i);
      if ( i < (N - 1) ) dependencies.set(i, i + 1);
    }
  }

  /// Initial condition
  real u0(unsigned int i)
  {
    const real x = static_cast<real>(i)*h;
    return 1.0 / (1.0 + exp(lambda*(x - 1.0)));
  }

  /// Right-hand side, mono-adaptive version
  void f(const real u[], real t, real y[])
  {
    for (unsigned int i = 0; i < N; i++)
    {
      const real ui = u[i];

      real sum = 0.0;
      if ( i == 0 )
	sum = u[i + 1] - ui;
      else if ( i == (N - 1) )
	sum = u[i - 1] - ui;
      else
	sum = u[i + 1] - 2.0*ui + u[i - 1];

      y[i] = epsilon * sum / (h*h) + gamma * ui*ui * (1.0 - ui);
    }
  }

  /// Right-hand side, multi-adaptive version
  real f(const real u[], real t, unsigned int i)
  {
    const real ui = u[i];
    
    real sum = 0.0;
    if ( i == 0 )
      sum = u[i + 1] - ui;
    else if ( i == (N - 1) )
      sum = u[i - 1] - ui;
    else
      sum = u[i + 1] - 2.0*ui + u[i - 1];
    
    return epsilon * sum / (h*h) + gamma * ui*ui * (1.0 - ui);
  }

  /// Specify time step, mono-adaptive version (used for testing)
  real timestep(real t, real k0) const
  {
    return 1e-5;
  }

  /// Specify time step, mono-adaptive version (used for testing)
  real timestep(real t, unsigned int i, real k0) const
  {
    const real w  = 0.1;
    const real v  = 2.22;
    const real x  = static_cast<real>(i)*h;
    if ( fabs(x - 1.0 - v*t) < w )
      return 1e-5;
    else
      return 1e-3;
  }

public:

  real L;       // Length of domain
  real epsilon; // Diffusivity
  real gamma;   // Reaction rate
  real h;       // Mesh size
  real lambda;  // Parameter for initial data
  real v;       // Speed of reaction front

};

int main(int argc, char* argv[])
{
  // Parse command line arguments
  if ( argc != 7 )
  {
    dolfin_info("Usage: dolfin-reaction method solver tol kmax N L");
    dolfin_info("");
    dolfin_info("method - 'cg' or 'mcg'");
    dolfin_info("solver - 'fixed-point' or 'newton'");
    dolfin_info("tol    - tolerance");
    dolfin_info("kmax   - initial time step");
    dolfin_info("N      - number of components");
    dolfin_info("L      - length of domain");
    return 1;
  }
  const char* method = argv[1];
  const char* solver = argv[2];
  const real tol = static_cast<real>(atof(argv[3]));
  const real kmax = static_cast<real>(atof(argv[4]));
  const unsigned int N = static_cast<unsigned int>(atoi(argv[5]));
  const real L = static_cast<unsigned int>(atof(argv[6]));
  
  // Load solver parameters from file
  File file("parameters.xml");
  file >> ParameterSystem::parameters;

  // Set remaining solver parameters from command-line arguments
  set("method", method);
  set("solver", solver);
  set("tolerance", tol);
  set("maximum time step", kmax);

  // Set fixed parameters for test problem
  const real T = 1.0;
  const real epsilon = 0.01;
  const real gamma = 1000.0;

  // Solve system of ODEs
  Reaction ode(N, T, L, epsilon, gamma);
  ode.solve();

  return 0;
}
