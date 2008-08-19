// Copyright (C) 2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2006-07-04
// Last changed: 2006-07-04

#ifndef __UBLAS_DUMMY_PRECONDITIONER_H
#define __UBLAS_DUMMY_PRECONDITIONER_H

#include "uBlasPreconditioner.h"

namespace dolfin
{

  /// This class provides a dummy (do nothing) preconditioner for the
  /// uBlas Krylov solver.

  class uBlasDummyPreconditioner : public uBlasPreconditioner
  {
  public:

    /// Constructor
    uBlasDummyPreconditioner();

    /// Destructor
    ~uBlasDummyPreconditioner();

    /// Solve linear system Ax = b approximately
    void solve(uBlasVector& x, const uBlasVector& b) const;

  };

}

#endif
