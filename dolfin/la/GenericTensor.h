// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2007.
// Modified by Ola Skavhaug, 2007.
// Modified by Martin Alnæs, 2008.
//
// First added:  2007-01-17
// Last changed: 2008-04-23

#ifndef __GENERIC_TENSOR_H
#define __GENERIC_TENSOR_H

#include <dolfin/common/types.h>
#include <dolfin/log/log.h>

enum FinalizeType { 
  FINALIZE, 
  FLUSH, 
  PETSC_HACK
};


namespace dolfin
{

  class GenericSparsityPattern;
  class LinearAlgebraFactory;

  /// This class defines a common interface for arbitrary rank tensors.

  class GenericTensor
  {
  public:

    /// Destructor
    virtual ~GenericTensor() {}

    //--- Basic GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern
    virtual void init(const GenericSparsityPattern& sparsity_pattern) = 0;

    /// Return copy of tensor
    virtual GenericTensor* copy() const = 0;

    /// Return tensor rank (number of dimensions)
    virtual uint rank() const = 0;

    /// Return size of given dimension
    virtual uint size(uint dim) const = 0;

    /// Get block of values
    virtual void get(real* block, const uint* num_rows, const uint * const * rows) const = 0;

    /// Set block of values
    virtual void set(const real* block, const uint* num_rows, const uint * const * rows) = 0;

    /// Add block of values
    virtual void add(const real* block, const uint* num_rows, const uint * const * rows) = 0;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero() = 0;

    /// Finalize assembly of tensor
    virtual void apply(FinalizeType finaltype=FINALIZE) = 0;

    /// Display tensor
    virtual void disp(uint precision=2) const = 0;

    //--- Special functions, downcasting to concrete types ---

    /// Return linear algebra backend factory
    virtual LinearAlgebraFactory& factory() const = 0;

    /// Cast a GenericTensor to its derived class (const version)
    template<class T> const T& down_cast() const
    {
      const T* t = dynamic_cast<const T*>(instance());
      if (!t) error("GenericTensor cannot be cast to the requested type.");
      return *t;
    }

    /// Cast a GenericTensor to its derived class (non-const version)
    template<class T> T& down_cast()
    {
      T* t = dynamic_cast<T*>(instance());
      if (!t) error("GenericTensor cannot be cast to the requested type.");
      return *t;
    }

    /// Check whether the GenericTensor instance matches a specific type
    template<class T> bool has_type() const
    { return bool(dynamic_cast<const T*>(instance())); }

    //--- Special functions, intended for library use only ---

    /// Return concrete instance / unwrap (const version)
    virtual const GenericTensor* instance() const
    { return this; }

    /// Return concrete instance / unwrap (non-const version)
    virtual GenericTensor* instance()
    { return this; }

    /// Assignment (must be overloaded by subclass)
    virtual const GenericTensor& operator= (const GenericTensor& x)
    { error("Assignment operator not implemented by subclass"); return *this; }

  };

}

#endif
