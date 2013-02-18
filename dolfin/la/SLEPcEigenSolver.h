// Copyright (C) 2005-2011 Garth N. Wells
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
// Modified by Ola Skavhaug, 2008.
// Modified by Anders Logg, 2008.
// Modified by Marie Rognes, 2009.
//
// First added:  2005-08-31
// Last changed: 2011-02-02

#ifndef __SLEPC_EIGEN_SOLVER_H
#define __SLEPC_EIGEN_SOLVER_H

#ifdef HAS_SLEPC

#include <string>
#include <boost/shared_ptr.hpp>
#include <slepceps.h>
#include "dolfin/common/types.h"
#include "PETScObject.h"

namespace dolfin
{

  /// Forward declarations
  class GenericVector;
  class PETScMatrix;
  class PETScVector;

  /// This class provides an eigenvalue solver for PETSc matrices.
  /// It is a wrapper for the SLEPc eigenvalue solver.
  ///
  /// The following parameters may be specified to control the solver.
  ///
  /// 1. "spectrum"
  ///
  /// This parameter controls which part of the spectrum to compute.
  /// Possible values are
  ///
  ///   "largest magnitude"   (eigenvalues with largest magnitude)
  ///   "smallest magnitude"  (eigenvalues with smallest magnitude)
  ///   "largest real"        (eigenvalues with largest double part)
  ///   "smallest real"       (eigenvalues with smallest double part)
  ///   "largest imaginary"   (eigenvalues with largest imaginary part)
  ///   "smallest imaginary"  (eigenvalues with smallest imaginary part)
  ///
  /// For SLEPc versions >= 3.1 , the following values are also possible
  ///
  ///   "target magnitude"    (eigenvalues closest to target in magnitude)
  ///   "target real"         (eigenvalues closest to target in real part)
  ///   "target imaginary"    (eigenvalues closest to target in imaginary part)
  ///
  /// The default is "largest magnitude"
  ///
  /// 2. "solver"
  ///
  /// This parameter controls which algorithm is used by SLEPc.
  /// Possible values are
  ///
  ///   "power"               (power iteration)
  ///   "subspace"            (subspace iteration)
  ///   "arnoldi"             (Arnoldi)
  ///   "lanczos"             (Lanczos)
  ///   "krylov-schur"        (Krylov-Schur)
  ///   "lapack"              (LAPACK, all values, direct, small systems only)
  ///
  /// The default is "krylov-schur"
  ///
  /// 3. "tolerance"
  ///
  /// This parameter controls the tolerance used by SLEPc.
  /// Possible values are positive double numbers.
  ///
  /// The default is 1e-15;
  ///
  /// 4. "maximum_iterations"
  ///
  /// This parameter controls the maximum number of iterations used by SLEPc.
  /// Possible values are positive integers.
  ///
  /// Note that both the tolerance and the number of iterations must be
  /// specified if either one is specified.
  ///
  /// 5. "problem_type"
  ///
  /// This parameter can be used to give extra information about the
  /// type of the eigenvalue problem. Some solver types require this
  /// extra piece of information. Possible values are:
  ///
  ///   "hermitian"               (Hermitian)
  ///   "non_hermitian"           (Non-Hermitian)
  ///   "gen_hermitian"           (Generalized Hermitian)
  ///   "gen_non_hermitian"       (Generalized Non-Hermitian)
  ///
  /// 6. "spectral_transform"
  ///
  /// This parameter controls the application of a spectral transform. A
  /// spectral transform can be used to enhance the convergence of the
  /// eigensolver and in particular to only compute eigenvalues in the
  /// interior of the spectrum. Possible values are:
  ///
  ///   "shift-and-invert"      (A shift-and-invert transform)
  ///
  /// Note that if a spectral transform is given, then also a non-zero
  /// spectral shift parameter has to be provided.
  ///
  /// The default is no spectral transform.
  ///
  /// 7. "spectral_shift"
  ///
  /// This parameter controls the spectral shift used by the spectral
  /// transform and must be provided if a spectral transform is given. The
  /// possible values are real numbers.
  ///

  class SLEPcEigenSolver : public Variable, public PETScObject
  {
  public:

    /// Create eigenvalue solver for Ax = \lambda x
    SLEPcEigenSolver(const PETScMatrix& A);

    /// Create eigenvalue solver Ax = \lambda Bx
    SLEPcEigenSolver(const PETScMatrix& A, const PETScMatrix& B);

    /// Create eigenvalue solver for Ax = \lambda x
    SLEPcEigenSolver(boost::shared_ptr<const PETScMatrix> A);

    /// Create eigenvalue solver for Ax = \lambda x
    SLEPcEigenSolver(boost::shared_ptr<const PETScMatrix> A,
                     boost::shared_ptr<const PETScMatrix> B);

    /// Destructor
    ~SLEPcEigenSolver();

    /// Compute all eigenpairs of the matrix A (solve Ax = \lambda x)
    void solve();

    /// Compute the n first eigenpairs of the matrix A (solve Ax = \lambda x)
    void solve(std::size_t n);

    /// Get the first eigenvalue
    void get_eigenvalue(double& lr, double& lc) const;

    /// Get the first eigenpair
    void get_eigenpair(double& lr, double& lc,
                       GenericVector& r, GenericVector& c) const;

    /// Get the first eigenpair
    void get_eigenpair(double& lr, double& lc,
                       PETScVector& r, PETScVector& c) const;

    /// Get eigenvalue i
    void get_eigenvalue(double& lr, double& lc, std::size_t i) const;

    /// Get eigenpair i
    void get_eigenpair(double& lr, double& lc,
                       GenericVector& r, GenericVector& c, std::size_t i) const;

    /// Get eigenpair i
    void get_eigenpair(double& lr, double& lc,
                       PETScVector& r, PETScVector& c, std::size_t i) const;

    // Get the number of iterations used by the solver
    std::size_t get_iteration_number() const;

    // Get the number of converged eigenvalues
    std::size_t get_number_converged() const;

    // Set deflation space
    void set_deflation_space(const PETScVector& deflation_space);

    /// Default parameter values
    static Parameters default_parameters()
    {
      Parameters p("slepc_eigenvalue_solver");

      p.add("problem_type",       "default");
      p.add("spectrum",           "largest magnitude");
      p.add("solver",             "krylov-schur");
      p.add("tolerance",          1e-15);
      p.add("maximum_iterations", 10000);
      p.add("spectral_transform", "default");
      p.add("spectral_shift",     0.0);
      p.add("verbose",            false);

      return p;
    }

  private:

    /// Callback for changes in parameter values
    void read_parameters();

    // Set problem type (used for SLEPc internals)
    void set_problem_type(std::string type);

    // Set spectral transform
    void set_spectral_transform(std::string transform, double shift);

    // Set spectrum
    void set_spectrum(std::string solver);

    // Set solver
    void set_solver(std::string spectrum);

    // Set tolerance
    void set_tolerance(double tolerance, std::size_t maxiter);

    // Operators (A x = \lambda x or Ax = \lambda B x)
    boost::shared_ptr<const PETScMatrix> A;
    boost::shared_ptr<const PETScMatrix> B;

    // SLEPc solver pointer
    EPS eps;

  };

}

#endif

#endif
