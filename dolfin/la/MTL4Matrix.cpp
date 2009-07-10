// Copyright (C) 2008 Dag Lindbo
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Garth N. Wells, 2008.
//
// First added:  2008-07-06
// Last changed: 2008-10-04

#ifdef HAS_MTL4

#include <dolfin/log/dolfin_log.h>
#include "GenericSparsityPattern.h"
#include "MTL4Vector.h"
#include "MTL4Matrix.h"
#include "MTL4Factory.h"

using namespace dolfin;
using namespace mtl;

//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(): Variable("A", "MTL4 matrix"), ins(0), nnz_row(0)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(uint M, uint N): Variable("A", "MTL4 matrix"), ins(0),
                                        nnz_row(0)
{
  resize(M, N);
}
//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(uint M, uint N, uint nz): Variable("A", "MTL4 matrix"),
                                                 ins(0), nnz_row(nz)
{
  resize(M, N);
}
//-----------------------------------------------------------------------------
MTL4Matrix::MTL4Matrix(const MTL4Matrix& mat): Variable("A", "MTL4 matrix"),
                                               ins(0), nnz_row(0)
{
  assert_no_inserter();

  // Deep copy
  A = mat.mat();
  nnz_row = mat.nnz_row;
}
//-----------------------------------------------------------------------------
MTL4Matrix::~MTL4Matrix()
{
  if(ins)
    delete ins;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::resize(uint M, uint N)
{
  assert_no_inserter();
  A.change_dim(M, N);
}
//-----------------------------------------------------------------------------
void MTL4Matrix::init(const GenericSparsityPattern& sparsity_pattern)
{
  resize(sparsity_pattern.size(0), sparsity_pattern.size(1));
  zero();
}
//-----------------------------------------------------------------------------
MTL4Matrix* MTL4Matrix::copy() const
{
  // Why is this needed???
  assert_no_inserter();
  return new MTL4Matrix(*this);
}
//-----------------------------------------------------------------------------
dolfin::uint MTL4Matrix::size(uint dim) const
{
  if(dim == 0)
    return mtl::matrix::num_rows(A);
  else if(dim == 1)
    return mtl::matrix::num_cols(A);

  error("dim not < 2 in MTL4Matrix::size.");
  return 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::get(double* block, uint m, const uint* rows, uint n,
                     const uint* cols) const
{
  assert_no_inserter();

  for (uint i = 0; i < m; i++)
    for (uint j = 0; j < n; j++)
      block[i*n+j] = A[rows[i]][cols[j]];
}
//-----------------------------------------------------------------------------
void MTL4Matrix::set(const double* block, uint m, const uint* rows, uint n,
                     const uint* cols)
{
  if(!ins)
    init_inserter();

  for (uint i = 0; i < m; i++)
    for (uint j = 0; j < n; j++)
      (*ins)[rows[i]][cols[j]] = block[i*n +j];
}
//-----------------------------------------------------------------------------
void MTL4Matrix::add(const double* block, uint m, const uint* rows, uint n, const uint* cols)
{
  if(!ins)
    init_inserter();

  // Block insertion
  *ins << element_array(mtl::dense2D<double>(m, n, const_cast<double*>(block)), 
                        mtl::dense_vector<uint>(m, const_cast<uint*>(rows)),
                        mtl::dense_vector<uint>(n, const_cast<uint*>(cols))); 
  /*
  for (uint i = 0; i < m; i++)
    for (uint j = 0; j < n; j++)
      (*ins)[rows[i]][cols[j]] << block[i*n +j];
  */
}
//-----------------------------------------------------------------------------
void MTL4Matrix::MTL4Matrix::axpy(double a, const GenericMatrix& A,
                                  bool same_nonzero_pattern)
  {
    // Check for same size
    if ( size(0) != A.size(0) or size(1) != A.size(1) )
      error("Matrices must be of same size.");

    // Do we need to check for same sparsity pattern?
    this->A += (a)*(A.down_cast<MTL4Matrix>().mat());
  }
//-----------------------------------------------------------------------------
void MTL4Matrix::zero()
{
  assert_no_inserter();
  A *= 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::apply()
{
  if(ins)
    delete ins;
  ins = 0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::disp(uint precision) const
{
  assert_no_inserter();

  // FIXME: This ignores precision
  std::stringstream stream;
  stream << A;
  cout << stream.str() << endl;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::ident(uint m, const uint* rows)
{
  // Zero rows
  zero(m, rows);

  // Place one on the diagonal
  mtl::matrix::inserter< mtl4_sparse_matrix > ins_A(A);
  for(uint i = 0; i < m ; ++i)
    ins_A[ rows[i] ][ rows[i] ] << 1.0;
}
//-----------------------------------------------------------------------------
void MTL4Matrix::zero(uint m, const uint* rows)
{
  assert_no_inserter();

  // Define cursors
  typedef mtl::traits::range_generator<mtl::tag::row, mtl4_sparse_matrix >::type c_type;
  typedef mtl::traits::range_generator<mtl::tag::nz, c_type>::type  ic_type;

  mtl::traits::value<mtl4_sparse_matrix >::type value(A);

  // Create row cursors
  c_type cursor(mtl::begin<mtl::tag::row>(A));
  c_type cend(mtl::end<mtl::tag::row>(A));

  for(uint i = 0; i < m; ++i)
  {
    // Increment cursor
    if(i == 0)
      cursor += rows[i];
    else
      cursor += rows[i] -rows[i-1];

    // Check that we haven't gone beyond last row
    assert(*cursor <= *cend);

    // Zero row and place one on the diagonal
    for (ic_type icursor(mtl::begin<mtl::tag::nz>(cursor)), icend(mtl::end<mtl::tag::nz>(cursor)); icursor != icend; ++icursor)
      value(*icursor, 0.0);
  }
}
//-----------------------------------------------------------------------------
void MTL4Matrix::mult(const GenericVector& x_, GenericVector& Ax_, bool transposed) const
{
  assert_no_inserter();

  // FIXME: Transposed view multiply, e.g. y = trans(A)*x
  // does not work in const-declared member function.

  if(transposed)
    error("MTL4Matrix: Transposed view multiply is not implemented yet.");
  else
    Ax_.down_cast<MTL4Vector>().vec() = A*x_.down_cast<MTL4Vector>().vec();
}
//-----------------------------------------------------------------------------
void MTL4Matrix::getrow(uint row_idx, std::vector<uint>& columns, std::vector<double>& values) const
{
  assert_no_inserter();
  assert(row_idx < this->size(0));

  columns.clear();
  values.clear();

  // Define cursors
  typedef mtl::traits::range_generator<mtl::tag::row, mtl4_sparse_matrix >::type c_type;
  typedef mtl::traits::range_generator<mtl::tag::nz, c_type>::type  ic_type;

  mtl::traits::col<mtl4_sparse_matrix >::type   col(A);
  mtl::traits::const_value<mtl4_sparse_matrix >::type value(A);

  // Row cursors
  c_type cursor(mtl::begin<mtl::tag::row>(A));
  cursor += row_idx;

  for (ic_type icursor(mtl::begin<mtl::tag::nz>(cursor)), icend(mtl::end<mtl::tag::nz>(cursor)); icursor != icend; ++icursor)
  {
    columns.push_back(col(*icursor));
    values.push_back(value(*icursor));
  }
}
//-----------------------------------------------------------------------------
void MTL4Matrix::setrow(uint row, const std::vector<uint>& columns, const std::vector<double>& values)
{
  assert(columns.size() == values.size());
  assert(row < this->size(0));

  if(!ins)
    init_inserter();

  for(uint i=0; i<columns.size(); i++)
    (*ins)[row][columns[i] ] = values[i];
}
//-----------------------------------------------------------------------------
LinearAlgebraFactory& MTL4Matrix::factory() const
{
  return MTL4Factory::instance();
}
//-----------------------------------------------------------------------------
const mtl4_sparse_matrix& MTL4Matrix::mat() const
{
  assert_no_inserter();
  return A;
}
//-----------------------------------------------------------------------------
mtl4_sparse_matrix& MTL4Matrix::mat()
{
  assert_no_inserter();
  return A;
}
//-----------------------------------------------------------------------------
const MTL4Matrix& MTL4Matrix::operator*= (double a)
{
  assert_no_inserter();
  A *= a;
  return *this;
}
//-----------------------------------------------------------------------------
const MTL4Matrix& MTL4Matrix::operator/= (double a)
{
  assert_no_inserter();
  A /= a;
  return *this;
}
//-----------------------------------------------------------------------------
const GenericMatrix& MTL4Matrix::operator= (const GenericMatrix& A)
{
  *this = A.down_cast<MTL4Matrix>();
  return *this;
}
//-----------------------------------------------------------------------------
const MTL4Matrix& MTL4Matrix::operator= (const MTL4Matrix& A)
{
  // Check for self-assignment
  if (this != &A)
  {
    // Assume MTL4 take care of deleting an existing Matrix
    // using its assignment operator
    this->A = A.mat();
  }
  nnz_row = A.nnz_row;
  return *this;
}
//-----------------------------------------------------------------------------
std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int> MTL4Matrix::data() const
{
  assert_no_inserter();
  typedef std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int> tuple;
  return tuple(A.address_major(), A.address_minor(), A.address_data(), A.nnz());
}
//-----------------------------------------------------------------------------
void MTL4Matrix::init_inserter(void)
{
  if(nnz_row > 0)
    ins = new mtl::matrix::inserter<mtl4_sparse_matrix, mtl::update_plus<double> >(A, nnz_row);
  else
    ins = new mtl::matrix::inserter<mtl4_sparse_matrix, mtl::update_plus<double> >(A, 50);
}
//-----------------------------------------------------------------------------
inline void MTL4Matrix::assert_no_inserter(void) const
{
  if(ins)
    error("MTL4: Matrix read operation attempted while inserter active. Did you forget to apply()?");
}
//-----------------------------------------------------------------------------
LogStream& dolfin::operator<< (LogStream& stream, const  MTL4Matrix& A)
{
  // FIXME: "Redirect" mtl::matix::op<< to the dolfin log stream

  int M = mtl::matrix::num_rows(A.mat());
  int N = mtl::matrix::num_cols(A.mat());

  stream << "[ MTL4 Matrix of size " << M << "x" << N << " ]";
  return stream;
}
//-----------------------------------------------------------------------------

#endif
