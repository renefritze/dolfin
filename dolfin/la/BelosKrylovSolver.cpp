// Copyright (C) 2015 Chris Richardson
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//

#ifdef HAS_TRILINOS

#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/Timer.h>
#include "GenericMatrix.h"
#include "GenericVector.h"
#include "KrylovSolver.h"
#include "TpetraMatrix.h"
#include "TpetraVector.h"
#include "BelosKrylovSolver.h"
#include "Ifpack2Preconditioner.h"
#include "MueluPreconditioner.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
std::map<std::string, std::string> BelosKrylovSolver::preconditioners()
{
  std::map<std::string, std::string> allowed_precs
    = Ifpack2Preconditioner::preconditioners();

  allowed_precs.insert(std::make_pair<std::string, std::string>("muelu",
                                                                "muelu"));

  return allowed_precs;
}
//-----------------------------------------------------------------------------
std::map<std::string, std::string> BelosKrylovSolver::methods()
{
  Belos::SolverFactory<double, TpetraVector::vector_type, op_type> factory;
  Teuchos::Array<std::string> methods = factory.supportedSolverNames();

  std::map<std::string, std::string> result;
  result.insert(std::make_pair("default", "default method"));

  for (auto &m : methods)
    result.insert(std::make_pair(m, m));

  return result;
}
//-----------------------------------------------------------------------------
BelosKrylovSolver::BelosKrylovSolver(std::string method,
                                     std::string preconditioner)
{
  // Check that the requested method is known
  const std::map<std::string, std::string> _methods = methods();
  if (_methods.find(method) == _methods.end())
  {
    dolfin_error("BelosKrylovSolver.cpp",
                 "create Belos Krylov solver",
                 "Unknown Krylov method \"%s\"", method.c_str());
  }

  // Set parameter values
  parameters = default_parameters();

  if (preconditioner != "none")
  {
    if (preconditioner == "muelu")
      _prec.reset(new MueluPreconditioner());
    else
      _prec.reset(new Ifpack2Preconditioner(preconditioner));
  }

  init(method);
}
//-----------------------------------------------------------------------------
BelosKrylovSolver::~BelosKrylovSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Parameters BelosKrylovSolver::default_parameters()
{
  Parameters p(KrylovSolver::default_parameters());
  p.rename("belos_krylov_solver");

  // Norm type used in convergence test
  // std::set<std::string> allowed_norm_types;
  // allowed_norm_types.insert("preconditioned");
  // allowed_norm_types.insert("true");
  // allowed_norm_types.insert("none");
  // p.add("convergence_norm_type", allowed_norm_types);

  // // Control PETSc performance profiling
  // p.add<bool>("profile");

  // p.add("options_prefix", "default");

  return p;
}
//-----------------------------------------------------------------------------
void
BelosKrylovSolver::set_operator(std::shared_ptr<const GenericLinearOperator> A)
{
  set_operators(A, A);
}
//-----------------------------------------------------------------------------
void BelosKrylovSolver::set_operator(std::shared_ptr<const TpetraMatrix> A)
{
  set_operators(A, A);
}
//-----------------------------------------------------------------------------
void
BelosKrylovSolver::set_operators(std::shared_ptr<const GenericLinearOperator> A,
                                 std::shared_ptr<const GenericLinearOperator> P)
{
  set_operators(as_type<const TpetraMatrix>(A),
                as_type<const TpetraMatrix>(P));
}
//-----------------------------------------------------------------------------
void BelosKrylovSolver::set_operators(std::shared_ptr<const TpetraMatrix> A,
                                      std::shared_ptr<const TpetraMatrix> P)
{
  dolfin_assert(!_solver.is_null());
  dolfin_assert(!_problem.is_null());
  dolfin_assert(A);
  dolfin_assert(!A->mat().is_null());
  dolfin_assert(P);
  dolfin_assert(!P->mat().is_null());

  _matA = A;
  _problem->setOperator(A->mat());

  if (_prec)
  {
    _prec->init(P);
    _prec->set(*this);
  }

}
//-----------------------------------------------------------------------------
const TpetraMatrix& BelosKrylovSolver::get_operator() const
{
  if (!_matA)
  {
    dolfin_error("BelosKrylovSolver.cpp",
                 "access operator for Belos Krylov solver",
                 "Operator has not been set");
  }
  return *_matA;
}
//-----------------------------------------------------------------------------
std::size_t BelosKrylovSolver::solve(GenericVector& x, const GenericVector& b)
{
  return solve(as_type<TpetraVector>(x), as_type<const TpetraVector>(b));
}
//-----------------------------------------------------------------------------
std::size_t BelosKrylovSolver::solve(const GenericLinearOperator& A,
                                     GenericVector& x,
                                     const GenericVector& b)
{
  return solve(as_type<const TpetraMatrix>(A),
               as_type<TpetraVector>(x),
               as_type<const TpetraVector>(b));
}
//-----------------------------------------------------------------------------
std::size_t BelosKrylovSolver::solve(TpetraVector& x, const TpetraVector& b)
{
  Timer timer("Belos Krylov solver");

  dolfin_assert(_matA);

  // Check dimensions
  const std::size_t M = _matA->size(0);
  const std::size_t N = _matA->size(1);

  if (_matA->size(0) != b.size())
  {
    dolfin_error("BelosKrylovSolver.cpp",
                 "unable to solve linear system with Belos Krylov solver",
                 "Non-matching dimensions for linear system (matrix has %ld rows and right-hand side vector has %ld rows)",
                 _matA->size(0), b.size());
  }

  // Write a message
  const bool report = parameters["report"];
  const int mpi_rank = MPI::rank(_matA->mpi_comm());

  if (report && mpi_rank == 0)
  {
    info("Solving linear system of size %ld x %ld (Belos Krylov solver).",
         M, N);
  }

  // Reinitialize solution vector if necessary
  if (x.empty())
  {
    _matA->init_vector(x, 1);
    x.zero();
  }

  // Set any Belos-specific options
  set_options();

  // Solve linear system
  if (mpi_rank == 0)
  {
    log(PROGRESS, "Belos Krylov solver starting to solve %i x %i system.",
        _matA->size(0), _matA->size(1));
  }

  // Clear LHS if unless nonzero_initial_guess is set
  const bool nonzero_guess = parameters["nonzero_initial_guess"];
  if (!nonzero_guess)
    x.zero();

  _problem->setProblem(x.vec(), b.vec());
  _solver->setProblem(_problem);

  Belos::ReturnType result =_solver->solve();
  const std::size_t num_iterations = _solver->getNumIters();

  if (result == Belos::Converged)
  {
    log(PROGRESS, "Belos Krylov Solver converged in %d iterations.",
        num_iterations);
  }
    else
  {
    bool error_non_converge = parameters["error_on_nonconvergence"];
    if (error_non_converge)
      dolfin_error("BelosKrylovSolver.cpp",
                   "solve linear system using Belos Krylov solver",
                   "Solution failed to converge in %d iterations",
                   num_iterations);
    else
    {
      log(PROGRESS, "Belos Krylov Solver did not converge in %d iterations.",
          num_iterations);
    }
  }

  x.update_ghost_values();

  return num_iterations;
}
//-----------------------------------------------------------------------------
std::size_t BelosKrylovSolver::solve(const TpetraMatrix& A, TpetraVector& x,
                                     const TpetraVector& b)
{
  // Set operator
  std::shared_ptr<const TpetraMatrix> Atmp(&A, NoDeleter());
  set_operator(Atmp);

  // Call solve
  return solve(x, b);
}
//-----------------------------------------------------------------------------
std::string BelosKrylovSolver::str(bool verbose) const
{
  std::stringstream s;
  if (verbose)
  {
    s << "Belos Krylov Solver" << std::endl;
    s << "-------------------" << std::endl;
    s << _solver->description();
  }
  else
    s << "<BelosKrylovSolver>";

  return s.str();
}
//-----------------------------------------------------------------------------
void BelosKrylovSolver::init(const std::string& method)
{
  Teuchos::RCP<Teuchos::ParameterList> dummyParams = Teuchos::parameterList();

  std::string method_name = method;
  if (method=="default")
    method_name = "GMRES";

  Belos::SolverFactory<double, TpetraVector::vector_type, op_type> factory;
  _solver = factory.create(method_name, dummyParams);
  _problem = Teuchos::rcp(new problem_type);
}
//-----------------------------------------------------------------------------
void BelosKrylovSolver::set_options()
{
  dolfin_assert(!_solver.is_null());

  Teuchos::RCP<Teuchos::ParameterList> solverParams
    = Teuchos::parameterList(*_solver->getCurrentParameters());

  const int gmres_restart = parameters("gmres")["restart"];
  if (solverParams->isParameter("Num Blocks"))
      solverParams->set("Num Blocks", gmres_restart);

  const int max_iterations = parameters["maximum_iterations"];
  solverParams->set("Maximum Iterations", max_iterations);

  const double rel_tol = parameters["relative_tolerance"];
  solverParams->set("Convergence Tolerance", rel_tol);

  const bool monitor_convergence = parameters["monitor_convergence"];
  if (monitor_convergence)
  {
    solverParams->set("Verbosity",
                      Belos::Warnings
                      | Belos::IterationDetails
                      | Belos::StatusTestDetails
                      | Belos::TimingDetails
                      | Belos::FinalSummary);
    solverParams->set("Output Style", (int)Belos::Brief);
    solverParams->set("Output Frequency", 1);
  }
  _solver->setParameters(solverParams);
}
//-----------------------------------------------------------------------------
void BelosKrylovSolver::check_dimensions(const TpetraMatrix& A,
                                         const GenericVector& x,
                                         const GenericVector& b) const
{
  // Check dimensions of A
  if (A.size(0) == 0 || A.size(1) == 0)
  {
    dolfin_error("BelosKrylovSolver.cpp",
                 "unable to solve linear system with Belos Krylov solver",
                 "Matrix does not have a nonzero number of rows and columns");
  }

  // Check dimensions of A vs b
  if (A.size(0) != b.size())
  {
    dolfin_error("BelosKrylovSolver.cpp",
                 "unable to solve linear system with Belos Krylov solver",
                 "Non-matching dimensions for linear system (matrix has %ld rows and right-hand side vector has %ld rows)",
                 A.size(0), b.size());
  }

  // Check dimensions of A vs x
  if (!x.empty() && x.size() != A.size(1))
  {
    dolfin_error("BelosKrylovSolver.cpp",
                 "unable to solve linear system with Belos Krylov solver",
                 "Non-matching dimensions for linear system (matrix has %ld columns and solution vector has %ld rows)",
                 A.size(1), x.size());
  }

}
//-----------------------------------------------------------------------------

#endif
