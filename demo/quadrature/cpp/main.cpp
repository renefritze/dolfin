// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-06-04
// Last changed: 2008-11-16

#include <cstring>
#include <dolfin.h>

using namespace dolfin;

int main(int argc, char** argv)
{
  if (argc != 3)
  {
    dolfin::cout << "Usage: dolfin-quadrature rule n' where rule is one of" << dolfin::endl;
    dolfin::cout << "gauss, radau, lobatto, and n is the number of points" << dolfin::endl;
    return 1;
  }

  int n = atoi(argv[2]);

  if (strcmp(argv[1], "gauss") == 0)
  {
    GaussQuadrature q(n);
    q.disp();
  }
  else if (strcmp(argv[1], "radau") == 0)
  {
    RadauQuadrature q(n);
    q.disp();
  }
  else if (strcmp(argv[1], "lobatto") == 0)
  {
    LobattoQuadrature q(n);
    q.disp();
  }
  else
  {
    dolfin::cout << "Unknown quadrature rule." << dolfin::endl;
    return 1;
  }

  return 0;
}
