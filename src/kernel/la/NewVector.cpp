// Copyright (C) 2004 Johan Hoffman, Johan Jansson  and Anders Logg.
// Licensed under the GNU GPL Version 2.
//

#include <dolfin/dolfin_math.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/PETScManager.h>
#include <dolfin/NewVector.h>
#include <cmath>

using namespace dolfin;

//-----------------------------------------------------------------------------
NewVector::NewVector()
{
  // Initialize PETSc
  PETScManager::init();

  // Don't initialize the vector
  v = 0;
}
//-----------------------------------------------------------------------------
NewVector::NewVector(int size)
{
  if(size < 0)
    dolfin_error("Size of vector must be non-negative.");

  // Initialize PETSc
  PETScManager::init();

  // Create PETSc vector
  v = 0;
  init(static_cast<unsigned int>(size));
}
//-----------------------------------------------------------------------------
NewVector::~NewVector()
{
  clear();
}
//-----------------------------------------------------------------------------
void NewVector::init(unsigned int size)
{
  // Two cases:
  //
  //   1. Already allocated and dimension changes -> reallocate
  //   2. Not allocated -> allocate
  //
  // Otherwise do nothing
  
  if(v)
  {
    if(n == size)
    {
      return;      
    }
  }
  else
  {
    clear();
  }

  VecCreate(PETSC_COMM_WORLD, &v);
  VecSetSizes(v, PETSC_DECIDE, size);
  VecSetFromOptions(v);
}
//-----------------------------------------------------------------------------
void NewVector::clear()
{
  if(v)
  {
    VecDestroy(v);
  }

  v = 0;
}
//-----------------------------------------------------------------------------
unsigned int NewVector::size() const
{
  return n;
}
//-----------------------------------------------------------------------------
Vec NewVector::vec()
{
  return v;
}
//-----------------------------------------------------------------------------
const Vec NewVector::vec() const
{
  return v;
}
//-----------------------------------------------------------------------------
/*
real NewVector::operator()(unsigned int i) const
{
  // Assumes uniprocessor case.

  PetscScalar    *array;
  VecGetArray(v, &array);
  VecRestoreArray(v, &array);

  real val = array[i];

  return val;
}
*/
//-----------------------------------------------------------------------------
NewVector::Index NewVector::operator()(unsigned int i)
{
  Index ind(i, *this);

  return ind;
}
//-----------------------------------------------------------------------------
void NewVector::disp() const
{
  VecView(v, PETSC_VIEWER_STDOUT_SELF);
}
//-----------------------------------------------------------------------------
NewVector::Index::Index(unsigned int i, NewVector &v) : i(i), v(v)
{
}
//-----------------------------------------------------------------------------
void NewVector::Index::operator =(const real r)
{
  Vec petscv;
  petscv = v.vec();

  VecSetValue(petscv, i, r, INSERT_VALUES);
}
//-----------------------------------------------------------------------------
NewVector::Index::operator real() const
{
  // Assumes uniprocessor case.

  Vec petscv;
  petscv = v.vec();

  PetscScalar    *array;
  VecGetArray(petscv, &array);
  real val = array[i];
  VecRestoreArray(petscv, &array);

  return val;
}
//-----------------------------------------------------------------------------
