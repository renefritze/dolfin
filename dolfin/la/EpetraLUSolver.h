// Copyright (C) 2008 Kent-Andre Mardal.
// Licensed under the GNU LGPL Version 2.1.
//
// Last changed: 2009-09-08

#ifdef HAS_TRILINOS
#ifndef __EPETRA_LU_SOLVER_H
#define __EPETRA_LU_SOLVER_H

#include "GenericLinearSolver.h"

namespace dolfin
{
  /// Forward declarations
  class GenericMatrix;
  class GenericVector;
  class EpetraMatrix;
  class EpetraVector;

  /// This class implements the direct solution (LU factorization) for
  /// linear systems of the form Ax = b. It is a wrapper for the LU
  /// solver of Epetra.

  class EpetraLUSolver : public GenericLinearSolver
  {
  public:

    /// Constructor
    EpetraLUSolver();

    /// Destructor
    ~EpetraLUSolver();

    /// Solve linear system Ax = b
    uint solve(const GenericMatrix& A, GenericVector& x, const GenericVector& b);

    /// Solve linear system Ax = b
    uint solve(const EpetraMatrix& A, EpetraVector& x, const EpetraVector& b);

    /// Default parameter values
    static Parameters default_parameters();

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

  };

}

#endif





#endif


