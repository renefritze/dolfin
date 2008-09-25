// Copyright (C) 2008 Kent-Andre Mardal.
// Licensed under the GNU LGPL Version 2.1.
//
// Last changed: 2008-05-16

#ifdef HAS_TRILINOS

#include "Epetra_ConfigDefs.h"
#include "Epetra_Vector.h"
#include "Epetra_FEVector.h"
#include "Epetra_RowMatrix.h"
#include "Epetra_CrsMatrix.h"
#include "Epetra_FECrsMatrix.h"
#include "Epetra_LinearProblem.h"
#include "Epetra_Map.h"


#include "AztecOO.h"

//#include "ml_config.h"
#include "ml_include.h"
#include "Epetra_LinearProblem.h"
#include "ml_MultiLevelOperator.h"
#include "ml_epetra_utils.h"

#include <dolfin/log/dolfin_log.h>
#include "GenericMatrix.h"
#include "GenericVector.h"
#include "EpetraKrylovSolver.h"
#include "EpetraMatrix.h"
#include "EpetraVector.h"





using namespace dolfin; 
//-----------------------------------------------------------------------------
EpetraKrylovSolver::EpetraKrylovSolver(SolverType method, PreconditionerType pc) 
                    : method(method), pc_type(pc), prec(0) 
{ 
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraKrylovSolver::EpetraKrylovSolver(SolverType method, EpetraPreconditioner& prec) 
                    : method(method), pc_type(default_pc), prec(&prec) 
{ 
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraKrylovSolver::~EpetraKrylovSolver() 
{
  // Do nothing
}
//-----------------------------------------------------------------------------
dolfin::uint EpetraKrylovSolver::solve(const GenericMatrix& A, GenericVector& x, 
                                       const GenericVector& b) 
{
  return  solve(A.down_cast<EpetraMatrix>(), x.down_cast<EpetraVector>(), 
                b.down_cast<EpetraVector>());
}
//-----------------------------------------------------------------------------
dolfin::uint EpetraKrylovSolver::solve(const EpetraMatrix& A, EpetraVector& x, 
                                       const EpetraVector& b) 
{
  //FIXME need the ifdef AztecOO 


  // FIXME: check vector size
  // FIXME: permit initial guess

  // cast matrix and vectors to proper type
  Epetra_RowMatrix* row_matrix =  dynamic_cast<Epetra_RowMatrix*>(&(A.mat()));
  Epetra_MultiVector* x_vec = dynamic_cast<Epetra_MultiVector*>(&x.vec());
  Epetra_MultiVector* b_vec = dynamic_cast<Epetra_MultiVector*>(&b.vec());

  /*
  // Create linear system 
  Epetra_LinearProblem linear_system; 
  linear_system.SetOperator(row_matrix); 
  linear_system.SetLHS(x_vec); 
  linear_system.SetRHS(b_vec); 
  AztecOO linear_solver(linear_system);;
  */

  // Create solver 
  AztecOO linear_solver;
  linear_solver.SetUserMatrix(row_matrix);
  linear_solver.SetLHS(x_vec); 
  linear_solver.SetRHS(b_vec); 

  if ( method == cg) 
    linear_solver.SetAztecOption( AZ_solver, AZ_cg);
  else if ( method == gmres) 
    linear_solver.SetAztecOption( AZ_solver, AZ_gmres);
  else if ( method == bicgstab) 
    linear_solver.SetAztecOption( AZ_solver, AZ_bicgstab);
  else if ( method == lu ) 
    error("EpetraKrylovSolver::solve LU not supported."); 
  else
    error("EpetraKrylovSolver::solve solver type not supported."); 

  //FIXME GS or SSOR not a PreconditionerType not in 
  if ( pc_type == jacobi) 
    linear_solver.SetAztecOption( AZ_precond, AZ_Jacobi);
  else if ( pc_type == sor) 
    linear_solver.SetAztecOption( AZ_precond, AZ_sym_GS);
  else if ( pc_type == ilu) 
  {
    linear_solver.SetAztecOption( AZ_precond, AZ_dom_decomp);
    linear_solver.SetAztecOption( AZ_subdomain_solve, AZ_ilu); 
  }
  else if ( pc_type == icc) 
    linear_solver.SetAztecOption( AZ_precond, AZ_icc);
  else if ( pc_type == amg) 
    ;// Do nothing. Confiugured below    
  else
    error("EpetraKrylovSolver::solve pc type not supported."); 

  if (pc_type == amg) 
  {  
//#ifdef HAVE_ML_AZTECOO
    //FIXME ifdef ML 
    // Code from trilinos-8.0.3/packages/didasko/examples/ml/ex1.cpp 

    // Create and set an ML multilevel preconditioner
    ML *ml_handle;

    // Maximum number of levels
    int N_levels = 10;

    // output level
    ML_Set_PrintLevel(0);

    ML_Create(&ml_handle,N_levels);

    // wrap Epetra Matrix into ML matrix (data is NOT copied)
    EpetraMatrix2MLMatrix(ml_handle, 0, &(A.mat()));

    // create a ML_Aggregate object to store the aggregates
    ML_Aggregate *agg_object;
    ML_Aggregate_Create(&agg_object);

    // specify max coarse size 
    ML_Aggregate_Set_MaxCoarseSize(agg_object,1);

    // generate the hierady
    N_levels = ML_Gen_MGHierarchy_UsingAggregation(ml_handle, 0, ML_INCREASING, agg_object);

    // Set a symmetric Gauss-Seidel smoother for the MG method 
    ML_Gen_Smoother_SymGaussSeidel(ml_handle, ML_ALL_LEVELS, ML_BOTH, 1, ML_DEFAULT);

    // generate solver
    ML_Gen_Solver(ml_handle, ML_MGV, 0, N_levels-1);

    // wrap ML_Operator into Epetra_Operator
    ML_Epetra::MultiLevelOperator  MLop(ml_handle,A.mat().Comm(),A.mat().DomainMap(),A.mat().RangeMap());

    // set this operator as preconditioner for AztecOO
    linear_solver.SetPrecOperator(&MLop);

//#else 
//    error("EpetraKrylovSolver::solve not compiled with ML support."); 
//#endif 
  }   
  std::cout <<"starting to iterate "<<std::endl; 
  linear_solver.Iterate(1000, 1.0e-9);
  return linear_solver.NumIters(); 
}
//-----------------------------------------------------------------------------
void EpetraKrylovSolver::disp() const 
{
  error("EpetraKrylovSolver::disp not implemented"); 
}
//-----------------------------------------------------------------------------
#endif 

