// Copyright (C) 2008 Martin Sandve Alnes, Kent-Andre Mardal and Johannes Ring
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Anders Logg 2008-2012
// Modified by Garth N. Wells 2008, 2009
//
// First added:  2008-04-21
// Last changed: 2012-08-20

#ifndef __EPETRA_MATRIX_H
#define __EPETRA_MATRIX_H

#ifdef HAS_TRILINOS

#include <memory>
#include "GenericMatrix.h"
#include <Teuchos_RCP.hpp>

class Epetra_FECrsMatrix;
class Epetra_CrsGraph;

namespace dolfin
{

  /// Forward declarations
  class TensorLayout;

  /// This class provides a simple matrix class based on Epetra.
  /// It is a simple wrapper for an Epetra matrix object (Epetra_FECrsMatrix)
  /// implementing the GenericMatrix interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the Epetra_FECrsMatrix object using the function mat() and
  /// use the standard Epetra interface.

  class EpetraMatrix : public GenericMatrix
  {
  public:

    /// Create empty matrix
    EpetraMatrix();

    /// Copy constuctor
    EpetraMatrix(const EpetraMatrix& A);

    /// Create matrix from given Epetra_FECrsMatrix pointer
    explicit EpetraMatrix(Teuchos::RCP<Epetra_FECrsMatrix> A);

    /// Create matrix from given Epetra_FECrsMatrix pointer
    explicit EpetraMatrix(std::shared_ptr<Epetra_FECrsMatrix> A);

    /// Create matrix from given Epetra_CrsGraph
    explicit EpetraMatrix(const Epetra_CrsGraph& graph);

    /// Destructor
    virtual ~EpetraMatrix();

    //--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using tensor layout
    virtual void init(const TensorLayout& tensor_layout);

    /// Return true if empty
    virtual bool empty() const;

    /// Return size of given dimension
    virtual std::size_t size(std::size_t dim) const;

    /// Return local ownership range
    virtual std::pair<std::size_t, std::size_t>
      local_range(std::size_t dim) const;

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor. The following values are recognized
    /// for the mode parameter:
    ///
    ///   add    - corresponding to Epetra GlobalAssemble(Add)
    ///   insert - corresponding to Epetra GlobalAssemble(Insert)
    virtual void apply(std::string mode);

    /// Return MPI communicator
    virtual MPI_Comm mpi_comm() const;

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const;

    //--- Implementation of the GenericMatrix interface ---

    /// Return copy of matrix
    virtual std::shared_ptr<GenericMatrix> copy() const;

    /// Intialize vector z to be compatible with the matrix-vector
    /// product y = Ax. In the parallel case, both size and layout are
    /// important.
    ///
    /// *Arguments*
    ///     dim (std::size_t)
    ///         The dimension (axis): dim = 0 --> z = y, dim = 1 --> z = x
    virtual void init_vector(GenericVector& z, std::size_t dim) const;

    /// Get block of values
    virtual void get(double* block, std::size_t m, const dolfin::la_index* rows,
                     std::size_t n, const dolfin::la_index* cols) const;

    /// Set block of values
    virtual void set(const double* block, std::size_t m,
                     const dolfin::la_index* rows, std::size_t n,
                     const dolfin::la_index* cols);

    /// Add block of values
    virtual void add(const double* block, std::size_t m,
                     const dolfin::la_index* rows, std::size_t n,
                     const dolfin::la_index* cols);

    /// Add multiple of given matrix (AXPY operation)
    virtual void axpy(double a, const GenericMatrix& A,
                      bool same_nonzero_pattern);

    /// Return norm of matrix
    virtual double norm(std::string norm_type) const;

    /// Get non-zero values of given row
    virtual void getrow(std::size_t row, std::vector<std::size_t>& columns,
                        std::vector<double>& values) const;

    /// Set values for given row
    virtual void setrow(std::size_t row,
                        const std::vector<std::size_t>& columns,
                        const std::vector<double>& values);

    /// Set given rows to zero
    virtual void zero(std::size_t m, const dolfin::la_index* rows);

    /// Set given rows to identity matrix
    virtual void ident(std::size_t m, const dolfin::la_index* rows);

    // Matrix-vector product, y = Ax
    virtual void mult(const GenericVector& x, GenericVector& y) const;

    // Matrix-vector product, y = A^T x
    virtual void transpmult(const GenericVector& x, GenericVector& y) const;

    /// Set diagonal of a matrix
    virtual void set_diagonal(const GenericVector& x);

    /// Multiply matrix by given number
    virtual const EpetraMatrix& operator*= (double a);

    /// Divide matrix by given number
    virtual const EpetraMatrix& operator/= (double a);

    /// Assignment operator
    virtual const GenericMatrix& operator= (const GenericMatrix& x);

    //--- Special functions ---

    /// Return linear algebra backend factory
    virtual GenericLinearAlgebraFactory& factory() const;

    //--- Special Epetra functions ---

    /// Return Epetra_FECrsMatrix pointer
    std::shared_ptr<Epetra_FECrsMatrix> mat() const;

    /// Assignment operator
    const EpetraMatrix& operator= (const EpetraMatrix& x);

  private:

    // Epetra_FECrsMatrix pointer
    std::shared_ptr<Epetra_FECrsMatrix> _matA;

    // Epetra_FECrsMatrix pointer, used when initialized with a
    // Teuchos::RCP shared_ptr
    Teuchos::RCP<Epetra_FECrsMatrix> ref_keeper;
  };

}

#endif // HAS_TRILINOS
#endif // __EPETRA_MATRIX_H
