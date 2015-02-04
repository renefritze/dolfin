// Copyright (C) 2015 Chris Richardson
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
// First added:  2015-02-03

#include <dolfin/common/NoDeleter.h>
#include <dolfin/common/Timer.h>
#include <dolfin/parameter/GlobalParameters.h>

// #define EIGEN_CHOLMOD_SUPPORT 1
// #define EIGEN_UMFPACK_SUPPORT 1

#include <Eigen/SparseLU>
#ifdef EIGEN_CHOLMOD_SUPPORT
// Works with Cholmod downloaded by PETSc
#include <Eigen/CholmodSupport>
#endif
#ifdef EIGEN_UMFPACK_SUPPORT
// Works with Suitesparse downloaded by PETSc
#include <Eigen/UmfPackSupport>
#endif
#ifdef EIGEN_PARDISO_SUPPORT
// Requires Intel MKL
#include <Eigen/PardisoSupport>
#endif
#ifdef EIGEN_SUPERLU_SUPPORT
// SuperLU bundled with PETSc is not compatible
#include <Eigen/SuperLUSupport>
#endif
#ifdef EIGEN_PASTIX_SUPPORT
// Seems to require COMPLEX support
#include <Eigen/PaStiXSupport>
#endif

#include "LUSolver.h"
#include "EigenMatrix.h"
#include "EigenVector.h"
#include "EigenLUSolver.h"

using namespace dolfin;

