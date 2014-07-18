// Copyright (C) 2014 Tianyi Li
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
// First added:  2014-06-22
// Last changed: 2014-06-22

#ifdef ENABLE_PETSC_TAO

#include <map>
#include <string>
#include <utility>
#include <petscsys.h>
#include <boost/assign/list_of.hpp>

#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/timing.h>
#include <dolfin/common/Timer.h>
#include <dolfin/la/KrylovSolver.h>
#include <dolfin/la/PETScKrylovSolver.h>
#include <dolfin/la/PETScMatrix.h>
#include <dolfin/la/PETScPreconditioner.h>
#include <dolfin/la/PETScVector.h>
#include "OptimisationProblem.h"
#include "PETScTAOSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
const std::map<std::string, std::pair<std::string, const TaoType> >
  PETScTAOSolver::_methods
   = boost::assign::map_list_of
      ("default", std::make_pair("Default TAO method (tron)",          TAOTRON))
      ("tron",    std::make_pair("Newton trust region method",             TAOTRON))
      ("bqpip",   std::make_pair("Interior point newton algorithm",        TAOBQPIP))
      ("gpcg",    std::make_pair("Gradient Projection Conjugate Gradient", TAOGPCG))
      ("blmvm",   std::make_pair("Limited memory variable metric method",  TAOBLMVM));
