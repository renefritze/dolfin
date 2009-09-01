// Copyright (C) 2008-2009 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Dag Lindbo, 2008
//
// First added:  2008-05-16
// Last changed: 2009-08-10

#ifdef HAS_MTL4

// Order of header files is important
#include <dolfin/log/dolfin_log.h>
#include "ITLKrylovSolver.h"
#include "MTL4Matrix.h"
#include "MTL4Vector.h"
#include <boost/numeric/itl/itl.hpp>
#include "KrylovSolver.h"

using namespace dolfin;
using namespace itl;
using namespace mtl;

//-----------------------------------------------------------------------------
Parameters ITLKrylovSolver::default_parameters()
{
  Parameters p(KrylovSolver::default_parameters());
  p.rename("mtl4_krylov_solver");
  return p;
}
//-----------------------------------------------------------------------------
ITLKrylovSolver::ITLKrylovSolver(std::string method, std::string pc_type)
                               : method(method), pc_type(pc_type)
{
  // Set parameter values
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
ITLKrylovSolver::~ITLKrylovSolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
dolfin::uint ITLKrylovSolver::solve(const GenericMatrix& A, GenericVector& x,
                                    const GenericVector& b)
{
  return solve(A.down_cast<MTL4Matrix>(), x.down_cast<MTL4Vector>(),
               b.down_cast<MTL4Vector>());
}
//-----------------------------------------------------------------------------
dolfin::uint ITLKrylovSolver::solve(const MTL4Matrix& A, MTL4Vector& x,
                                    const MTL4Vector& b)
{
  // Fall back in default method if unknown
  if(method != "cg" && method != "bicgstab" && method != "default")
  {
    warning("Requested ITL Krylov method unknown. Using BiCGStab.");
    method = "bicgstab";
  }
  // Fall back in default preconditioner if unknown
  if(pc_type != "ilu" && pc_type != "icc" && pc_type != "none" && pc_type != "default")
  {
    warning("Requested ITL preconditioner unknown. Using ILU.");
    pc_type = "ilu";
  }

  // Set convergence criteria
  itl::basic_iteration<double> iter(b.vec(), parameters("maximum_iterations"),
                                             parameters("relative_tolerance"));

  // Check vector size
  if( x.size() != b.size() )
  {
    x.resize(b.size());
    x.zero();
  }

  // Solve
  int errno_ = 0;

  // Developers note: the following code is not very elegant.
  // The problem is that ITL are all templates, but DOLFIN selects
  // solvers at runtime. All solvers and preconditioners are instantiated.
  if(pc_type == "ilu" || pc_type == "default")
  {
    itl::pc::ilu_0<mtl4_sparse_matrix> P(A.mat());
    if(method == "cg")
      errno_ = itl::cg(A.mat(), x.vec(), b.vec(), P, iter);
    else if(method == "bicgstab" || method == "default")
      errno_ = itl::bicgstab(A.mat(), x.vec(), b.vec(), P, iter);
  }
  else if(pc_type == "icc")
  {
    itl::pc::ic_0<mtl4_sparse_matrix> P(A.mat());
    if(method == "cg")
      errno_ = itl::cg(A.mat(), x.vec(), b.vec(), P, iter);
    else if(method == "bicgstab" || method == "default")
      errno_ = itl::bicgstab(A.mat(), x.vec(), b.vec(), P, iter);
  }
  else if(pc_type == "none")
  {
    itl::pc::identity<mtl4_sparse_matrix> P(A.mat());
    if(method == "cg")
      errno_ = itl::cg(A.mat(), x.vec(), b.vec(), P, iter);
    else if(method == "bicgstab" || method == "default")
      errno_ = itl::bicgstab(A.mat(), x.vec(), b.vec(), P, iter);
  }

  // Check exit condition
  if(errno_ == 0)
    info("ITLSolver (%s, %s) converged in %d iterations. Resid=%8.2e",
	    method.c_str(), pc_type.c_str(), iter.iterations(), iter.resid());
  else
    warning("ITLKrylovSolver: (%s, %s) failed to converge!\n\t%d iterations,"
	  " Resid=%8.2e", method.c_str(), pc_type.c_str(), iter.iterations(), iter.resid());

  return iter.iterations();
}
//-----------------------------------------------------------------------------
std::string ITLKrylovSolver::str(bool verbose) const
{
  dolfin_not_implemented();
  return std::string();
}
//-----------------------------------------------------------------------------

#endif


