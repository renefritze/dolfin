// Copyright (C) 2002 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin.h>
#include <dolfin/Partition.h>
#include <dolfin/TimeSlab.h>

using namespace dolfin;

class Minimal : public ODE {
public:

  Minimal() : ODE(3)
  {
    // Parameters
    lambda1 = 1.0;
    lambda2 = 1.0;

    // Final time
    T = 4.0;

    // Initial value
    //u0(0) = 1.0;
    //u0(1) = 1.0;

    u0(0) = 0.0;
    u0(1) = 1.0;
    u0(2) = 1.0;

    // Sparsity (not really necessary here)
    sparse();
  }

  real f(const Vector& u, real t, int i)
  {
    //dolfin_debug("foo");
    //dolfin::cout << "u: " << dolfin::endl;
    //u.show();

    if (i == 0)
    {
      //return -lambda1 * u(0);
      return u(1);
    }
    else if (i == 1)
    {
      //return -lambda2 * u(1);
      return -u(0);
    }
    else if (i == 2)
    {
      return -lambda2 * u(2);
      //return -u(0);
    }
    return 0;
  }

private:

  real lambda1, lambda2;

};

int main()
{
  dolfin_set("output", "plain text");
  dolfin_set("debug time slab", 1);

  Minimal minimal;

  minimal.solve();
  
  return 0;
}
