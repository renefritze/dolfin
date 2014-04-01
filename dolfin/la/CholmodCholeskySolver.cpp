// Copyright (C) 2008-2011 Dag Lindbo and Garth N. Wells
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
// Modified by Anders Logg 2012
//
// First added:  2008-08-15
// Last changed: 2012-08-21

#include <cstring>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/la/GenericLinearOperator.h>
#include <dolfin/la/GenericMatrix.h>
#include <dolfin/la/GenericVector.h>
#include <dolfin/log/dolfin_log.h>
#include "LUSolver.h"
#include "UmfpackLUSolver.h"
#include "CholmodCholeskySolver.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
Parameters CholmodCholeskySolver::default_parameters()
{
  Parameters p(LUSolver::default_parameters());
  p.rename("cholmod_lu_solver");
  return p;
}
//-----------------------------------------------------------------------------
CholmodCholeskySolver::CholmodCholeskySolver()
{
  // Set parameter values
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
CholmodCholeskySolver::CholmodCholeskySolver(std::shared_ptr<const GenericLinearOperator> A)
  : _matA(A)
{
  // Set parameter values
  parameters = default_parameters();
}
//-----------------------------------------------------------------------------
CholmodCholeskySolver::~CholmodCholeskySolver()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
//=============================================================================
#ifdef HAS_CHOLMOD
std::size_t CholmodCholeskySolver::solve(const GenericLinearOperator& A,
                                         GenericVector& x,
                                         const GenericVector& b)
{
  // Factorize matrix
  factorize(A);

  // Solve system
  factorized_solve(x, b);

  // Clear data
  cholmod.clear();

  return 1;
}
//-----------------------------------------------------------------------------
std::size_t CholmodCholeskySolver::factorize(const GenericLinearOperator& A)
{
  // Need matrix data
  const GenericMatrix& _matA = require_matrix(A);

  // Check dimensions and get number of non-zeroes
  boost::tuples::tuple<const std::size_t*,
                       const std::size_t*,
                       const double*, int> data = _matA.data();
  const std::size_t M = _matA.size(0);
  const std::size_t nnz = boost::tuples::get<3>(data);
  dolfin_assert(_matA.size(0) == _matA.size(1));

  dolfin_assert(nnz >= M);

  // Initialise cholmod data
  // NOTE: Casting away const here
  cholmod.init((UF_long*) boost::tuples::get<0>(data),
               (UF_long*) boost::tuples::get<1>(data),
               (double*) boost::tuples::get<2>(data), M, nnz);

  // Factorize
  log(PROGRESS, "Cholesky-factorizing linear system of size %d x %d (CHOLMOD).",
      M, M);
  cholmod.factorize();

  return 1;
}
//-----------------------------------------------------------------------------
std::size_t CholmodCholeskySolver::factorized_solve(GenericVector& x,
                                                    const GenericVector& b)
{
  const std::size_t N = b.size();

  if (!cholmod.factorized)
  {
    dolfin_error("CholmodCholeskySolver.cpp",
                 "perform factorized solve using CHOLMOD Cholesky solver",
                 "Factorized solve no preceded by call to factorize()");
  }

  if (N != cholmod.N)
  {
    dolfin_error("CholmodCholeskySolver.cpp",
                 "perform factorized solve using CHOLMOD Cholesky solver",
                 "Vector does not match size of factored matrix");
  }

  // Initialise solution vector and solve
  x.resize(b.mpi_comm(), b.local_range());

  log(PROGRESS, "Solving factorized linear system of size %d x %d (CHOLMOD).",
      N, N);

  cholmod.factorized_solve(x.data(), b.data());

  return 1;
}
//-----------------------------------------------------------------------------
#else
// ============================================================================
std::size_t CholmodCholeskySolver::solve(const GenericLinearOperator& A,
                                         GenericVector& x,
                                          const GenericVector& b)
{
  warning("CHOLMOD must be installed to peform a Cholesky solve for the current backend. Attemping to use UMFPACK solver.");

  std::shared_ptr<const GenericLinearOperator> Atmp(&A, NoDeleter());
  UmfpackLUSolver solver(Atmp);
  return solver.solve(x, b);
}
//-----------------------------------------------------------------------------
std::size_t CholmodCholeskySolver::factorize(const GenericLinearOperator& A)
{
  dolfin_error("CholdmodCholeskySolver.cpp",
               "perform factorization using CHOLMOD Cholesky solver",
               "CHOLMOD is not installed");
  return 0;
}
//-----------------------------------------------------------------------------
std::size_t CholmodCholeskySolver::factorized_solve(GenericVector& x,
                                                    const GenericVector& b)
{
  dolfin_error("CholdmodCholeskySolver.cpp",
               "perform factorized solve using CHOLMOD Cholesky solver",
               "CHOLMOD is not installed");
  return 0;
}
#endif

//==============================================================================
// CholmodCholeskySolver::Cholmod implementation
#ifdef HAS_CHOLMOD
//==============================================================================
CholmodCholeskySolver::Cholmod::Cholmod()
  : N(0), factorized(false), A_chol(0), L_chol(0)
{
  // "Start" cholmod
  cholmod_l_start(&c);
}
//-----------------------------------------------------------------------------
CholmodCholeskySolver::Cholmod::~Cholmod()
{
  clear();

  // "stop" cholmod
  cholmod_l_finish(&c);
}
//-----------------------------------------------------------------------------
void CholmodCholeskySolver::Cholmod::clear()
{
  if (A_chol)
  {
    cholmod_l_free(1, sizeof(cholmod_sparse), A_chol, &c);
    A_chol = 0;
  }
  if (L_chol)
  {
    cholmod_l_free_factor(&L_chol, &c);
    L_chol = 0;
  }
}
//-----------------------------------------------------------------------------
void CholmodCholeskySolver::Cholmod::init(UF_long* Ap, UF_long* Ai, double* Ax,
                                          std::size_t M, std::size_t nz)
{
  if (factorized)
    log(DBG, "CholeskySolver already contains a factorized matrix! Clearing and starting over.");

  // Clear any data
  clear();

  A_chol = (cholmod_sparse*) cholmod_l_malloc(1, sizeof(cholmod_sparse), &c);

  // The matrix
  A_chol->p = Ap;
  A_chol->i = Ai;
  A_chol->x = Ax;

  A_chol->nzmax = nz;
  A_chol->ncol = M;
  A_chol->nrow = M;

  A_chol->sorted = 1;
  A_chol->packed = 1;

  A_chol->xtype = CHOLMOD_REAL;
  A_chol->stype = -1;
  A_chol->dtype = CHOLMOD_DOUBLE;
  A_chol->itype = CHOLMOD_LONG;

  N = M;
}
//-----------------------------------------------------------------------------
void CholmodCholeskySolver::Cholmod::factorize()
{
  // Analyze
  L_chol = cholmod_l_analyze(A_chol, &c);

  // Factorize
  cholmod_l_factorize(A_chol, L_chol, &c);

  check_status("factorize()");

  factorized = true;
}
//-----------------------------------------------------------------------------
void CholmodCholeskySolver::Cholmod::factorized_solve(double*x,
                                                      const double* b)
{
  cholmod_dense *x_chol, *b_chol;

  // initialize rhs
  b_chol = (cholmod_dense*) cholmod_l_malloc(1, sizeof(cholmod_dense), &c);
  b_chol->x = (double*) b;
  b_chol->nrow = N;
  b_chol->ncol = 1;
  b_chol->d = N;
  b_chol->nzmax = N;
  b_chol->xtype = A_chol->xtype;
  b_chol->dtype = A_chol->dtype;

  // Solve
  x_chol = cholmod_l_solve(CHOLMOD_A, L_chol, b_chol, &c);

  // Compute residual and residual norm
  cholmod_dense* r_chol = residual(x_chol,b_chol);
  double residn = residual_norm(r_chol, x_chol, b_chol);

  // Iterative refinement
  if (residn > 1.0e-14)
  {
    refine_once(x_chol, r_chol);

    cholmod_l_free_dense(&r_chol, &c);

    r_chol = residual(x_chol, b_chol);
    residn = residual_norm(r_chol, x_chol,b_chol);
  }

  // Solution vector
  // FIXME: Cholmod allocates its own solution vector.
  memcpy(x, x_chol->x, N*sizeof(double));
  cholmod_l_free_dense(&x_chol, &c);

  // Clear rhs
  cholmod_l_free(1, sizeof(cholmod_dense), b_chol, &c);

  // Clear residual
  cholmod_l_free_dense(&r_chol, &c);

  check_status("factorized_solve()");
}
//-----------------------------------------------------------------------------
cholmod_dense* CholmodCholeskySolver::Cholmod::residual(cholmod_dense* x,
                                                        cholmod_dense* b)
{
  double  one[2] = { 1, 0};
  double mone[2] = {-1, 0};

  // Residual r = r-A*x
  cholmod_dense* r = cholmod_l_copy_dense(b, &c);
  cholmod_l_sdmult(A_chol, 0, mone, one, x, r, &c);

  return r;
}
//-----------------------------------------------------------------------------
double CholmodCholeskySolver::Cholmod::residual_norm(cholmod_dense* r,
                                                     cholmod_dense* x,
                                                     cholmod_dense* b)
{
  double r_norm = cholmod_l_norm_dense(r, 0, &c);
  double x_norm = cholmod_l_norm_dense(x, 0, &c);
  double b_norm = cholmod_l_norm_dense(b, 0, &c);
  double A_norm = cholmod_l_norm_sparse(A_chol, 0, &c);
  double Axb_norm = A_norm*x_norm + b_norm;

  return r_norm/Axb_norm;
}
//-----------------------------------------------------------------------------
void CholmodCholeskySolver::Cholmod::refine_once(cholmod_dense* x,
                                                 cholmod_dense* r)
{
  cholmod_dense* r_iter;
  r_iter = cholmod_l_solve(CHOLMOD_A, L_chol, r, &c);

  double* xx = (double*) x->x;
  double* rx = (double*) r_iter->x;

  for(std::size_t i = 0; i < N; i++)
    xx[i] = xx[i] + rx[i];

  cholmod_l_free_dense(&r_iter, &c);
}
//-----------------------------------------------------------------------------
void CholmodCholeskySolver::Cholmod::check_status(std::string function)
{
  UF_long status = c.status;

  if ( status < 0)
  {
    cout << "\nCHOLMOD Warning: problem related to call to " << function
	    << ".\n_full CHOLMOD common dump:" << endl;

    cholmod_l_print_common(NULL, &c);
  }
  else if (status > 0)
  {
    cout << "\nCHOLMOD Fatal error: problem related to call to " << function
	    << ".\n_full CHOLMOD common dump:" << endl;
    cholmod_l_print_common(NULL, &c);
  }
}
//-----------------------------------------------------------------------------
#endif
