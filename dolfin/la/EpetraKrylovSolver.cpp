// Copyright (C) 2008 Kent-Andre Mardal.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2009.
//
// Last changed: 2010-04-19

#ifdef HAS_TRILINOS

#include <boost/assign/list_of.hpp>

#include <dolfin/common/MPI.h>

#include <Epetra_ConfigDefs.h>
#include <Epetra_CrsMatrix.h>
#include <Epetra_FECrsMatrix.h>
#include <Epetra_FEVector.h>
#include <Epetra_LinearProblem.h>
#include <Epetra_RowMatrix.h>
#include <Epetra_Vector.h>
#include <Epetra_Map.h>
#include <AztecOO.h>
#include <Epetra_LinearProblem.h>

#include <dolfin/log/dolfin_log.h>
#include "GenericMatrix.h"
#include "GenericVector.h"
#include "EpetraKrylovSolver.h"
#include "EpetraMatrix.h"
#include "EpetraVector.h"
#include "TrilinosPreconditioner.h"
#include "KrylovSolver.h"

using namespace dolfin;

// Available solvers
const std::map<std::string, int> EpetraKrylovSolver::methods
  = boost::assign::map_list_of("default",  AZ_gmres)
                              ("cg",       AZ_cg)
                              ("gmres",    AZ_gmres)
                              ("tfqmr",    AZ_tfqmr)
                              ("bicgstab", AZ_bicgstab);
//-----------------------------------------------------------------------------
Parameters EpetraKrylovSolver::default_parameters()
{
  Parameters p(KrylovSolver::default_parameters());
  p.rename("epetra_krylov_solver");
  return p;
}
//-----------------------------------------------------------------------------
EpetraKrylovSolver::EpetraKrylovSolver(std::string method, std::string pc_type)
                    : method(method), solver(new AztecOO),
                      preconditioner(new TrilinosPreconditioner(pc_type)),
                      preconditioner_set(false)
{
  parameters = default_parameters();

  // Check that requsted solver is supported
  if (methods.count(method) == 0)
    error("Requested EpetraKrylovSolver method '%s' in unknown", method.c_str());

  // Set solver type
  solver->SetAztecOption(AZ_solver, methods.find(method)->second);
  solver->SetAztecOption(AZ_kspace, parameters("gmres")["restart"]);
}
//-----------------------------------------------------------------------------
EpetraKrylovSolver::EpetraKrylovSolver(std::string method,
                  TrilinosPreconditioner& preconditioner)
                : method(method), solver(new AztecOO),
                  preconditioner(reference_to_no_delete_pointer(preconditioner)),
                  preconditioner_set(false)
{
  // Set parameter values
  parameters = default_parameters();

  // Check that requsted solver is supported
  if (methods.count(method) == 0)
    error("Requested EpetraKrylovSolver method '%s' in unknown", method.c_str());

  // Set solver type
  solver->SetAztecOption(AZ_solver, methods.find(method)->second);
  solver->SetAztecOption(AZ_kspace, parameters("gmres")["restart"]);
}
//-----------------------------------------------------------------------------
EpetraKrylovSolver::~EpetraKrylovSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void EpetraKrylovSolver::set_operator(const GenericMatrix& A)
{
  set_operators(A, A);
}
//-----------------------------------------------------------------------------
void EpetraKrylovSolver::set_operators(const GenericMatrix& A,
                                       const GenericMatrix& P)
{
  this->A = reference_to_no_delete_pointer(A.down_cast<EpetraMatrix>());
  this->P = reference_to_no_delete_pointer(P.down_cast<EpetraMatrix>());
  assert(this->A);
  assert(this->P);
}
//-----------------------------------------------------------------------------
dolfin::uint EpetraKrylovSolver::solve(GenericVector& x,
                                       const GenericVector& b)
{
  return solve(x.down_cast<EpetraVector>(), b.down_cast<EpetraVector>());
}
//-----------------------------------------------------------------------------
dolfin::uint EpetraKrylovSolver::solve(EpetraVector& x, const EpetraVector& b)
{
  assert(solver);
  assert(A);
  assert(P);

  // Check dimensions
  const uint M = A->size(0);
  const uint N = A->size(1);
  if (N != b.size())
    error("Non-matching dimensions for linear system.");

  // Write a message
  if (parameters["report"])
    info(PROGRESS, "Solving linear system of size %d x %d (Epetra Krylov solver).", M, N);

  // Reinitialize solution vector if necessary
  if (x.size() != M)
  {
    x.resize(M);
    x.zero();
  }

  // FIXME: permit initial guess

  // Create linear problem
  Epetra_LinearProblem linear_problem(A->mat().get(), x.vec().get(),
                                      b.vec().get());
  // Set-up linear solver
  solver->SetProblem(linear_problem);

  // Set output level
  if(parameters["monitor_convergence"])
    solver->SetAztecOption(AZ_output, 1);
  else
    solver->SetAztecOption(AZ_output, AZ_none);

  // Configure preconditioner
  //if (preconditioner && !preconditioner_set)
  //{
    assert(P);
    preconditioner->set(*this, *P);
    //preconditioner_set = true;
  //}

  // Start solve
  solver->Iterate(parameters["maximum_iterations"], parameters["relative_tolerance"]);

  // Check solve status
  const double* status = solver->GetAztecStatus();
  if ((int) status[AZ_why] != AZ_normal)
  {
    const bool error_on_nonconvergence = parameters["error_on_nonconvergence"];
    if (error_on_nonconvergence)
    {
      error("Epetra (Aztec00) Krylov solver failed to converge (error code %i).",
            status[AZ_why]);
    }
    else
    {
      warning("Epetra (Aztec00) Krylov solver failed to converge (error code %i).",
              status[AZ_why]);
    }
  }
  else
  {
    info("Epetra (AztecOO) Krylov solver (%s, %s) converged in %d iterations.",
          method.c_str(), preconditioner->name().c_str(), solver->NumIters());
  }

  return solver->NumIters();
}
//-----------------------------------------------------------------------------
dolfin::uint EpetraKrylovSolver::solve(const GenericMatrix& A, GenericVector& x,
                                       const GenericVector& b)
{
  return solve(A.down_cast<EpetraMatrix>(), x.down_cast<EpetraVector>(),
               b.down_cast<EpetraVector>());
}
//-----------------------------------------------------------------------------
dolfin::uint EpetraKrylovSolver::solve(const EpetraMatrix& A, EpetraVector& x,
                                       const EpetraVector& b)
{
  set_operator(A);
  return solve(x, b);
}
//-----------------------------------------------------------------------------
std::string EpetraKrylovSolver::str(bool verbose) const
{
  dolfin_not_implemented();
  return std::string();
}
//-----------------------------------------------------------------------------
boost::shared_ptr<AztecOO> EpetraKrylovSolver::aztecoo() const
{
  return solver;
}
//-----------------------------------------------------------------------------
#endif
