// Copyright (C) 2006 Garth N. Wells
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2006.
//
// First added:  2006-03-04
// Last changed: 2006-05-07

#ifndef __DENSE_MATRIX_H
#define __DENSE_MATRIX_H

// FIXME: boost first

#include <dolfin/dolfin_log.h>
#include <dolfin/Variable.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <dolfin/GenericMatrix.h>

namespace dolfin
{

  class DenseVector;
  namespace ublas = boost::numeric::ublas;
  typedef ublas::matrix<double> ublas_matrix;

  /// This class represents a dense matrix of dimension M x N.
  /// It is a simple wrapper for a Boost ublas matrix.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// refer to the documentation for ublas which can be found at
  /// http://www.boost.org/libs/numeric/ublas/doc/index.htm.

  class DenseMatrix : public GenericMatrix,
		      public Variable,
		      public ublas_matrix
  {
  public:
    
    /// Constructor
    DenseMatrix();
    
    /// Constructor
    DenseMatrix(uint M, uint N);

    /// Destructor
    ~DenseMatrix();

    /// Initialize M x N matrix
    void init(uint M, uint N);

    /// Initialize M x N matrix with given maximum number of nonzeros in each row
    /// nzmax is a dummy argument ans is passed for compatibility with sparse matrces
    void init(uint M, uint N, uint nzmax);

    /// Assignment from a matrix_expression
    template <class E>
    DenseMatrix& operator=(const ublas::matrix_expression<E>& A) const
    { 
      ublas_matrix::operator=(A); 
      return *this;
    } 

    /// Return reference to matrix component
    real& operator() (uint i, uint j) 
      { return ublas_matrix::operator() (i, j); }; 

    /// Return number of rows (dim = 0) or columns (dim = 1) 
    uint size(uint dim) const;

    /// Set block of values
    void set(const real block[], const int rows[], int m, const int cols[], int n);

    /// Add block of values
    void add(const real block[], const int rows[], int m, const int cols[], int n);

    /// Solve Ax = b out-of-place (A is not destroyed)
    void solve(DenseVector& x, const DenseVector& b) const;

    // FIXME: Rename to solveInPlace()

    /// Solve Ax = b in-place (A is destroyed)
    void solve_in_place(DenseVector& x, const DenseVector& b);

    /// Compute inverse of matrix
    void invert();

    /// Apply changes to matrix (dummy function for compatibility)
    void apply();

    /// Set all entries to zero
    void zero();

    /// Set given rows to identity matrix
    void ident(const int rows[], int m);

    /// Maximum number of non-zero terms on a row. Not relevant for dense matrix
    /// so just return 0
    uint nzmax() const
      { return 0; }

    /// Compute A*x
    void mult(const DenseVector& x, DenseVector& Ax) const;

    /// Display matrix
    void disp(uint precision = 2) const;

    /// Output
    friend LogStream& operator<< (LogStream& stream, const DenseMatrix& A);

  };
}

#endif
