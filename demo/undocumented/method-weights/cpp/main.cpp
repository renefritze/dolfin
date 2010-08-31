// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2003-10-21
// Last changed: 2009-08-11

#include <cstring>
#include <dolfin.h>

using namespace dolfin;

int main(int argc, char** argv)
{

  if (argc != 3)
  {
    dolfin::cout << "Usage: dolfin-ode method q' where method is one of" << dolfin::endl;
    dolfin::cout << "cg or dg, and q is the order" << dolfin::endl;
    return 1;
  }

  int q = atoi(argv[2]);

  if (strcmp(argv[1], "cg") == 0)
  {
    cGqMethod cGq(q);
    info(cGq, true);
  }
  else if (strcmp(argv[1], "dg") == 0)
  {
    dGqMethod dGq(q);
    info(dGq, true);
  }
  else
  {
    dolfin::cout << "Unknown method." << dolfin::endl;
    return 1;
  }

  return 0;
}
