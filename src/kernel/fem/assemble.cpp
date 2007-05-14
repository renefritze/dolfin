// Copyright (C) 2007 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-01-17
// Last changed: 2007-05-14

#include <dolfin/Assembler.h>
#include <dolfin/assemble.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
void dolfin::assemble(GenericTensor& A, const Form& form, Mesh& mesh)
{
  Assembler assembler;
  assembler.assemble(A, form, mesh);
}
//-----------------------------------------------------------------------------
dolfin::real dolfin::assemble(const Form& form, Mesh& mesh)
{
  Assembler assembler;
  return assembler.assemble(form, mesh);
}
//-----------------------------------------------------------------------------
