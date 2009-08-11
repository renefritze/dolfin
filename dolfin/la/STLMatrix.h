// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Ola Skavhaug, 2007.
// Modified by Garth N. Wells, 2007, 2009.
// Modified by Ilmar Wilbers, 2008.
//
// First added:  2007-01-17
// Last changed: 2009-08-10

#ifndef __STL_MATRIX_H
#define __STL_MATRIX_H

#include <vector>
#include <map>

#include "GenericSparsityPattern.h"
#include "GenericMatrix.h"

namespace dolfin
{

  /// Simple implementation of a GenericMatrix for experimenting
  /// with new assembly. Not sure this will be used later but it
  /// might be useful.

  class STLMatrix : public GenericMatrix
  {
  public:

    /// Create empty matrix
    STLMatrix()
    { dims[0] = dims[1] = 0; }

    /// Create M x N matrix
    STLMatrix(uint M, uint N)
    { resize(M, N); }

    /// Copy constructor
    explicit STLMatrix(const STLMatrix& A)
    { error("Not implemented."); }

    /// Destructor
    virtual ~STLMatrix() {}

    ///--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern
    virtual void init(const GenericSparsityPattern& sparsity_pattern)
    { resize(sparsity_pattern.size(0), sparsity_pattern.size(1)); }

    /// Return copy of tensor
    virtual STLMatrix* copy() const
    { error("Not implemented."); return 0; }

    /// Return size of given dimension
    virtual uint size(uint dim) const
    { return dims[dim]; }

    /// Set all entries to zero and keep any sparse structure
    virtual void zero()
    {
      for (uint i = 0; i < A.size(); i++)
        for (std::map<uint, double>::iterator it = A[i].begin(); it != A[i].end(); it++)
          it->second = 0.0;
    }

    /// Finalize assembly of tensor
    virtual void apply() {}

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose=false) const;

    //--- Implementation of the GenericMatrix interface ---

    /// Initialize M x N matrix
    virtual void resize(uint M, uint N)
    { dims[0] = M; dims[1] = N; A.resize(M); }

    /// Get block of values
    virtual void get(double* block, uint m, const uint* rows, uint n, const uint* cols) const
    { error("Not implemented."); }

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows, uint n, const uint* cols)
    { error("Not implemented."); }

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows, uint n, const uint* cols)
    {
      uint pos = 0;
      for (uint i = 0; i < m; i++)
      {
        std::map<uint, double>& row = A[rows[i]];
        for (uint j = 0; j < n; j++)
        {
          const uint col = cols[j];
          const std::map<uint, double>::iterator it = row.find(col);
          if ( it == row.end() )
            row.insert(it, std::map<uint, double>::value_type(col, block[pos++]));
          else
            it->second += block[pos++];
        }
      }
    }

    /// Add multiple of given matrix (AXPY operation)
    virtual void axpy(double a, const GenericMatrix& A, bool same_nonzero_pattern = false)
    { error("Not implemented."); }

    /// Return norm of matrix
    virtual double norm(std::string norm_type = "frobenius") const
    { error("Not implemented."); return 0.0; }

    /// Get non-zero values of given row
    virtual void getrow(uint row, std::vector<uint>& columns, std::vector<double>& values) const
    {
      columns.clear();
      values.clear();
      const std::map<uint, double>& rowid = A[row];
      for (std::map<uint, double>::const_iterator it = rowid.begin(); it != rowid.end(); it++)
      {
        columns.push_back(it->first);
        values.push_back(it->second);
      }
    }

    /// Set values for given row
    virtual void setrow(uint row, const std::vector<uint>& columns, const std::vector<double>& values)
    { error("Not implemented."); }

    /// Set given rows to zero
    virtual void zero(uint m, const uint* rows)
    { error("Not implemented."); }

    /// Set given rows to identity matrix
    virtual void ident(uint m, const uint* rows)
    { error("Not implemented."); }

    // Matrix-vector product, y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y, bool transposed=false) const
    { error("Not implemented."); }

    /// Multiply matrix by given number
    virtual const STLMatrix& operator*= (double a)
    { error("Not implemented."); return *this; }

    /// Divide matrix by given number
    virtual const STLMatrix& operator/= (double a)
    { error("Not implemented."); return *this; }

    /// Assignment operator
    virtual const GenericMatrix& operator= (const GenericMatrix& A)
    { error("Not implemented."); return *this; }

    ///--- Specialized matrix functions ---

    /// Return linear algebra backend factory
    virtual LinearAlgebraFactory& factory() const;

    /// Resize tensor of given rank and dimensions
    virtual void resize(uint rank, const uint* dims, bool reset=true)
    {
      // Check that the rank is 2
      if ( rank != 2 )
        error("Illegal tensor rank (%d) for matrix. Rank must be 2.", rank);

      // Initialize matrix
      resize(dims[0], dims[1]);

      // Save dimensions
      this->dims[0] = dims[0];
      this->dims[1] = dims[1];
    }

  private:

    // The matrix representation
    std::vector<std::map<uint, double> > A;

    // The size of the matrix
    uint dims[2];

  };

}

#endif
