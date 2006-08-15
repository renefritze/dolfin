// Copyright (C) 2005 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2006.
//
// First added:  2005-12-02
// Last changed: 2006-08-15

#include <dolfin/GMRES.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
GMRES::GMRES() : KrylovSolver(gmres)
{
  // Do nothing.
}
//-----------------------------------------------------------------------------
#ifdef HAVE_PETSC_H
GMRES::GMRES(Preconditioner pc) : KrylovSolver(gmres, pc)
{
  // Do nothing.
}
//-----------------------------------------------------------------------------
GMRES::GMRES(PETScPreconditioner& preconditioner) : KrylovSolver(gmres, preconditioner)
{
  // Do nothing.
}
//-----------------------------------------------------------------------------
#endif
GMRES::~GMRES()
{
  // Do nothing.
}
//-----------------------------------------------------------------------------

