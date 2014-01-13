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
// Modified by Corrado Maurini 2013
// Modified by Anders Logg 2013
//
// First added:  2012-10-13
// Last changed: 2013-11-21

#ifdef HAS_PETSC

#include <map>
#include <string>
#include <utility>
#include <petscsys.h>
#include <boost/assign/list_of.hpp>

#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/timing.h>
#include <dolfin/common/Timer.h>
#include <dolfin/la/PETScKrylovSolver.h>
#include <dolfin/la/PETScMatrix.h>
#include <dolfin/la/PETScLUSolver.h>
#include <dolfin/la/PETScPreconditioner.h>
#include <dolfin/la/PETScVector.h>
#include "NonlinearProblem.h"
#include "PETScSNESSolver.h"

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
  const PETScVector* xl;
  const PETScVector* xu;
};

#if PETSC_VERSION_RELEASE
  #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 2
  // Mapping from method string to PETSc
  const std::map<std::string, std::pair<std::string, const SNESType> >
  PETScSNESSolver::_methods
    = boost::assign::map_list_of
        ("default", std::make_pair("default SNES method", ""))
        ("ls",      std::make_pair("Line search method",  SNESLS))
        ("tr",      std::make_pair("Trust region method", SNESTR))
        ("vi",      std::make_pair("Reduced space active set solver method (for bounds)", SNESVI))
        ("test",    std::make_pair("Tool to verify Jacobian approximation", SNESTEST));
  #elif PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 3 // PETSc 3.3
  // Mapping from method string to PETSc
  const std::map<std::string, std::pair<std::string, const SNESType> >
  PETScSNESSolver::_methods
    = boost::assign::map_list_of
        ("default",     std::make_pair("default SNES method", ""))
        ("ls",          std::make_pair("Line search method", SNESLS))
        ("tr",          std::make_pair("Trust region method",  SNESTR))
        ("test",        std::make_pair("Tool to verify Jacobian approximation", SNESTEST))
        ("ngmres",      std::make_pair("Nonlinear generalised minimum residual method", SNESNGMRES))
        ("nrichardson", std::make_pair("Richardson nonlinear method (Picard iteration)", SNESNRICHARDSON))
        ("virs",        std::make_pair("Reduced space active set solver method (for bounds)", SNESVIRS))
        ("viss",        std::make_pair("Reduced space active set solver method (for bounds)", SNESVISS))
        ("qn",          std::make_pair("Limited memory quasi-Newton", SNESQN))
        ("ncg",         std::make_pair("Nonlinear conjugate gradient method", SNESNCG))
        ("fas",         std::make_pair("Full Approximation Scheme nonlinear multigrid method", SNESFAS))
        ("ms",          std::make_pair("Multistage smoothers", SNESMS));
  #elif PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 4 // PETSc 3.4
  // Mapping from method string to PETSc
  const std::map<std::string, std::pair<std::string, const SNESType> >
  PETScSNESSolver::_methods
   = boost::assign::map_list_of
      ("default",      std::make_pair("default SNES method", ""))
      ("newtonls",     std::make_pair("Line search method", SNESNEWTONLS))
      ("newtontr",     std::make_pair("Trust region method", SNESNEWTONTR))
      ("test",         std::make_pair("Tool to verify Jacobian approximation", SNESTEST))
      ("ngmres",       std::make_pair("Nonlinear generalised minimum residual method", SNESNGMRES))
      ("nrichardson",  std::make_pair("Richardson nonlinear method (Picard iteration)", SNESNRICHARDSON))
      ("vinewtonrsls", std::make_pair("Reduced space active set solver method (for bounds)", SNESVINEWTONRSLS))
      ("vinewtonssls", std::make_pair("Reduced space active set solver method (for bounds)", SNESVINEWTONSSLS))
      ("qn",           std::make_pair("Limited memory quasi-Newton", SNESQN))
      ("ncg",          std::make_pair("Nonlinear conjugate gradient method", SNESNCG))
      ("fas",          std::make_pair("Full Approximation Scheme nonlinear multigrid method", SNESFAS))
      ("nasm",         std::make_pair("Nonlinear Additive Schwartz", SNESNASM))
      ("anderson",     std::make_pair("Anderson mixing method", SNESANDERSON))
      ("aspin",        std::make_pair("Additive-Schwarz Preconditioned Inexact Newton", SNESASPIN))
      ("ms",           std::make_pair("Multistage smoothers", SNESMS));
  #endif
