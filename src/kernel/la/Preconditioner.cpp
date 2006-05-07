// Copyright (C) 2005 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Garth N. Wells 2005.
// Modified by Anders Logg 2006.
//
// First added:  2005
// Last changed: 2006-05-07

#ifdef HAVE_PETSC_H

#include <petsc.h>
#if PETSC_VERSION_MAJOR==2 && PETSC_VERSION_MINOR==3 && PETSC_VERSION_SUBMINOR==0
  #include <src/ksp/pc/pcimpl.h>
#else
  #include <private/pcimpl.h>
#endif

#include <dolfin/Preconditioner.h>
#include <dolfin/Vector.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Preconditioner::Preconditioner()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Preconditioner::~Preconditioner()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void Preconditioner::setup(const KSP ksp, Preconditioner &pc)
{
  PC petscpc;
  KSPGetPC(ksp, &petscpc);

  Preconditioner::PCCreate(petscpc);

  petscpc->data = &pc;
  petscpc->ops->apply = Preconditioner::PCApply;
  petscpc->ops->applytranspose = Preconditioner::PCApply;
  petscpc->ops->applysymmetricleft = Preconditioner::PCApply;
  petscpc->ops->applysymmetricright = Preconditioner::PCApply;
}
//-----------------------------------------------------------------------------
int Preconditioner::PCApply(PC pc, Vec x, Vec y)
{
  // Convert vectors to DOLFIN wrapper format and pass to DOLFIN preconditioner

  Preconditioner* newpc = (Preconditioner*)pc->data;

  Vector dolfinx(x), dolfiny(y);

  newpc->solve(dolfiny, dolfinx);

  return 0;
}
//-----------------------------------------------------------------------------
int Preconditioner::PCCreate(PC pc)
{
  // Initialize function pointers to 0

  pc->ops->setup               = 0;
  pc->ops->apply               = 0;
  pc->ops->applyrichardson     = 0;
  pc->ops->applytranspose      = 0;
  pc->ops->applysymmetricleft  = 0;
  pc->ops->applysymmetricright = 0;
  pc->ops->setfromoptions      = 0;
  pc->ops->view                = 0;
  pc->ops->destroy             = 0;

  // Set PETSc name of preconditioner
  PetscObjectChangeTypeName((PetscObject)pc, "DOLFIN");

  return 0;
}
//-----------------------------------------------------------------------------
PCType Preconditioner::getType(const Type type)
{
  switch (type)
  {
  case default_pc:
    return "default";
  case hypre_amg:
    return PCHYPRE;
  case icc:
    return PCICC;
  case ilu:
    return PCILU;
  case jacobi:
    return PCJACOBI;
  case sor:
    return PCSOR;
  case none:
    return PCNONE;
  default:
    dolfin_warning("Requested preconditioner unkown. Using incomplete LU.");
    return PCILU;
  }
}
//-----------------------------------------------------------------------------

#endif
