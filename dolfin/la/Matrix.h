// Copyright (C) 2006-2008 Anders Logg and Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2007-2008.
// Modified by Kent-Andre Mardal 2008.
// Modified by Martin Sandve Alnes 2008.
//
// First added:  2006-05-15
// Last changed: 2008-04-23

#ifndef __MATRIX_H
#define __MATRIX_H

#include "default_la_types.h"
#include "GenericMatrix.h"

namespace dolfin
{
  
  /// This class provides an interface to the default DOLFIN
  /// matrix implementation as decided in default_la_types.h.
  
  class Matrix : public GenericMatrix, public Variable
  {
  public:
    
    /// Constructor
    Matrix() : Variable("A", "DOLFIN matrix"),
	           matrix(new DefaultMatrix())
    {}
    
    /// Constructor
    Matrix(uint M, uint N) : Variable("A", "DOLFIN matrix"),
			                 matrix(new DefaultMatrix(M, N))
    {}
    
    /// Destructor
    ~Matrix()
    { delete matrix; }
    
    /// Initialize M x N matrix
    void init(uint M, uint N)
    { matrix->init(M, N); }
    
    /// Initialize zero matrix using sparsity pattern
    void init(const GenericSparsityPattern& sparsity_pattern)
    { matrix->init(sparsity_pattern); }
    
    /// Return copy of matrix
    Matrix* copy() const
    { Matrix* A = new Matrix(); delete A->matrix; A->matrix = matrix->copy(); return A; }

    /// Return size of given dimension
    uint size(uint dim) const
    { return matrix->size(dim); }

    /// Set all entries to zero and keep any sparse structure (implemented by sub class)
    void zero()
    { matrix->zero(); }

    /// Finalise assembly of matrix
    void apply()
    { matrix->apply(); }
    
    /// Display matrix (sparse output is default)
    void disp(uint precision = 2) const
    { matrix->disp(precision); }

    /// Get block of values
    void get(real* block, uint m, const uint* rows, uint n, const uint* cols) const
    { matrix->get(block, m, rows, n, cols); }
    
    /// Set block of values
    void set(const real* block, uint m, const uint* rows, uint n, const uint* cols)
    { matrix->set(block, m, rows, n, cols); }
    
    /// Add block of values
    void add(const real* block, uint m, const uint* rows, uint n, const uint* cols)
    { matrix->add(block, m, rows, n, cols); }

    /// Get non-zero values of given row
    void getrow(uint i, Array<uint>& columns, Array<real>& values) const
    { matrix->getrow(i, columns, values); }

    /// Set given rows to zero
    void zero(uint m, const uint* rows)
    { matrix->zero(m, rows); }
    
    /// Set given rows to identity matrix
    void ident(uint m, const uint* rows)
    { matrix->ident(m, rows); }

    // Matrix-vector product, y = Ax
    void mult(const GenericVector& x, GenericVector& y, bool transposed=false) const
    { matrix->mult(x, y, transposed); }

    /// Multiply matrix by given number
    virtual const Matrix& operator*= (real a)
    { *matrix *= a; return *this; }

    /// Assignment operator
    const GenericMatrix& operator= (const GenericMatrix& A)
    { *matrix = A; return *this; }

    /// Assignment operator
    const Matrix& operator= (const Matrix& A)
    { *matrix = *A.matrix; return *this; }

    ///--- Special functions ---

    /// Get linear algebra backend factory
    LinearAlgebraFactory& factory() const
    { return matrix->factory(); }

    ///--- Special functions, intended for library use only ---

    /// Return instance (const version)
    virtual const GenericMatrix* instance() const 
    { return matrix; }

    /// Return instance (non-const version)
    virtual GenericMatrix* instance() 
    { return matrix; }

  private:

    GenericMatrix* matrix;
    
  };

}

#endif
