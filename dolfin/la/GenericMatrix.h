// Copyright (C) 2006-2007 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Johan Jansson 2006.
// Modified by Anders Logg 2006-2008.
// Modified by Ola Skavhaug 2007-2008.
// Modified by Kent-Andre Mardal 2008.
// Modified by Martin Alnæs, 2008.
//
// First added:  2006-04-24
// Last changed: 2008-04-22

#ifndef __GENERIC_MATRIX_H
#define __GENERIC_MATRIX_H

#include <dolfin/common/types.h>
#include "GenericTensor.h"

namespace dolfin
{

  class GenericVector; 
  template<class M> class Array;
  
  /// This class defines a common interface for matrices.
  
  class GenericMatrix : public GenericTensor
  {
  public:

    /// Destructor
    virtual ~GenericMatrix() {}
    
    ///--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern (implemented by sub class)
    virtual void init(const GenericSparsityPattern& sparsity_pattern) = 0;

    /// Return rank of tensor (number of dimensions)
    uint rank() const 
    { return 2; }

    /// Return size of given dimension (implemented by sub class)
    virtual uint size(uint dim) const = 0;

    /// Get block of values
    void get(real* block, const uint* num_rows, const uint * const * rows) const
    { get(block, num_rows[0], rows[0], num_rows[1], rows[1]); }

    /// Set block of values
    void set(const real* block, const uint* num_rows, const uint * const * rows)
    { set(block, num_rows[0], rows[0], num_rows[1], rows[1]); }

    /// Add block of values
    void add(const real* block, const uint* num_rows, const uint * const * rows)
    { add(block, num_rows[0], rows[0], num_rows[1], rows[1]); }

    /// Set all entries to zero and keep any sparse structure (implemented by sub class)
    virtual void zero() = 0;

    /// Finalise assembly of tensor (implemented by sub class)
    virtual void apply() = 0;

    /// Display tensor (implemented by sub class)
    virtual void disp(uint precision = 2) const = 0;

    ///--- Matrix interface ---

    /// Initialize M x N matrix
    virtual void init(uint M, uint N) = 0;
    
    /// Get block of values
    virtual void get(real* block, uint m, const uint* rows, uint n, const uint* cols) const = 0;

    /// Set block of values
    virtual void set(const real* block, uint m, const uint* rows, uint n, const uint* cols) = 0;

    /// Add block of values
    virtual void add(const real* block, uint m, const uint* rows, uint n, const uint* cols) = 0;

    /// Get non-zero values of given row
    virtual void getrow(uint i, int& ncols, Array<int>& columns, Array<real>& values) const = 0;

    /// Set given rows to zero
    virtual void zero(uint m, const uint* rows) = 0;

    /// Set given rows to identity matrix
    virtual void ident(uint m, const uint* rows) = 0;

    /// Matrix-vector product, y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y, bool transposed=false) const = 0;

    ///--- Convenience functions ---

    /// Set given matrix entry to value
    virtual void setitem(std::pair<uint, uint> idx, real value)
    { set(&value, 1, &idx.first, 1, &idx.second); }

    /// Get given matrix entry 
    virtual real getitem(std::pair<uint, uint> idx)
    { real value(0); get(&value, 1, &idx.first, 1, &idx.second); return value; }

    ///--- Special functions, intended for library use only ---

    /// Return instance (const version)
    virtual const GenericMatrix* instance() const
    { return this; }

    /// Return instance (non-const version)
    virtual GenericMatrix* instance()
    { return this; }

  };

}

#endif
