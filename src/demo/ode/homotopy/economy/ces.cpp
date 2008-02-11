// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005
// Last changed: 2005-12-19

#include <stdlib.h>
#include <dolfin.h>
#include "CES.h"

using namespace dolfin;

void ces(unsigned int m, unsigned int n, unsigned int alpha, unsigned int beta)
{
  // Seed random number generator so we get the same system every time
  seed(0);

  PolynomialIntegerCES ec(m, n, true);

  ec.alpha = alpha;

  for (unsigned int i = 0; i < m; i++)
    ec.beta[i] = beta;

  ec.disp();
  ec.solve();
}

int main(int argc, const char* argv[])
{
  // Parse command line arguments
  if ( argc != 5 )
  {
    message("Usage: dolfin-ode-homotopy-ces m n alpha beta");
    message("");
    message("m     - number of traders");
    message("n     - number of goods");
    message("alpha - 1/scaling factor");
    message("beta  - scaled exponents");
    return 1;
  }
  const unsigned int m = static_cast<unsigned int>(atoi(argv[1]));
  const unsigned int n = static_cast<unsigned int>(atoi(argv[2]));
  const unsigned int a = static_cast<unsigned int>(atoi(argv[3]));
  const unsigned int b = static_cast<unsigned int>(atoi(argv[4]));

  dolfin_set("ODE method", "cg");
  dolfin_set("ODE order", 1);
  dolfin_set("ODE tolerance", 1e-3);
  dolfin_set("ODE discrete tolerance", 1e-10);
  dolfin_set("ODE initial time step", 0.001);
  //dolfin_set("ODE linear solver", "direct");
  dolfin_set("ODE adaptive samples", false);
  dolfin_set("homotopy monitoring", false);
  dolfin_set("homotopy divergence tolerance", 10.0);
  dolfin_set("homotopy randomize", false);
  dolfin_set("homotopy maximum size", 100);
  dolfin_set("homotopy maximum degree", 5);

  ces(m, n, a, b);

  return 0;
}
