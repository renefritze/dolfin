// Copyright (C) 2005-2006 Anders Logg.
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2005-02-13
// Last changed: 2006-05-07

#include <dolfin/common/constants.h>
#include <dolfin/log/dolfin_log.h>
#include "SubSystemsManager.h"
#include "init.h"

//-----------------------------------------------------------------------------
void dolfin::dolfin_init(int argc, char* argv[])
{
  info("Initializing DOLFIN version %s.", DOLFIN_VERSION);

#ifdef HAS_PETSC
  SubSystemsManager::init_petsc(argc, argv);
#endif
}
//-----------------------------------------------------------------------------
