// Copyright (C) 2004-2008 Johan Hoffman, Johan Jansson and Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Andy R. Terrel, 2005.
// Modified by Garth N. Wells, 2006-2007.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Ola Skavhaug, 2008.
//
// First added:  2004-01-01
// Last changed: 2008-05-15

#ifndef __PETSC_MATRIX_H
#define __PETSC_MATRIX_H

#ifdef HAS_PETSC

#include <boost/shared_ptr.hpp>
#include <petscmat.h>

#include <dolfin/log/LogStream.h>
#include <dolfin/common/Variable.h>
#include "enums_la.h"
#include "PETScObject.h"
#include "GenericMatrix.h"

namespace dolfin
{

  class PETScVector;

  /// This class provides a simple matrix class based on PETSc.
  /// It is a wrapper for a PETSc matrix pointer (Mat)
  /// implementing the GenericMatrix interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the PETSc Mat pointer using the function mat() and
  /// use the standard PETSc interface.

  class PETScMatrix : public GenericMatrix, public PETScObject, public Variable
  {
  public:

    /// PETSc sparse matrix types
    enum Type
    {
      default_matrix, // Default matrix type
      spooles,        // Spooles
      superlu,        // Super LU
      umfpack         // UMFPACK
    };

    /// Create empty matrix
    explicit PETScMatrix(Type type=default_matrix);

    /// Create M x N matrix
    PETScMatrix(uint M, uint N, Type type=default_matrix);

    /// Copy constructor
    explicit PETScMatrix(const PETScMatrix& A);

    /// Create matrix from given PETSc Mat pointer
    explicit PETScMatrix(boost::shared_ptr<Mat> A);

    /// Destructor
    virtual ~PETScMatrix();

    //--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern
    virtual void init(const GenericSparsityPattern& sparsity_pattern);

    /// Return copy of tensor
    virtual PETScMatrix* copy() const;

    /// Return size of given dimension
    virtual uint size(uint dim) const;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply();

    /// Display tensor
    virtual void disp(uint precision=2) const;

    //--- Implementation of the GenericMatrix interface --

    /// Resize matrix to M x N
    virtual void resize(uint M, uint N);

    /// Get block of values
    virtual void get(double* block, uint m, const uint* rows, uint n, const uint* cols) const;

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows, uint n, const uint* cols);

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows, uint n, const uint* cols);

    /// Add multiple of given matrix (AXPY operation)
    virtual void axpy(double a, const GenericMatrix& A, bool same_nonzero_pattern = false);

    /// Get non-zero values of given row
    virtual void getrow(uint row, std::vector<uint>& columns, std::vector<double>& values) const;

    /// Set values for given row
    virtual void setrow(uint row, const std::vector<uint>& columns, const std::vector<double>& values);

    /// Set given rows to zero
    virtual void zero(uint m, const uint* rows);

    /// Set given rows to identity matrix
    virtual void ident(uint m, const uint* rows);

    // Matrix-vector product, y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y, bool transposed=false) const;

    /// Multiply matrix by given number
    virtual const PETScMatrix& operator*= (double a);
    
    /// Divide matrix by given number
    virtual const PETScMatrix& operator/= (double a);

    /// Assignment operator
    virtual const GenericMatrix& operator= (const GenericMatrix& A);

    //--- Special functions ---

    /// Return linear algebra backend factory
    virtual LinearAlgebraFactory& factory() const;

    //--- Special PETScFunctions ---

    /// Return PETSc Mat pointer
    boost::shared_ptr<Mat> mat() const;

    /// Return PETSc matrix type 
    Type type() const;

    /// Return norm of matrix
    double norm(const dolfin::NormType) const;

    /// Assignment operator
    const PETScMatrix& operator= (const PETScMatrix& A);

  private:

    // Initialize M x N matrix with a given number of nonzeros per row
    void init(uint M, uint N, const uint* nz);

    // Initialize M x N matrix with a given number of nonzeros per row diagonal and off-diagonal
    void init(uint M, uint N, const uint* d_nzrow, const uint* o_nzrow);

    // Set PETSc matrix type
    void setType();

    // Return PETSc matrix type
    #if PETSC_VERSION_MAJOR > 2 
    const MatType getPETScType() const;
    #else
    MatType getPETScType() const;
    #endif

    // Check that requested type has been compiled into PETSc
    void checkType();

    // PETSc Mat pointer
    boost::shared_ptr<Mat> A;

    // PETSc matrix type
    Type _type;

  };

  /// Output of PETScMatrix
  LogStream& operator<< (LogStream& stream, const PETScMatrix& A);

}

#endif

#endif