//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> > PETScTAOSolver::methods()
{
  std::vector<std::pair<std::string, std::string> > available_methods;
  std::map<std::string, std::pair<std::string, const TaoType> >::const_iterator it;
  for (it = _methods.begin(); it != _methods.end(); ++it)
    available_methods.push_back(std::make_pair(it->first, it->second.first));
  return available_methods;
}
//-----------------------------------------------------------------------------
Parameters PETScTAOSolver::default_parameters()
{
  Parameters p("tao_solver");

  p.add("monitor_convergence"    , false);
  p.add("report"                 , false);
  p.add("function_absolute_tol"  , 1.0e-10);
  p.add("function_relative_tol"  , 1.0e-10);
  p.add("gradient_absolute_tol"  , 1.0e-08);
  p.add("gradient_relative_tol"  , 1.0e-08);
  p.add("gradient_t_tol"         , 0.0);
  p.add("error_on_nonconvergence", true);
  p.add("maximum_iterations"     , 100);
  p.add("options_prefix"         , "default");

  p.add(KrylovSolver::default_parameters());

  return p;
}
//-----------------------------------------------------------------------------
PETScTAOSolver::PETScTAOSolver(const std::string tao_type,
                               const std::string ksp_type,
                               const std::string pc_type) : _tao(NULL)
{
  // Set parameter values
  parameters = default_parameters();

  // Initialize and set the TAO/KSP solvers
  init(tao_type, ksp_type, pc_type);
}
//-----------------------------------------------------------------------------
PETScTAOSolver::~PETScTAOSolver()
{
  if (_tao)
    TaoDestroy(&_tao);
}
//-----------------------------------------------------------------------------
void PETScTAOSolver::init(const std::string tao_type,
                          const std::string ksp_type,
                          const std::string pc_type)
{
  if (_tao)
    TaoDestroy(&_tao);

  // Create TAO object
  TaoCreate(PETSC_COMM_WORLD, &_tao);

  // Set the TAO solver
  set_solver(tao_type);

  // Set the PETSC KSP used by TAO
  set_ksp_pc(ksp_type, pc_type);
}
//-----------------------------------------------------------------------------
void PETScTAOSolver::set_solver(const std::string tao_type)
{
  dolfin_assert(_tao);

  // Check that the requested method is known
  if (_methods.count(tao_type) == 0)
  {
    dolfin_error("PETScTAOSolver.cpp",
                 "set PETSc TAO solver",
                 "Unknown TAO method \"%s\"", tao_type.c_str());
  }

  // Set solver type
  std::map<std::string, std::pair<std::string, const TaoType> >::const_iterator it;
  it = _methods.find(tao_type);
  dolfin_assert(it != _methods.end());
  TaoSetType(_tao, it->second.second);
}
//-----------------------------------------------------------------------------
void PETScTAOSolver::set_ksp_pc(const std::string ksp_type,
                                const std::string pc_type)
{
  // Set ksp type
  if (ksp_type != "default")
  {
    dolfin_assert(_tao);
    KSP ksp;
    TaoGetKSP(_tao, &ksp);

    if (ksp)
    {
      PC pc;
      KSPGetPC(ksp, &pc);
      MPI_Comm comm = MPI_COMM_NULL;
      PetscObjectGetComm((PetscObject)_tao, &comm);

      std::map<std::string, const KSPType>::const_iterator ksp_pair
        = PETScKrylovSolver::_methods.find(ksp_type);
      dolfin_assert(ksp_pair != PETScKrylovSolver::_methods.end());
      KSPSetType(ksp, ksp_pair->second);

      if (pc_type != "default")
      {

        std::map<std::string, const PCType>::const_iterator pc_pair
          = PETScPreconditioner::_methods.find(pc_type);
        dolfin_assert(pc_pair != PETScPreconditioner::_methods.end());
        PCSetType(pc, pc_pair->second);
      }
    }

    else
    {
      log(WARNING, "The selected TAO solver does not allow to set a specific "\
      "Krylov solver. Option %s is ignored", ksp_type.c_str());
    }
  }
}
//-----------------------------------------------------------------------------
std::size_t PETScTAOSolver::solve(OptimisationProblem& optimisation_problem,
                                  GenericVector& x,
                                  const GenericVector& lb,
                                  const GenericVector& ub)
{
  return solve(optimisation_problem,
               x.down_cast<PETScVector>(),
               lb.down_cast<PETScVector>(),
               ub.down_cast<PETScVector>());
}
//-----------------------------------------------------------------------------
std::size_t PETScTAOSolver::solve(OptimisationProblem& optimisation_problem,
                                  PETScVector& x,
                                  const PETScVector& lb,
                                  const PETScVector& ub)
{
  // Form the optimisation problem object
  _tao_ctx.optimisation_problem = &optimisation_problem;
  // _tao_ctx.x = &x;

  // Initialize the Hessian matrix
  PETScMatrix H;
  PETScVector g;
  optimisation_problem.form(H, g, x);
  optimisation_problem.H(H, x);
  dolfin_assert(H.mat());

  // Set initial vector
  TaoSetInitialVector(_tao, x.vec());

  // Set the bound on the variables
  TaoSetVariableBounds(_tao, lb.vec(), ub.vec());

  // Set parameters
  set_options();
  set_ksp_options();

  // Set the user function, gradient and Hessian evaluation routines and
  // data structures
  TaoSetObjectiveAndGradientRoutine(_tao, FormFunctionGradient, &_tao_ctx);
  TaoSetHessianRoutine(_tao, H.mat(), H.mat(), FormHessian, &_tao_ctx);

  // Clear previous monitors
  TaoCancelMonitors(_tao);

  // Check for any tao command line options
  std::string prefix = std::string(parameters["options_prefix"]);
  if (prefix != "default")
  {
    // Make sure that the prefix has a '_' at the end if the user
    // didn't provide it
    char lastchar = *prefix.rbegin();
    if (lastchar != '_')
      prefix += "_";

    TaoSetOptionsPrefix(_tao, prefix.c_str());
  }
  TaoSetFromOptions(_tao);

  // Solve the bound constrained problem
  Timer timer("TAO solver");
  const char* tao_type;
  TaoGetType(_tao, &tao_type);
  log(PROGRESS, "TAO solver %s starting to solve a %i x %i system", tao_type,
      H.size(0), H.size(1));

  // Solve
  TaoSolve(_tao);

  // Update ghost values
  x.update_ghost_values();

  // Print the report on convergences and methods used
  if (parameters["report"])
    TaoView(_tao, PETSC_VIEWER_STDOUT_WORLD);

  // Check for convergence
  TaoConvergedReason reason;
  TaoGetConvergedReason(_tao, &reason);

  // Get the number of iterations
  PetscInt num_iterations = 0;
  TaoGetMaximumIterations(_tao, &num_iterations);

  // Report number of iterations
  if (reason >= 0)
    log(PROGRESS, "TAO solver converged\n");
  else
  {
    bool error_on_nonconvergence = parameters["error_on_nonconvergence"];
    if (error_on_nonconvergence)
    {
      TaoView(_tao, PETSC_VIEWER_STDOUT_WORLD);
      dolfin_error("PETScTAOSolver.cpp",
                   "solve nonlinear optimisation problem",
                   "Solution failed to converge in %i iterations (TAO reason %d)",
                   num_iterations, reason);
    }
    else
    {
      log(WARNING, "TAO solver %s failed to converge. Try a different TAO method" \
                   " or adjust some parameters.", tao_type);
    }
  }

  return num_iterations;
}
//-----------------------------------------------------------------------------
PetscErrorCode PETScTAOSolver::FormFunctionGradient(Tao tao, Vec x,
                                                    PetscReal *fobj, Vec g,
                                                    void *ctx)
{
  // Get the optimisation problem object
  struct tao_ctx_t tao_ctx = *(struct tao_ctx_t*) ctx;
  OptimisationProblem* optimisation_problem = tao_ctx.optimisation_problem;
  // PETScVector* _x = tao_ctx.x;

  // Wrap the PETSc objects
  PETScVector x_wrap(x);
  PETScVector g_wrap(g);

  // Update ghost values
  // *_x = x_wrap;
  // _x ->update_ghost_values();

  // Compute the objective function f and its gradient g = f'
  PETScMatrix H;
  *fobj = optimisation_problem->f(x_wrap);
  optimisation_problem->form(H, g_wrap, x_wrap);
  optimisation_problem->g(g_wrap, x_wrap);

  return 0;
}
//-----------------------------------------------------------------------------
PetscErrorCode PETScTAOSolver::FormHessian(Tao tao, Vec x, Mat H, Mat Hpre,
                                           void *ctx)
{
  // Get the optimisation problem object
  struct tao_ctx_t tao_ctx = *(struct tao_ctx_t*) ctx;
  OptimisationProblem* optimisation_problem = tao_ctx.optimisation_problem;

  // Wrap the PETSc objects
  PETScVector x_wrap(x);
  PETScMatrix H_wrap(H);

  // Compute the hessian H(x) = f''(x)
  PETScVector g;
  optimisation_problem->form(H_wrap, g, x_wrap);
  optimisation_problem->H(H_wrap, x_wrap);

  return 0;
}
//-----------------------------------------------------------------------------
void PETScTAOSolver::set_options()
{
  dolfin_assert(_tao);

  // Set tolerances
  TaoSetTolerances(_tao, parameters["function_absolute_tol"],
                         parameters["function_relative_tol"],
                         parameters["gradient_absolute_tol"],
                         parameters["gradient_relative_tol"],
                         parameters["gradient_t_tol"]);

  // Set TAO solver maximum iterations
  int maxits = parameters["maximum_iterations"];
  TaoSetMaximumIterations(_tao, maxits);
}
//-----------------------------------------------------------------------------
void PETScTAOSolver::set_ksp_options()
{
  dolfin_assert(_tao);

  KSP ksp;
  TaoGetKSP(_tao, &ksp);
  if (ksp)
  {
    Parameters krylov_parameters = parameters("krylov_solver");

    // GMRES restart parameter
    const int gmres_restart = krylov_parameters("gmres")["restart"];
    KSPGMRESSetRestart(ksp, gmres_restart);

    // Non-zero initial guess
    const bool nonzero_guess = krylov_parameters["nonzero_initial_guess"];
    if (nonzero_guess)
      KSPSetInitialGuessNonzero(ksp, PETSC_TRUE);
    else
      KSPSetInitialGuessNonzero(ksp, PETSC_FALSE);

    // KSP monitor
    if (krylov_parameters["monitor_convergence"])
      KSPMonitorSet(ksp, KSPMonitorTrueResidualNorm, 0, 0);

    // Set tolerances
    KSPSetTolerances(ksp,
                     krylov_parameters["relative_tolerance"],
                     krylov_parameters["absolute_tolerance"],
                     krylov_parameters["divergence_limit"],
                     krylov_parameters["maximum_iterations"]);
  }
}
//-----------------------------------------------------------------------------
#endif
