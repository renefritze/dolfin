// Copyright (C) 2007-2008 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2007-01-17
// Last changed: 2008-07-22

#include "Assembler.h"
#include "assemble.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
void dolfin::assemble(GenericTensor& A, Form& form, Mesh& mesh,
                      bool reset_tensor)
{
  Assembler assembler(mesh);
  assembler.assemble(A, form, reset_tensor);
}
//-----------------------------------------------------------------------------
void dolfin::assemble(GenericTensor& A, Form& form, Mesh& mesh,
                      const SubDomain& sub_domain,
                      bool reset_tensor)
{
  Assembler assembler(mesh);
  assembler.assemble(A, form, sub_domain, reset_tensor);
}
//-----------------------------------------------------------------------------
void dolfin::assemble(GenericTensor& A, Form& form, Mesh& mesh,
                      const MeshFunction<dolfin::uint>& cell_domains,
                      const MeshFunction<dolfin::uint>& exterior_facet_domains,
                      const MeshFunction<dolfin::uint>& interior_facet_domains,
                      bool reset_tensor)
{
  Assembler assembler(mesh);
  assembler.assemble(A, form,
                     cell_domains,
                     exterior_facet_domains,
                     interior_facet_domains,
                     reset_tensor);
}
//-----------------------------------------------------------------------------
dolfin::real dolfin::assemble(Form& form, Mesh& mesh,
                              bool reset_tensor)
{
  Assembler assembler(mesh);
  return assembler.assemble(form, reset_tensor);
}
//-----------------------------------------------------------------------------
dolfin::real dolfin::assemble(Form& form, Mesh& mesh, const SubDomain& sub_domain,
                              bool reset_tensor)
{
  Assembler assembler(mesh);
  return assembler.assemble(form, sub_domain, reset_tensor);
}
//-----------------------------------------------------------------------------
dolfin::real dolfin::assemble(Form& form, Mesh& mesh,
                              const MeshFunction<dolfin::uint>& cell_domains,
                              const MeshFunction<dolfin::uint>& exterior_facet_domains,
                              const MeshFunction<dolfin::uint>& interior_facet_domains,
                              bool reset_tensor)
{
  Assembler assembler(mesh);
  return assembler.assemble(form,
                            cell_domains,
                            exterior_facet_domains,
                            interior_facet_domains,
                            reset_tensor);
}
//----------------------------------------------------------------------------
void dolfin::assemble(GenericTensor& A, const ufc::form& form, Mesh& mesh, 
                      Array<Function*>& coefficients,
                      DofMapSet& dof_map_set,
                      const MeshFunction<uint>* cell_domains,
                      const MeshFunction<uint>* exterior_facet_domains,
                      const MeshFunction<uint>* interior_facet_domains,
                      bool reset_tensor)
{
  Assembler assembler(mesh);
  assembler.assemble(A, form, coefficients, dof_map_set,
                     cell_domains, exterior_facet_domains, interior_facet_domains,
                     reset_tensor);
}
//----------------------------------------------------------------------------
void dolfin::assemble_system(GenericTensor& A, const ufc::form& A_form, 
                             const Array<Function*>& A_coefficients, const DofMapSet& A_dof_map_set,
                             GenericTensor& b, const ufc::form& b_form, 
                             const Array<Function*>& b_coefficients, const DofMapSet& b_dof_map_set,
                             Mesh& mesh, 
                             DirichletBC& bc, const MeshFunction<uint>* cell_domains, 
                             const MeshFunction<uint>* exterior_facet_domains,
                             const MeshFunction<uint>* interior_facet_domains,
                             bool reset_tensors)
{
  Assembler assembler(mesh);
  assembler.assemble_system(A, A_form, A_coefficients, A_dof_map_set,
                            b, b_form, b_coefficients, b_dof_map_set,
                            bc, cell_domains, exterior_facet_domains, 
                            interior_facet_domains, reset_tensors);
}
//----------------------------------------------------------------------------
