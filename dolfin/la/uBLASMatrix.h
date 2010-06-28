// Copyright (C) 2006-2009 Garth N. Wells.
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Anders Logg, 2006-2009.
// Modified by Ola Skavhaug, 2007-2008.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Martin Sandve Alnes, 2008.
// Modified by Dag Lindbo, 2008
//
// First added:  2006-07-05
// Last changed: 2010-06-28

#ifndef __UBLAS_MATRIX_H
#define __UBLAS_MATRIX_H

#include <sstream>
#include <iomanip>
#include <tr1/tuple>

#include "LinearAlgebraFactory.h"
#include "SparsityPattern.h"
#include "ublas.h"
#include "uBLASFactory.h"
#include "uBLASVector.h"
#include "GenericMatrix.h"

namespace dolfin
{

  // Forward declarations
  class uBLASVector;
  template< class T> class uBLASFactory;

  namespace ublas = boost::numeric::ublas;

  /// This class provides a simple matrix class based on uBLAS.
  /// It is a simple wrapper for a uBLAS matrix implementing the
  /// GenericMatrix interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the underlying uBLAS matrix and use the standard
  /// uBLAS interface which is documented at
  /// http://www.boost.org/libs/numeric/ublas/doc/index.htm.
  ///
  /// Developer note: specialised member functions must be
  /// inlined to avoid link errors.

  template<class Mat>
  class uBLASMatrix : public GenericMatrix
  {
  public:

    /// Create empty matrix
    uBLASMatrix();

    /// Create M x N matrix
    uBLASMatrix(uint M, uint N);

    /// Copy constructor
    uBLASMatrix(const uBLASMatrix& A);

    /// Create matrix from given uBLAS matrix expression
    template <class E>
    explicit uBLASMatrix(const ublas::matrix_expression<E>& A) : Mat(A) {}

    /// Destructor
    virtual ~uBLASMatrix();

    //--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern
    virtual void init(const GenericSparsityPattern& sparsity_pattern);

    /// Return copy of tensor
    virtual uBLASMatrix<Mat>* copy() const;

    /// Return size of given dimension
    virtual uint size(uint dim) const;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply(std::string mode);

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const;

    //--- Implementation of the GenericMatrix interface ---

    /// Resize matrix to M x N
    virtual void resize(uint M, uint N);

    /// Get block of values
    virtual void get(double* block, uint m, const uint* rows, uint n, const uint* cols) const;

    /// Set block of values
    virtual void set(const double* block, uint m, const uint* rows, uint n, const uint* cols);

    /// Add block of values
    virtual void add(const double* block, uint m, const uint* rows, uint n, const uint* cols);

    /// Add multiple of given matrix (AXPY operation)
    virtual void axpy(double a, const GenericMatrix& A,
                      bool same_nonzero_pattern);

    /// Return norm of matrix
    virtual double norm(std::string norm_type) const;

    /// Get non-zero values of given row
    virtual void getrow(uint row, std::vector<uint>& columns, std::vector<double>& values) const;

    /// Set values for given row
    virtual void setrow(uint row_idx, const std::vector<uint>& columns, const std::vector<double>& values);

    /// Set given rows to zero
    virtual void zero(uint m, const uint* rows);

    /// Set given rows to identity matrix
    virtual void ident(uint m, const uint* rows);

    /// Matrix-vector product, y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y) const;

    /// Matrix-vector product, y = A^T x
    virtual void transpmult(const GenericVector& x, GenericVector& y) const;

    /// Multiply matrix by given number
    virtual const uBLASMatrix<Mat>& operator*= (double a);

    /// Divide matrix by given number
    virtual const uBLASMatrix<Mat>& operator/= (double a);

    /// Assignment operator
    virtual const GenericMatrix& operator= (const GenericMatrix& A);

    /// Return pointers to underlying compresssed storage data
    /// See GenericMatrix for documentation.
    virtual std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int> data() const;

    //--- Special functions ---

    /// Return linear algebra backend factory
    virtual LinearAlgebraFactory& factory() const
    { return uBLASFactory<Mat>::instance(); }

    //--- Special uBLAS functions ---

    /// Return reference to uBLAS matrix (const version)
    const Mat& mat() const
    { return A; }

