// Copyright (C) 2012 Patrick E. Farrell
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
//
// First added:  2012-10-13
// Last changed: 2012-10-13

#ifdef HAS_PETSC

#include "PETScSNESSolver.h"
#include <boost/assign/list_of.hpp>
#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/la/PETScVector.h>
#include <dolfin/la/PETScMatrix.h>
#include "NonlinearProblem.h"

using namespace dolfin;

// Utility function
namespace dolfin
{
  class PETScSNESDeleter
  {
  public:
    void operator() (SNES* _snes)
    {
      if (_snes)
        SNESDestroy(_snes);
      delete _snes;
    }
  };
}

struct snes_ctx_t
{
  NonlinearProblem* nonlinear_problem;
  PETScVector* dx;
};

// Mapping from method string to PETSc
const std::map<std::string, const SNESType> PETScSNESSolver::_methods
  = boost::assign::map_list_of("default",  "")
                              ("ls",          SNESLS)
                              ("tr",          SNESTR)
                              ("test",        SNESTEST)
                              ("ngmres",      SNESNGMRES);

// These later ones are only available from PETSc 3.3 on, I think
// but at the moment we support PETSc >= 3.2
// so I'm leaving them commented out.
//                              ("nrichardson", SNESNRICHARDSON)
//                              ("virs",        SNESVIRS)
//                              ("qn",          SNESQN)
//                              ("ncg",         SNESNCG)
//                              ("fas",         SNESFAS)
//                              ("ms",          SNESMS);

// Mapping from method string to description
const std::vector<std::pair<std::string, std::string> >
  PETScSNESSolver::_methods_descr = boost::assign::pair_list_of
    ("default",     "default SNES method")
    ("ls",          "Line search method")
    ("tr",          "Trust region method")
    ("test",        "Tool to verify Jacobian approximation")
    ("ngmres",      "Nonlinear generalised minimum residual method");
//    ("nrichardson", "Richardson nonlinear method (Picard iteration)")
//    ("virs",        "Reduced space active set solver method")
//    ("qn",          "Limited memory quasi-Newton")
//    ("ncg",         "Nonlinear conjugate gradient method")
//    ("fas",         "Full Approximation Scheme nonlinear multigrid method")
//    ("ms",          "Multistage smoothers");

