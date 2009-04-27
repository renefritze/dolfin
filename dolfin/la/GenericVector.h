// Copyright (C) 2006-2007 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2006-2008.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Ola Skavhaug, 2008.
// Modified by Martin Sandve Alnes, 2009.
//
// First added:  2006-04-25
// Last changed: 2009-03-09

#ifndef __GENERIC_VECTOR_H
#define __GENERIC_VECTOR_H

#include "enums_la.h"
#include "GenericSparsityPattern.h"
#include "GenericTensor.h"

namespace dolfin
{

  /// This class defines a common interface for vectors.
  class NewXMLVector;

  class GenericVector : public GenericTensor
  {
  public:

    /// Destructor
    virtual ~GenericVector() {}

    //--- Implementation of the GenericTensor interface ---

    /// Resize tensor with given dimensions
    virtual void resize(uint rank, const uint* dims)
    { dolfin_assert(rank == 1); resize(dims[0]); }

    /// Initialize zero tensor using sparsity pattern
    inline void init(const GenericSparsityPattern& sparsity_pattern)
    { resize(sparsity_pattern.size(0)); zero(); }

    /// Return copy of tensor
    virtual GenericVector* copy() const = 0;

    /// Return tensor rank (number of dimensions)
    virtual uint rank() const
    { return 1; }

    /// Return size of given dimension
    virtual uint size(uint dim) const
    { dolfin_assert(dim == 0); return size(); }

    /// Get block of values
    virtual void get(double* block, const uint* num_rows, const uint * const * rows) const
    { get(block, num_rows[0], rows[0]); }

    /// Set block of values
    virtual void set(const double* block, const uint* num_rows, const uint * const * rows)
    { set(block, num_rows[0], rows[0]); }

    /// Add block of values
    virtual void add(const double* block, const uint* num_rows, const uint * const * rows)
    { add(block, num_rows[0], rows[0]); }

    /// Set all entries to zero and keep any sparse structure
    virtual void zero() = 0;

    /// Finalize assembly of tensor
    virtual void apply() = 0;

    /// Display tensor
    virtual void disp(uint precision=2) const = 0;

    //--- Vector interface ---

    /// Resize vector to size N
    virtual void resize(uint N) = 0;

    /// Return size of vector
    virtual uint size() const = 0;

    /// Get block of values
    virtual void get(double* block, uint m, const uint* rows) const = 0;

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows) = 0;

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows) = 0;

    /// Get all values
    virtual void get(double* values) const = 0;

    /// Set all values
    virtual void set(double* values) = 0;

    /// Add values to each entry
    virtual void add(double* values) = 0;

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(double a, const GenericVector& x) = 0;

    /// Return inner product with given vector
    virtual double inner(const GenericVector& x) const = 0;

    /// Return norm of vector
    virtual double norm(dolfin::NormType type=l2) const = 0;

    /// Return minimum value of vector
    virtual double min() const = 0;

    /// Return maximum value of vector
    virtual double max() const = 0;

    /// Return sum of vector
    virtual double sum() const = 0;

    /// Multiply vector by given number
    virtual const GenericVector& operator*= (double a) = 0;

    /// Divide vector by given number
    virtual const GenericVector& operator/= (double a) = 0;

    /// Add given vector
    virtual const GenericVector& operator+= (const GenericVector& x) = 0;

    /// Subtract given vector
    virtual const GenericVector& operator-= (const GenericVector& x) = 0;

    /// Assignment operator
    virtual const GenericVector& operator= (const GenericVector& x) = 0;

    /// Assignment operator
    virtual const GenericVector& operator= (double a) = 0;

    /// Return pointer to underlying data (const version)
    virtual const double* data() const
    {
      error("Unable to return pointer to underlying vector data.");
      return 0;
    }

    /// Return pointer to underlying data
    virtual double* data()
    {
      error("Unable to return pointer to underlying vector data.");
      return 0;
    }

    //--- Convenience functions ---

    /// Get value of given entry
    virtual double operator[] (uint i) const
    { double value(0); get(&value, 1, &i); return value; }

    /// Get value of given entry
    virtual double getitem(uint i) const
    { double value(0); get(&value, 1, &i); return value; }

    /// Set given entry to value
    virtual void setitem(uint i, double value)
    { set(&value, 1, &i); }


    typedef NewXMLVector XMLHandler;
  };

}

#endif
