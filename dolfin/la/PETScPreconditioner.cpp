// Copyright (C) 2010 Garth N. Wells
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2010-02-25
// Last changed:

#ifdef HAS_PETSC

#include <boost/assign/list_of.hpp>
#include <petscksp.h>
#include <dolfin/la/KrylovSolver.h>
#include <dolfin/la/PETScKrylovSolver.h>
#include <dolfin/log/dolfin_log.h>
#include "PETScPreconditioner.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
// Available preconditioners
const std::map<std::string, const PCType> PETScPreconditioner:methods
  = boost::assign::map_list_of("default",   "")
                              ("none",      PCNONE)
                              ("ilu",       PCILU)
                              ("jacobi",    PCJACOBI)
                              ("sor",       PCSOR)
                              ("icc",       PCICC)
                              ("amg_hypre", PCHYPRE)
                              ("amg_ml",    PCML);
//-----------------------------------------------------------------------------
Parameters PETScPreconditioner::default_parameters()
{
  Parameters p(KrylovSolver::default_parameters());
  p.rename("petsc_preconditioner");
  p.add("ilu_fill_level", 0);
  return p;
}
//-----------------------------------------------------------------------------
PETScPreconditioner::PETScPreconditioner(std::string type) : type(type)
{
  // Set parameter values
  parameters = default_parameters();

  // Check that the requested method is known
  if (methods.count(type) == 0)
    error("Requested PETSc proconditioner '%s' is unknown,", type.c_str());
}
//-----------------------------------------------------------------------------
PETScPreconditioner::~PETScPreconditioner()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void PETScPreconditioner::set(PETScKrylovSolver& solver) const
{
  // Handle default preconditioner (do nothing)
  if (type == "default")
    return;

  assert(solver.ksp());

  // Get PETSc PC pointer
  PC pc;
  KSPGetPC(*(solver.ksp()), &pc);

  // Make sure options are set
  PCSetFromOptions(pc);

  // Treat special cases  first
  if (type == "amg_hypre")
  {
    #if PETSC_HAVE_HYPRE
    PCSetType(pc, PCHYPRE);
    PCHYPRESetType(pc, "boomeramg");
    PCSetFromOptions(pc);
    #else
    warning("PETSc has not been compiled with the HYPRE library for   "
            "algebraic multigrid. Default PETSc solver will be used. "
            "For performance, installation of HYPRE is recommended.");
    #endif
    return;
  }

  if (type == "amg_ml")
  {
    #if PETSC_HAVE_ML
    PCSetType(pc, PCML);
    PCFactorSetShiftNonzero(pc, PETSC_DECIDE);
    #else
    warning("PETSc has not been compiled with the ML library for   "
            "algerbraic multigrid. Default PETSc solver will be used. "
            "For performance, installation of ML is recommended.");
    #endif
    return;
  }

  // Set preconditioner
  PCSetType(pc, methods.find(type)->second);

  // Set preconditioner parameters
  PCFactorSetShiftNonzero(pc, parameters["shift_nonzero"]);
}
//-----------------------------------------------------------------------------
std::string PETScPreconditioner::str(bool verbose) const
{
  std::stringstream s;
  if (verbose)
  {
    warning("Verbose output for PETScPreconditioner not implemented.");
    //PCView(*pc, PETSC_VIEWER_STDOUT_WORLD);
  }
  else
    s << "<PETScPreconditioner>";

  return s.str();
}
//-----------------------------------------------------------------------------

#endif
