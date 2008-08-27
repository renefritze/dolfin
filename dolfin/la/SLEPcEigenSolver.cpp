// Copyright (C) 2005-2006 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug 2008
//
// First added:  2005-08-31
// Last changed: 2008-08-13

#ifdef HAS_SLEPC

#include <dolfin/log/dolfin_log.h>
#include "PETScMatrix.h"
#include "PETScVector.h"
#include "SLEPcEigenSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
SLEPcEigenSolver::SLEPcEigenSolver()
{
  // Set up solver environment
  EPSCreate(PETSC_COMM_SELF, &eps);
}
//-----------------------------------------------------------------------------
SLEPcEigenSolver::~SLEPcEigenSolver()
{
  // Destroy solver environment
  if (eps) EPSDestroy(eps);
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::solve(const PETScMatrix& A)
{
  solve(&A, 0, A.size(0));
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::solve(const PETScMatrix& A, uint n)
{
  solve(&A, 0, n);
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::solve(const PETScMatrix& A, const PETScMatrix& B)
{
  solve(&A, &B, A.size(0));
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::solve(const PETScMatrix& A, const PETScMatrix& B, uint n)
{
  solve(A, &B, n);
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::getEigenvalue(real& xr, real& xc)
{
  getEigenvalue(xr, xc, 0);
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::getEigenpair(real& xr, real& xc, PETScVector& r,  PETScVector& c)
{
  getEigenpair(xr, xc, r, c, 0);
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::getEigenvalue(real& xr, real& xc, const int i)
{
  // Get number of computed values
  int num_computed_eigenvalues;
  EPSGetConverged(eps, &num_computed_eigenvalues);

  if( i < num_computed_eigenvalues )
    EPSGetValue(eps, i, &xr, &xc);
  else
    error("Requested eigenvalue has not been computed");
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::getEigenpair(real& xr, real& xc, PETScVector& r, PETScVector& c, const int i)
{
  // Get number of computed eigenvectors/values
  int num_computed_eigenvalues;
  EPSGetConverged(eps, &num_computed_eigenvalues);

  if( i < num_computed_eigenvalues )
    EPSGetEigenpair(eps, i, &xr, &xc, r.vec(), c.vec());
  else
    error("Requested eigenvalue/vector has not been computed");
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::solve(const PETScMatrix* A,
                             const PETScMatrix* B,
                             uint n)
{
  // Associate matrix (matrices) with eigenvalue solver
  dolfin_assert(A);
  dolfin_assert(A.size(0) == A.size(1));
  if (B)
  {
    dolfin_assert(B->size(0) == B->size(1) && B->size(0) == A->size(0));
    EPSSetOperators(eps, A->mat(), B->mat());
  }
  else
  {
    EPSSetOperators(eps, A->mat(), PETSC_NULL);
  }
  
  // Set number of eigenpairs to compute
  dolfin_assert(n <= A.size(0));
  EPSSetDimensions(eps, n, PETSC_DECIDE);

  // Set algorithm type (Hermitian matrix)
  //EPSSetProblemType(eps, EPS_NHEP);
  
  // Set options from database
  EPSSetFromOptions(eps);
  
  // Solve
  EPSSolve(eps);
  
  // Check for convergence
  EPSConvergedReason reason;
  EPSGetConvergedReason(eps, &reason);
  if (reason < 0)
  {
    warning("Eigenvalue solver did not converge");
    return;
  }

  // Report solver status  
  int num_iterations = 0;
  EPSGetIterationNumber(eps, &num_iterations);
  EPSType eps_type = 0;
  EPSGetType(eps, &eps_type);
  message("Eigenvalue solver (%s) converged in %d iterations.",
          eps_type, num_iterations);
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::readParameters()
{
  if (has("eigenvalue spectrum"))
    setSpectrum(get("eigenvalue spectrum"));
  if (has("eigenvalue solver"))
    setSolver(get("eigenvalue solver"));
  if (has("eigenvalue tolerance") && has("eigenvalue iterations"))
    setTolerance(get("eigenvalue tolerance"), get("eigenvalue spectrum"));
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::setSpectrum(EigenspectrumType type)
{
  // Do nothing if default type is specified
  if (type == "default")
    return;

  // Choose spectrum
  if (type == "largest magnitude")
    EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE);
  else if (type == "smallest magnitude")
    EPSSetWhichEigenpairs(eps, EPS_SMALLEST_MAGNITUDE);
  else if (type == "largest real")
    EPSSetWhichEigenpairs(eps, EPS_LARGEST_REAL);
  else if (type == "smallest real")
    EPSSetWhichEigenpairs(eps, EPS_SMALLEST_REAL);
  else if (type == "largest imaginary")
    EPSSetWhichEigenpairs(eps, EPS_LARGEST_IMAGINARY);
  else if (type == "smallest imaginary")
    EPSSetWhichEigenpairs(eps, EPS_SMALLEST_IMAGINARY);
  else
  {
    warning("Requested spectrum unknown. Using largest magnitude.");
    EPSSetWhichEigenpairs(eps, EPS_LARGEST_MAGNITUDE);
  }

  // FIXME: Need to add some test here as most algorithms only compute
  // FIXME: largest eigenvalues. Asking for smallest leads to a PETSc error.
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::setSolver(EigenvalueSolverType type)
{
  // Do nothing if default type is specified
  if (type == "default")
    return;

  // Choose solver
  if (type == "power")
    EPSSetType(eps, EPSPOWER);
  else if (type == "subspace")
    EPSSetType(eps, EPSSUBSPACE);
  else if (type == "arnoldi")
    EPSSetType(eps, EPSARNOLDI);
  else if (type == "lanczos")
    EPSSetType(eps, EPSLANCZOS);
  else if (type == "krylov-schur")
    EPSSetType(eps, EPSKRYLOVSCHUR);
  else
  {
    warning("Requested Krylov method unknown. Using Krylov-Schur.");
    EPSSetType(eps, EPSKRYLOVSCHUR);
  }
}
//-----------------------------------------------------------------------------
void SLEPcEigenSolver::setTolerance(real tolerance, uint maxiter)
{
  dolfin_assert(tolerance > 0.0);
  EPSSetTolerances(eps, tolerance, static_cast<int>(maxiter));
}
//-----------------------------------------------------------------------------

#endif
