// Copyright (C) 2003-2005 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// First added:  2003-06-04
// Last changed: 2005

#include <stdlib.h>
#include <dolfin.h>

using namespace dolfin;

int main(int argc, char** argv)
{
  dolfin_set("output", "plain text");

  if ( argc != 3 ) {
    dolfin::cout << "Usage: dolfin-quadrature rule n' where rule is one of" << dolfin::endl;
    dolfin::cout << "gauss, radau, lobatto, and n is the number of points" << dolfin::endl;
    return 1;
  }
  
  int n = atoi(argv[2]);

  if ( strcasecmp(argv[1], "gauss") == 0 ) {
    
    GaussQuadrature q(n);
    q.show();

  }
  else if ( strcasecmp(argv[1], "radau") == 0 ) {
    
    RadauQuadrature q(n);
    q.show();
    
  } 
  else if ( strcasecmp(argv[1], "lobatto") == 0 ) {

     LobattoQuadrature q(n);
     q.show();
     
  }
  else {
    dolfin::cout << "Unknown quadrature rule." << dolfin::endl;
    return 1;
  }
  
  return 0;
}
