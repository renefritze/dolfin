// Copyright (C) 2004 Johan Jansson.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2005.

#include <dolfin/dolfin_math.h>
#include <dolfin/dolfin_log.h>
#include <dolfin/PETScManager.h>
#include <dolfin/Vector.h>
#include <cmath>

using namespace dolfin;

//-----------------------------------------------------------------------------
Vector::Vector() : x(0), copy(false)
{
  // Initialize PETSc
  PETScManager::init();
}
//-----------------------------------------------------------------------------
Vector::Vector(uint size) : x(0), copy(false)
{
  if(size < 0)
    dolfin_error("Size of vector must be non-negative.");

  // Initialize PETSc
  PETScManager::init();

  // Create PETSc vector
  init(size);
}
//-----------------------------------------------------------------------------
Vector::Vector(Vec x) : x(x), copy(true)
{
  // Initialize PETSc 
  PETScManager::init();
}
//-----------------------------------------------------------------------------
Vector::Vector(const Vector& v) : x(0), copy(false)
{
  // Initialize PETSc 
  PETScManager::init();

  *this = v;
}
//-----------------------------------------------------------------------------
Vector::~Vector()
{
  clear();
}
//-----------------------------------------------------------------------------
void Vector::init(uint size)
{
  // Two cases:
  //
  //   1. Already allocated and dimension changes -> reallocate
  //   2. Not allocated -> allocate
  //
  // Otherwise do nothing
  
  if (x)
  {
    const uint n = this->size();

    if (n == size)
    {
      return;      
    }
  }
  else
  {
    clear();
  }

  // Create vector
  VecCreate(PETSC_COMM_SELF, &x);
  VecSetSizes(x, PETSC_DECIDE, size);
  VecSetFromOptions(x);

  // Set all entries to zero
  real a = 0.0;
  VecSet(&a, x);
}
//-----------------------------------------------------------------------------
void Vector::axpy(const real a, const Vector& x) const
{
  VecAXPY(&a, x.vec(), this->x);
}
//-----------------------------------------------------------------------------
void Vector::add(const real block[], const int cols[], int n)
{
  VecSetValues(x, n, cols, block, ADD_VALUES); 
}
//-----------------------------------------------------------------------------
void Vector::apply()
{
  VecAssemblyBegin(x);
  VecAssemblyEnd(x);
}
//-----------------------------------------------------------------------------
void Vector::clear()
{
  if ( x && !copy )
  {
    VecDestroy(x);
  }

  x = 0;
}
//-----------------------------------------------------------------------------
dolfin::uint Vector::size() const
{
  int n = 0;
  VecGetSize(x, &n);

  return static_cast<uint>(n);
}
//-----------------------------------------------------------------------------
Vec Vector::vec()
{
  return x;
}
//-----------------------------------------------------------------------------
const Vec Vector::vec() const
{
  return x;
}
//-----------------------------------------------------------------------------
real* Vector::array()
{
  dolfin_assert(x);

  real* data = 0;
  VecGetArray(x, &data);
  dolfin_assert(data);

  return data;
}
//-----------------------------------------------------------------------------
const real* Vector::array() const
{
  dolfin_assert(x);

  real* data = 0;
  VecGetArray(x, &data);

  return data;
}
//-----------------------------------------------------------------------------
void Vector::restore(real data[])
{
  dolfin_assert(x);

  VecRestoreArray(x, &data);
}
//-----------------------------------------------------------------------------
void Vector::restore(const real data[]) const
{
  dolfin_assert(x);

  // Cast away the constness and trust PETSc to do the right thing
  real* tmp = const_cast<real *>(data);
  VecRestoreArray(x, &tmp);
}
//-----------------------------------------------------------------------------
real Vector::operator() (uint i) const
{
  return getval(i);
}
//-----------------------------------------------------------------------------
Vector::Element Vector::operator() (uint i)
{
  Element index(i, *this);
  return index;
}
//-----------------------------------------------------------------------------
const Vector& Vector::operator= (const Vector& x)
{
  if ( !x.x )
  {
    clear();
    return *this;
  }

  init(x.size());
  VecCopy(x.x, this->x);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector& Vector::operator= (real a)
{
  dolfin_assert(x);
  VecSet(&a, x);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector& Vector::operator+= (const Vector& x)
{
  dolfin_assert(x.x);
  dolfin_assert(this->x);

  const real a = 1.0;
  VecAXPY(&a, x.x, this->x);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector& Vector::operator-= (const Vector& x)
{
  dolfin_assert(x.x);
  dolfin_assert(this->x);

  const real a = -1.0;
  VecAXPY(&a, x.x, this->x);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector& Vector::operator*= (real a)
{
  dolfin_assert(x);
  VecScale(&a, x);
  
  return *this;
}
//-----------------------------------------------------------------------------
const Vector& Vector::operator/= (real a)
{
  dolfin_assert(x);
  dolfin_assert(a != 0.0);

  const real b = 1.0 / a;
  VecScale(&b, x);
  
  return *this;
}
//-----------------------------------------------------------------------------
real Vector::norm(NormType type) const
{
  dolfin_assert(x);

  real value = 0.0;

  switch (type) {
  case l1:
    VecNorm(x, NORM_1, &value);
    break;
  case l2:
    VecNorm(x, NORM_2, &value);
    break;
  default:
    VecNorm(x, NORM_INFINITY, &value);
  }
  
  return value;
}
//-----------------------------------------------------------------------------
void Vector::disp() const
{
  // FIXME: Maybe this could be an option?
  //VecView(x, PETSC_VIEWER_STDOUT_SELF);
 
  const uint M = size();
  cout << "[ ";
  for (uint i = 0; i < M; i++)
    cout << getval(i) << " ";
  cout << "]" << endl;
}
//-----------------------------------------------------------------------------
real Vector::getval(uint i) const
{
  dolfin_assert(x);

  // Assumes uniprocessor case

  real val = 0.0;

  PetscScalar *array = 0;
  VecGetArray(x, &array);
  val = array[i];
  VecRestoreArray(x, &array);

  return val;
}
//-----------------------------------------------------------------------------
void Vector::setval(uint i, const real a)
{
  dolfin_assert(x);

  VecSetValue(x, static_cast<int>(i), a, INSERT_VALUES);

  VecAssemblyBegin(x);
  VecAssemblyEnd(x);
}
//-----------------------------------------------------------------------------
void Vector::addval(uint i, const real a)
{
  dolfin_assert(x);

  VecSetValue(x, static_cast<int>(i), a, ADD_VALUES);

  VecAssemblyBegin(x);
  VecAssemblyEnd(x);
}
//-----------------------------------------------------------------------------
// Vector::Element
//-----------------------------------------------------------------------------
Vector::Element::Element(uint i, Vector& x) : i(i), x(x)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
Vector::Element::operator real() const
{
  return x.getval(i);
}
//-----------------------------------------------------------------------------
const Vector::Element& Vector::Element::operator=(const real a)
{
  x.setval(i, a);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector::Element& Vector::Element::operator+=(const real a)
{
  x.addval(i, a);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector::Element& Vector::Element::operator-=(const real a)
{
  x.addval(i, -a);

  return *this;
}
//-----------------------------------------------------------------------------
const Vector::Element& Vector::Element::operator*=(const real a)
{
  const real val = x.getval(i) * a;
  x.setval(i, val);

  return *this;
}
//-----------------------------------------------------------------------------
