// Copyright (C) 2006-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2006-2008.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Ola Skavhaug, 2008.
// Modified by Martin Alnæs, 2008.
//
// First added:  2006-03-04
// Last changed: 2008-04-29

#ifndef __UBLAS_VECTOR_H
#define __UBLAS_VECTOR_H

#include <dolfin/log/LogStream.h>
#include <dolfin/common/Variable.h>
#include "ublas.h"
#include "GenericVector.h"

namespace dolfin
{

  namespace ublas = boost::numeric::ublas;

  /// This class provides a simple vector class based on uBLAS.
  /// It is a simple wrapper for a uBLAS vector implementing the
  /// GenericVector interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the underlying uBLAS vector and use the standard
  /// uBLAS interface which is documented at
  /// http://www.boost.org/libs/numeric/ublas/doc/index.htm.

  class uBlasVector : public GenericVector, public Variable
  {
  public:

    /// Create empty vector
    uBlasVector();

    /// Create vector of size N
    explicit uBlasVector(uint N);

    /// Copy constructor
    explicit uBlasVector(const uBlasVector& x);

    /// Create vector from given uBLAS vector expression
    template <class E>
    explicit uBlasVector(const ublas::vector_expression<E>& x) : x(x) {}

    /// Destructor
    virtual ~uBlasVector();

    //--- Implementation of the GenericTensor interface ---

    /// Create copy of tensor
    virtual uBlasVector* copy() const;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply();

    /// Display tensor
    virtual void disp(uint precision=2) const;    

    //--- Implementation of the GenericVector interface ---

    /// Initialize vector of size N
    virtual void init(uint N);

    /// Return size of vector
    virtual uint size() const
    { return x.size(); }

    /// Get block of values
    virtual void get(real* block, uint m, const uint* rows) const;

    /// Set block of values
    virtual void set(const real* block, uint m, const uint* rows);

    /// Add block of values
    virtual void add(const real* block, uint m, const uint* rows);

    /// Get all values
    virtual void get(real* values) const;

    /// Set all values
    virtual void set(real* values);

    /// Add values to each entry
    virtual void add(real* values);

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(real a, const GenericVector& x);

    /// Return inner product with given vector
    virtual real inner(const GenericVector& x) const;

    /// Compute norm of vector
    virtual real norm(VectorNormType type=l2) const;

    /// Return minimum value of vector
    virtual real min() const;

    /// Return maximum value of vector
    virtual real max() const;

    /// Multiply vector by given number
    virtual const uBlasVector& operator *= (real a);

    /// Divide vector by given number
    virtual const uBlasVector& operator /= (real a);

    /// Add given vector
    virtual const uBlasVector& operator+= (const GenericVector& x);

    /// Subtract given vector
    virtual const uBlasVector& operator-= (const GenericVector& x);

    /// Assignment operator
    virtual const GenericVector& operator= (const GenericVector& x);

    //--- Special functions ---

    /// Return linear algebra backend factory
    LinearAlgebraFactory& factory() const;

    //--- Special uBLAS functions ---

    /// Return reference to uBLAS vector (const version)
    const ublas_vector& vec() const
    { return x; }

    /// Return reference to uBLAS vector (non-const version)
    ublas_vector& vec()
    { return x; }

    /// Access value of given entry (const version)
    virtual real operator[] (uint i) const
    { return x(i); };

    /// Access value of given entry (non-const version)
    real& operator[] (uint i)
    { return x(i); };

    /// Assignment operator
    const uBlasVector& operator= (const uBlasVector& x);

  private:

    // uBLAS vector object
    ublas_vector x;

  };

  LogStream& operator<< (LogStream& stream, const uBlasVector& x);
 
}

#endif