#else // Development version
  // Mapping from method string to PETSc
  const std::map<std::string, std::pair<std::string, const SNESType> >
  PETScSNESSolver::_methods
   = boost::assign::map_list_of
      ("default",      std::make_pair("default SNES method", ""))
      ("newtonls",     std::make_pair("Line search method", SNESNEWTONLS))
      ("newtontr",     std::make_pair("Trust region method", SNESNEWTONTR))
      ("test",         std::make_pair("Tool to verify Jacobian approximation", SNESTEST))
      ("ngmres",       std::make_pair("Nonlinear generalised minimum residual method", SNESNGMRES))
      ("nrichardson",  std::make_pair("Richardson nonlinear method (Picard iteration)", SNESNRICHARDSON))
      ("vinewtonrsls", std::make_pair("Reduced space active set solver method (for bounds)", SNESVINEWTONRSLS))
      ("vinewtonssls", std::make_pair("Reduced space active set solver method (for bounds)", SNESVINEWTONSSLS))
      ("qn",           std::make_pair("Limited memory quasi-Newton", SNESQN))
      ("ncg",          std::make_pair("Nonlinear conjugate gradient method", SNESNCG))
      ("fas",          std::make_pair("Full Approximation Scheme nonlinear multigrid method", SNESFAS))
      ("ms",           std::make_pair("Multistage smoothers", SNESMS));
#endif

