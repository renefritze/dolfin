// Copyright (C) 2005-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005
// Last changed: 2009-10-08

#include <dolfin.h>
#include "Poisson2D_1.h"
#include "Poisson2D_2.h"
#include "Poisson2D_3.h"
#include "Poisson2D_4.h"
#include "Poisson2D_5.h"
#include "Poisson3D_1.h"
#include "Poisson3D_2.h"
#include "Poisson3D_3.h"
#include "Poisson3D_4.h"
#include "Poisson3D_5.h"

using namespace dolfin;

// Boundary condition
class DirichletBoundary : public SubDomain
{
  bool inside(const double* x, bool on_boundary) const
  {
    return on_boundary;
  }
};

// Right-hand side, 2D
class Source2D : public Expression
{
public:

  Source2D() : Expression(2) {}

  void eval(double* values, const double* x) const
  {
    values[0] = 2.0*DOLFIN_PI*DOLFIN_PI*sin(DOLFIN_PI*x[0])*sin(DOLFIN_PI*x[1]);
  }

};

// Right-hand side, 3D
class Source3D : public Expression
{
public:

  Source3D() : Expression(3) {}

  void eval(double* values, const double* x) const
  {
    values[0] = 3.0*DOLFIN_PI*DOLFIN_PI*sin(DOLFIN_PI*x[0])*sin(DOLFIN_PI*x[1])*sin(DOLFIN_PI*x[2]);
  }

};

// Solve equation and compute error, 2D
double solve2D(int q, int n)
{
  printf("BENCH --------------------------------------------------\n");
  printf("BENCH Solving Poisson's equation in 2D for q = %d, n = %d.\n", q, n);
  printf("BENCH --------------------------------------------------\n");

  // Set up problem
  UnitSquare mesh(n, n);
  Source2D f;
  Constant zero(2, 0.0);

  // Choose forms
  Form* a = 0;
  Form* L = 0;
  FunctionSpace* V = 0;
  switch (q)
  {
  case 1:
    V = new Poisson2D_1::FunctionSpace(mesh);
    a = new Poisson2D_1::BilinearForm(*V, *V);
    L = new Poisson2D_1::LinearForm(*V, f);
    break;
  case 2:
    V = new Poisson2D_2::FunctionSpace(mesh);
    a = new Poisson2D_2::BilinearForm(*V, *V);
    L = new Poisson2D_2::LinearForm(*V, f);
    break;
  case 3:
    V = new Poisson2D_3::FunctionSpace(mesh);
    a = new Poisson2D_3::BilinearForm(*V, *V);
    L = new Poisson2D_3::LinearForm(*V, f);
    break;
  case 4:
    V = new Poisson2D_4::FunctionSpace(mesh);
    a = new Poisson2D_4::BilinearForm(*V, *V);
    L = new Poisson2D_4::LinearForm(*V, f);
    break;
  case 5:
    V = new Poisson2D_5::FunctionSpace(mesh);
    a = new Poisson2D_5::BilinearForm(*V, *V);
    L = new Poisson2D_5::LinearForm(*V, f);
    break;
  default:
    error("Forms not compiled for q = %d.", q);
  }

  // Set up boundary conditions
  DirichletBoundary boundary;
  DirichletBC bc(*V, zero, boundary);

  // Discretize equation
  Matrix A;
  Vector x, b;
  assemble(A, *a);
  assemble(b, *L);
  bc.apply(A, b);

  // Solve the linear system
  KrylovSolver solver("gmres");
  solver.parameters["relative_tolerance"] = 1e-14;
  solver.solve(A, x, b);

  // Compute maximum norm of error
  double emax = 0.0;
  double* U = new double[x.size()];
  x.get_local(U);
  for (VertexIterator v(mesh); !v.end(); ++v)
  {
    const Point p = v->point();
    const double u = sin(DOLFIN_PI*p.x())*sin(DOLFIN_PI*p.y());
    const double e = std::abs(U[v->index()] - u);
    emax = std::max(emax, e);
  }
  delete [] U;

  delete a;
  delete L;
  delete V;

  return emax;
}

