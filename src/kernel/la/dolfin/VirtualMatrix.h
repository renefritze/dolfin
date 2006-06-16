// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Andy R. Terrel, 2005.
//
// First added:  2005-01-17
// Last changed: 2006-05-07

#ifndef __VIRTUAL_MATRIX_H
#define __VIRTUAL_MATRIX_H

#ifdef HAVE_PETSC_H

#include <dolfin/PETScManager.h>
#include <dolfin/constants.h>
#include <dolfin/dolfin_log.h>

namespace dolfin
{

  class PETScVector;
  
  /// This class represents a matrix-free matrix of dimension m x m.
  /// It is a simple wrapper for a PETSc shell matrix. The interface
  /// is intentionally simple. For advanced usage, access the PETSc
  /// Mat pointer using the function mat() and use the standard PETSc
  /// interface.
  ///
  /// The class VirtualMatrix enables the use of Krylov subspace
  /// methods for linear systems Ax = b, without having to explicitly
  /// store the matrix A. All that is needed is that the user-defined
  /// VirtualMatrix implements multiplication with vectors. Note that
  /// the multiplication operator needs to be defined in terms of
  /// PETSc data structures (Vec), since it will be called from PETSc.

  class VirtualMatrix
  {
  public:

    /// Constructor
    VirtualMatrix();

    /// Create a virtual matrix matching the given vectors
    VirtualMatrix(const PETScVector& x, const PETScVector& y);

    /// Destructor
    virtual ~VirtualMatrix();

    /// Initialize virtual matrix matching the given vectors
    void init(const PETScVector& x, const PETScVector& y);

    /// Initialize virtual matrix of given size, all on one processor
    void init(int M, int N);

    /// Return number of rows (dim = 0) or columns (dim = 1) along dimension dim
    uint size(uint dim) const;

    /// Return PETSc Mat pointer
    Mat mat() const;

    /// Compute product y = Ax
    virtual void mult(const PETScVector& x, PETScVector& y) const = 0;

    /// Display matrix (sparse output is default)
    void disp(bool sparse = true, int precision = 2) const;

    /// Output
    friend LogStream& operator<< (LogStream& stream, const VirtualMatrix& A);

  private:

    // PETSc Mat pointer
    Mat A;

  };

}

#endif

#endif
