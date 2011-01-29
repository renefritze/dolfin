// Copyright (C) 2005-2009 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2009-2010.
// Modified by Niclas Jansson, 2009.
//
// First added:  2005
// Last changed: 2010-04-05

#ifdef HAS_PETSC

#include <dolfin/common/Timer.h>

#include <boost/assign/list_of.hpp>
#include <dolfin/common/constants.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/MPI.h>
#include <dolfin/parameter/GlobalParameters.h>
#include "LUSolver.h"
#include "PETScMatrix.h"
#include "PETScVector.h"
#include "PETScLUSolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
namespace dolfin
{
  class PETScKSPDeleter
  {
  public:
    void operator() (KSP* ksp)
    {
      if (ksp)
        KSPDestroy(*ksp);
      delete ksp;
    }
  };
}

// Forward compatibility with petsc-dev
#if (PETSC_VERSION_RELEASE == 0)
#define MAT_SOLVER_UMFPACK      MATSOLVERUMFPACK
#define MAT_SOLVER_MUMPS        MATSOLVERMUMPS
#define MAT_SOLVER_PASTIX       MATSOLVERPASTIX
#define MAT_SOLVER_PETSC        MATSOLVERPETSC
#define MAT_SOLVER_SPOOLES      MATSOLVERSPOOLES
#define MAT_SOLVER_SUPERLU_DIST MATSOLVERSUPERLU_DIST
#define MAT_SOLVER_SUPERLU      MATSOLVERSUPERLU
#endif

//-----------------------------------------------------------------------------
// Available LU solver
const std::map<std::string, const MatSolverPackage> PETScLUSolver::lu_packages
  = boost::assign::map_list_of("default", "")
                              ("umfpack",      MAT_SOLVER_UMFPACK)
                              ("mumps",        MAT_SOLVER_MUMPS)
                              ("pastix",       MAT_SOLVER_PASTIX)
                              ("petsc",        MAT_SOLVER_PETSC)
                              ("spooles",      MAT_SOLVER_SPOOLES)
                              ("superlu_dist", MAT_SOLVER_SUPERLU_DIST)
                              ("superlu",      MAT_SOLVER_SUPERLU);