// Solve equation and compute error, 3D
double solve3D(int q, int n)
{
  printf("BENCH --------------------------------------------------\n");
  printf("BENCH Solving Poisson's equation in 3D for q = %d, n = %d.\n", q, n);
  printf("BENCH --------------------------------------------------\n");

  // Set up problem
  UnitCube mesh(n, n, n);
  Source3D f;
  Constant zero(3, 0.0);

  // Choose forms
  Form* a = 0;
  Form* L = 0;
  FunctionSpace* V = 0;
  switch (q)
  {
  case 1:
    V = new Poisson3D_1::FunctionSpace(mesh);
    a = new Poisson3D_1::BilinearForm(*V, *V);
    L = new Poisson3D_1::LinearForm(*V, f);
    break;
  case 2:
    V = new Poisson3D_2::FunctionSpace(mesh);
    a = new Poisson3D_2::BilinearForm(*V, *V);
    L = new Poisson3D_2::LinearForm(*V, f);
    break;
  case 3:
    V = new Poisson3D_3::FunctionSpace(mesh);
    a = new Poisson3D_3::BilinearForm(*V, *V);
    L = new Poisson3D_3::LinearForm(*V, f);
    break;
  case 4:
    V = new Poisson3D_4::FunctionSpace(mesh);
    a = new Poisson3D_4::BilinearForm(*V, *V);
    L = new Poisson3D_4::LinearForm(*V, f);
    break;
  case 5:
    V = new Poisson3D_5::FunctionSpace(mesh);
    a = new Poisson3D_5::BilinearForm(*V, *V);
    L = new Poisson3D_5::LinearForm(*V, f);
    break;
  default:
    error("Forms not compiled for q = %d.", q);
  }

  // Set up boundary conditions
  DirichletBoundary boundary;
  DirichletBC bc(*V, zero, boundary);

  // Discretize equation
  Matrix A;
  Vector x, b;
  assemble(A, *a);
  assemble(b, *L);
  bc.apply(A, b);

  // Solve the linear system
  KrylovSolver solver("gmres");
  solver.parameters["relative_tolerance"] = -1e-14;
  solver.solve(A, x, b);

  // Compute maximum norm of error
  double emax = 0.0;
  double* U = new double[x.size()];
  x.get_local(U);
  for (VertexIterator v(mesh); !v.end(); ++v)
  {
    const Point p = v->point();
    const double u = sin(DOLFIN_PI*p.x())*sin(DOLFIN_PI*p.y())*sin(DOLFIN_PI*p.z());
    const double e = std::abs(U[v->index()] - u);
    emax = std::max(emax, e);
  }
  delete [] U;

  delete a;
  delete L;
  delete V;

  return emax;
}

int main()
{
  logging(false);

  const int qmax = 5;
  const int num_meshes = 3;
  double e2D[qmax][num_meshes];
  double e3D[qmax][num_meshes];

  // Compute errors in 2D
  for (int q = 1; q <= qmax; q++)
  {
    int n = 2;
    for (int i = 0; i < num_meshes; i++)
    {
      e2D[q - 1][i] = solve2D(q, n);
      n *= 2;
    }
  }

  // Compute errors in 3D
  for (int q = 1; q <= qmax; q++)
  {
    int n = 2;
    for (int i = 0; i < num_meshes; i++)
    {
      e3D[q - 1][i] = solve3D(q, n);
      n *= 2;
    }
  }

  // Write errors in 2D
  printf("\nMaximum norm error in 2D:\n");
  printf("-------------------------\n");
  for (int q = 1; q <= qmax; q++)
  {
    printf("q = %d:", q);
    for (int i = 0; i < num_meshes; i++)
      printf(" %.3e", e2D[q - 1][i]);
    printf("\n");
  }

  // Write errors in 3D
  printf("\nMaximum norm error in 3D:\n");
  printf("-------------------------\n");
  for (int q = 1; q <= qmax; q++)
  {
    printf("q = %d:", q);
    for (int i = 0; i < num_meshes; i++)
      printf(" %.3e", e3D[q - 1][i]);
    printf("\n");
  }
};
