// Copyright (C) 2010-2013 Garth N. Wells
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
// Modified by Anders Logg 2011-2012
//
// First added:  2010-02-25
// Last changed: 2013-06-25

#ifdef HAS_TRILINOS

#include <dolfin/common/MPI.h>
#include <boost/assign/list_of.hpp>

#include <AztecOO.h>
#include <Epetra_CombineMode.h>
#include <Epetra_FECrsMatrix.h>
#include <Epetra_FEVector.h>
#include <Epetra_RowMatrix.h>
#include <Epetra_MultiVector.h>
#include <Epetra_Vector.h>
#include <Ifpack.h>
#include <ml_include.h>
#include <ml_epetra_utils.h>
#include <ml_MultiLevelPreconditioner.h>
#include <Teuchos_ParameterList.hpp>

#include <dolfin/log/log.h>
#include "EpetraKrylovSolver.h"
#include "EpetraMatrix.h"
#include "EpetraVector.h"
#include "GenericVector.h"
#include "KrylovSolver.h"
#include "VectorSpaceBasis.h"
#include "TrilinosPreconditioner.h"

using namespace dolfin;

// Mapping from preconditioner string to Trilinos
const std::map<std::string, int> TrilinosPreconditioner::_preconditioners
  = boost::assign::map_list_of("default",   AZ_ilu)
                              ("none",      AZ_none)
                              ("ilu",       AZ_ilu)
                              ("icc",       AZ_icc)
                              ("jacobi",    AZ_Jacobi)
                              ("sor",       AZ_sym_GS)
                              ("amg",       -1)
                              ("ml_amg",    -1);

// Mapping from preconditioner string to Trilinos
const std::vector<std::pair<std::string, std::string> >
TrilinosPreconditioner::_preconditioners_descr
  = boost::assign::pair_list_of
    ("default",   "default preconditioner")
    ("none",      "No preconditioner")
    ("ilu",       "Incomplete LU factorization")
    ("icc",       "Incomplete Cholesky factorization")
    ("jacobi",    "Jacobi iteration")
    ("sor",       "Successive over-relaxation")
    ("amg",       "Algebraic multigrid")
    ("ml_amg",    "ML algebraic multigrid");

