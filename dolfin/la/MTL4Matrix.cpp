// Copyright (C) 2008 Dag Lindbo
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2008-07-06
// Last changed: 2008-07-06

#ifdef HAS_MTL4

#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/Array.h>
#include "MTL4Vector.h"
#include "MTL4Matrix.h"
#include "GenericSparsityPattern.h"
#include "MTL4Factory.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix():
  Variable("A", "MTL4 matrix"), ins(0), nnz_row(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(uint M, uint N):
  Variable("A", "MTL4 matrix"), ins(0), nnz_row(0)
{
  init(M, N);
}
//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(const MTL4Matrix& A):
  Variable("A", "MTL4 matrix"), ins(0), nnz_row(0)
{
  error("Not implemented.");
}
//-----------------------------------------------------------------------------
MTL4Matrix::~MTL4Matrix()
{
  if(ins) 
    delete ins;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::init(uint M, uint N)
{
  A.change_dim(M,N);
  A = 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::init(const GenericSparsityPattern& sparsity_pattern)
{
  init(sparsity_pattern.size(0), sparsity_pattern.size(1));
}
//-----------------------------------------------------------------------------
MTL4Matrix* MTL4Matrix::copy() const
{
  error("MTL4Matrix::copy not yet implemented.");
  return (MTL4Matrix*) NULL;
}
//-----------------------------------------------------------------------------
dolfin::uint MTL4Matrix::size(uint dim) const
{
  error("MTL4Matrix::size not yet implemented.");
  return 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::get(real* block,
		     uint m, const uint* rows,
		     uint n, const uint* cols) const
{
  error("MTL4Matrix::get not yet implemented.");
}
//-----------------------------------------------------------------------------
void MTL4Matrix::set(const real* block,
		     uint m, const uint* rows,
		     uint n, const uint* cols)
{
  error("MTL4Matrix::set not yet implemented.");
}
//-----------------------------------------------------------------------------
void MTL4Matrix::add(const real* block,
		     uint m, const uint* rows,
		     uint n, const uint* cols)
{
  if(!ins)
  {
    if(nnz_row > 0)
      ins = new mtl::matrix::inserter<mtl4_sparse_matrix, mtl::update_plus<real> >(A, nnz_row);
    else
      ins = new mtl::matrix::inserter<mtl4_sparse_matrix, mtl::update_plus<real> >(A);
  }
  real val;
  for (uint i = 0; i < m; i++)
    for (uint j = 0; j < n; j++)
    {
      val = block[i*n +j];
      if(val != 0.0)
        (*ins)[rows[i]][cols[j]] <<  val;
    }
}
//-----------------------------------------------------------------------------
void MTL4Matrix::zero()
{
  A *= 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::apply(FinalizeType finaltype)
{
  if(ins) 
    delete ins;
  ins = 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::disp(uint precision) const
{
  error("MTL4Matrix::disp not yet implemented.");
}
//-----------------------------------------------------------------------------
void MTL4Matrix::ident(uint m, const uint* rows)
{
  error("MTL4Matrix::ident not yet implemented.");
}
//-----------------------------------------------------------------------------
void MTL4Matrix::zero(uint m, const uint* rows)
{
  error("MTL4Matrix::zero not yet implemented.");
}

//-----------------------------------------------------------------------------
void MTL4Matrix::mult(const GenericVector& x_, GenericVector& Ax_, bool transposed) const
{
  error("MTL4Matrix::mult not yet implemented.");
}

//-----------------------------------------------------------------------------
void MTL4Matrix::getrow(uint row, Array<uint>& columns, Array<real>& values) const
{
  error("MTL4Matrix::getrow not yet implemented.");
}
//-----------------------------------------------------------------------------
void MTL4Matrix::setrow(uint row, const Array<uint>& columns, const Array<real>& values)
{
  error("MTL4Matrix::setrow not yet implemented.");
}
//-----------------------------------------------------------------------------
LinearAlgebraFactory& MTL4Matrix::factory() const
{
  return MTL4Factory::instance();
}
//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(uint M, uint N, uint nz):
  Variable("A", "MTL4 matrix"), ins(0), nnz_row(nz)
{
  init(M, N);
}
//-----------------------------------------------------------------------------
const mtl4_sparse_matrix& MTL4Matrix::mat() const
{
  return A;
}
//-----------------------------------------------------------------------------
mtl4_sparse_matrix& MTL4Matrix::mat()
{
  return A;
}
//-----------------------------------------------------------------------------
const MTL4Matrix& MTL4Matrix::operator*= (real a)
{
  A *= a;
  return *this;
}
//-----------------------------------------------------------------------------
const MTL4Matrix& MTL4Matrix::operator/= (real a)
{
  A /= a;
  return *this;
}
//-----------------------------------------------------------------------------
LogStream& dolfin::operator<< (LogStream& stream, const mtl4_sparse_matrix& A)
{
  error("operator << MTL4Matrix not implemented yet"); 
  return stream;
}
//-----------------------------------------------------------------------------

#endif
