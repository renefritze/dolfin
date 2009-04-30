// Copyright (C) 2008 Martin Sandve Alnes, Kent-Andre Mardal and Johannes Ring.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2008.
// Modified by Garth N. Wells, 2008-2008.
//
// First added:  2008-04-21
// Last changed: 2009-02-16

#ifndef __EPETRA_VECTOR_H
#define __EPETRA_VECTOR_H

#ifdef HAS_TRILINOS

#include <boost/shared_ptr.hpp>

#include <dolfin/log/LogStream.h>
#include <dolfin/common/Variable.h>
#include "GenericVector.h"

class Epetra_FEVector;
class Epetra_Map;

namespace dolfin
{

  /// This class provides a simple vector class based on Epetra.
  /// It is a simple wrapper for an Epetra vector object (Epetre_FEVector)
  /// implementing the GenericVector interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the Epetra_FEVector object using the function vec() or vec_ptr()
  /// and use the standard Epetra interface.

  class EpetraVector: public GenericVector, public Variable
  {
  public:

    /// Create empty vector
    EpetraVector();

    /// Create vector of size N
    explicit EpetraVector(uint N);

    /// Copy constructor
    explicit EpetraVector(const EpetraVector& x);

    /// Create vector view from given Epetra_FEVector pointer
    explicit EpetraVector(boost::shared_ptr<Epetra_FEVector> vector);

    /// Create vector from given Epetra_Map
    explicit EpetraVector(const Epetra_Map& map);

    /// Destructor
    virtual ~EpetraVector();

    //--- Implementation of the GenericTensor interface ---

    /// Return copy of tensor
    virtual EpetraVector* copy() const;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply();

    /// Display vector
    virtual void disp(uint precision = 2) const;

    //--- Implementation of the GenericVector interface ---

    /// Resize vector to size N
    virtual void resize(uint N);

    /// Return size of vector
    virtual uint size() const;

    /// Get block of values
    virtual void get(double* block, uint m, const uint* rows) const;

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows);

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows);

    /// Get all values
    virtual void get(double* values) const;

    /// Set all values
    virtual void set(double* values);

    /// Add all values to each entry
    virtual void add(double* values);

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(double a, const GenericVector& x);

    /// Return inner product with given vector
    virtual double inner(const GenericVector& vector) const;

    /// Return norm of vector
    virtual double norm(dolfin::NormType type=l2) const;

    /// Return minimum value of vector
    virtual double min() const;

    /// Return maximum value of vector
    virtual double max() const;

    /// Return sum of values of vector
    virtual double sum() const;

    /// Multiply vector by given number
    virtual const EpetraVector& operator*= (double a);

    /// Multiply vector by another vector pointwise
    virtual const EpetraVector& operator*= (const GenericVector& x);

    /// Divide vector by given number
    virtual const EpetraVector& operator/= (double a);

    /// Add given vector
    virtual const EpetraVector& operator+= (const GenericVector& x);

    /// Subtract given vector
    virtual const EpetraVector& operator-= (const GenericVector& x);

    /// Assignment operator
    virtual const EpetraVector& operator= (const GenericVector& x);

    /// Assignment operator
    virtual const EpetraVector& operator= (double a);

    //--- Special functions ---

    /// Return linear algebra backend factory
    virtual LinearAlgebraFactory& factory() const;

    //--- Special Epetra functions ---

    /// Return Epetra_FEVector pointer
    boost::shared_ptr<Epetra_FEVector> vec() const;

    /// Assignment operator
    const EpetraVector& operator= (const EpetraVector& x);

  private:

    // Epetra_FEVector pointer
    boost::shared_ptr<Epetra_FEVector> x;

  };

  LogStream& operator<< (LogStream& stream, const EpetraVector& A);

}

#endif
#endif