    /// Return reference to uBLAS matrix (non-const version)
    Mat& mat()
    { return A; }

    /// Solve Ax = b out-of-place using uBLAS (A is not destroyed)
    void solve(uBLASVector& x, const uBLASVector& b) const;

    /// Solve Ax = b in-place using uBLAS(A is destroyed)
    void solveInPlace(uBLASVector& x, const uBLASVector& b);

    /// Compute inverse of matrix
    void invert();

    /// Lump matrix into vector m
    void lump(uBLASVector& m) const;

    /// Compress matrix (eliminate all non-zeros from a sparse matrix)
    void compress();

    /// Access value of given entry
    double operator() (uint i, uint j) const
    { return A(i, j); }

    /// Assignment operator
    const uBLASMatrix<Mat>& operator= (const uBLASMatrix<Mat>& A);

  private:

    /// General uBLAS LU solver which accepts both vector and matrix right-hand sides
    template<class B>
    void solveInPlace(B& X);

    // uBLAS matrix object
    Mat A;

  };

  //---------------------------------------------------------------------------
  // Implementation of uBLASMatrix
  //---------------------------------------------------------------------------
  template <class Mat>
  uBLASMatrix<Mat>::uBLASMatrix() : GenericMatrix(), A(0, 0)
  {
    // Do nothing
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  uBLASMatrix<Mat>::uBLASMatrix(uint M, uint N) : GenericMatrix(), A(M, N)
  {
    // Do nothing
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  uBLASMatrix<Mat>::uBLASMatrix(const uBLASMatrix& A) : GenericMatrix(), A(A.A)
  {
    // Do nothing
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  uBLASMatrix<Mat>::~uBLASMatrix()
  {
    // Do nothing
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix< Mat >::resize(uint M, uint N)
  {
    // Resize matrix
    if( size(0) != M || size(1) != N )
      A.Mat::resize(M, N, false);
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  uBLASMatrix<Mat>* uBLASMatrix<Mat>::copy() const
  {
    return new uBLASMatrix<Mat>(*this);
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  uint uBLASMatrix<Mat>::size(uint dim) const
  {
    assert(dim < 2);
    return (dim == 0 ? A.Mat::size1() : A.Mat::size2());
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  double uBLASMatrix<Mat>::norm(std::string norm_type) const
  {
    if (norm_type == "l1")
      return norm_1(A);
    else if (norm_type == "linf")
      return norm_inf(A);
    else if (norm_type == "frobenius")
      return norm_frobenius(A);
    else
    {
      error("Unknown norm type in uBLASMatrix.");
      return 0.0;
    }
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::getrow(uint row_idx, std::vector<uint>& columns,
                                std::vector<double>& values) const
  {
    assert(row_idx < this->size(0));

    // Reference to matrix row
    const ublas::matrix_row<const Mat> row(A, row_idx);

    // Insert values into std::vectors
    columns.clear();
    values.clear();
    typename ublas::matrix_row<const Mat>::const_iterator component;
    for (component=row.begin(); component != row.end(); ++component)
    {
      columns.push_back(component.index());
      values.push_back(*component );
    }
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::setrow(uint row_idx, const std::vector<uint>& columns,
                                const std::vector<double>& values)
  {
    assert(columns.size() == values.size());
    assert(row_idx < this->size(0));

    ublas::matrix_row<Mat> row(A, row_idx);
    assert(columns.size() <= row.size());

    row *= 0;
    for(uint i = 0; i < columns.size(); i++)
      row(columns[i])=values[i];
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::set(const double* block, uint m, const uint* rows,
                                                uint n, const uint* cols)
  {
    for (uint i = 0; i < m; i++)
      for (uint j = 0; j < n; j++)
        A(rows[i] , cols[j]) = block[i*n + j];
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::add(const double* block, uint m, const uint* rows,
                                                uint n, const uint* cols)
  {
    for (uint i = 0; i < m; i++)
      for (uint j = 0; j < n; j++)
        A(rows[i] , cols[j]) += block[i*n + j];
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::get(double* block, uint m, const uint* rows,
                                          uint n, const uint* cols) const
  {
    for(uint i = 0; i < m; ++i)
      for(uint j = 0; j < n; ++j)
        block[i*n + j] = A(rows[i], cols[j]);
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::lump(uBLASVector& m) const
  {
    const uint n = size(1);
    m.resize(n);
    m.zero();
    ublas::scalar_vector<double> one(n, 1.0);
    ublas::axpy_prod(A, one, m.vec(), true);
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::solve(uBLASVector& x, const uBLASVector& b) const
  {
    // Make copy of matrix and vector
    uBLASMatrix<Mat> Atemp;
    Atemp.mat().resize(size(0), size(1));
    Atemp.mat().assign(A);
    x.vec().resize(b.vec().size());
    x.vec().assign(b.vec());

    // Solve
    Atemp.solveInPlace(x.vec());
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::solveInPlace(uBLASVector& x, const uBLASVector& b)
  {
    const uint M = A.size1();
    assert(M == b.size());

    // Initialise solution vector
    if( x.vec().size() != M )
      x.vec().resize(M);
    x.vec().assign(b.vec());

    // Solve
    solveInPlace(x.vec());
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::invert()
  {
    const uint M = A.size1();
    assert(M == A.size2());

    // Create indentity matrix
    Mat X(M, M);
    X.assign(ublas::identity_matrix<double>(M));

    // Solve
    solveInPlace(X);
    A.assign_temporary(X);
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::zero()
  {
    // Iterate through no-zero pattern and zero entries
    typename Mat::iterator1 row;    // Iterator over rows
    typename Mat::iterator2 entry;  // Iterator over entries
    for (row = A.begin1(); row != A.end1(); ++row)
      for (entry = row.begin(); entry != row.end(); ++entry)
        *entry = 0;
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::zero(uint m, const uint* rows)
  {
    for(uint i = 0; i < m; ++i)
      ublas::row(A, rows[i]) *= 0.0;
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::ident(uint m, const uint* rows)
  {
    // Copy row indices to a vector
    std::vector<uint> _rows(rows, rows+m);

    uint counter = 0;
    typename Mat::iterator1 row;    // Iterator over rows
    typename Mat::iterator2 entry;  // Iterator over entries
    for (row = A.begin1(); row != A.end1(); ++row)
    {
      entry = row.begin();
      if (std::find(_rows.begin(), _rows.end(), entry.index1()) != _rows.end())
      {
        // Iterate over entries to zero and place one on the diagonal
        for (entry = row.begin(); entry != row.end(); ++entry)
        {
          if (entry.index1() == entry.index2())
            *entry = 1.0;
          else
            *entry = 0.0;
        }
        ++ counter;
      }
      if (counter == _rows.size())
        continue;
    }
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::mult(const GenericVector& x, GenericVector& y) const
  {
    ublas::axpy_prod(A, x.down_cast<uBLASVector>().vec(), y.down_cast<uBLASVector>().vec(), true);
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  void uBLASMatrix<Mat>::transpmult(const GenericVector& x, GenericVector& y) const
  {
    error("The transposed version of the uBLAS matrix-vector product is not yet implemented");
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  const uBLASMatrix<Mat>& uBLASMatrix<Mat>::operator*= (double a)
   {
    A *= a;
    return *this;
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  const uBLASMatrix<Mat>& uBLASMatrix<Mat>::operator/= (double a)
  {
    A /= a;
    return *this;
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  const GenericMatrix& uBLASMatrix<Mat>::operator= (const GenericMatrix& A)
  {
    *this = A.down_cast< uBLASMatrix<Mat> >();
    return *this;
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  inline const uBLASMatrix<Mat>& uBLASMatrix<Mat>::operator= (const uBLASMatrix<Mat>& A)
  {
    // Check for self-assignment
    if (this != &A)
    {
      // Assume uBLAS take care of deleting an existing Matrix
      // using its assignment operator
      this->A = A.mat();
    }
    return *this;
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  inline void uBLASMatrix<Mat>::compress()
  {
    Mat A_temp(this->size(0), this->size(1));
    A_temp.assign(A);
    A.swap(A_temp);
  }
  //-----------------------------------------------------------------------------
  template <class Mat>
  std::string uBLASMatrix<Mat>::str(bool verbose) const
  {
    typename Mat::const_iterator1 it1;  // Iterator over rows
    typename Mat::const_iterator2 it2;  // Iterator over entries

    std::stringstream s;

    if (verbose)
    {
      s << str(false) << std::endl << std::endl;
      for (it1 = A.begin1(); it1 != A.end1(); ++it1)
      {
        s << "|";
        for (it2 = it1.begin(); it2 != it1.end(); ++it2)
        {
          std::stringstream entry;
          entry << std::setiosflags(std::ios::scientific);
          entry << std::setprecision(16);
          entry << " (" << it2.index1() << ", " << it2.index2() << ", " << *it2 << ")";
          s << entry.str();
        }
        s << " |" << std::endl;
      }
    }
    else
    {
      s << "<uBLASMatrix of size " << size(0) << " x " << size(1) << ">";
    }

    return s.str();
  }
  //-----------------------------------------------------------------------------
  // Specialised member functions (must be inlined to avoid link errors)
  //-----------------------------------------------------------------------------
  template <>
  inline void uBLASMatrix<ublas_sparse_matrix>::init(const GenericSparsityPattern& sparsity_pattern)
  {
    resize(sparsity_pattern.size(0), sparsity_pattern.size(1));
    A.clear();

    // Reserve space for non-zeroes
    A.reserve(sparsity_pattern.num_nonzeros());

    // Get underlying pattern
    const SparsityPattern* pattern_pointer = dynamic_cast<const SparsityPattern*>(&sparsity_pattern);
    if (!pattern_pointer)
      error("Cannot convert GenericSparsityPattern to concrete SparsityPattern type. Aborting.");
    const std::vector<Set<uint> >& pattern = pattern_pointer->diagonal_pattern();

    // Add entries
    std::vector<Set<uint> >::const_iterator row;
    Set<uint>::const_iterator element;
    for(row = pattern.begin(); row != pattern.end(); ++row)
      for(element = row->begin(); element != row->end(); ++element)
        A.push_back(row - pattern.begin(), *element, 0.0);
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  inline void uBLASMatrix<Mat>::init(const GenericSparsityPattern& sparsity_pattern)
  {
    resize(sparsity_pattern.size(0), sparsity_pattern.size(1));
    A.clear();
  }
  //---------------------------------------------------------------------------
  template <>
  inline void uBLASMatrix<ublas_sparse_matrix>::apply(std::string mode)
  {
    // Make sure matrix assembly is complete
    A.complete_index1_data();
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  inline void uBLASMatrix<Mat>::apply(std::string mode)
  {
    // Do nothing
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  inline void uBLASMatrix<Mat>::axpy(double a, const GenericMatrix& A, bool same_nonzero_pattern)
  {
    // Check for same size
    if ( size(0) != A.size(0) or size(1) != A.size(1) )
      error("Matrices must be of same size.");

    this->A += (a)*(A.down_cast<uBLASMatrix>().mat());
  }
  //---------------------------------------------------------------------------
  template <>
  inline std::tr1::tuple<const std::size_t*, const std::size_t*,
             const double*, int> uBLASMatrix<ublas_sparse_matrix>::data() const
  {
    typedef std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int> tuple;
    return tuple(&A.index1_data()[0], &A.index2_data()[0], &A.value_data()[0], A.nnz());
  }
  //---------------------------------------------------------------------------
  template <class Mat>
  inline std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int>
                                                 uBLASMatrix<Mat>::data() const
  {
    error("Unable to return pointers to underlying data for this uBLASMatrix type.");
    return std::tr1::tuple<const std::size_t*, const std::size_t*, const double*, int>(0, 0, 0, 0);
  }
  //---------------------------------------------------------------------------
  template<class Mat> template<class B>
  void uBLASMatrix<Mat>::solveInPlace(B& X)
  {
    const uint M = A.size1();
    assert(M == A.size2());

    // Create permutation matrix
    ublas::permutation_matrix<std::size_t> pmatrix(M);

    // Factorise (with pivoting)
    uint singular = ublas::lu_factorize(A, pmatrix);
    if( singular > 0)
      error("Singularity detected in uBLAS matrix factorization on line %u.", singular-1);

    // Back substitute
    ublas::lu_substitute(A, pmatrix, X);
  }
  //-----------------------------------------------------------------------------
}

#endif
