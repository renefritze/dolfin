// Copyright (C) 2007 Magnus Vikstrøm.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2007, 2008.
// Modified by Anders Logg, 2007.
// Modified by Ola Skavhaug, 2008.
//
// First added:  2007-11-30
// Last changed: 2008-08-14

#include "MPI.h"
#include "SubSystemsManager.h"

#ifdef HAS_MPI

#include <mpi.h>

using MPI::COMM_WORLD;

//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::processNumber()
{
  SubSystemsManager::initMPI();
  return static_cast<uint>(COMM_WORLD.Get_rank());
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::numProcesses()
{
  SubSystemsManager::initMPI();
  return static_cast<uint>(COMM_WORLD.Get_size());
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::broadcast()
{
  // Always broadcast from processor number 0
  return numProcesses() > 1 && processNumber() == 0;
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::receive()
{
  // Always receive on processors with numbers > 0
  return numProcesses() > 1 && processNumber() > 0;
}
//-----------------------------------------------------------------------------

#else

//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::processNumber()
{
  return 0;
}
//-----------------------------------------------------------------------------
dolfin::uint dolfin::MPI::numProcesses()
{
  return 1;
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::broadcast()
{
  return false;
}
//-----------------------------------------------------------------------------
bool dolfin::MPI::receive()
{
  return false;
}
//-----------------------------------------------------------------------------

#endif
