// Copyright (C) 2004 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg, 2005.

#ifndef __NEW_VECTOR_H
#define __NEW_VECTOR_H

#include <petsc/petscvec.h>
#include <dolfin/constants.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/Vector.h>

namespace dolfin
{
  
  /// This class represents a vector of dimension n. It is a
  /// simple wrapper for a PETSc vector (Vec). The interface is
  /// intentionally simple. For advanced usage, access the PETSc Vec
  /// pointer using the function vec() and use the standard PETSc
  /// interface.

  class NewVector
  {
  public:

    class Index;

    /// Empty vector
    NewVector();

    /// Create vector of given size
    NewVector(uint size);

    /// Create vector from given PETSc Vec pointer
    NewVector(Vec x);

    /// Copy constructor
    NewVector(const NewVector& x);
    
    /// Create vector from old vector (will be removed)
    NewVector(const Vector& x);

    /// Destructor
    ~NewVector ();

    /// Initialize vector data
    void init(uint size);

    /// Clear vector data
    void clear();

    /// Return size of vector
    uint size() const;

    /// Return PETSc Vec pointer
    Vec vec();

    /// Return PETSc Vec pointer, const version
    const Vec vec() const;

    /// Return array containing this processor's portion of the data.
    /// After usage, the function restore() must be called.
    real* array();

    /// Return array containing this processor's portion of the data.
    /// After usage, the function restore() must be called. (const version)
    const real* array() const;

    /// Restore array after a call to array()
    void restore(real data[]);

    /// Restore array after a call to array(), const version
    void restore(const real data[]) const;

    /// Addition (AXPY)
    void axpy(const real a, const NewVector& x) const;

    /// Add block of values
    void add(const real block[],const int cols[], int n); 

    /// Apply changes to vector
    void apply();

    /// Element assignment operator
    Index operator() (uint i);

    /// Assignment of vecto
    const NewVector& operator= (const NewVector& x);

    /// Assignment of all elements to a single scalar value
    const NewVector& operator= (real a);

    /// Display vector
    void disp() const;

    /// Reference to a position in the vector
    class Index
    {
    public:
      Index(uint i, NewVector& v);
      operator real() const;
      void operator=(const real r);
    protected:
      uint i;
      NewVector& x;
    };

  protected:

    // Element assignment
    void setvalue(uint i, const real r);

    // Element access
    real getvalue(uint i) const;

  private:

    // PETSc Vec pointer
    Vec x;
    
    // True if the pointer is a copy of someone else's data
    bool copy;

  };
}

#endif
