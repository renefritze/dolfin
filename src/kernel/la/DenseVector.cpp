// Copyright (C) 2006 Garth N. Wells.
// Licensed under the GNU GPL Version 2.
//
// Modified by Anders Logg 2006.
//
// First added:  2006-04-04
// Last changed: 2006-05-15

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
void DenseVector::apply()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void DenseVector::zero()
{
  uint N = this->size();
  for (uint i = 0; i < N; ++i)
    (*this)(i) = 0.0;
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