//-----------------------------------------------------------------------------
Parameters PETScLUSolver::default_parameters()
{
  Parameters p(LUSolver::default_parameters());
  p.rename("petsc_lu_solver");

  // Number of threads per process for multi-threaded solvers
  p.add<uint>("num_threads");

  return p;
}
//-----------------------------------------------------------------------------
PETScLUSolver::PETScLUSolver(std::string lu_package)
{
  // Set parameter values
  parameters = default_parameters();

  // Initialize PETSc LU solver
  init_solver(lu_package);
}
//-----------------------------------------------------------------------------
PETScLUSolver::PETScLUSolver(const GenericMatrix& A, std::string lu_package)
               : A(reference_to_no_delete_pointer(A.down_cast<PETScMatrix>()))
{
  // Check dimensions
  if (A.size(0) != A.size(1))
    error("Cannot LU factorize non-square PETSc matrix.");

  // Set parameter values
  parameters = default_parameters();

  // Initialize PETSc LU solver
  init_solver(lu_package);
}
//-----------------------------------------------------------------------------
PETScLUSolver::PETScLUSolver(boost::shared_ptr<const PETScMatrix> A,
                             std::string lu_package) : A(A)
{
  // Check dimensions
  if (A->size(0) != A->size(1))
    error("Cannot LU factorize non-square PETSc matrix.");

  // Set parameter values
  parameters = default_parameters();

  // Initialize PETSc LU solver
  init_solver(lu_package);
}
//-----------------------------------------------------------------------------
PETScLUSolver::~PETScLUSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void PETScLUSolver::set_operator(const GenericMatrix& A)
{
  set_operator(A.down_cast<PETScMatrix>());
}
//-----------------------------------------------------------------------------
void PETScLUSolver::set_operator(const PETScMatrix& A)
{
  this->A = reference_to_no_delete_pointer(A);
  assert(this->A);
}
//-----------------------------------------------------------------------------
dolfin::uint PETScLUSolver::solve(GenericVector& x, const GenericVector& b)
{
  assert(_ksp);
  assert(A);

  // Downcast matrix and vectors
  const PETScVector& _b = b.down_cast<PETScVector>();
  PETScVector& _x = x.down_cast<PETScVector>();

  // Check dimensions
  if (A->size(0) != b.size())
    error("Cannot LU factorize non-square PETSc matrix.");

  // Initialize solution vector (remains untouched if dimensions match)
  x.resize(A->size(1));

  // Set PETSc operators (depends on factorization re-use options);
  set_petsc_operators();

  // Write a pre-solve message
  pre_report(A->down_cast<PETScMatrix>());

  // FIXME: Check for solver type
  // Set number of threads if using PaStiX
  if (parameters["num_threads"].is_set())
  {
    // Use number of threads specified for LU solver
    PetscOptionsSetValue("-mat_pastix_threadnbr", parameters["num_threads"].value_str().c_str());
  }
  else
  {
    // Use global number of threads
    PetscOptionsSetValue("-mat_pastix_threadnbr", dolfin::parameters["num_threads"].value_str().c_str());
  }
  //PetscOptionsSetValue("-mat_pastix_verbose", "2");

  // Solve linear system
  //tic();
  KSPSolve(*_ksp, *_b.vec(), *_x.vec());
  //std::cout << "Time to solve linear system: " <<  toc() << std::endl;;

  return 1;
}
//-----------------------------------------------------------------------------
dolfin::uint PETScLUSolver::solve(const GenericMatrix& A, GenericVector& x,
                                  const GenericVector& b)
{
  return solve(A.down_cast<PETScMatrix>(), x.down_cast<PETScVector>(),
               b.down_cast<PETScVector>());
}
//-----------------------------------------------------------------------------
dolfin::uint PETScLUSolver::solve(const PETScMatrix& A, PETScVector& x,
                                  const PETScVector& b)
{
  set_operator(A);
  return solve(x, b);
}
//-----------------------------------------------------------------------------
std::string PETScLUSolver::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    warning("Verbose output for PETScLUSolver not implemented, calling PETSc KSPView directly.");
    KSPView(*_ksp, PETSC_VIEWER_STDOUT_WORLD);
  }
  else
    s << "<PETScLUSolver>";

  return s.str();
}
//-----------------------------------------------------------------------------
boost::shared_ptr<KSP> PETScLUSolver::ksp() const
{
  return _ksp;
}
//-----------------------------------------------------------------------------
const MatSolverPackage PETScLUSolver::select_solver(std::string& lu_package) const
{
  // Check package string
  if (lu_packages.count(lu_package) == 0)
    error("Requested PETSc LU solver '%s' is unknown,", lu_package.c_str());

  // Choose appropriate 'default' solver
  if (lu_package == "default")
  {
    if (MPI::num_processes() == 1)
    {
      #if PETSC_HAVE_UMFPACK
      lu_package = "umfpack";
      #elif PETSC_HAVE_MUMPS
      lu_package = "mumps";
      #elif PETSC_HAVE_PASTIX
      lu_package = "pastix";
      #elif PETSC_HAVE_SUPERLU
      lu_package = "superlu";
      #elif PETSC_HAVE_SPOOLES
      lu_package = "spooles";
      #else
      lu_package = "petsc";
      warning("Using PETSc native LU solver. Consider configuring PETSc with an efficient LU solver (e.g. UMFPACK, MUMPS).");
      #endif
    }
    else
    {
      #if PETSC_HAVE_MUMPS
      lu_package = "mumps";
      #elif PETSC_HAVE_PASTIX
      lu_package = "pastix";
      #elif PETSC_HAVE_SPOOLES
      lu_package = "spooles";
      #elif PETSC_HAVE_SUPERLU_DIST
      lu_package = "superlu_dist";
      #else
      error("No suitable solver for parallel LU. Consider configuring PETSc with MUMPS or SPOOLES.");
      #endif
    }
  }

  return lu_packages.find(lu_package)->second;
}
//-----------------------------------------------------------------------------
void PETScLUSolver::init_solver(std::string& lu_package)
{
  // Select solver
  const MatSolverPackage solver_package = select_solver(lu_package);

  // Destroy old solver environment if necessary
  if (_ksp)
  {
    if (!_ksp.unique())
      error("Cannot create new KSP Krylov solver. More than one object points to the underlying PETSc object.");
  }
  _ksp.reset(new KSP, PETScKSPDeleter());

  // Create solver
  if (MPI::num_processes() > 1)
    KSPCreate(PETSC_COMM_WORLD, _ksp.get());
  else
    KSPCreate(PETSC_COMM_SELF, _ksp.get());

  // Make solver preconditioner only
  KSPSetType(*_ksp, KSPPREONLY);

  // Set preconditioner to LU factorization
  PC pc;
  KSPGetPC(*_ksp, &pc);
  PCSetType(pc, PCLU);

  // Set solver package
  PCFactorSetMatSolverPackage(pc, solver_package);

  // Allow matrices with zero diagonals to be solved
  #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 1
  PCFactorSetShiftType(pc, MAT_SHIFT_NONZERO);
  PCFactorSetShiftAmount(pc, PETSC_DECIDE);
  #else
  PCFactorSetShiftNonzero(pc, PETSC_DECIDE);
  #endif
}
//-----------------------------------------------------------------------------
void PETScLUSolver::set_petsc_operators()
{
  assert(A->mat());

  // Get some parameters
  const bool reuse_fact   = parameters["reuse_factorization"];
  const bool same_pattern = parameters["same_nonzero_pattern"];

  // Set operators with appropriate preconditioner option
  if (reuse_fact)
    KSPSetOperators(*_ksp, *A->mat(), *A->mat(), SAME_PRECONDITIONER);
  else if (same_pattern)
    KSPSetOperators(*_ksp, *A->mat(), *A->mat(), SAME_NONZERO_PATTERN);
  else
    KSPSetOperators(*_ksp, *A->mat(), *A->mat(), DIFFERENT_NONZERO_PATTERN);
}
//-----------------------------------------------------------------------------
void PETScLUSolver::pre_report(const PETScMatrix& A) const
{
  const MatSolverPackage solver_type;
  PC pc;
  KSPGetPC(*_ksp, &pc);
  PCFactorGetMatSolverPackage(pc, &solver_type);

  // Get parameter
  const bool report = parameters["report"];

  if (report && dolfin::MPI::process_number() == 0)
  {
    info(PROGRESS, "Solving linear system of size %d x %d (PETSc LU solver, %s).",
         A.size(0), A.size(1), solver_type);
  }
}
//-----------------------------------------------------------------------------

#endif
