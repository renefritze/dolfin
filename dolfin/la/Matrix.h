// Copyright (C) 2006-2008 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2007-2008.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Martin Sandve Alnes, 2008.
//
// First added:  2006-05-15
// Last changed: 2009-09-08

#ifndef __MATRIX_H
#define __MATRIX_H

#include <tr1/tuple>
#include "DefaultFactory.h"
#include "GenericMatrix.h"

namespace dolfin
{

  /// This class provides the default DOLFIN matrix class,
  /// based on the default DOLFIN linear algebra backend.

  class Matrix : public GenericMatrix
  {
  public:

    /// Create empty matrix
    Matrix() : matrix(0)
    { DefaultFactory factory; matrix = factory.create_matrix(); }

    /// Create M x N matrix
    Matrix(uint M, uint N) : matrix(0)
    { DefaultFactory factory; matrix = factory.create_matrix(); matrix->resize(M, N); }

    /// Copy constructor
    explicit Matrix(const Matrix& A) : matrix(A.matrix->copy())
    {}

    /// Destructor
    virtual ~Matrix()
    { delete matrix; }

    //--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern
    virtual void init(const GenericSparsityPattern& sparsity_pattern)
    { matrix->init(sparsity_pattern); }

    /// Return copy of tensor
    virtual Matrix* copy() const
    { Matrix* A = new Matrix(); delete A->matrix; A->matrix = matrix->copy(); return A; }

    /// Return size of given dimension
    virtual uint size(uint dim) const
    { return matrix->size(dim); }

    /// Set all entries to zero and keep any sparse structure
    virtual void zero()
    { matrix->zero(); }

    /// Finalize assembly of tensor
    virtual void apply(std::string mode)
    { matrix->apply(mode); }

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const
    { return matrix->str(verbose); }

    //--- Implementation of the GenericMatrix interface ---

    /// Resize matrix to M x N
    virtual void resize(uint M, uint N)
    { matrix->resize(M, N); }

    /// Get block of values
    virtual void get(double* block, uint m, const uint* rows, uint n, const uint* cols) const
    { matrix->get(block, m, rows, n, cols); }

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows, uint n, const uint* cols)
    { matrix->set(block, m, rows, n, cols); }

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows, uint n, const uint* cols)
    { matrix->add(block, m, rows, n, cols); }

    /// Add multiple of given matrix (AXPY operation)
    virtual void axpy(double a, const GenericMatrix& A, bool same_nonzero_pattern)
    { matrix->axpy(a, A, same_nonzero_pattern); }

    /// Return norm of matrix
    virtual double norm(std::string norm_type) const
    { return matrix->norm(norm_type); }

    /// Get non-zero values of given row
    virtual void getrow(uint row, std::vector<uint>& columns, std::vector<double>& values) const
    { matrix->getrow(row, columns, values); }

    /// Set values for given row
    virtual void setrow(uint row, const std::vector<uint>& columns, const std::vector<double>& values)
    { matrix->setrow(row, columns, values); }

    /// Set given rows to zero
    virtual void zero(uint m, const uint* rows)
    { matrix->zero(m, rows); }

    /// Set given rows to identity matrix
    virtual void ident(uint m, const uint* rows)
    { matrix->ident(m, rows); }

    // Matrix-vector product, y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y) const
    { matrix->mult(x, y); }

    // Matrix-vector product, y = Ax
    virtual void transpmult(const GenericVector& x, GenericVector& y) const
    { matrix->transpmult(x, y); }

    /// Multiply matrix by given number
    virtual const Matrix& operator*= (double a)
    { *matrix *= a; return *this; }

    /// Divide matrix by given number
    virtual const Matrix& operator/= (double a)
    { *matrix /= a; return *this; }

    /// Assignment operator
    virtual const GenericMatrix& operator= (const GenericMatrix& A)
    { *matrix = A; return *this; }

    /// Return pointers to underlying compressed storage data.
    /// See GenericMatrix for documentation.
    virtual std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int> data() const
    { return matrix->data(); }

    //--- Special functions ---

    /// Return linear algebra backend factory
    virtual LinearAlgebraFactory& factory() const
    { return matrix->factory(); }

    //--- Special functions, intended for library use only ---

    /// Return concrete instance / unwrap (const version)
    virtual const GenericMatrix* instance() const
    { return matrix; }

    /// Return concrete instance / unwrap (non-const version)
    virtual GenericMatrix* instance()
    { return matrix; }

    //--- Special Matrix functions ---

    /// Assignment operator
    const Matrix& operator= (const Matrix& A)
    { *matrix = *A.matrix; return *this; }

  private:

    // Pointer to concrete implementation
    GenericMatrix* matrix;

  };

}

#endif
