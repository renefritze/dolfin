// Copyright (C) 2010 Garth N. Wells
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2010.
//
// First added:  2010-02-25
// Last changed: 2010-03-02

#ifdef HAS_PETSC

#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <petscksp.h>
#include <petscmat.h>
#include <dolfin/la/KrylovSolver.h>
#include <dolfin/la/PETScKrylovSolver.h>
#include <dolfin/log/dolfin_log.h>
#include "PETScPreconditioner.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
// Available preconditioners
const std::map<std::string, const PCType> PETScPreconditioner::methods
  = boost::assign::map_list_of("default",          "")
                              ("none",             PCNONE)
                              ("additive_schwarz", PCASM)
                              ("ilu",              PCILU)
                              ("bjacobi",          PCBJACOBI)
                              ("jacobi",           PCJACOBI)
                              ("sor",              PCSOR)
                              ("icc",              PCICC)
                              ("hypre_amg",     PCHYPRE)
                              ("hypre_euclid",     PCHYPRE)
                              ("hypre_parasails",  PCHYPRE)
                              ("hypre_pilut",      PCHYPRE)
                              ("amg_hypre",        PCHYPRE)
                              ("amg_ml",           PCML);
//-----------------------------------------------------------------------------
Parameters PETScPreconditioner::default_parameters()
{
  Parameters p(KrylovSolver::default_parameters());
  p.rename("petsc_preconditioner");
  p.add("schwarz_overlap", 1);

  // ILU parameters
  Parameters p_ilu("ilu");
  p_ilu.add("fill_level", 0);

  // Hypre/parasails parameters
  Parameters p_parasails("parasails");
  p_parasails.add("threshold", 0.15);
  p_parasails.add("levels", 0);

  // Hypre package parameters
  Parameters p_hypre("hypre");
  p_hypre.add(p_parasails);

  p.add(p_ilu);
  p.add(p_hypre);

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
  assert(solver.ksp());

  // Get PETSc PC pointer
  PC pc;
  KSPGetPC(*(solver.ksp()), &pc);

  // Treat special cases  first
  if (type.find("hypre") != std::string::npos)
  {
    #if PETSC_HAVE_HYPRE
    PCSetType(pc, PCHYPRE);
    if (type == "amg_hypre" || type == "hypre_amg")
      PCHYPRESetType(pc, "boomeramg");
    else if (type == "hypre_parasails")
    {
      PCHYPRESetType(pc, "parasails");
      const double thresh = parameters("hypre")("parasails")["threshold"];
      const int levels = parameters("hypre")("parasails")["levels"];

      PetscOptionsSetValue("-pc_hypre_parasails_thresh", boost::lexical_cast<std::string>(thresh).c_str());
      PetscOptionsSetValue("-pc_hypre_parasails_nlevels", boost::lexical_cast<std::string>(levels).c_str());
      //PetscOptionsSetValue("-pc_hypre_parasails_thresh", "0.15");
      //PetscOptionsSetValue("-pc_hypre_parasails_nlevels", "0");

    }
    else if (type == "hypre_euclid")
      PCHYPRESetType(pc, "euclid");
    else
      error("Requested Hypre preconditioner unknown (Note: pilut is not supported).");

    #else
    warning("PETSc has not been compiled with the HYPRE library for "
            "algebraic multigrid. Default PETSc solver will be used. "
            "For performance, installation of HYPRE is recommended.");
    #endif
  }
  else if (type == "amg_ml")
  {
    #if PETSC_HAVE_ML
    PCSetType(pc, PCML);
      #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 1
      PCFactorSetShiftType(pc, MAT_SHIFT_NONZERO);
      PCFactorSetShiftAmount(pc, PETSC_DECIDE);
      #else
      PCFactorSetShiftNonzero(pc, PETSC_DECIDE);
      #endif
    #else
    warning("PETSc has not been compiled with the ML library for   "
            "algerbraic multigrid. Default PETSc solver will be used. "
            "For performance, installation of ML is recommended.");
    #endif
  }
  else if (type == "additive_schwarz")
  {
    // Select method and and overlap
    PCSetType(pc, methods.find("additive_schwarz")->second);
    PCASMSetOverlap(pc, parameters["schwarz_overlap"]);

    // Make sure the data structures have been constructed
    KSPSetUp(*solver.ksp());

    // Get sub-solvers and set su- solver parameters
    KSP* sub_ksps;
    int num_local(0), first(0);
    PCASMGetSubKSP(pc, &num_local, &first, &sub_ksps);
    for (int i = 0; i < num_local; ++i)
    {
      PC sub_pc;
      KSPGetPC(sub_ksps[i], &sub_pc);
      //PCSetType(sub_pc, PCLU);
      //PCFactorSetMatSolverPackage(sub_pc, MAT_SOLVER_UMFPACK);
      //PCSetType(sub_pc, PCILU);
      //KSPSetType(sub_ksps[i], KSPGMRES);
      PCFactorSetLevels(sub_pc, parameters("ilu")["fill_level"]);
      //PCFactorSetLevels(sub_pc, 4);
      //PCView(sub_pc, PETSC_VIEWER_STDOUT_WORLD);
    }
    //KSPSetTolerances(sub_ksps[0], 1.0e-1,
		//                      parameters["absolute_tolerance"],
		//                      parameters["divergence_limit"],
		//                      100);
    //KSPMonitorSet(sub_ksps[0], KSPMonitorTrueResidualNorm, 0, 0);
  }
  else if (type != "default")
  {
    PCSetType(pc, methods.find(type)->second);
    #if PETSC_VERSION_MAJOR == 3 && PETSC_VERSION_MINOR == 1
    PCFactorSetShiftType(pc, MAT_SHIFT_NONZERO);
    PCFactorSetShiftAmount(pc, parameters["shift_nonzero"]);
    #else
    PCFactorSetShiftNonzero(pc, parameters["shift_nonzero"]);
    #endif
  }

  PCFactorSetLevels(pc, parameters("ilu")["fill_level"]);

  // Make sure options are set
  PCSetFromOptions(pc);
  PCView(pc, PETSC_VIEWER_STDOUT_WORLD);
}
//-----------------------------------------------------------------------------
std::string PETScPreconditioner::str(bool verbose) const
{
  std::stringstream s;
  if (verbose)
    warning("Verbose output for PETScPreconditioner not implemented.");
  else
    s << "<PETScPreconditioner>";

  return s.str();
}
//-----------------------------------------------------------------------------

#endif
