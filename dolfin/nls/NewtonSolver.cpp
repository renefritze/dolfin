// Copyright (C) 2005-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2005-2009.
// Modified by Martin Alnes, 2008.
// Modified by Johan Hake, 2010.
//
// First added:  2005-10-23
// Last changed: 2011-02-28

#include <iostream>
#include <dolfin/common/constants.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/la/GenericLinearSolver.h>
#include <dolfin/la/LinearSolver.h>
#include <dolfin/la/Matrix.h>
#include <dolfin/la/Vector.h>
#include <dolfin/log/log.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/MPI.h>
#include "NonlinearProblem.h"
#include "NewtonSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Parameters NewtonSolver::default_parameters()
{
  Parameters p("newton_solver");

  p.add("maximum_iterations",      10);
  p.add("relative_tolerance",      1e-9);
  p.add("absolute_tolerance",      1e-10);
  p.add("convergence_criterion",   "residual");
  p.add("method",                  "full");
  p.add("relaxation_parameter",    1.0);
  p.add("report",                  false);
  p.add("error_on_nonconvergence", true);

  //p.add("reuse_preconditioner", false);

  return p;
}
//-----------------------------------------------------------------------------
NewtonSolver::NewtonSolver(std::string solver_type, std::string pc_type)
             : Variable("Newton solver", "unamed"),
               newton_iteration(0), _residual(0.0), residual0(0.0),
               solver(new LinearSolver(solver_type, pc_type)),
               A(new Matrix), dx(new Vector), b(new Vector)
{
  // Set default parameters
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
NewtonSolver::NewtonSolver(GenericLinearSolver& solver,
                           LinearAlgebraFactory& factory)
                         : Variable("Newton solver", "unamed"),
                           newton_iteration(0), _residual(0.0), residual0(0.0),
                           solver(reference_to_no_delete_pointer(solver)),
                           A(factory.create_matrix()),
                           dx(factory.create_vector()),
                           b(factory.create_vector())
{
  // Set default parameters
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
NewtonSolver::~NewtonSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, bool> NewtonSolver::solve(NonlinearProblem& nonlinear_problem,
                                                  GenericVector& x)
{
  assert(A);
  assert(b);
  assert(dx);
  assert(solver);

  const uint maxiter = parameters["maximum_iterations"];

  begin("Starting Newton solve.");

  uint krylov_iterations = 0;
  newton_iteration = 0;
  bool newton_converged = false;

  // Compute F(u)
  nonlinear_problem.form(*A, *b, x);
  nonlinear_problem.F(*b, x);

  solver->set_operator(*A);

  // Start iterations
  while (!newton_converged && newton_iteration < maxiter)
  {
    // Compute Jacobian
    nonlinear_problem.J(*A, x);

    // FIXME: This reset is a hack to handle a deficiency in the Trilinos wrappers
    solver->set_operator(*A);

    // Perform linear solve and update total number of Krylov iterations
    if (dx->size() > 0)
      dx->zero();
    krylov_iterations += solver->solve(*dx, *b);

    // Compute initial residual
    if (newton_iteration == 0)
      newton_converged = converged(*b, *dx, nonlinear_problem);

    // Update solution
    const double relaxation = parameters["relaxation_parameter"];
    if (std::abs(1.0 - relaxation) < DOLFIN_EPS)
      x -= (*dx);
    else
      x.axpy(-relaxation, *dx);

    // Update number of iterations
    ++newton_iteration;

    //FIXME: this step is not needed if residual is based on dx and this has converged.
    // Compute F
    nonlinear_problem.form(*A, *b, x);
    nonlinear_problem.F(*b, x);

    // Test for convergence
    newton_converged = converged(*b, *dx, nonlinear_problem);
  }

  if (newton_converged)
  {
    if (dolfin::MPI::process_number() == 0)
     info(PROGRESS, "Newton solver finished in %d iterations and %d linear solver iterations.",
            newton_iteration, krylov_iterations);
  }
  else
  {
    const bool error_on_nonconvergence = parameters["error_on_nonconvergence"];
    if (error_on_nonconvergence)
      error("Newton solver did not converge.");
    else
      warning("Newton solver did not converge.");
  }

  end();

  return std::make_pair(newton_iteration, newton_converged);
}
//-----------------------------------------------------------------------------
dolfin::uint NewtonSolver::iteration() const
{
  return newton_iteration;
}
//-----------------------------------------------------------------------------
double NewtonSolver::residual() const
{
  return _residual;
}
//-----------------------------------------------------------------------------
double NewtonSolver::relative_residual() const
{
  return _residual/residual0;
}
//-----------------------------------------------------------------------------
GenericLinearSolver& NewtonSolver::linear_solver() const
{
  assert(solver);
  return *solver;
}
//-----------------------------------------------------------------------------
bool NewtonSolver::converged(const GenericVector& b, const GenericVector& dx,
                             const NonlinearProblem& nonlinear_problem)
{
  const std::string convergence_criterion = parameters["convergence_criterion"];
  const double rtol = parameters["relative_tolerance"];
  const double atol = parameters["absolute_tolerance"];
  const bool report = parameters["report"];

  // Compute resdiual
  if (convergence_criterion == "residual")
    _residual = b.norm("l2");
  else if (convergence_criterion == "incremental")
    _residual = dx.norm("l2");
  else
    error("Unknown Newton convergence criterion");

  // If this is the first iteration step, set initial residual
  if (newton_iteration == 0)
    residual0 = _residual;

  // Relative residual
  const double relative_residual = _residual / residual0;

  // Output iteration number and residual
  if (report && newton_iteration > 0 && dolfin::MPI::process_number() == 0)
  {
    info("Newton iteration %d: r (abs) = %.3e (tol = %.3e) r (rel) = %.3e (tol = %.3e)",
         newton_iteration, _residual, atol, relative_residual, rtol);
  }

  // Return true of convergence criterion is met
  if (relative_residual < rtol || _residual < atol)
    return true;

  // Otherwise return false
  return false;
}
//-----------------------------------------------------------------------------
