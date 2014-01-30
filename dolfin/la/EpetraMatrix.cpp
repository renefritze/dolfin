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
// Modified by Garth N. Wells 2008-2010
// Modified by Mikael Mortensen 2011
//
// First added:  2008-04-21
// Last changed: 2012-03-15

#ifdef HAS_TRILINOS

// Included here to avoid a C++ problem with some MPI implementations
#include <dolfin/common/MPI.h>

#include <cstring>
#include <iostream>
#include <iomanip>
#include <set>
#include <sstream>
#include <utility>

#include <Epetra_BlockMap.h>
#include <Epetra_CrsGraph.h>
#include <Epetra_CrsMatrix.h>
#include <Epetra_FECrsGraph.h>
#include <Epetra_FECrsMatrix.h>
#include <Epetra_FEVector.h>
#include <Epetra_MpiComm.h>
#include <Epetra_SerialComm.h>
#include <EpetraExt_MatrixMatrix.h>

#include <dolfin/common/Timer.h>
#include <dolfin/common/MPI.h>
#include <dolfin/common/NoDeleter.h>
#include <dolfin/log/dolfin_log.h>
#include "EpetraVector.h"
#include "GenericSparsityPattern.h"
#include "SparsityPattern.h"
#include "EpetraFactory.h"
#include "TensorLayout.h"
#include "EpetraMatrix.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
EpetraMatrix::EpetraMatrix()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraMatrix::EpetraMatrix(const EpetraMatrix& A)
{
  if (A.mat())
    _A.reset(new Epetra_FECrsMatrix(*A.mat()));
}
//-----------------------------------------------------------------------------
EpetraMatrix::EpetraMatrix(Teuchos::RCP<Epetra_FECrsMatrix> A)
  : _A(reference_to_no_delete_pointer(*A.get())), ref_keeper(A)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraMatrix::EpetraMatrix(boost::shared_ptr<Epetra_FECrsMatrix> A) : _A(A)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraMatrix::EpetraMatrix(const Epetra_CrsGraph& graph)
    : _A(new Epetra_FECrsMatrix(Copy, graph))
{
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraMatrix::~EpetraMatrix()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
void EpetraMatrix::init(const TensorLayout& tensor_layout)
{
  if (this->empty())
  {
    #ifdef DOLFIN_DEPRECATION_ERROR
    error("EpetraMatrix cannot be initialized more than once. Remove build definition -DDOLFIN_DEPRECATION_ERROR to change this to a warning.");
    #else
    warning("EpetraMatrix should not be initialized more than once. In version > 1.4, this will become an error.");
    #endif
  }

  if (_A && !_A.unique())
  {
    dolfin_error("EpetraMatrix.cpp",
                 "initialize Epetra matrix",
                 "More than one object points to the underlying Epetra object");
  }

  // Get local range
  const std::pair<std::size_t, std::size_t> range
    = tensor_layout.local_range(0);
  const std::size_t num_local_rows = range.second - range.first;
  const std::size_t n0 = range.first;

  dolfin_assert(tensor_layout.sparsity_pattern());
  const SparsityPattern& _pattern
    = dynamic_cast<const SparsityPattern&>(*tensor_layout.sparsity_pattern());
  const std::vector<std::vector<std::size_t> > d_pattern
    = _pattern.diagonal_pattern(SparsityPattern::unsorted);
  const std::vector<std::vector<std::size_t> > o_pattern
      = _pattern.off_diagonal_pattern(SparsityPattern::unsorted);

  // Get number of non-zeroes per row
  std::vector<std::size_t> num_nonzeros;
  _pattern.num_local_nonzeros(num_nonzeros);

  // Create row map
  #ifdef HAS_MPI
  Epetra_MpiComm epetra_comm(tensor_layout.mpi_comm());
  #else
  Epetra_SerialComm epetra_comm;
  #endif

  Epetra_Map row_map((dolfin::la_index) tensor_layout.size(0),
                     (dolfin::la_index) num_local_rows, 0, epetra_comm);

  // For rectangular matrices with more columns than rows, the columns
  // which are larger than those in row_map are marked as nonlocal
  // (and assembly fails).  The domain_map fixes that problem, at
  // least in the serial case.  FIXME: Needs attention in the parallel
  // case. Maybe range_map is also req'd.
  const std::pair<std::size_t, std::size_t> colrange
    = tensor_layout.local_range(1);
  const int num_local_cols = colrange.second - colrange.first;
  Epetra_Map domain_map((dolfin::la_index) tensor_layout.size(1),
                        num_local_cols, 0, epetra_comm);

  // Create Epetra_FECrsGraph
  const std::vector<int> _num_nonzeros(num_nonzeros.begin(),
                                       num_nonzeros.end());
  Epetra_CrsGraph matrix_map(Copy, row_map, _num_nonzeros.data());

  // Add diagonal block indices
  for (std::size_t local_row = 0; local_row < d_pattern.size(); local_row++)
  {
    const dolfin::la_index global_row = local_row + n0;
    std::vector<dolfin::la_index> entries(d_pattern[local_row].begin(),
                                          d_pattern[local_row].end());
    matrix_map.InsertGlobalIndices(global_row,
                                   (dolfin::la_index) entries.size(),
                                   entries.data());
  }

  // Add off-diagonal block indices (parallel only)
  for (std::size_t local_row = 0; local_row < o_pattern.size(); local_row++)
  {
    const std::size_t global_row = local_row + n0;
    std::vector<dolfin::la_index> entries(o_pattern[local_row].begin(),
                                          o_pattern[local_row].end());
    matrix_map.InsertGlobalIndices(global_row, entries.size(), entries.data());
  }

  try
  {
    // Finalise map. Here, row_map is standing in for RangeMap, which is
    // probably ok but should be double-checked.
    //matrix_map.GlobalAssemble(domain_map, row_map);
    //matrix_map.OptimizeStorage();
    matrix_map.FillComplete(domain_map, row_map);
    matrix_map.OptimizeStorage();
  }
  catch (int err)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "initialize Epetra matrix",
                 "Epetra threw error %d in assembly of domain map", err);
  }

  // Create matrix
  _A.reset(new Epetra_FECrsMatrix(Copy, matrix_map));
}
//-----------------------------------------------------------------------------
boost::shared_ptr<GenericMatrix> EpetraMatrix::copy() const
{
  boost::shared_ptr<EpetraMatrix> B(new EpetraMatrix(*this));
  return B;
}
//-----------------------------------------------------------------------------
bool EpetraMatrix::empty() const
{
  return _A ? true : false;
}
//-----------------------------------------------------------------------------
std::size_t EpetraMatrix::size(std::size_t dim) const
{
  if (dim > 1)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "access size of Epetra matrix",
                 "Illegal axis (%d), must be 0 or 1", dim);
  }

  if (_A)
  {
    const std::size_t M = _A->NumGlobalRows64();
    const std::size_t N = _A->NumGlobalCols64();
    return (dim == 0 ? M : N);
  }
  else
    return 0;
}
//-----------------------------------------------------------------------------
std::pair<std::size_t, std::size_t>
EpetraMatrix::local_range(std::size_t dim) const
{
  dolfin_assert(dim < 2);
  if (dim == 1)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "access local column range for Epetra matrix",
                 "Only local row range is available for Epetra matrices");
  }

  dolfin_assert(_A);
  const Epetra_BlockMap& row_map = _A->RowMap();
  dolfin_assert(row_map.LinearMap());

  return std::make_pair(row_map.MinMyGID64(), row_map.MaxMyGID64() + 1);
}
//-----------------------------------------------------------------------------
void EpetraMatrix::init_vector(GenericVector& z, std::size_t dim) const
{
  if (!z.empty())
  {
    #ifdef DOLFIN_DEPRECATION_ERROR
    error("EpetraVector may not be initialized more than once. Remove build definiton -DDOLFIN_DEPRECATION_ERROR to change this to a warning.");
    #else
    warning("EpetraVector may not be initialized more than once. In version > 1.4, this will become an error.");
    #endif
  }

  dolfin_assert(_A);

  // Get map appropriate map
  const Epetra_Map* map = 0;
  if (dim == 0)
    map = &(_A->RangeMap());
  else if (dim == 1)
    map = &(_A->DomainMap());
  else
  {
    dolfin_error("EpetraMatrix.cpp",
                 "initialize Epetra vector to match Epetra matrix",
                 "Dimension must be 0 or 1, not %d", dim);
  }

  // Reset vector with new map
  EpetraVector& _z = as_type<EpetraVector>(z);
  _z.init(*map);
}
//-----------------------------------------------------------------------------
void EpetraMatrix::get(double* block, std::size_t m,
                       const dolfin::la_index* rows,
                       std::size_t n, const dolfin::la_index* cols) const
{
  dolfin_assert(_A);

  int num_entities = 0;
  int* indices;
  double* values;

  // For each row in rows
  for(std::size_t i = 0; i < m; ++i)
  {
    // Extract the values and indices from row: rows[i]
    if (_A->IndicesAreLocal())
    {
      const int err = _A->ExtractMyRowView(rows[i], num_entities, values,
                                          indices);
      if (err != 0)
      {
        dolfin_error("EpetraMatrix",
                     "get block of values from Epetra matrix",
                     "Did not manage to perform Epetra_FECrsMatrix::ExtractMyRowView");
      }
    }
    else
    {
      const int err = _A->ExtractGlobalRowView(rows[i], num_entities, values,
                                               indices);
      if (err != 0)
      {
        dolfin_error("EpetraMatrix",
                     "get block of values from Epetra matrix",
                     "Did not manage to perform Epetra_FECrsMatrix::ExtractMyRowView");
      }
    }

    // Step the indices to the start of cols
    int k = 0;
    while (indices[k] < (int) cols[0])
      k++;

    // Fill the collumns in the block
    for (std::size_t j = 0; j < n; j++)
    {
      if (k < num_entities and indices[k] == (int) cols[j])
      {
        block[i*n + j] = values[k];
        k++;
      }
      else
        block[i*n + j] = 0.0;
    }
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::set(const double* block,
                       std::size_t m, const dolfin::la_index* rows,
                       std::size_t n, const dolfin::la_index* cols)
{
  // This function is awkward and somewhat restrictive because of the
  // poor support for setting off-process values in Epetra

  dolfin_assert(_A);

  const int err = _A->ReplaceGlobalValues(m, rows, n, cols, block,
                                          Epetra_FECrsMatrix::ROW_MAJOR);
  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "set block of values for Epetra matrix",
                 "Did not manage to perform Epetra_FECrsMatrix::ReplaceGlobalValues");
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::add(const double* block,
                       std::size_t m, const dolfin::la_index* rows,
                       std::size_t n, const dolfin::la_index* cols)
{
  dolfin_assert(_A);
  const int err = _A->SumIntoGlobalValues(m, rows, n, cols, block,
                                         Epetra_FECrsMatrix::ROW_MAJOR);
  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "add block of values to Epetra matrix",
                 "Did not manage to perform Epetra_FECrsMatrix::SumIntoGlobalValues");
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::axpy(double a, const GenericMatrix& A,
                        bool same_nonzero_pattern)
{
  const EpetraMatrix* AA = &as_type<const EpetraMatrix>(A);
  if (!AA->mat()->Filled())
  {
    dolfin_error("EpetraMatrix.cpp",
                 "perform axpy operation with Epetra matrix",
                 "Epetra matrix is not in the correct state");
  }

  const int err = EpetraExt::MatrixMatrix::Add(*(AA->mat()), false, a,
                                               *_A, 1.0);
  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "perform axpy operation with Epetra matrix",
                 "Did not manage to perform EpetraExt::MatrixMatrix::Add. If the matrix has been assembled, the nonzero patterns must match");
  }
}
//-----------------------------------------------------------------------------
double EpetraMatrix::norm(std::string norm_type) const
{
  dolfin_assert(_A);
  if (norm_type == "l1")
    return _A->NormOne();
  else if (norm_type == "linf")
    return _A->NormInf();
  else if (norm_type == "frobenius")
    return _A->NormFrobenius();
  else
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute norm of Epetra matrix",
                 "Unknown norm type: \"%s\"", norm_type.c_str());
    return 0.0;
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::zero()
{
  dolfin_assert(_A);
  const int err = _A->PutScalar(0.0);
  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "zero Epetra matrix",
                 "Did not manage to perform Epetra_FECrsMatrix::PutScalar");
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::apply(std::string mode)
{
  Timer timer("Apply (EpetraMatrix)");

  dolfin_assert(_A);
  int err = 0;
  if (mode == "add")
    err = _A->GlobalAssemble(true, Add);
  else if (mode == "insert")
    err = _A->GlobalAssemble(true);
  else if (mode == "flush")
  {
    // Do nothing
  }
  else
  {
    dolfin_error("EpetraMatrix.cpp",
                 "apply changes to Epetra matrix",
                 "Unknown apply mode \"%s\"", mode.c_str());
  }

  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "apply changes to Epetra matrix",
                 "Did not manage to perform Epetra_FECrsMatrix::GlobalAssemble");
  }
}
//-----------------------------------------------------------------------------
MPI_Comm EpetraMatrix::mpi_comm() const
{
  dolfin_assert(_A);
  MPI_Comm mpi_comm = MPI_COMM_NULL;
  #ifdef HAS_MPI
  // Get Epetra MPI communicator (downcast)
  const Epetra_MpiComm* epetra_mpi_comm
    = dynamic_cast<const Epetra_MpiComm*>(&(_A->Map().Comm()));
  dolfin_assert(epetra_mpi_comm);
  mpi_comm = epetra_mpi_comm->Comm();
  #else
  mpi_comm = MPI_COMM_SELF;
  #endif

  return mpi_comm;
}
//-----------------------------------------------------------------------------
std::string EpetraMatrix::str(bool verbose) const
{
  if (!_A)
    return "<Uninitialized EpetraMatrix>";

  std::stringstream s;
  if (verbose)
  {
    warning("Verbose output for EpetraMatrix not implemented, calling Epetra Print directly.");
    dolfin_assert(_A);
    _A->Print(std::cout);
  }
  else
    s << "<EpetraMatrix of size " << size(0) << " x " << size(1) << ">";

  return s.str();
}
//-----------------------------------------------------------------------------
void EpetraMatrix::ident(std::size_t m, const dolfin::la_index* rows)
{
  dolfin_assert(_A);
  dolfin_assert(_A->Filled() == true);

  // FIXME: This is a major hack and will not scale for large numbers
  // of processes. The problem is that a dof is not guaranteed to
  // reside on the same process as one of cells to which it belongs
  // (which is bad, but is due to the sparsity pattern
  // computation). This function only work for locally owned rows (the
  // PETSc version works for any row).

  typedef boost::unordered_set<std::size_t> MySet;

  // Number of MPI processes
  const std::size_t num_processes = _A->Comm().NumProc();
  const std::size_t process_number = _A->Comm().MyPID();

  // Build lists of local and nonlocal rows
  MySet local_rows;
  std::vector<std::size_t> non_local_rows;
  for (std::size_t i = 0; i < m; ++i)
  {
    if (_A->MyGlobalRow(rows[i]))
      local_rows.insert(rows[i]);
    else
      non_local_rows.push_back(rows[i]);
  }

  // If parallel, send non_local rows to all processes
  if (num_processes > 1)
  {
    // Send list of nonlocal rows to all processes
    std::vector<std::vector<std::size_t> >  send_data(num_processes);
    for (std::size_t p = 0; p < num_processes; ++p)
    {
      if (p != process_number)
      {
        send_data[p].insert(send_data[p].end(), non_local_rows.begin(),
                            non_local_rows.end());
      }
    }

    std::vector<std::vector<std::size_t> > received_data;
    MPI::all_to_all(mpi_comm(), send_data, received_data);

    // Unpack data
    for (std::size_t p = 0; p < num_processes; ++p)
    {
      for (std::size_t i = 0; i < received_data[p].size(); ++i)
      {
        // Insert row into set if it's local
        const dolfin::la_index new_index = received_data[p][i];
        if (_A->MyGlobalRow(new_index))
          local_rows.insert(new_index);
      }
    }
  }

  const Epetra_CrsGraph& graph = _A->Graph();
  MySet::const_iterator global_row;
  for (global_row = local_rows.begin(); global_row != local_rows.end();
       ++global_row)
  {
    // Get local row index
    const dolfin::la_index _global_row = *global_row;
    const int local_row = _A->LRID(_global_row);

    // If this process owns row, then zero row
    if (local_row >= 0)
    {
      // Get row map
      int num_nz = 0;
      int* column_indices;
      int err = graph.ExtractMyRowView(local_row, num_nz, column_indices);
      if (err != 0)
      {
        dolfin_error("EpetraMatrix.cpp",
                     "set rows of Epetra matrix to identity matrix",
                     "Did not manage to extract row map");
      }

      // Zero row
      std::vector<double> block(num_nz, 0.0);
      err = _A->ReplaceMyValues(local_row, num_nz, &block[0], column_indices);
      if (err != 0)
      {
        dolfin_error("EpetraMatrix.cpp",
                     "set rows of Epetra matrix to identity matrix",
                     "Did not manage to perform Epetra_FECrsMatrix::ReplaceGlobalValues");
      }

      // Place one on the diagonal
      const double one = 1.0;
      _A->ReplaceMyValues(local_row, 1, &one, &local_row);
    }
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::zero(std::size_t m, const dolfin::la_index* rows)
{
  // FIXME: This can be made more efficient by eliminating creation of
  //        some obejcts inside the loop

  dolfin_assert(_A);
  const Epetra_CrsGraph& graph = _A->Graph();
  for (std::size_t i = 0; i < m; ++i)
  {
    const int row = rows[i];
    const int num_nz = graph.NumGlobalIndices(row);
    std::vector<int> indices(num_nz);

    int out_num = 0;
    graph.ExtractGlobalRowCopy(row, num_nz, out_num, &indices[0]);

    std::vector<double> block(num_nz);
    const int err = _A->ReplaceGlobalValues(row, num_nz, block.data(),
                                            indices.data());
    if (err != 0)
    {
      dolfin_error("EpetraMatrix.cpp",
                   "zero Epetra matrix",
                   "Did not manage to perform Epetra_FECrsMatrix::ReplaceGlobalValues");
    }
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::mult(const GenericVector& x_, GenericVector& Ax_) const
{
  dolfin_assert(_A);
  const EpetraVector& x = as_type<const EpetraVector>(x_);
  EpetraVector& Ax = as_type<EpetraVector>(Ax_);

  if (x.size() != size(1))
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute matrix-vector product with Epetra matrix",
                 "Non-matching dimensions for matrix-vector product");
  }

  // Initialize RHS
  if (Ax.empty())
    this->init_vector(Ax, 0);

  if (Ax.size() != size(0))
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute matrix-vector product with Epetra matrix",
                 "Vector for matrix-vector result has wrong size");
  }

  dolfin_assert(x.vec());
  dolfin_assert(Ax.vec());
  const int err = _A->Multiply(false, *(x.vec()), *(Ax.vec()));
  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute matrix-vector product with Epetra matrix",
                 "Did not manage to perform Epetra_FECrsMatrix::Multiply.");
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::transpmult(const GenericVector& x_, GenericVector& Ax_) const
{
  dolfin_assert(_A);
  const EpetraVector& x = as_type<const EpetraVector>(x_);
  EpetraVector& Ax = as_type<EpetraVector>(Ax_);

  if (x.size() != size(0))
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute transpose matrix-vector product with Epetra matrix",
                 "Non-matching dimensions for transpose matrix-vector product");
  }

  // Initialize RHS
  if (Ax.empty())
    this->init_vector(Ax, 1);

  if (Ax.size() != size(1))
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute transpose matrix-vector product with Epetra matrix",
                 "Vector for transpose matrix-vector result has wrong size");
  }

  const int err = _A->Multiply(true, *(x.vec()), *(Ax.vec()));
  if (err != 0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "compute transpose matrix-vector product with Epetra matrix",
                 "Did not manage to perform Epetra_FECrsMatrix::Multiply");
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::getrow(std::size_t row, std::vector<std::size_t>& columns,
                          std::vector<double>& values) const
{
  dolfin_assert(_A);

  // Get local row index
  const dolfin::la_index _row = row;
  const int local_row_index = _A->LRID(_row);

  // If this process has part of the row, get values
  if (local_row_index >= 0)
  {
    // Temporary variables
    int* indices;
    double* vals;
    int num_entries;

    // Extract data from Epetra matrix
    const int err = _A->ExtractMyRowView(local_row_index, num_entries, vals,
                                         indices);
    if (err != 0)
    {
      dolfin_error("EpetraMatrix.cpp",
                   "extract row of Epetra matrix",
                   "Did not manage to perform Epetra_FECrsMatrix::ExtractMyRowView");
    }

    // Put data in columns and values
    columns.resize(num_entries);
    values.resize(num_entries);
    for (int i = 0; i < num_entries; i++)
    {
      columns[i] = _A->GCID(indices[i]);  // Return global column indices
      values[i]  = vals[i];
    }
  }
  else
  {
    columns.resize(0);
    values.resize(0);
  }
}
//-----------------------------------------------------------------------------
void EpetraMatrix::setrow(std::size_t row,
                          const std::vector<std::size_t>& columns,
                          const std::vector<double>& values)
{
  static bool print_msg_once = true;
  if (print_msg_once)
  {
    info("EpetraMatrix::setrow is implemented inefficiently");
    print_msg_once = false;
  }

  for (std::size_t i = 0; i < columns.size(); i++)
  {
    dolfin::la_index _row = row;
    dolfin::la_index _col = columns[i];
    set(&values[i], 1, &_row, 1, &_col);
  }
}
//-----------------------------------------------------------------------------
GenericLinearAlgebraFactory& EpetraMatrix::factory() const
{
  return EpetraFactory::instance();
}
//-----------------------------------------------------------------------------
boost::shared_ptr<Epetra_FECrsMatrix> EpetraMatrix::mat() const
{
  return _A;
}
//-----------------------------------------------------------------------------
const EpetraMatrix& EpetraMatrix::operator*= (double a)
{
  dolfin_assert(_A);
  const int err = _A->Scale(a);
  if (err !=0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "multiply Epetra matrix by scalar",
                 "Did not manage to perform Epetra_FECrsMatrix::Scale");
  }
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraMatrix& EpetraMatrix::operator/= (double a)
{
  dolfin_assert(_A);
  int err = _A->Scale(1.0/a);
  if (err !=0)
  {
    dolfin_error("EpetraMatrix.cpp",
                 "divide Epetra matrix by scalar",
                 "Did not manage to perform Epetra_FECrsMatrix::Scale");
  }
  return *this;
}
//-----------------------------------------------------------------------------
const GenericMatrix& EpetraMatrix::operator= (const GenericMatrix& A)
{
  *this = as_type<const EpetraMatrix>(A);
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraMatrix& EpetraMatrix::operator= (const EpetraMatrix& A)
{
  if (A.mat())
    _A.reset(new Epetra_FECrsMatrix(*A.mat()));
  else
    A.mat().reset();
  return *this;
}
//-----------------------------------------------------------------------------

#endif
