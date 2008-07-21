// Copyright (C) 2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-05-16
// Last changed: 2008-07-21

#ifdef HAS_MTL4

// Order of header files is important
#include <dolfin/log/dolfin_log.h>
#include "ITLKrylovSolver.h"
#include "MTL4Matrix.h"
#include "MTL4Vector.h"
#include <boost/numeric/itl/itl.hpp>

using namespace dolfin; 
using namespace itl;
using namespace mtl;

//-----------------------------------------------------------------------------
ITLKrylovSolver::ITLKrylovSolver(SolverType method_, PreconditionerType pc_)
{ 
  // Do nothing
}
//-----------------------------------------------------------------------------
//ITLKrylovSolver::ITLKrylovSolver(SolverType method_, EpetraPreconditioner& prec_)
//{ 
//  // Do nothing
//}
//-----------------------------------------------------------------------------
ITLKrylovSolver::~ITLKrylovSolver() 
{
  // Do nothing
}
//-----------------------------------------------------------------------------
dolfin::uint ITLKrylovSolver::solve(const MTL4Matrix& A, MTL4Vector& x, const MTL4Vector& b)
{
  warning("ITLKrylovSolver is experimental.");

  // Create preconditioner
  itl::pc::ilu_0<mtl4_sparse_matrix> P(A.mat());

  // Set convergence criteria
  itl::noisy_iteration<double> iter(b.vec(), 500, 1.e-6);

  // Solve
  x.init(b.size());
  x.vec() = 0.0;
  int iterations = itl::bicgstab(A.mat(), x.vec(), b.vec(), P, iter);
  cout << "Testing " << iterations << endl;
  //return iterations; 

  return 0; 
}
//-----------------------------------------------------------------------------
void ITLKrylovSolver::disp() const 
{
  error("ITLKrylovSolver::disp not implemented"); 
}
//-----------------------------------------------------------------------------
#endif 