//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> > PETScSNESSolver::methods()
{
  std::vector<std::pair<std::string, std::string> > available_methods;
  std::map<std::string, std::pair<std::string,
                                  const SNESType> >::const_iterator it;
  for (it = _methods.begin(); it != _methods.end(); ++it)
    available_methods.push_back(std::make_pair(it->first, it->second.first));
  return available_methods;
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

  // The line search business changed completely from PETSc 3.2 to 3.3.
  std::set<std::string> line_searches;
  #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 2
  line_searches.insert("basic");
  line_searches.insert("quadratic");
  line_searches.insert("cubic");
  p.add("line_search", "basic", line_searches);
  #else
  line_searches.insert("basic");
  line_searches.insert("bt");
  line_searches.insert("l2");
  line_searches.insert("cp");
  p.add("line_search", "basic", line_searches);
  #endif

  std::set<std::string> bound_types;
  bound_types.insert("default");
  bound_types.insert("nonnegative");
  bound_types.insert("nonpositive");
  p.add("sign", "default", bound_types);

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
  SNESCreate(PETSC_COMM_WORLD, _snes.get());

  // Set solver type
  if (method != "default")
  {
    std::map<std::string, std::pair<std::string,
                                    const SNESType> >::const_iterator it;
    it = _methods.find(method);
    dolfin_assert(it != _methods.end());
    SNESSetType(*_snes, it->second.second);
  }

  // Set some options
  SNESSetFromOptions(*_snes);

  // Set to default to not having explicit bounds
  has_explicit_bounds = false;
}
//-----------------------------------------------------------------------------
std::pair<std::size_t, bool>
PETScSNESSolver::solve(NonlinearProblem& nonlinear_problem,
                       GenericVector& x,
                       const GenericVector&  lb,
                       const GenericVector&  ub)
{
  // Set linear solver parameters
  set_linear_solver_parameters();

  // Check size of the bound vectors
  if (lb.size() != ub.size())
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "assigning upper and lower bounds",
                 "The size of the given upper and lower bounds is different");
  }
  else if (lb.size() != x.size())
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "assigning upper and lower bounds",
                 "The size of the bounds is different from the size of the solution vector");
  }

  // Set the bounds
  boost::shared_ptr<const PETScVector>
    _ub(&ub.down_cast<PETScVector>(), NoDeleter());
  boost::shared_ptr<const PETScVector>
    _lb(&lb.down_cast<PETScVector>(), NoDeleter());
  this->lb = _lb;
  this->ub = _ub;
  has_explicit_bounds = true;

  return solve(nonlinear_problem, x);
}
//-----------------------------------------------------------------------------
std::pair<std::size_t, bool>
  PETScSNESSolver::solve(NonlinearProblem& nonlinear_problem,
                         GenericVector& x)
{
  Timer timer("SNES solver");
  PETScVector f;
  PETScMatrix A;
  PetscInt its;
  SNESConvergedReason reason;
  struct snes_ctx_t snes_ctx;

  // Set linear solver parameters
  set_linear_solver_parameters();

  // Compute F(u)
  nonlinear_problem.form(A, f, x);
  nonlinear_problem.F(f, x);
  nonlinear_problem.J(A, x);

  snes_ctx.nonlinear_problem = &nonlinear_problem;
  snes_ctx.dx = &x.down_cast<PETScVector>();

  SNESSetFunction(*_snes, *f.vec(), PETScSNESSolver::FormFunction, &snes_ctx);
  SNESSetJacobian(*_snes, *A.mat(), *A.mat(), PETScSNESSolver::FormJacobian,
                  &snes_ctx);

  // Set some options from the parameters
  if (parameters["report"])
    SNESMonitorSet(*_snes, SNESMonitorDefault, PETSC_NULL, PETSC_NULL);

  // Set the bounds, if any
  set_bounds(x);

  // Set the method
  if (std::string(parameters["method"]) != "default")
  {
    std::map<std::string, std::pair<std::string,
                                    const SNESType> >::const_iterator it;
    it = _methods.find(std::string(parameters["method"]));
    dolfin_assert(it != _methods.end());
    SNESSetType(*_snes, it->second.second);
    SNESSetFromOptions(*_snes);
  // If
  //      a) the user has set bounds (is_vi())
  // AND  b) the user has not set a solver (method == default)
  // THEN set a good method that supports bounds
  // (most methods do not support bounds)
  }
  else if (std::string(parameters["method"]) == "default" && is_vi())
  {
    std::map<std::string, std::pair<std::string,
                                    const SNESType> >::const_iterator it;
    #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 2
    it = _methods.find("vi");
    #elif PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 3 && PETSC_VERSION_RELEASE
    it = _methods.find("viss");
    #else
    it = _methods.find("vinewtonssls");
    #endif
    dolfin_assert(it != _methods.end());
    SNESSetType(*_snes, it->second.second);
    SNESSetFromOptions(*_snes);
  }

  // The line search business changed completely from PETSc 3.2 to 3.3.
  #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 2
  if (parameters["report"])
    SNESLineSearchSetMonitor(*_snes, PETSC_TRUE);

  const std::string line_search = std::string(parameters["line_search"]);
  if (line_search == "basic")
    SNESLineSearchSet(*_snes, SNESLineSearchNo, PETSC_NULL);
  else if (line_search == "quadratic")
    SNESLineSearchSet(*_snes, SNESLineSearchQuadratic, PETSC_NULL);
  else if (line_search == "cubic")
    SNESLineSearchSet(*_snes, SNESLineSearchCubic, PETSC_NULL);
  else
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "set line search algorithm",
                 "Unknown line search \"%s\"", line_search.c_str());
  }
  #else
  SNESLineSearch linesearch;

  #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR < 4
  SNESGetSNESLineSearch(*_snes, &linesearch);
  #else
  SNESGetLineSearch(*_snes, &linesearch);
  #endif

  if (parameters["report"])
    SNESLineSearchSetMonitor(linesearch, PETSC_TRUE);
  const std::string line_search_type = std::string(parameters["line_search"]);
  SNESLineSearchSetType(linesearch, line_search_type.c_str());
  #endif

  // Tolerances
  const int max_iters = parameters["maximum_iterations"];
  const int max_residual_evals = parameters["maximum_residual_evaluations"];
  SNESSetTolerances(*_snes, parameters["absolute_tolerance"],
                    parameters["relative_tolerance"],
                    parameters["solution_tolerance"],
                    max_iters, max_residual_evals);

  if (parameters["report"])
    SNESView(*_snes, PETSC_VIEWER_STDOUT_WORLD);

  SNESSolve(*_snes, PETSC_NULL, *snes_ctx.dx->vec());

  SNESGetIterationNumber(*_snes, &its);
  SNESGetConvergedReason(*_snes, &reason);

  MPI_Comm comm = MPI_COMM_NULL;
  PetscObjectGetComm((PetscObject)*_snes, &comm);
  if (reason > 0 && parameters["report"]
    && dolfin::MPI::process_number(comm) == 0)
  {
    info("PETSc SNES solver converged in %d iterations with convergence reason %s.",
         its, SNESConvergedReasons[reason]);
  }
  else if (reason < 0 && dolfin::MPI::process_number(comm) == 0)
  {
    warning("PETSc SNES solver diverged in %d iterations with divergence reason %s.",
            its, SNESConvergedReasons[reason]);
  }

  if (parameters["error_on_nonconvergence"] && reason < 0)
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "solve nonlinear system with PETScSNESSolver",
                 "Solver did not converge",
                 "Bummer");
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

  // Cast from the PETSc Vec type to dolfin's PETScVector
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
PetscErrorCode PETScSNESSolver::FormJacobian(SNES snes, Vec x, Mat* A, Mat* B,
                                             MatStructure* flag, void* ctx)
{
  struct snes_ctx_t snes_ctx = *(struct snes_ctx_t*) ctx;
  NonlinearProblem* nonlinear_problem = snes_ctx.nonlinear_problem;
  PETScVector* dx = snes_ctx.dx;

  PETScVector f;
  PETScMatrix dA;

  // Cast from the PETSc Vec type to dolfin's PETScVector
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
//-----------------------------------------------------------------------------
void PETScSNESSolver::set_linear_solver_parameters()
{
  KSP ksp;
  PC pc;

  SNESGetKSP(*_snes, &ksp);
  KSPGetPC(ksp, &pc);

  MPI_Comm comm = MPI_COMM_NULL;
  PetscObjectGetComm((PetscObject)*_snes, &comm);

  if (parameters["report"])
    KSPMonitorSet(ksp, KSPMonitorDefault, PETSC_NULL, PETSC_NULL);

  const std::string linear_solver  = parameters["linear_solver"];
  const std::string preconditioner = parameters["preconditioner"];

  if (linear_solver == "default")
  {
    // Do nothing
  }
  else if (PETScKrylovSolver::_methods.count(linear_solver) != 0)
  {
    std::map<std::string, const KSPType>::const_iterator
      solver_pair = PETScKrylovSolver::_methods.find(linear_solver);
    dolfin_assert(solver_pair != PETScKrylovSolver::_methods.end());
    KSPSetType(ksp, solver_pair->second);
    if (preconditioner != "default"
        && PETScPreconditioner::_methods.count(preconditioner) != 0)
    {
      std::map<std::string, const PCType>::const_iterator it
        = PETScPreconditioner::_methods.find(preconditioner);
      dolfin_assert(it != PETScPreconditioner::_methods.end());
      PCSetType(pc, it->second);
    }
  }
  else if (linear_solver == "lu"
           || PETScLUSolver::_methods.count(linear_solver) != 0)
  {
    std::string lu_method;
    if (PETScLUSolver::_methods.find(linear_solver)
        != PETScLUSolver::_methods.end())
    {
      lu_method = linear_solver;
    }
    else
    {
      if (MPI::num_processes(comm) == 1)
      {
        #if PETSC_HAVE_UMFPACK
        lu_method = "umfpack";
        #elif PETSC_HAVE_MUMPS
        lu_method = "mumps";
        #elif PETSC_HAVE_PASTIX
        lu_method = "pastix";
        #elif PETSC_HAVE_SUPERLU
        lu_method = "superlu";
        #else
        lu_method = "petsc";
        warning("Using PETSc native LU solver. Consider configuring PETSc with an efficient LU solver (e.g. UMFPACK, MUMPS).");
        #endif
      }
      else
      {
        #if PETSC_HAVE_SUPERLU_DIST
        lu_method = "superlu_dist";
        #elif PETSC_HAVE_PASTIX
        lu_method = "pastix";
        #elif PETSC_HAVE_MUMPS
        lu_method = "mumps";
        #else
        dolfin_error("PETScSNESSolver.cpp",
                     "solve linear system using PETSc LU solver",
                     "No suitable solver for parallel LU found. Consider configuring PETSc with MUMPS or SuperLU_dist");
        #endif
      }
    }

    KSPSetType(ksp, KSPPREONLY);
    PCSetType(pc, PCLU);
    std::map<std::string, const MatSolverPackage>::const_iterator
      it = PETScLUSolver::_methods.find(lu_method);
    dolfin_assert(it != PETScLUSolver::_methods.end());
    PCFactorSetMatSolverPackage(pc, it->second);
  }
  else
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "set linear solver options",
                 "Unknown KSP method \"%s\"", linear_solver.c_str());
  }
}
//-----------------------------------------------------------------------------
void PETScSNESSolver::set_bounds(GenericVector& x)
{
  if (is_vi())
  {
    dolfin_assert(_snes);
    const std::string sign   = parameters["sign"];
    const std::string method = parameters["method"];
    #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 2
    MPI_Comm comm = MPI_COMM_NULL;
    PetscObjectGetComm((PetscObject)*_snes, &comm);
    if (dolfin::MPI::process_number(comm) == 0)
    {
      warning("Use of SNESVI solvers with PETSc 3.2 may lead to convergence issues and is strongly discouraged.");
    }

    if (method != "vi" && method != "default")
    {
      dolfin_error("PETScSNESSolver.cpp",
                   "set variational inequality bounds",
                   "With PETSc 3.2 need to use vi method if bounds are set");
    }
    #else
    if (method != "virs" && method != "viss" && method != "default")
    {
      dolfin_error("PETScSNESSolver.cpp",
                   "set variational inequality bounds",
                   "Need to use virs or viss methods if bounds are set");
    }
    #endif

    if (sign != "default")
    {
      // Here, x is the model vector from which we make our Vecs that
      // tell PETSc the bounds.
      Vec ub, lb;

      PETScVector dx = x.down_cast<PETScVector>();
      VecDuplicate(*dx.vec(), &ub);
      VecDuplicate(*dx.vec(), &lb);
      if (sign == "nonnegative")
      {
        VecSet(lb, 0.0);
        VecSet(ub, SNES_VI_INF);
      }
      else if (sign == "nonpositive")
      {
        VecSet(ub, 0.0);
        VecSet(lb, SNES_VI_NINF);
      }
      else
      {
        dolfin_error("PETScSNESSolver.cpp",
                     "set PETSc SNES solver bounds",
                     "Unknown bound type \"%s\"", sign.c_str());
      }

      SNESVISetVariableBounds(*_snes, lb, ub);
      VecDestroy(&ub);
      VecDestroy(&lb);
    }
    else if (has_explicit_bounds == true)
    {
      const PETScVector* lb = this->lb.get();
      const PETScVector* ub = this->ub.get();
      SNESVISetVariableBounds(*_snes, *(lb->vec()).get(), *(ub->vec()).get());
    }
  }
}
//-----------------------------------------------------------------------------
bool PETScSNESSolver::is_vi() const
{
  if (std::string(parameters["sign"]) != "default"
      && this->has_explicit_bounds == true)
  {
    dolfin_error("PETScSNESSolver.cpp",
                 "set variational inequality bounds",
                 "Both the sign parameter and the explicit bounds are set");
    return false;
  }
  else if (std::string(parameters["sign"]) != "default"
           || this->has_explicit_bounds == true)
  {
    return true;
  }
  else
    return false;
}
//-----------------------------------------------------------------------------

#endif