//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> >
PETScSNESSolver::methods()
{
  return PETScSNESSolver::_methods_descr;
}
//-----------------------------------------------------------------------------
Parameters PETScSNESSolver::default_parameters()
{
  Parameters p(NewtonSolver::default_parameters());
  p.rename("snes_solver");
  p.add("solution_tolerance", 1.0e-16);
  p.add("maximum_residual_evaluations", 2000);
  p.remove("convergence_criterion");
  p.remove("relaxation_parameter");
  p.remove("method");
  p.add("method", "default");

  return p;
}
//-----------------------------------------------------------------------------
PETScSNESSolver::PETScSNESSolver(std::string nls_type)
{
  // Check that the requested method is known
  if (_methods.count(nls_type) == 0)
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "create PETSc SNES solver",
                 "Unknown SNES method \"%s\"", nls_type.c_str());
  }

  // Set parameter values
  parameters = default_parameters();

  init(nls_type);
}
//-----------------------------------------------------------------------------
PETScSNESSolver::~PETScSNESSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void PETScSNESSolver::init(const std::string& method)
{
  // Check that nobody else shares this solver
  if (_snes && !_snes.unique())
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "initialize PETSc SNES solver",
                 "More than one object points to the underlying PETSc object");
  }

  _snes.reset(new SNES, PETScSNESDeleter());

  if (MPI::num_processes() > 1)
    SNESCreate(PETSC_COMM_WORLD, _snes.get());
  else
    SNESCreate(PETSC_COMM_SELF, _snes.get());

  // Set some options
  SNESSetFromOptions(*_snes);

  // Set solver type
  if (method != "default")
    SNESSetType(*_snes, _methods.find(method)->second);
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, bool> PETScSNESSolver::solve(NonlinearProblem& nonlinear_problem,
                                                  GenericVector& x)
{
  PETScVector f;
  PETScMatrix A;
  int its;
  SNESConvergedReason reason;
  struct snes_ctx_t snes_ctx;

  // Compute F(u)
  nonlinear_problem.form(A, f, x);
  nonlinear_problem.F(f, x);
  nonlinear_problem.J(A, x);

  snes_ctx.nonlinear_problem = &nonlinear_problem;
  snes_ctx.dx = &x.down_cast<PETScVector>();

  if (std::string(parameters["method"]) != "default")
  {
    info("Overriding nonlinear solver method with value %s from parameters.", std::string(parameters["method"]).c_str());
    SNESSetType(*_snes, _methods.find(std::string(parameters["method"]))->second);
  }

  SNESSetFunction(*_snes, *f.vec(), PETScSNESSolver::FormFunction, &snes_ctx);
  SNESSetJacobian(*_snes, *A.mat(), *A.mat(), PETScSNESSolver::FormJacobian, &snes_ctx);

  // Set some options from the parameters
  if (parameters["report"])
  {
    SNESMonitorSet(*_snes, SNESMonitorDefault, PETSC_NULL, PETSC_NULL);
  }

  // Tolerances
  SNESSetTolerances(*_snes, parameters["absolute_tolerance"], parameters["relative_tolerance"], parameters["solution_tolerance"], parameters["maximum_iterations"], parameters["maximum_residual_evaluations"]);

  SNESSolve(*_snes, PETSC_NULL, *snes_ctx.dx->vec());

  SNESGetIterationNumber(*_snes, &its);
  SNESGetConvergedReason(*_snes, &reason);

  if (reason > 0)
  {
    if (dolfin::MPI::process_number() == 0)
    {
      info("PETSc SNES solver converged in %d iterations with convergence reason %s.", its, SNESConvergedReasons[reason]);
    }
  }
  else
  {
    if (dolfin::MPI::process_number() == 0)
    {
      warning("PETSc SNES solver diverged in %d iterations with divergence reason %s.", its, SNESConvergedReasons[reason]);
    }
  }

  if (parameters["error_on_nonconvergence"] && reason < 0)
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "solve nonlinear system with PETScSNESSolver",
                 "Solver did not converge. Bummer");
  }

  return std::make_pair(its, reason > 0);
}
//-----------------------------------------------------------------------------
PetscErrorCode PETScSNESSolver::FormFunction(SNES snes, Vec x, Vec f, void* ctx)
{
  struct snes_ctx_t snes_ctx = *(struct snes_ctx_t*) ctx;
  NonlinearProblem* nonlinear_problem = snes_ctx.nonlinear_problem;
  PETScVector* dx = snes_ctx.dx;

  PETScMatrix A;
  PETScVector df;

  boost::shared_ptr<Vec> vptr(&x, NoDeleter());
  PETScVector x_wrap(vptr);

  *dx = x_wrap;

  // Compute F(u)
  nonlinear_problem->form(A, df, *dx);
  nonlinear_problem->F(df, *dx);

  VecCopy(*df.vec(), f);

  return 0;
}
//-----------------------------------------------------------------------------
PetscErrorCode PETScSNESSolver::FormJacobian(SNES snes, Vec x, Mat* A, Mat* B, MatStructure* flag, void* ctx)
{
  struct snes_ctx_t snes_ctx = *(struct snes_ctx_t*) ctx;
  NonlinearProblem* nonlinear_problem = snes_ctx.nonlinear_problem;
  PETScVector* dx = snes_ctx.dx;

  PETScVector f;
  PETScMatrix dA;

  boost::shared_ptr<Vec> vptr(&x, NoDeleter());
  PETScVector x_wrap(vptr);

  *dx = x_wrap;

  nonlinear_problem->form(dA, f, *dx);
  nonlinear_problem->J(dA, *dx);

  MatCopy(*dA.mat(), *A, SAME_NONZERO_PATTERN);
  if (B != A)
  {
    MatCopy(*dA.mat(), *B, SAME_NONZERO_PATTERN);
  }
  *flag = SAME_NONZERO_PATTERN;

  return 0;
}


#endif