// List of available LU solvers
const std::vector<std::pair<std::string, std::string> >
EigenLUSolver::_methods_descr
= { {"default", "default LU solver"},
    {"sparselu", "Supernodal LU factorization for general matrices"},
    {"cholesky", "Simplicial LDLT"},
#ifdef EIGEN_CHOLMOD_SUPPORT
    {"cholmod", "'CHOLMOD' sparse Cholesky factorisation"},
#endif
#ifdef EIGEN_UMFPACK_SUPPORT
    {"umfpack", "UMFPACK (Unsymmetric MultiFrontal sparse LU factorization)"},
#endif
#ifdef EIGEN_PARDISO_SUPPORT
    {"pardiso", "Intel MKL Pardiso"},
#endif
#ifdef EIGEN_SUPERLU_SUPPORT
    {"superlu", "SuperLU"},
#endif
#ifdef EIGEN_PASTIX_SUPPORT
    {"pastix", "PaStiX (Parallel Sparse matriX package)"}
#endif
};
//-----------------------------------------------------------------------------
std::vector<std::pair<std::string, std::string> >
EigenLUSolver::methods()
{
  return EigenLUSolver::_methods_descr;
}
//-----------------------------------------------------------------------------
Parameters EigenLUSolver::default_parameters()
{
  Parameters p(LUSolver::default_parameters());
  p.rename("eigen_lu_solver");

  return p;
}
//-----------------------------------------------------------------------------
EigenLUSolver::EigenLUSolver(std::string method)
{
  // Set parameter values
  parameters = default_parameters();

  _method = select_solver(method);
}
//-----------------------------------------------------------------------------
EigenLUSolver::EigenLUSolver(std::shared_ptr<const EigenMatrix> A,
                             std::string method) : _matA(A)
{
  // Check dimensions
  if (A->size(0) != A->size(1))
  {
    dolfin_error("EigenLUSolver.cpp",
                 "create Eigen LU solver",
                 "Cannot LU factorize non-square EigenMatrix");
  }

  // Set parameter values
  parameters = default_parameters();

  _method = select_solver(method);
}
//-----------------------------------------------------------------------------
EigenLUSolver::~EigenLUSolver()
{
}
//-----------------------------------------------------------------------------
void
EigenLUSolver::set_operator(std::shared_ptr<const GenericLinearOperator> A)
{
  // Attempt to cast as EigenMatrix
  std::shared_ptr<const EigenMatrix> mat
    = as_type<const EigenMatrix>(require_matrix(A));
  dolfin_assert(mat);

  // Set operator
  set_operator(mat);
}
//-----------------------------------------------------------------------------
void EigenLUSolver::set_operator(std::shared_ptr<const EigenMatrix> A)
{
  _matA = A;
  dolfin_assert(_matA);
  dolfin_assert(!_matA->empty());
}
//-----------------------------------------------------------------------------
const GenericLinearOperator& EigenLUSolver::get_operator() const
{
  if (!_matA)
  {
    dolfin_error("EigenLUSolver.cpp",
                 "access operator of Eigen LU solver",
                 "Operator has not been set");
  }
  return *_matA;
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve(GenericVector& x, const GenericVector& b)
{
  return solve(x, b, false);
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve(GenericVector& x,
                                 const GenericVector& b,
                                 bool transpose)
{
  if (_method == "sparselu")
  {
    Eigen::SparseLU<Eigen::SparseMatrix<double, Eigen::ColMajor>,
                    Eigen::COLAMDOrdering<int> > solver;
    call_solver(solver, x, b, transpose);
  }
  else if (_method == "cholesky")
  {
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double, Eigen::ColMajor>,
                          Eigen::Lower> solver;
    call_solver(solver, x, b, transpose);
  }
#ifdef EIGEN_CHOLMOD_SUPPORT
  else if (_method == "cholmod")
  {
    Eigen::CholmodDecomposition<Eigen::SparseMatrix<double, Eigen::ColMajor>,
                                Eigen::Lower> solver;
    solver.setMode(Eigen::CholmodLDLt);
    call_solver(solver, x, b, transpose);
  }
#endif
#ifdef EIGEN_PASTIX_SUPPORT
  else if (_method == "pastix")
  {
    Eigen::PastixLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
    call_solver(solver, x, b, transpose);
  }
#endif
#ifdef EIGEN_PARDISO_SUPPORT
  else if (_method == "pardiso")
  {
    Eigen::PardisoLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
    call_solver(solver, x, b, transpose);
  }
#endif
#ifdef EIGEN_SUPERLU_SUPPORT
  else if (_method == "superlu")
  {
    Eigen::SuperLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
    call_solver(solver, x, b, transpose);
  }
#endif
#ifdef EIGEN_UMFPACK_SUPPORT
  else if (_method == "umfpack")
  {
    Eigen::UmfPackLU<Eigen::SparseMatrix<double, Eigen::ColMajor> > solver;
    call_solver(solver, x, b, transpose);
  }
#endif
  else
    dolfin_error("EigenLUSolver.cpp", "solve A.x =b",
                 "Unknown method \"%s\"", _method.c_str());

  return 1;
}
//-----------------------------------------------------------------------------
template <typename Solver>
std::size_t EigenLUSolver::call_solver(Solver& solver,
                                      GenericVector& x,
                                      const GenericVector& b,
                                      bool transpose)
{
  std::string timer_title = "Eigen LU solver (" + _method + ")";
  Timer timer(timer_title);

  dolfin_assert(_matA);

  // Downcast matrix and vectors
  const EigenVector& _b = as_type<const EigenVector>(b);
  EigenVector& _x = as_type<EigenVector>(x);

  // Check dimensions
  if (_matA->size(0) != b.size())
  {
    dolfin_error("EigenLUSolver.cpp",
                 "solve linear system using Eigen LU solver",
                 "Cannot factorize non-square Eigen matrix");
  }

  // Initialize solution vector if required
  if (x.empty())
    _matA->init_vector(x, 1);

  // Copy to format suitable for solver
  // FIXME: remove this if possible
  // Eigen wants ColMajor matrices for solver

  typename Solver::MatrixType _A;
  if (transpose)
    _A = _matA->mat().transpose();
  else
    _A = _matA->mat();

  _A.makeCompressed();

  //  solver.analyzePattern(_A);
  //  solver.factorize(_A);

  solver.compute(_A);

  if (solver.info() != Eigen::Success)
  {
    dolfin_error("EigenLUSolver.cpp",
                 "compute matrix factorisation",
                 "The provided data did not satisfy the prerequisites");
  }

  // Solve linear system
  _x.vec() = solver.solve(_b.vec());
  if (solver.info() != Eigen::Success)
  {
    dolfin_error("EigenLUSolver.cpp",
                 "solve A.x = b",
                 "Solver failed");
  }

  return 1;
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve(const GenericLinearOperator& A,
                                 GenericVector& x,
                                 const GenericVector& b)
{
  return solve(as_type<const EigenMatrix>(require_matrix(A)),
               as_type<EigenVector>(x),
               as_type<const EigenVector>(b));
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve(const EigenMatrix& A, EigenVector& x,
                                 const EigenVector& b)
{
  std::shared_ptr<const EigenMatrix> Atmp(&A, NoDeleter());
  set_operator(Atmp);
  return solve(x, b);
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve_transpose(GenericVector& x,
                                           const GenericVector& b)
{
return solve(x, b, true);
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve_transpose(const GenericLinearOperator& A,
                                           GenericVector& x,
                                           const GenericVector& b)
{
  return solve_transpose(as_type<const EigenMatrix>(require_matrix(A)),
                         as_type<EigenVector>(x),
                         as_type<const EigenVector>(b));
}
//-----------------------------------------------------------------------------
std::size_t EigenLUSolver::solve_transpose(const EigenMatrix& A,
                                           EigenVector& x,
                                           const EigenVector& b)
{
  std::shared_ptr<const EigenMatrix> _matA(&A, NoDeleter());
  set_operator(_matA);
  return solve_transpose(x, b);
}
//-----------------------------------------------------------------------------
std::string EigenLUSolver::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << "Eigen LUSolver\n";
  }
  else
    s << "<EigenLUSolver>";

  return s.str();
}
//-----------------------------------------------------------------------------
const std::string EigenLUSolver::select_solver(std::string& method) const
{
  // Choose appropriate 'default' solver
  if (method == "default")
    method = "sparselu";

  // Check package string
  for (auto &m : _methods_descr)
  {
    if (m.first == method)
      return method;
  }

  dolfin_error("EigenLUSolver.cpp",
               "solve linear system using Eigen LU solver",
               "Unknown LU method \"%s\"", method.c_str());

  // Never reach here
  return method;
}
//-----------------------------------------------------------------------------
