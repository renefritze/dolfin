// Copyright (C) 2005 Anders Logg.
// Licensed under the GNU GPL Version 2.

// This file is used for testing out new features implemented
// in the library, which means that the contents of this file
// might be (should be) constantly changing. Anything can be
// thrown into this file at any time and may also be removed
// at any time. Use this for temporary test programs that are
// not suitable to be implemented as demos in src/demo.

#include <dolfin.h>
#include "L2Norm.h"

using namespace dolfin;

class MyPreconditioner : public Preconditioner
{
public:

  MyPreconditioner(const Vector& m) : m(m) {}

  void solve(Vector& x, const Vector& b)
  {
    dolfin_info("Calling preconditioner");
    for (unsigned int i = 0; i < x.size(); i++)
    {
      x(i) = b(i) / m(i);
    }
  }

private:

  const Vector& m;

};

void testPreconditioner()
{
  UnitSquare mesh(2, 2);
  MassMatrix M(mesh);
  Vector x;
  Vector b(M.size(0));
  b = 1.0;
  Vector m;
  FEM::lump(M, m);

 
  GMRES solver;
  MyPreconditioner pc(m);
  solver.setPreconditioner(pc);
  solver.solve(M, x, b);
  
  x.disp();
}

void testMatrixOutput()
{
  UnitSquare mesh(2, 2);
  MassMatrix M(mesh);

  File file("matrix.m");
  file << M;
}

class MyFunction : public Function
{
public:

  real operator() (const Point& p) const
  {
    return sin(DOLFIN_PI*p.x);
  }

};

void testFunctional()
{
  UnitSquare mesh(16, 16);
  MyFunction f;
  L2Norm::LinearForm L(f);
  Vector b;
  FEM::assemble(L, b, mesh);
  real norm = sqrt(b.sum());

  cout << "L2 norm of function: " << norm << endl;
}

int main(int argc, char* argv[])
{
  dolfin_info("Testing DOLFIN...");

  testFunctional();

  return 0;
}
