// Copyright (C) 2006-2008 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg 2006-2008.
// Modified by Kent-Andre Mardal 2008.
// Modified by Martin Sandve Alnes 2008.
//
// First added:  2006-04-04
// Last changed: 2009-08-11

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <dolfin/log/dolfin_log.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_expression.hpp>
#include "uBLASVector.h"
#include "uBLASFactory.h"
#include "LinearAlgebraFactory.h"

#ifdef HAS_PETSC
#include "PETScVector.h"
#endif

using namespace dolfin;

//-----------------------------------------------------------------------------
uBLASVector::uBLASVector(): x(new ublas_vector(0))
{
  // Do nothing
}
//-----------------------------------------------------------------------------
uBLASVector::uBLASVector(uint N): x(new ublas_vector(N))
{
  // Clear vector
  x->clear();
}
//-----------------------------------------------------------------------------
uBLASVector::uBLASVector(const uBLASVector& x): x(new ublas_vector(*(x.x)))
{
  //Do nothing
}
//-----------------------------------------------------------------------------
uBLASVector::uBLASVector(boost::shared_ptr<ublas_vector> x) : x(x)
{
  //Do nothing
}
//-----------------------------------------------------------------------------
uBLASVector::~uBLASVector()
{
  //Do nothing
}
//-----------------------------------------------------------------------------
void uBLASVector::resize(uint N)
{
  if(x->size() == N)
    return;

  x->resize(N, false);
}
//-----------------------------------------------------------------------------
dolfin::uint uBLASVector::size() const
{
  return x->size();
}
//-----------------------------------------------------------------------------
std::pair<dolfin::uint, dolfin::uint> uBLASVector::local_range() const
{
  return std::make_pair(0, size());
}
//-----------------------------------------------------------------------------
uBLASVector* uBLASVector::copy() const
{
  return new uBLASVector(*this);
}
//-----------------------------------------------------------------------------
void uBLASVector::get_local(double* values) const
{
  for (uint i = 0; i < size(); i++)
    values[i] = (*x)(i);
}
//-----------------------------------------------------------------------------
void uBLASVector::set_local(const double* values)
{
  for (uint i = 0; i < size(); i++)
    (*x)(i) = values[i];
}
//-----------------------------------------------------------------------------
void uBLASVector::add_local(const double* values)
{
  for (uint i = 0; i < size(); i++)
    (*x)(i) += values[i];
}
//-----------------------------------------------------------------------------
void uBLASVector::get(double* block, uint m, const uint* rows) const
{
  for (uint i = 0; i < m; i++)
    block[i] = (*x)(rows[i]);
}
//-----------------------------------------------------------------------------
void uBLASVector::set(const double* block, uint m, const uint* rows)
{
  for (uint i = 0; i < m; i++)
    (*x)(rows[i]) = block[i];
}
//-----------------------------------------------------------------------------
void uBLASVector::add(const double* block, uint m, const uint* rows)
{
  for (uint i = 0; i < m; i++)
    (*x)(rows[i]) += block[i];
}
//-----------------------------------------------------------------------------
void uBLASVector::apply()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void uBLASVector::zero()
{
  x->clear();
}
//-----------------------------------------------------------------------------
double uBLASVector::norm(std::string norm_type) const
{
  if (norm_type == "l1")
    return norm_1(*x);
  else if (norm_type == "l2")
    return norm_2(*x);
  else if (norm_type == "linf")
    return norm_inf(*x);
  else
    error("Requested vector norm type for uBLASVector unknown");

  return 0.0;
}
//-----------------------------------------------------------------------------
double uBLASVector::min() const
{
  double value = *std::min_element(x->begin(), x->end());
  return value;
}
//-----------------------------------------------------------------------------
double uBLASVector::max() const
{
  double value = *std::max_element(x->begin(), x->end());
  return value;
}
//-----------------------------------------------------------------------------
double uBLASVector::sum() const
{
  return ublas::sum(*x);
}
//-----------------------------------------------------------------------------
void uBLASVector::axpy(double a, const GenericVector& y)
{
  if ( size() != y.size() )
    error("Vectors must be of same size.");

  (*x) += a * y.down_cast<uBLASVector>().vec();
}
//-----------------------------------------------------------------------------
double uBLASVector::inner(const GenericVector& y) const
{
  return ublas::inner_prod(*x, y.down_cast<uBLASVector>().vec());
}
//-----------------------------------------------------------------------------
const GenericVector& uBLASVector::operator= (const GenericVector& y)
{
  *x = y.down_cast<uBLASVector>().vec();
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator= (const uBLASVector& y)
{
  *x = y.vec();
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator= (double a)
{
  x->ublas_vector::assign(ublas::scalar_vector<double> (x->size(), a));
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator*= (const double a)
{
  (*x) *= a;
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator*= (const GenericVector& y)
{
  *x = ublas::element_prod(*x,y.down_cast<uBLASVector>().vec());
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator/= (const double a)
{
  (*x) /= a;
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator+= (const GenericVector& y)
{
  *x += y.down_cast<uBLASVector>().vec();
  return *this;
}
//-----------------------------------------------------------------------------
const uBLASVector& uBLASVector::operator-= (const GenericVector& y)
{
  *x -= y.down_cast<uBLASVector>().vec();
  return *this;
}
//-----------------------------------------------------------------------------
std::string uBLASVector::str(bool verbose) const
{
  std::stringstream s;

  if (verbose)
  {
    s << str(false) << std::endl << std::endl;

    s << "[";
    for (ublas_vector::const_iterator it = x->begin(); it != x->end(); ++it)
    {
      std::stringstream entry;
      entry << std::setiosflags(std::ios::scientific);
      entry << std::setprecision(16);
      entry << *it << " ";
      s << entry.str() << std::endl;
    }
    s << "]";
  }
  else
  {
    s << "<uBLASVector of size " << size() << ">";
  }

  return s.str();
}
//-----------------------------------------------------------------------------
LinearAlgebraFactory& uBLASVector::factory() const
{
  return uBLASFactory<>::instance();
}
//-----------------------------------------------------------------------------
