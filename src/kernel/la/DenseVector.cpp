// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2006.
//
// First added:  2006-04-04
// Last changed: 2006-05-30

#include <dolfin/DenseVector.h>
#include <dolfin/dolfin_log.h>
#include <boost/numeric/ublas/vector.hpp>

using namespace dolfin;

//-----------------------------------------------------------------------------
DenseVector::DenseVector()
  : GenericVector(),
    Variable("x", "a dense vector"),
    ublas_vector()
{
  //Do nothing
}
//-----------------------------------------------------------------------------
DenseVector::DenseVector(uint N)
  : GenericVector(),
    Variable("x", "a dense vector"),
    ublas_vector(N)
{
  // Clear matrix (not done by ublas)
  clear();
}
//-----------------------------------------------------------------------------
//DenseVector::DenseVector(const DenseVector& x) : GenericVector<DenseVector>(), 
//    BaseVector(x), Variable("x", "a dense vector")
//{
//  //Do nothing
//}
//-----------------------------------------------------------------------------
DenseVector::~DenseVector()
{
  //Do nothing
}
//-----------------------------------------------------------------------------
void DenseVector::init(uint N)
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
void DenseVector::set(const real block[], const int pos[], int n)
{
  for(int i = 0; i < n; ++i)
    (*this)(pos[i]) = block[i];
}
//-----------------------------------------------------------------------------
void DenseVector::add(const real block[], const int pos[], int n)
{
  for(int i = 0; i < n; ++i)
    (*this)(pos[i]) += block[i];
}
//-----------------------------------------------------------------------------
void DenseVector::get(real block[], const int pos[], int n) const
{
  for(int i = 0; i < n; ++i)
    block[i] = (*this)(pos[i]);
}
//-----------------------------------------------------------------------------
real DenseVector::norm(NormType type) const
{
  switch (type) {
  case l1:
    return norm_1(*this);
  case l2:
    return norm_2(*this);
  case linf:
    return norm_inf(*this);
  default:
    dolfin_error("Requested vector norm type for DenseVector unknown");
  }
  return norm_inf(*this);
}
//-----------------------------------------------------------------------------
real DenseVector::sum() const
{
  return sum();
}
//-----------------------------------------------------------------------------
void DenseVector::apply()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void DenseVector::zero()
{
  clear();
}
//-----------------------------------------------------------------------------
const DenseVector& DenseVector::operator= (real a) 
{ 
  this->assign(ublas::scalar_vector<double> (this->size(), a));
  return *this;
}
//-----------------------------------------------------------------------------
void DenseVector::disp(uint precision) const
{
  std::cout.precision(precision+1);
  std::cout << *this << std::endl;
}
//-----------------------------------------------------------------------------
LogStream& dolfin::operator<< (LogStream& stream, const DenseVector& x)
{
  // Check if vector has been defined
  if ( x.size() == 0 )
  {
    stream << "[ DenseVector (empty) ]";
    return stream;
  }

  stream << "[ DenseVector of size " << x.size() << " ]";

  return stream;
}
//-----------------------------------------------------------------------------
