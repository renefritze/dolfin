// Copyright (C) 2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#include <petsc/petscksp.h>

#include <dolfin/dolfin_log.h>
#include <dolfin/PETScManager.h>
#include <dolfin/NewVector.h>
#include <dolfin/NewMatrix.h>
#include <dolfin/VirtualMatrix.h>
#include <dolfin/NewGMRES.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
NewGMRES::NewGMRES()
{
  // Initialize PETSc
  PETScManager::init();

}
//-----------------------------------------------------------------------------
NewGMRES::~NewGMRES()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void NewGMRES::solve(const NewMatrix& A, NewVector& x, const NewVector& b)
{
  //Set up solver environment.
  KSP  ksp;
  //PC   pc;

  // The default ILU preconditioner creates an extra matrix.
  // To save memory, use e.g. a Jacobi preconditioner.

  KSPCreate(PETSC_COMM_WORLD, &ksp);

  dolfin::cout << "Setting up PETSc solver environment." << dolfin::endl;

  KSPSetOperators(ksp,A.mat(),A.mat(),DIFFERENT_NONZERO_PATTERN);
  KSPSetInitialGuessNonzero(ksp, PETSC_TRUE);
  KSPSetFromOptions(ksp);

  /*
  // Set tolerances
  real rtol,abstol,dtol;
  int maxits; 
  KSPGetTolerances(ksp,&rtol,&abstol,&dtol,&maxits);
  KSPSetTolerances(ksp,rtol,abstol,dtol,maxits);
  */

  // Solve system
  dolfin::cout << "Solving system using KSPSolve()." << dolfin::endl;

  KSPSetRhs(ksp, b.vec());
  KSPSetSolution(ksp, x.vec());
  KSPSolve(ksp);

  int its = 0;
  KSPGetIterationNumber(ksp, &its);
  dolfin_info("Solution converged in %d iterations.", its);

  //KSPView(ksp, PETSC_VIEWER_STDOUT_WORLD);

  //Destroy solver environment.
  KSPDestroy(ksp);
}
//-----------------------------------------------------------------------------
void NewGMRES::solve(const VirtualMatrix& A, NewVector& x, const NewVector& b)
{
  //Set up solver environment.
  KSP  ksp;
  //PC   pc;

  // The default ILU preconditioner creates an extra matrix.
  // To save memory, use e.g. a Jacobi preconditioner.
  
  //dolfin::cout << "Setting up PETSc solver environment." << dolfin::endl;
  KSPCreate(PETSC_COMM_WORLD, &ksp);

  KSPSetOperators(ksp,A.mat(),A.mat(),DIFFERENT_NONZERO_PATTERN);
  KSPSetInitialGuessNonzero(ksp, PETSC_TRUE);
  KSPSetFromOptions(ksp);

  /*
  // Set tolerances
  real rtol,abstol,dtol;
  int maxits; 
  KSPGetTolerances(ksp,&rtol,&abstol,&dtol,&maxits);
  KSPSetTolerances(ksp,rtol,abstol,dtol,maxits);
  */

  //Solve system.
  //dolfin::cout << "Solving system using KSPSolve()." << dolfin::endl;

  KSPSetRhs(ksp, b.vec());
  KSPSetSolution(ksp, x.vec());
  KSPSolve(ksp);

  int its = 0;
  KSPGetIterationNumber(ksp, &its);
  dolfin_info("Solution converged in %d iterations.", its);

  //KSPView(ksp, PETSC_VIEWER_STDOUT_WORLD);

  //Destroy solver environment.
  KSPDestroy(ksp);
}
//-----------------------------------------------------------------------------
/*
void NewGMRES::changeRtol(real rt)
{
  KSPGetTolerances(ksp,&rtol,&abstol,&dtol,&maxits);
  rtol = rt;
  KSPSetTolerances(ksp,rtol,abstol,dtol,maxits);
}
//-----------------------------------------------------------------------------
void NewGMRES::changeAbstol(real at)
{
  KSPGetTolerances(ksp,&rtol,&abstol,&dtol,&maxits);
  abstol = at;
  KSPSetTolerances(ksp,rtol,abstol,dtol,maxits);
}
//-----------------------------------------------------------------------------
void NewGMRES::changeDtol(real dt)
{
  KSPGetTolerances(ksp,&rtol,&abstol,&dtol,&maxits);
  dtol = dt;
  KSPSetTolerances(ksp,rtol,abstol,dtol,maxits);
}
//-----------------------------------------------------------------------------
void NewGMRES::changeMaxits(int mi)
{
  KSPGetTolerances(ksp,&rtol,&abstol,&dtol,&maxits);
  maxits = mi;
  KSPSetTolerances(ksp,rtol,abstol,dtol,maxits);
}
//-----------------------------------------------------------------------------
*/
