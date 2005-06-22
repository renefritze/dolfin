// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.

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
    dolfin_info("Usage: dolfin-ode-homotopy-ces m n alpha beta");
    dolfin_info("");
    dolfin_info("m     - number of traders");
    dolfin_info("n     - number of goods");
    dolfin_info("alpha - 1/scaling factor");
    dolfin_info("beta  - scaled exponents");
    return 1;
  }
  const unsigned int m = static_cast<unsigned int>(atoi(argv[1]));
  const unsigned int n = static_cast<unsigned int>(atoi(argv[2]));
  const unsigned int a = static_cast<unsigned int>(atoi(argv[3]));
  const unsigned int b = static_cast<unsigned int>(atoi(argv[4]));

  dolfin_set("method", "cg");
  dolfin_set("order", 1);
  dolfin_set("tolerance", 1e-3);
  dolfin_set("discrete tolerance", 1e-10);
  dolfin_set("initial time step", 0.0001);
  dolfin_set("linear solver", "direct");
  dolfin_set("adaptive samples", false);
  dolfin_set("homotopy monitoring", false);
  dolfin_set("homotopy divergence tolerance", 10.0);
  dolfin_set("homotopy randomize", false);
  dolfin_set("homotopy maximum size", 9);

  ces(m, n, a, b);

  return 0;
}
