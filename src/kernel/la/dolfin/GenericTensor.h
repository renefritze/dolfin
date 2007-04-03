// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Garth N. Wells, 2007.
//
// First added:  2007-01-17
// Last changed: 2007-04-03

#ifndef __GENERIC_TENSOR_H
#define __GENERIC_TENSOR_H

#include <dolfin/constants.h>

namespace dolfin
{

  class SparsityPattern;

  /// This class defines a common interface for general tensors.

  class GenericTensor
  {
  public:

    /// Constructor
    GenericTensor() {};

    /// Destructor
    virtual ~GenericTensor() {}

    /// Initialize zero tensor of given rank and dimensions
    virtual void init(uint rank, const uint* dims) = 0;

    /// Initialize zero tensor using sparsity pattern
    virtual void init(const SparsityPattern& sparsity_pattern) = 0;

    /// Return size of given dimension
    virtual uint size(uint dim) const = 0;

    /// Get block of values
    virtual void get(real* block, const uint* num_rows, const uint * const * rows) const = 0;

    /// Set block of values
    virtual void set(const real* block, const uint* num_rows, const uint * const * rows) = 0;

    /// Add block of values
    virtual void add(const real* block, const uint* num_rows, const uint * const * rows) = 0;

    /// Finalise assembly of tensor
    virtual void apply() = 0;

  };

}

#endif