//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> >
TrilinosPreconditioner::preconditioners()
{
  return TrilinosPreconditioner::_preconditioners_descr;
}
//-----------------------------------------------------------------------------
Parameters TrilinosPreconditioner::default_parameters()
{
  Parameters p(KrylovSolver::default_parameters()("preconditioner"));
  p.rename("trilinos_preconditioner");

  // Add some extra Trilinos-specific Schwarz options
  //    Options are Zero, Insert, Add, Average, AbsMax
  p("schwarz").add("mode", "Zero");
  //    Options are rcm, metis, amd
  p("schwarz").add("reordering_type", "rcm");

  // ML options
  Parameters p_ml("ml");
  p_ml.add("output_level", 0);
  p_ml.add("max_levels", 10);

  // Add nested parameters sets
  p.add(p_ml);

  return p;
}
//-----------------------------------------------------------------------------
TrilinosPreconditioner::TrilinosPreconditioner(std::string preconditioner)
  : _preconditioner(preconditioner)
{
  // Set parameter values
  parameters = default_parameters();

  // Check that the requested method is known
  if (_preconditioners.count(preconditioner) == 0)
  {
    dolfin_error("TrilinosPreconditioner.cpp",
                 "create Trilinos preconditioner",
                 "Unknown preconditioner (\"%s\")", preconditioner.c_str());
  }
}
//-----------------------------------------------------------------------------
TrilinosPreconditioner::~TrilinosPreconditioner()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void TrilinosPreconditioner::set(EpetraKrylovSolver& solver,
                                 const EpetraMatrix& P)
{
  // Pointer to preconditioner matrix
  Epetra_RowMatrix* _P = P.mat().get();
  dolfin_assert(_P);

  // Get underlying solver object
  dolfin_assert(solver.aztecoo());
  AztecOO& _solver = *(solver.aztecoo());

  // Set preconditioner
  if (_preconditioner == "default" || _preconditioner == "ilu"
      || _preconditioner == "icc")
  {
    // Get/set some parameters
    const int ilu_fill_level       = parameters("ilu")["fill_level"];
    const int overlap              = parameters("schwarz")["overlap"];
    const std::string reordering   = parameters("schwarz")["reordering_type"];
    const std::string schwarz_mode = parameters("schwarz")["mode"];
    Teuchos::ParameterList plist;
    plist.set("fact: level-of-fill",      ilu_fill_level);
    plist.set("schwarz: combine mode",    schwarz_mode);
    plist.set("schwarz: reordering type", reordering);

    // Create preconditioner
    if (_preconditioner == "icc")
      _preconditioner = "IC";
    else
      _preconditioner = "ILU";
    Ifpack ifpack_factory;
    ifpack_preconditioner.reset(ifpack_factory.Create(_preconditioner, _P,
                                                      overlap));
    dolfin_assert(ifpack_preconditioner != 0);

    // Set any user-provided parameters
    if (parameter_list)
      plist.setParameters(*parameter_list);

    // Set up preconditioner
    ifpack_preconditioner->SetParameters(plist);
    ifpack_preconditioner->Initialize();
    ifpack_preconditioner->Compute();
    _solver.SetPrecOperator(ifpack_preconditioner.get());
  }
  else if (_preconditioner == "hypre_amg")
  {
    info("Hypre AMG not available for Trilinos. Using ML instead.");
    set_ml(_solver, *_P);
  }
  else if (_preconditioner == "ml_amg" || _preconditioner == "amg")
    set_ml(_solver, *_P);
  else
  {
    _solver.SetAztecOption(AZ_precond,
                           _preconditioners.find(_preconditioner)->second);
    _solver.SetPrecMatrix(_P);
  }
}
//-----------------------------------------------------------------------------
void TrilinosPreconditioner::set_parameters(boost::shared_ptr<const Teuchos::ParameterList> list)
{
  parameter_list = list;
}
//-----------------------------------------------------------------------------
void TrilinosPreconditioner::set_parameters(Teuchos::RCP<Teuchos::ParameterList> list)
{
  parameter_list = boost::shared_ptr<const Teuchos::ParameterList>(reference_to_no_delete_pointer(*list.get()));
  parameter_ref_keeper = list;
}
//-----------------------------------------------------------------------------
void TrilinosPreconditioner::set_nullspace(const VectorSpaceBasis& nullspace)
{
  // Clear nullspace
  _nullspace.reset();

  for (std::size_t i = 0; i < nullspace.dim(); ++i)
  {
    dolfin_assert(nullspace[i]);

    // Get Epetra vector
    const EpetraVector& v = as_type<const EpetraVector>(*nullspace[i]);
    dolfin_assert(v.vec());

      // Initialise null space multivector on first pass
    if (i == 0)
    {
      _nullspace.reset(new Epetra_MultiVector(v.vec()->Map(),
                                              nullspace.dim()));
    }

    // Copy data into Epetra_MultiVector object
    const Epetra_Vector& _v = *(*(v.vec()))(0);
    *(*_nullspace)(i) = _v;
  }
}
//-----------------------------------------------------------------------------
std::string TrilinosPreconditioner::name() const
{
  return _preconditioner;
}
//-----------------------------------------------------------------------------
std::string TrilinosPreconditioner::str(bool verbose) const
{
  std::stringstream s;
  if (verbose)
    warning("Verbose output for TrilinosPreconditioner not implemented.");
  else
    s << "<TrilinosPreconditioner>";
  return s.str();
}
//-----------------------------------------------------------------------------
void TrilinosPreconditioner::set_ml(AztecOO& solver, const Epetra_RowMatrix& P)
{

  Teuchos::ParameterList mlist;
  ML_Epetra::SetDefaults("SA", mlist);

  //ML_Epetra::SetDefaults("DD", mlist);
  //mlist.set("increasing or decreasing", "decreasing");
  //mlist.set("aggregation: type", "ParMETIS");
  //mlist.set("coarse: max size", 1024);

  //mlist.set("aggregation: nodes per aggregate", 4);
  //mlist.set("coarse: type","Amesos-KLU");
  //mlist.set("coarse: type", "Amesos-UMFPACK");

  // Set maximum numer of level
  //const int max_levels = parameters("ml")["max_levels"];
  //mlist.set("max levels", max_levels);

  // Set output level
  //const int output_level = parameters("ml")["output_level"];
  //mlist.set("ML output", output_level);

  /*
  mlist.set("smoother: type", "Chebyshev");
  mlist.set("smoother: sweeps", 2);
  mlist.set("smoother: pre or post", "both");
  mlist.set("coarse: type","Chebyshev");
  mlist.set("coarse: sweeps", 2);
  mlist.set("coarse: pre or post", "both");
  */

  // Set any user-provided parameters
  if (parameter_list)
    mlist.setParameters(*parameter_list);

  // Set null space
  if (_nullspace)
  {
    mlist.set("null space: add default vectors", false);
    mlist.set("null space: type", "pre-computed");
    mlist.set("null space: dimension", _nullspace->NumVectors());
    mlist.set("null space: vectors", _nullspace->Values());
  }

  // Create preconditioner
  ml_preconditioner.reset(new ML_Epetra::MultiLevelPreconditioner(P, mlist,
                                                                  true));

  // Set this operator as preconditioner for AztecOO
  solver.SetPrecOperator(ml_preconditioner.get());
}
//-----------------------------------------------------------------------------

#endif
