// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// First added:  2005-10-24
// Last changed: 2006-02-23

#include <dolfin/dolfin_log.h>
#include <dolfin/NonlinearProblem.h>

#ifdef HAVE_PETSC_H

using namespace dolfin;

NonlinearProblem::NonlinearProblem()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
NonlinearProblem::~NonlinearProblem()
{
  // Do nothing 
}
//-----------------------------------------------------------------------------
void NonlinearProblem::form(Matrix& A, Vector& b, const Vector& x)
{
  dolfin_error("Nonlinear problem update for F(u) and J  has not been supplied by user.");
}
//-----------------------------------------------------------------------------
void NonlinearProblem::F(Vector& b, const Vector& x)
{
  dolfin_error("Nonlinear problem update for F(u)  has not been supplied by user.");
}
//-----------------------------------------------------------------------------
void NonlinearProblem::J(Matrix& A, const Vector& x)
{
  dolfin_error("Nonlinear problem update for Jacobian has not been supplied by user.");
}
//-----------------------------------------------------------------------------

#endif
