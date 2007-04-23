// Copyright (C) 2006 Garth N. Wells
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg 2006.
//
// First added:  2006-05-29
// Last changed: 2006-08-15

#ifndef __SPARSE_VECTOR_H
#define __SPARSE_VECTOR_H

#ifdef HAVE_PETSC_H
#include <dolfin/PETScVector.h>
#endif

#include <dolfin/uBlasVector.h>

namespace dolfin
{

  /// SparseVector is a synonym PETScVector if PETSc is enabled, otherwise 
  /// it's uBlasSparseVector.
  
#ifdef HAVE_PETSC_H
  typedef PETScVector SparseVector;
#else
  typedef uBlasVector SparseVector;
#endif

}

#endif
