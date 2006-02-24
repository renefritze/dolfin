// Copyright (C) 2005 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005
//
// First added:  2005
// Last changed: 2005-12-28
//
// This program illustrates the use of the DOLFIN nonlinear solver for solving 
// problems of the form F(u) = 0. The user must provide functions for the 
// function (Fu) and update of the (approximate) Jacobian.  
//
// This simple program solves a nonlinear variant of Poisson's equation
//
//     - div (1+u^2) grad u(x, y) = f(x, y)
//
// on the unit square with source f given by
//
//     f(x, y) = t * x * sin(y)
//
// and boundary conditions given by
//
//     u(x, y)     = t  for x = 0
//     du/dn(x, y) = 0  otherwise
//
// where t is pseudo time.
//
// This is equivalent to solving: 
// F(u) = (grad(v), (1-u^2)*grad(u)) - f(x,y) = 0
//

#include <dolfin.h>
#include "NonlinearPoisson.h"
  
using namespace dolfin;

// Right-hand side
class MyFunction : public Function
{
  real eval(const Point& p, unsigned int i)
  {
    return time()*p.x*sin(p.y);
  }
};

// Boundary condition
class MyBC : public BoundaryCondition
{
  void eval(BoundaryValue& value, const Point& p, unsigned int i)
  {
    if ( std::abs(p.x - 1.0) < DOLFIN_EPS )
      value = 1.0*time();
  }
};

// User defined nonlinear problem 
class MyNonlinearProblem : public NonlinearProblem
{
  public:

    // Constructor 
    MyNonlinearProblem(BilinearForm& a, LinearForm& L, Mesh& mesh,
      BoundaryCondition& bc) : NonlinearProblem(),
      _a(&a), _L(&L), _mesh(&mesh), _bc(&bc) {}
 

    // User defined assemble of Jacobian and residual vector 
    void form(Matrix&A, Vector& b, const Vector& x)
    {
      BilinearForm& a = *_a;
      LinearForm& L   = *_L;
      BoundaryCondition& bc = *_bc;
      Mesh& mesh = *_mesh;

      dolfin_log(false);
      FEM::assemble(a, L, A, b, mesh);
      FEM::applyBC(A, mesh, a.test(), bc);
      FEM::assembleBCresidual(b, x, mesh, a.test(), bc);
      dolfin_log(true);
    }
    
  private:

    // Pointers to forms, mesh data and boundary conditions
    BilinearForm* _a;
    LinearForm* _L;
    Mesh* _mesh;
    BoundaryCondition* _bc;
};

int main(int argc, char* argv[])
{
  dolfin_init(argc, argv);
 
  // Set up problem
  UnitSquare mesh(16, 16);
  MyFunction f;
  MyBC bc;
  Function u;

  // Create forms and user-defined nonlinear problem
  NonlinearPoisson::BilinearForm a(u);
  NonlinearPoisson::LinearForm L(u, f);
  MyNonlinearProblem nonlinear_problem(a, L, mesh, bc);


  // Create nonlinear solver and set parameters
  NewtonSolver nonlinear_solver;
  nonlinear_solver.set("Newton maximum iterations", 50);
  nonlinear_solver.set("Newton relative tolerance", 1e-10);
  nonlinear_solver.set("Newton absolute tolerance", 1e-10);

  // Solve nonlinear problem in a series of steps
  real dt = 1.0; real t  = 0.0; real T  = 3.0;
  f.sync(t);
  bc.sync(t);

  u.init(mesh, a.trial());
  Vector& x = u.vector();
  while( t < T)
  {
    t += dt;
    nonlinear_solver.solve(nonlinear_problem, x);
  }

  // Save function to file
  File file("nonlinear_poisson.pvd");
  file << u;

  return 0;
}
