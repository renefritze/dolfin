// Copyright (C) 2008 Garth N. Wells and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-08-24
// Last changed:

#ifndef __ENUMS_LA_H
#define __ENUMS_LA_H

namespace dolfin
{

  // List of predefined vector  norm types
  enum VectorNormType {l1, l2, linf};

  /// List of predefined solvers
  enum SolverType
  {
    lu,            // LU factorization
    cg,            // Krylov conjugate gradient method
    gmres,         // Krylov GMRES method
    bicgstab,      // Krylov stabilised biconjugate gradient squared method 
    default_solver // Default Krylov solver
  };

  /// List of predefined preconditioners
  enum PreconditionerType
  {
    none,      // No preconditioning
    jacobi,    // Jacobi
    sor,       // SOR (successive over relaxation)
    ilu,       // Incomplete LU factorization
    icc,       // Incomplete Cholesky factorization
    amg,       // Algebraic multigrid (through Hypre when available)
    default_pc // Default choice of preconditioner
  };

  /// Two different normalizations are available:
  /// 
  /// normalize_l2norm:  Normalizes a vector x according to x --> x / ||x||
  ///   where ||x|| is the l2 norm of x
  ///
  /// normalize_average: Normalizes a vector x according to x --> x - avg(x)
  ///   where avg(x) is the average of x. This is useful to satisfy the 
  ///   compatibility condition for the Neumann problem.
  enum NormalizationType {normalize_average, normalize_l2norm};

}

#endif
