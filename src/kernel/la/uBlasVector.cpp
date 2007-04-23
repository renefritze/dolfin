// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg 2006-2007.
//
// First added:  2006-04-04
// Last changed: 2007-04-16

#include <dolfin/dolfin_log.h>
#include <boost/numeric/ublas/vector.hpp>
#include <dolfin/uBlasVector.h>

#ifdef HAVE_PETSC_H
#include <dolfin/PETScVector.h>
#endif

using namespace dolfin;

//-----------------------------------------------------------------------------
uBlasVector::uBlasVector()
  : GenericVector(),
    Variable("x", "a dense vector"),
    ublas_vector()
{
  //Do nothing
}
//-----------------------------------------------------------------------------
uBlasVector::uBlasVector(uint N)
  : GenericVector(),
    Variable("x", "a dense vector"),
    ublas_vector(N)
{
  // Clear matrix (not done by ublas)
  clear();
}
//-----------------------------------------------------------------------------
uBlasVector::~uBlasVector()
{
  //Do nothing
}
//-----------------------------------------------------------------------------
void uBlasVector::init(uint N)
{
  if( this->size() == N)
  {
    clear();
    return;
  }
  
  this->resize(N, false);
  clear();
}
//-----------------------------------------------------------------------------
void uBlasVector::init(const SparsityPattern& sparsity_pattern)
{
  dolfin_error("Cannot initialise a uBLAS vector using a sparsity pattern.");
}
//-----------------------------------------------------------------------------
void uBlasVector::get(real* block, uint m, const uint* rows) const
{
  for (uint i = 0; i < m; i++)
    block[i] = (*this)(rows[i]);
}
//-----------------------------------------------------------------------------
void uBlasVector::set(const real* block, uint m, const uint* rows)
{
  for (uint i = 0; i < m; i++)
    (*this)(rows[i]) = block[i];
}
//-----------------------------------------------------------------------------
void uBlasVector::add(const real* block, uint m, const uint* rows)
{
  for (uint i = 0; i < m; i++)
    (*this)(rows[i]) += block[i];
}
//-----------------------------------------------------------------------------
void uBlasVector::apply()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void uBlasVector::zero()
{
  clear();
}
//-----------------------------------------------------------------------------
real uBlasVector::norm(NormType type) const
{
  switch (type) {
  case l1:
    return norm_1(*this);
  case l2:
    return norm_2(*this);
  case linf:
    return norm_inf(*this);
  default:
    dolfin_error("Requested vector norm type for uBlasVector unknown");
  }
  return norm_inf(*this);
}
void uBlasVector::div(const uBlasVector& y)
{
  uBlasVector& x = *this;
  uint s = size();

  for(uint i = 0; i < s; i++)
  {
    x[i] = x[i] / y[i];
  }
}
//-----------------------------------------------------------------------------
void uBlasVector::axpy(real a, const uBlasVector& x)
{
  uBlasVector& y = *this;
  
  y += a * x;
}
//-----------------------------------------------------------------------------
void uBlasVector::mult(real a)
{
  uBlasVector& y = *this;
  
  y *= a;
}
//-----------------------------------------------------------------------------
const uBlasVector& uBlasVector::operator= (real a) 
{ 
  this->assign(ublas::scalar_vector<double> (this->size(), a));
  return *this;
}
//-----------------------------------------------------------------------------
void uBlasVector::disp(uint precision) const
{
  dolfin::cout << "[ ";
  for (uint i = 0; i < size(); i++)
    cout << (*this)(i) << " ";
  dolfin::cout << "]" << endl;
}
//-----------------------------------------------------------------------------
LogStream& dolfin::operator<< (LogStream& stream, const uBlasVector& x)
{
  // Check if vector has been defined
  if ( x.size() == 0 )
  {
    stream << "[ uBlasVector (empty) ]";
    return stream;
  }

  stream << "[ uBlasVector of size " << x.size() << " ]";

  return stream;
}
//-----------------------------------------------------------------------------
#ifdef HAVE_PETSC_H
void uBlasVector::copy(const PETScVector& y, uint off1, uint off2, uint len)
{
  // FIXME: Verify if there's a more efficient implementation

  uBlasVector& x = *this;
  const real* vals = 0;
  vals = y.array();
  for(uint i = 0; i < len; i++)
  {
    x[i + off1] = vals[i + off2];
  }
  y.restore(vals);
}
#endif
//-----------------------------------------------------------------------------
void uBlasVector::copy(const uBlasVector& y, uint off1, uint off2, uint len)
{
  uBlasVector& x = *this;

  subrange(x, off1, off1 + len) = subrange(y, off2, off2 + len);
}
//-----------------------------------------------------------------------------
