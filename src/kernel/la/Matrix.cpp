// Copyright (C) 2004-2005 Johan Hoffman, Johan Jansson and Anders Logg.
// Licensed under the GNU GPL Version 2.
//
// Modified by Garth N. Wells 2005,2006.
// Modified by Andy R. Terrel 2005.
//
// First added:  2004
// Last changed: 2006-03-22

#include <iostream>
#include <sstream>
#include <iomanip>
#include <dolfin/dolfin_log.h>
#include <dolfin/PETScManager.h>
#include <dolfin/Matrix.h>
#include <dolfin/Vector.h>

using namespace dolfin;

//-----------------------------------------------------------------------------
Matrix::Matrix() : Variable("A", "a sparse matrix"), A(0), type(default_matrix)
{
  // Initialize PETSc
  PETScManager::init();
}
//-----------------------------------------------------------------------------
Matrix::Matrix(Type type) : Variable("A", "a sparse matrix"), A(0), type(type)
{
  // Initialize PETSc
  PETScManager::init();

  // Check type
  checkType();
}
//-----------------------------------------------------------------------------
Matrix::Matrix(Mat A) : Variable("A", "a sparse matrix"), A(A)
{
  // Initialize PETSc
  PETScManager::init();

  // FIXME: get PETSc matrix type and set
  type = default_matrix;
}
//-----------------------------------------------------------------------------
Matrix::Matrix(uint M, uint N) : Variable("A", "a sparse matrix"), A(0), type(default_matrix)
{
  // Initialize PETSc
  PETScManager::init();

  // Create PETSc matrix
  init(M, N);
}
//-----------------------------------------------------------------------------
Matrix::Matrix(uint M, uint N, Type type) : Variable("A", "a sparse matrix"), A(0), 
    type(type)
{
  // Initialize PETSc
  PETScManager::init();

  // Check type
  checkType();

  // Create PETSc matrix
  init(M, N);
}
//-----------------------------------------------------------------------------
Matrix::Matrix(const Matrix& B) : Variable("A", "a sparse matrix"), A(0)
{
  // Initialize PETSc
  PETScManager::init();

  // Set PETSc matrix type (same as B)
  type = B.getMatrixType();

  // Create PETSc matrix
  init(B.size(0), B.size(1));
  
  uint M = B.size(0);
  uint N = B.size(1);

  // FIXME: Use PETSc function to copy
  for(uint i = 0; i < M; i++)
  {
    for(uint j = 0; j < N; j++)
    {
      setval(i, j, B(i, j));
    }
  }
}
//-----------------------------------------------------------------------------
Matrix::~Matrix()
{
  // Free memory of matrix
  if ( A ) MatDestroy(A);
}
//-----------------------------------------------------------------------------
void Matrix::init(uint M, uint N)
{
  // Free previously allocated memory if necessary
  if ( A )
  {
    if ( M == size(0) && N == size(1) )
      return;
    else
      MatDestroy(A);
  }
  
  // FIXME: maybe 50 should be a parameter?

  // Create a sparse matrix in compressed row format
  MatCreateSeqAIJ(PETSC_COMM_SELF, M, N, 50, PETSC_NULL, &A);
  MatSetFromOptions(A);
  MatSetOption(A, MAT_KEEP_ZEROED_ROWS);
}
//-----------------------------------------------------------------------------
void Matrix::init(uint M, uint N, uint nz)
{
  // Free previously allocated memory if necessary
  if ( A )
  {
    if ( M == size(0) && N == size(1) )
      return;
    else
      MatDestroy(A);
  }

  // Create a sparse matrix in compressed row format
  MatCreate(PETSC_COMM_SELF, &A);
  MatSetSizes(A,  PETSC_DECIDE,  PETSC_DECIDE, M, N);
  setType();
  MatSeqAIJSetPreallocation(A, nz, PETSC_NULL);
  MatSetFromOptions(A);
  MatSetOption(A, MAT_KEEP_ZEROED_ROWS);
}
//-----------------------------------------------------------------------------
void Matrix::init(uint M, uint N, uint bs, uint nz)
{
  // Free previously allocated memory if necessary
  if ( A )
  {
    if ( M == size(0) && N == size(1) )
      return;
    else
      MatDestroy(A);
  }
  
  // Creates a sparse matrix in block AIJ (block compressed row) format.
  // Given blocksize bs, and max no connectivity mnc.  
  MatCreateSeqBAIJ(PETSC_COMM_SELF, bs, bs*M, bs*N, nz, PETSC_NULL, &A);
  MatSetFromOptions(A);
  MatSetOption(A, MAT_KEEP_ZEROED_ROWS);
}
//-----------------------------------------------------------------------------
dolfin::uint Matrix::size(uint dim) const
{
  int M = 0;
  int N = 0;
  MatGetSize(A, &M, &N);
  return (dim == 0 ? M : N);
}
//-----------------------------------------------------------------------------
dolfin::uint Matrix::nz(uint row) const
{
  // FIXME: this can probably be done better
  int ncols = 0;
  const int* cols = 0;
  const double* vals = 0;
  MatGetRow(A, static_cast<int>(row), &ncols, &cols, &vals);
  MatRestoreRow(A, static_cast<int>(row), &ncols, &cols, &vals);

  return ncols;
}
//-----------------------------------------------------------------------------
dolfin::uint Matrix::nzsum() const
{
  uint M = size(0);
  uint sum = 0;
  for (uint i = 0; i < M; i++)
    sum += nz(i);

  return sum;
}
//-----------------------------------------------------------------------------
dolfin::uint Matrix::nzmax() const
{
  uint M = size(0);
  uint max = 0;
  for (uint i = 0; i < M; i++)
    max = std::max(max, nz(i));

  return max;
}
//-----------------------------------------------------------------------------
Matrix& Matrix::operator= (real zero)
{
  if ( zero != 0.0 )
    dolfin_error("Argument must be zero.");
  MatZeroEntries(A);
  return *this;
}
//-----------------------------------------------------------------------------
void Matrix::add(const real block[],
		    const int rows[], int m,
		    const int cols[], int n)
{
  MatSetValues(A, m, rows, n, cols, block, ADD_VALUES);
}
//-----------------------------------------------------------------------------
void Matrix::ident(const int rows[], int m)
{
  IS is = 0;
  ISCreateGeneral(PETSC_COMM_SELF, m, rows, &is);
  PetscScalar one = 1.0;
  MatZeroRowsIS(A, is, one);
  ISDestroy(is);
}
//-----------------------------------------------------------------------------
void Matrix::mult(const Vector& x, Vector& Ax) const
{
  MatMult(A, x.vec(), Ax.vec());
}
//-----------------------------------------------------------------------------
real Matrix::mult(const Vector& x, uint row) const
{
  // FIXME: Temporary fix (assumes uniprocessor case)

  int ncols = 0;
  const int* cols = 0;
  const double* Avals = 0;
  double* xvals = 0;
  MatGetRow(A, static_cast<int>(row), &ncols, &cols, &Avals);
  VecGetArray(x.x, &xvals);

  real sum = 0.0;
  for (int i = 0; i < ncols; i++)
    sum += Avals[i] * xvals[cols[i]];

  MatRestoreRow(A, static_cast<int>(row), &ncols, &cols, &Avals);
  VecRestoreArray(x.x, &xvals);

  return sum;
}
//-----------------------------------------------------------------------------
real Matrix::mult(const real x[], uint row) const
{
  // FIXME: Temporary fix (assumes uniprocessor case)

  int ncols = 0;
  const int* cols = 0;
  const double* Avals = 0;
  MatGetRow(A, static_cast<int>(row), &ncols, &cols, &Avals);

  real sum = 0.0;
  for (int i = 0; i < ncols; i++)
    sum += Avals[i] * x[cols[i]];

  MatRestoreRow(A, static_cast<int>(row), &ncols, &cols, &Avals);

  return sum;
}
//-----------------------------------------------------------------------------
real Matrix::norm(Norm type) const
{
  real value = 0.0;

  switch ( type )
  {
  case l1:
    MatNorm(A, NORM_1, &value);
    break;
  case linf:
    MatNorm(A, NORM_INFINITY, &value);
    break;
  case frobenius:
    MatNorm(A, NORM_FROBENIUS, &value);
    break;
  default:
    dolfin_error("Unknown norm type.");
  }
  
  return value;
}
//-----------------------------------------------------------------------------
void Matrix::apply()
{
  MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
}
//-----------------------------------------------------------------------------
const Matrix::Type Matrix::getMatrixType() const
{
  return type;
}
//-----------------------------------------------------------------------------
Mat Matrix::mat()
{
  return A;
}
//-----------------------------------------------------------------------------
const Mat Matrix::mat() const
{
  return A;
}
//-----------------------------------------------------------------------------
void Matrix::disp(bool sparse, int precision) const
{
  // FIXME: Maybe this could be an option?
  //MatView(A, PETSC_VIEWER_STDOUT_SELF);

  const uint M = size(0);
  const uint N = size(1);

  // Sparse output
  for (uint i = 0; i < M; i++)
  {
    std::stringstream line;
    line << std::setiosflags(std::ios::scientific);
    line << std::setprecision(precision);
    
    line << "|";
    
    if ( sparse )
    {
      int ncols = 0;
      const int* cols = 0;
      const double* vals = 0;
      MatGetRow(A, i, &ncols, &cols, &vals);
      for (int pos = 0; pos < ncols; pos++)
      {
	       line << " (" << i << ", " << cols[pos] << ", " << vals[pos] << ")";
      }
      MatRestoreRow(A, i, &ncols, &cols, &vals);
    }
    else
    {
      for (uint j = 0; j < N; j++)
      {
        real value = getval(i, j);
        if ( fabs(value) < DOLFIN_EPS )
        value = 0.0;	
        line << " " << value;
      }
    }

    line << "|";
    cout << line.str().c_str() << endl;
  }
}
//-----------------------------------------------------------------------------
LogStream& dolfin::operator<< (LogStream& stream, const Matrix& A)
{
  // Check if matrix has been defined
  if ( !A.A )
  {
    stream << "[ PETSc matrix (empty) ]";
    return stream;
  }

  MatType type = 0;
  MatGetType(A.mat(), &type);
  int m = A.size(0);
  int n = A.size(1);
  stream << "[ PETSc matrix (type " << type << ") of size "
	 << m << " x " << n << " ]";

  return stream;
}
//-----------------------------------------------------------------------------
MatrixElement Matrix::operator()(uint i, uint j)
{
  MatrixElement element(i, j, *this);

  return element;
}
//-----------------------------------------------------------------------------
real Matrix::operator() (uint i, uint j) const
{
  return getval(i, j);
}
//-----------------------------------------------------------------------------
real Matrix::getval(uint i, uint j) const
{
  const int ii = static_cast<int>(i);
  const int jj = static_cast<int>(j);

  dolfin_assert(A);
  PetscScalar val;
  MatGetValues(A, 1, &ii, 1, &jj, &val);

  return val;
}
//-----------------------------------------------------------------------------
void Matrix::setval(uint i, uint j, const real a)
{
  MatSetValue(A, i, j, a, INSERT_VALUES);

  MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
}
//-----------------------------------------------------------------------------
void Matrix::addval(uint i, uint j, const real a)
{
  MatSetValue(A, i, j, a, ADD_VALUES);

  MatAssemblyBegin(A, MAT_FINAL_ASSEMBLY);
  MatAssemblyEnd(A, MAT_FINAL_ASSEMBLY);
}
//-----------------------------------------------------------------------------
void Matrix::setType() 
{
  MatType mat_type = getType();
  dolfin_log(true);
  cout << "matrix type " << mat_type << endl; 
  MatSetType(A, mat_type);
}
//-----------------------------------------------------------------------------
void Matrix::checkType()
{
  switch (type)
  {
  case spooles:
    #if !PETSC_HAVE_SPOOLES
      dolfin_warning("PETSc has not been complied with Spooles. Using default matrix type");
      type = default_matrix;
    #endif
    return;
  case superlu:
    #if !PETSC_HAVE_SUPERLU
      dolfin_warning("PETSc has not been complied with Super LU. Using default matrix type");
      type = default_matrix;
    #endif
    return;
  case umfpack:
    #if !PETSC_HAVE_UMFPACK
      dolfin_warning("PETSc has not been complied with UMFPACK. Using default matrix type");
      type = default_matrix;
    #endif
    return;
  default:
    dolfin_warning("Requested matrix type unknown. Using default.");
    type = default_matrix;
  }
}
//-----------------------------------------------------------------------------
MatType Matrix::getType() const
{
  switch (type)
  {
  case default_matrix:
    return MATSEQAIJ;
  case spooles:
      return MATSEQAIJSPOOLES;
  case superlu:
      return MATSUPERLU;
  case umfpack:
      return MATUMFPACK;
  default:
    return "default";
  }
}
//-----------------------------------------------------------------------------
// MatrixElement
//-----------------------------------------------------------------------------
MatrixElement::MatrixElement(uint i, uint j, Matrix& A) : i(i), j(j), A(A)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
MatrixElement::MatrixElement(const MatrixElement& e) : i(i), j(j), A(A)
{
}
//-----------------------------------------------------------------------------
MatrixElement::operator real() const
{
  return A.getval(i, j);
}
//-----------------------------------------------------------------------------
const MatrixElement& MatrixElement::operator=(const real a)
{
  A.setval(i, j, a);

  return *this;
}
//-----------------------------------------------------------------------------
const MatrixElement& MatrixElement::operator=(const MatrixElement& e)
{
  A.setval(i, j, e.A.getval(e.i, e.j));

  return *this;
}
//-----------------------------------------------------------------------------
const MatrixElement& MatrixElement::operator+=(const real a)
{
  A.addval(i, j, a);

  return *this;
}
//-----------------------------------------------------------------------------
const MatrixElement& MatrixElement::operator-=(const real a)
{
  A.addval(i, j, -a);

  return *this;
}
//-----------------------------------------------------------------------------
const MatrixElement& MatrixElement::operator*=(const real a)
{
  const real val = A.getval(i, j) * a;
  A.setval(i, j, val);
  
  return *this;
}
//-----------------------------------------------------------------------------
