// Copyright (C) 2005 Johan Hoffman and Anders Logg.
// Licensed under the GNU GPL Version 2.

#include <dolfin/NewPreconditioner.h>
#include <dolfin/NewVector.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
NewPreconditioner::NewPreconditioner()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
NewPreconditioner::~NewPreconditioner()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
int NewPreconditioner::PCApply(PC pc, Vec x, Vec y)
{
  NewPreconditioner* newpc = (NewPreconditioner*)pc->data;

  NewVector dolfinx(x), dolfiny(y);

  newpc->solve(dolfinx, dolfiny);

  return 0;
}
//-----------------------------------------------------------------------------
int NewPreconditioner::PCCreate(PC pc)
{
  pc->ops->setup               = (int (*)(PC)) 0;
  pc->ops->apply               = (int (*)(PC,Vec,Vec)) 0;
  pc->ops->applyrichardson     = (int (*)(PC,Vec,Vec,Vec,double,double,double,int)) 0;
  pc->ops->applyBA             = (int (*)(PC,int,Vec,Vec,Vec)) 0;
  pc->ops->applytranspose      = (int (*)(PC,Vec,Vec)) 0;
  pc->ops->applyBAtranspose    = (int (*)(PC,int,Vec,Vec,Vec)) 0;
  pc->ops->setfromoptions      = (int (*)(PC)) 0;
  pc->ops->presolve            = (int (*)(PC,KSP,Vec,Vec)) 0;
  pc->ops->postsolve           = (int (*)(PC,KSP,Vec,Vec)) 0;
  pc->ops->getfactoredmatrix   = (int (*)(PC,Mat*)) 0;
  pc->ops->applysymmetricleft  = (int (*)(PC,Vec,Vec)) 0;
  pc->ops->applysymmetricright = (int (*)(PC,Vec,Vec)) 0;
  pc->ops->setuponblocks       = (int (*)(PC)) 0;
  pc->ops->destroy             = (int (*)(PC)) 0;
  pc->ops->view                = (int (*)(PC,PetscViewer)) 0;
  //pc->modifysubmatrices        = (int (*)(PC,int,const IS[],const IS[],Mat[],void*)) 0;

  pc->ops->destroy = 0;
  pc->ops->setup = 0;

  std::string pcname("DOLFIN");

  pc->type_name = strdup(pcname.c_str());

  return 0;
}
//-----------------------------------------------------------------------------
