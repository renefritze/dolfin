// Copyright (C) 2008 Dag Lindbo
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008.
// Modified by Garth N. Wells, 2009.
//
// First added:  2008-07-06
// Last changed: 2009-09-08

#ifdef HAS_MTL4

#ifndef __MTL4_VECTOR_H
#define __MTL4_VECTOR_H

#include <dolfin/common/types.h>
#include "mtl4.h"
#include "GenericVector.h"

//  Developers note:
//
//  This class implements a minimal backend for MTL4.
//
//  There are certain inline decisions that have been deferred.
//  Due to the extensive calling of this backend through the generic LA
//  interface, it is not clear where inlining will be possible and
//  improve performance.

namespace dolfin
{

  template<class T> class Array;

  class MTL4Vector: public GenericVector
  {
  public:

    /// Create empty vector
    MTL4Vector();

    /// Create vector of size N
    explicit MTL4Vector(uint N);

    /// Copy constructor
    MTL4Vector(const MTL4Vector& x);

    /// Destructor
    virtual ~MTL4Vector();

    //--- Implementation of the GenericTensor interface ---

    /// Return copy of tensor
    virtual MTL4Vector* copy() const;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply(std::string mode);

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const;

    //--- Implementation of the GenericVector interface ---

    /// Resize vector to size N
    virtual void resize(uint N);

    /// Return size of vector
    virtual uint size() const;

    /// Return local size of vector
    virtual uint local_size() const
    { return size(); }

    /// Return local ownership range of a vector
    virtual std::pair<uint, uint> local_range() const;

    /// Determine whether global vector index is owned by this process
    virtual bool owns_index(uint i) const;

    /// Get block of values
    virtual void get_local(double* block, uint m, const uint* rows) const;

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows);

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows);

    /// Get all values on local process
    virtual void get_local(Array<double>& values) const;

    /// Set all values on local process
    virtual void set_local(const Array<double>& values);

    /// Add all values to each entry on local process
    virtual void add_local(const Array<double>& values);

    /// Gather entries into local vector x
    virtual void gather(GenericVector& x, const Array<uint>& indices) const;

    virtual void gather(Array<double>& x, const Array<uint>& indices) const;

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(double a, const GenericVector& x);

    /// Return inner product with given vector
    virtual double inner(const GenericVector& vector) const;

    /// Return norm of vector
    virtual double norm(std::string norm_type) const;

    /// Return minimum value of vector
    virtual double min() const;

    /// Return maximum value of vector
    virtual double max() const;

    /// Return sum of values of vector
    virtual double sum() const;

    /// Multiply vector by given number
    virtual const MTL4Vector& operator*= (double a);

    /// Multiply vector by another vector pointwise
    virtual const MTL4Vector& operator*= (const GenericVector& x);

    /// Divide vector by given number
    virtual const MTL4Vector& operator/= (double a);

    /// Assignment operator
    virtual const MTL4Vector& operator= (double a);

    /// Add given vector
    virtual const MTL4Vector& operator+= (const GenericVector& x);

    /// Subtract given vector
    virtual const MTL4Vector& operator-= (const GenericVector& x);

    /// Assignment operator
    virtual const GenericVector& operator= (const GenericVector& x);

    /// Return pointer to underlying data (const version)
    virtual const double* data() const
    { return x.address_data(); }

    /// Return pointer to underlying data (non-const version)
    virtual double* data()
    { return x.address_data(); }

    //--- Special functions ---
    virtual LinearAlgebraFactory& factory() const;

    //--- Special MTL4 functions ---

    /// Return const mtl4_vector reference
    const mtl4_vector& vec() const;

    /// Return mtl4_vector reference
    mtl4_vector& vec();

    /// Assignment operator
    virtual const MTL4Vector& operator= (const MTL4Vector& x);

  private:

    // MTL4 vector object
    mtl4_vector x;

  };

}

#endif
#endif
