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
// Modified by Garth N. Wells 2008-2010
// Modified by Anders Logg 2011-2012
//
// First added:  2008-04-21
// Last changed: 2012-08-22

#ifdef HAS_TRILINOS

// Included here to avoid a C++ problem with some MPI implementations
#include <dolfin/common/MPI.h>

#include <cmath>
#include <cstring>
#include <numeric>
#include <utility>
#include <boost/scoped_ptr.hpp>

#include <Epetra_FEVector.h>
#include <Epetra_Export.h>
#include <Epetra_Import.h>
#include <Epetra_BlockMap.h>
#include <Epetra_MultiVector.h>
#include <Epetra_MpiComm.h>
#include <Epetra_SerialComm.h>
#include <Epetra_Vector.h>
#include <Epetra_DataAccess.h>

#include <dolfin/common/Timer.h>
#include <dolfin/common/Array.h>
#include <dolfin/common/Set.h>
#include <dolfin/common/MPI.h>
#include <dolfin/log/dolfin_log.h>
#include "uBLASVector.h"
#include "PETScVector.h"
#include "EpetraVector.h"
#include "EpetraFactory.h"

using namespace dolfin;

//-----------------------------------------------------------------------------
EpetraVector::EpetraVector()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraVector::EpetraVector(MPI_Comm comm, std::size_t N)
{
  // Create Epetra vector
  resize(comm, N);
}
//-----------------------------------------------------------------------------
EpetraVector::EpetraVector(boost::shared_ptr<Epetra_FEVector> x) : _x(x)
{
  // Do nothing
}
//-----------------------------------------------------------------------------
EpetraVector::EpetraVector(const Epetra_BlockMap& map)
{
  _x.reset(new Epetra_FEVector(map));
}
//-----------------------------------------------------------------------------
EpetraVector::EpetraVector(const EpetraVector& v)
{
  // Copy Epetra vector
  dolfin_assert(v._x);
  _x.reset(new Epetra_FEVector(*(v._x)));

  // Copy ghost data
  if (v.x_ghost)
    x_ghost.reset(new Epetra_Vector(*(v.x_ghost)));
  ghost_global_to_local = v.ghost_global_to_local;
}
//-----------------------------------------------------------------------------
EpetraVector::~EpetraVector()
{
  // Do nothing
}
//-----------------------------------------------------------------------------
boost::shared_ptr<GenericVector> EpetraVector::copy() const
{
  dolfin_assert(_x);
  boost::shared_ptr<GenericVector> y(new EpetraVector(*this));
  return y;
}
//-----------------------------------------------------------------------------
void EpetraVector::resize(MPI_Comm comm, std::size_t N)
{
  // Create empty ghost vertices vector
  std::vector<la_index> ghost_indices;

  // Compute local ownership range
  const std::pair<std::size_t, std::size_t> range = MPI::local_range(comm, N);

  // Resize vector
  resize(comm, range, ghost_indices);
}
//-----------------------------------------------------------------------------
void EpetraVector::resize(MPI_Comm comm,
                          std::pair<std::size_t, std::size_t> range)
{
  std::vector<la_index> ghost_indices;
  resize(comm, range, ghost_indices);
}
//-----------------------------------------------------------------------------
void EpetraVector::resize(MPI_Comm comm,
                          std::pair<std::size_t, std::size_t> range,
                          const std::vector<la_index>& ghost_indices)
{
  if (_x && !_x.unique())
  {
    dolfin_error("EpetraVector.cpp",
                 "resize Epetra vector",
                 "More than one object points to the underlying Epetra object");
  }

  // Create ghost data structures
  ghost_global_to_local.clear();

  Epetra_SerialComm epetra_serial_comm;
  #ifdef HAS_MPI
    Epetra_MpiComm epetra_comm(comm);
  #else
    Epetra_SerialComm epetra_comm;
    if (!ghost_indices.empty() && epetra_comm.NumProc() > 1)
    {
      dolfin_error("EpetraVector.cpp",
                   "resize Epetra vector",
                   "Serial EpetraVectors do not support ghost points");
    }
  #endif

  // Pointer to Epetra map
  boost::scoped_ptr<Epetra_BlockMap> epetra_map;

  // Compute local size
  const dolfin::la_index local_size = range.second - range.first;
  dolfin_assert(range.second >= range.first);

  const dolfin::la_index _global_size = -1;
  const int _local_size = local_size;
  const int _element_size = 1;
  const int _index_base = 0;
  epetra_map.reset(new Epetra_BlockMap(_global_size, _local_size, _element_size,
                                       _index_base, epetra_comm));

  // Build global-to-local map for ghost indices
  for (std::size_t i = 0; i < ghost_indices.size(); ++i)
  {
    ghost_global_to_local.insert(std::pair<std::size_t,
                                 std::size_t>(ghost_indices[i], i));
  }

  // Create vector
  _x.reset(new Epetra_FEVector(*epetra_map));

  // Create local ghost vector
  const dolfin::la_index num_ghost_entries = ghost_indices.size();
  const std::vector<dolfin::la_index> ghost_entries(ghost_indices.begin(),
                                                    ghost_indices.end());
  Epetra_BlockMap ghost_map(num_ghost_entries, num_ghost_entries,
                            ghost_entries.data(), 1, 0, epetra_serial_comm);
  x_ghost.reset(new Epetra_Vector(ghost_map));
}
//-----------------------------------------------------------------------------
bool EpetraVector::empty() const
{
  return _x ? _x->GlobalLength64() == 0: true;
}
//-----------------------------------------------------------------------------
std::size_t EpetraVector::size() const
{
  return _x ? _x->GlobalLength64() : 0;
}
//-----------------------------------------------------------------------------
std::size_t EpetraVector::local_size() const
{
  return _x ? _x->MyLength(): 0;
}
//-----------------------------------------------------------------------------
std::pair<std::size_t, std::size_t> EpetraVector::local_range() const
{
  dolfin_assert(_x);
  dolfin_assert(_x->Map().LinearMap());
  const Epetra_BlockMap& map = _x->Map();
  return std::make_pair<std::size_t, std::size_t>(map.MinMyGID(),
                                                  map.MaxMyGID() + 1);
}
//-----------------------------------------------------------------------------
bool EpetraVector::owns_index(std::size_t i) const
{
  dolfin_assert(_x);
  const dolfin::la_index _i = i;
  return _x->Map().MyGID(_i);
}
//-----------------------------------------------------------------------------
void EpetraVector::zero()
{
  Timer timer("Apply (EpetraVector)");
  dolfin_assert(_x);
  const int err = _x->PutScalar(0.0);
  if (err != 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "zero Epetra vector",
                 "Did not manage to perform Epetra_Vector::PutScalar");
  }
}
//-----------------------------------------------------------------------------
void EpetraVector::apply(std::string mode)
{
  dolfin_assert(_x);

  // Special treatement required for values applied using 'set'
  // because Epetra_FEVector::ReplaceGlobalValues does not behave well.
  // This would be simpler if we required that only local values (on
  // this process) can be set

  // Get communicator
  const Epetra_Comm& epetra_comm = _x->Map().Comm();

  int err = 0;
  int num_my_off_process = off_process_set_values.size();
  int num_off_proc = 0;
  epetra_comm.SumAll(&num_my_off_process, &num_off_proc, 1);
  if (num_off_proc  > 0)
  {
    std::vector<dolfin::la_index> non_local_indices;
    non_local_indices.reserve(off_process_set_values.size());
    std::vector<double> non_local_values;
    non_local_values.reserve(off_process_set_values.size());
    boost::unordered_map<std::size_t, double>::const_iterator entry;
    for (entry = off_process_set_values.begin();
         entry != off_process_set_values.end(); ++entry)
    {
      non_local_indices.push_back(entry->first);
      non_local_values.push_back(entry->second);
    }

    // Create map for y
    Epetra_BlockMap target_map(-1, non_local_indices.size(),
                               non_local_indices.data(), 1, 0, epetra_comm);

    // Create vector y (view of non_local_values)
    Epetra_Vector y(View, target_map, non_local_values.data());

    // Create importer
    Epetra_Import importer(_x->Map(), target_map);

    // Import off-process 'set' data
    if (mode == "add")
      _x->Import(y, importer, Add);
    else if (mode == "insert")
      _x->Import(y, importer, Insert);

    err = _x->GlobalAssemble(Add);
  }
  else
  {
    if (mode == "add")
      err = _x->GlobalAssemble(Add);
    else if (mode == "insert")
    {
      // This is 'Add' because we take of data that is set in the previous
      // code block. Epetra behaviour is very poor w.r.t 'set' - probably
      // need to keep track of state set/add internally.
      err = _x->GlobalAssemble(Add);
    }
    else
    {
      dolfin_error("EpetraVector.cpp",
                   "apply changes to Epetra vector",
                   "Unknown apply mode (\"%s\")", mode.c_str());
    }
  }

  // Check that call to GlobalAssemble was successful
  if (err != 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "apply changes to Epetra vector",
                 "Did not manage to perform Epetra_Vector::GlobalAssemble");
  }

  // Clear map of off-process set values
  off_process_set_values.clear();
}
//-----------------------------------------------------------------------------
const MPI_Comm EpetraVector::mpi_comm() const
{
  dolfin_assert(_x);
  MPI_Comm mpi_comm = MPI_COMM_NULL;
#ifdef HAS_MPI
  // Get Epetra MPI communicator (downcast)
  const Epetra_MpiComm* epetra_mpi_comm
    = dynamic_cast<const Epetra_MpiComm*>(&(_x->Map().Comm()));
  dolfin_assert(epetra_mpi_comm);
  mpi_comm = epetra_mpi_comm->Comm();
#else
  mpi_comm = MPI_COMM_SELF;
#endif

  return mpi_comm;
}
//-----------------------------------------------------------------------------
std::string EpetraVector::str(bool verbose) const
{
  if (!_x)
    return "<Uninitialized EpetraVector>";

  std::stringstream s;
  if (verbose)
  {
    dolfin_assert(_x);
    _x->Print(std::cout);
  }
  else
    s << "<EpetraVector of size " << size() << ">";

  return s.str();
}
//-----------------------------------------------------------------------------
void EpetraVector::get_local(std::vector<double>& values) const
{
  if (!_x)
  {
    values.clear();
    return;
  }

  values.resize(_x->MyLength());

  const int err = _x->ExtractCopy(values.data(), 0);
  if (err!= 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "access local values from Epetra vector",
                 "Did not manage to perform Epetra_Vector::ExtractCopy");
  }
}
//-----------------------------------------------------------------------------
void EpetraVector::set_local(const std::vector<double>& values)
{
  dolfin_assert(_x);
  const std::size_t local_size = _x->MyLength();

  if (values.size() != local_size)
  {
    dolfin_error("EpetraVector.cpp",
                 "set local values of Epetra vector",
                 "Size of values array is not equal to local vector size");
  }

  for (std::size_t i = 0; i < local_size; ++i)
    (*_x)[0][i] = values[i];
}
//-----------------------------------------------------------------------------
void EpetraVector::add_local(const Array<double>& values)
{
  dolfin_assert(_x);
  const std::size_t local_size = _x->MyLength();
  if (values.size() != local_size)
  {
    dolfin_error("EpetraVector.cpp",
                 "add local values to Epetra vector",
                 "Size of values array is not equal to local vector size");
  }

  for (std::size_t i = 0; i < local_size; ++i)
    (*_x)[0][i] += values[i];
}
//-----------------------------------------------------------------------------
void EpetraVector::set(const double* block, std::size_t m,
                       const dolfin::la_index* rows)
{
  dolfin_assert(_x);
  const Epetra_BlockMap& map = _x->Map();
  dolfin_assert(_x->Map().LinearMap());
  const dolfin::la_index n0 = map.MinMyGID64();
  const dolfin::la_index n1 = map.MaxMyGID64();

  // Set local values, or add to off-process cache for later communication
  for (std::size_t i = 0; i < m; ++i)
  {
    if (rows[i] >= n0 && rows[i] <= n1)
      (*_x)[0][rows[i] - n0] = block[i];
    else
      off_process_set_values[rows[i]] = block[i];
  }
}
//-----------------------------------------------------------------------------
void EpetraVector::add(const double* block, std::size_t m,
                       const dolfin::la_index* rows)
{
  if (!off_process_set_values.empty())
  {
    dolfin_error("EpetraVector.cpp",
                 "add block of values to Epetra vector",
                 "apply() must be called between calling EpetraVector::set and EpetraVector::add");
  }

  dolfin_assert(_x);
  int err = _x->SumIntoGlobalValues((int) m, rows, block);

  if (err != 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "add block of values to Epetra vector",
                 "Did not manage to perform Epetra_Vector::SumIntoGlobalValues");
  }
}
//-----------------------------------------------------------------------------
void EpetraVector::get_local(double* block, std::size_t m,
                             const dolfin::la_index* rows) const
{
  dolfin_assert(_x);
  const Epetra_BlockMap& map = _x->Map();
  dolfin_assert(_x->Map().LinearMap());
  const dolfin::la_index n0 = map.MinMyGID64();

  // Get values
  if (ghost_global_to_local.empty())
  {
    for (std::size_t i = 0; i < m; ++i)
      block[i] = (*_x)[0][rows[i] - n0];
  }
  else
  {
    dolfin_assert(x_ghost);
    const dolfin::la_index n1 = map.MaxMyGID();
    const Epetra_BlockMap& ghost_map = x_ghost->Map();
    for (std::size_t i = 0; i < m; ++i)
    {
      if (rows[i] >= n0 && rows[i] <= n1)
        block[i] = (*_x)[0][rows[i] - n0];
      else
      {
        // FIXME: Check if look-up in std::map is faster than
        // Epetra_BlockMap::LID Get local index
        const dolfin::la_index local_index = ghost_map.LID(rows[i]);
        dolfin_assert(local_index != -1);

        //boost::unordered_map<std::size_t, std::size_t>::const_iterator
        //  _local_index = ghost_global_to_local.find(rows[i]);
        //dolfin_assert(_local_index != ghost_global_to_local.end());
        //const int local_index = _local_index->second;

        // Get value
        block[i] = (*x_ghost)[local_index];
      }
    }
  }
}
//-----------------------------------------------------------------------------
void EpetraVector::gather(GenericVector& y,
                          const std::vector<dolfin::la_index>& indices) const
{
  dolfin_assert(_x);

  // Down cast to an EpetraVector
  EpetraVector& _y = as_type<EpetraVector>(y);

  // Create serial communicator
  Epetra_SerialComm epetra_serial_comm;

  // Create map for y
  Epetra_BlockMap target_map(indices.size(), indices.size(), indices.data(),
                             1, 0, epetra_serial_comm);

  // Reset vector y
  _y.reset(target_map);
  dolfin_assert(_y.vec());

  // Create importer
  Epetra_Import importer(target_map, _x->Map());

  // Import values into y
  _y.vec()->Import(*_x, importer, Insert);
}
//-----------------------------------------------------------------------------
void EpetraVector::gather(std::vector<double>& x,
                          const std::vector<dolfin::la_index>& indices) const
{
  const std::size_t _size = indices.size();
  x.resize(_size);
  dolfin_assert(x.size() == _size);

  // Gather values into a vector
  EpetraVector y;
  gather(y, indices);

  dolfin_assert(y.size() == _size);
  const Epetra_FEVector& _y = *(y.vec());

  // Copy values into x
  for (std::size_t i = 0; i < _size; ++i)
    x[i] = (_y)[0][i];
}
//-----------------------------------------------------------------------------
void EpetraVector::gather_on_zero(std::vector<double>& x) const
{
  // FIXME: Is there an Epetra function for this?
  dolfin_assert(_x);
  std::vector<dolfin::la_index> indices;
  if (_x->Comm().MyPID() == 0)
  {
    indices.resize(this->size());
    for (std::size_t i = 0; i < size(); ++i)
      indices[i] = i;
  }

  gather(x, indices);
}
//-----------------------------------------------------------------------------
void EpetraVector::reset(const Epetra_BlockMap& map)
{
  // Clear ghost data
  x_ghost.reset();
  ghost_global_to_local.clear();
  off_process_set_values.clear();

  _x.reset(new Epetra_FEVector(map));
}
//-----------------------------------------------------------------------------
boost::shared_ptr<Epetra_FEVector> EpetraVector::vec() const
{
  return _x;
}
//-----------------------------------------------------------------------------
double EpetraVector::inner(const GenericVector& y) const
{
  dolfin_assert(_x);

  const EpetraVector& v = as_type<const EpetraVector>(y);
  if (!v._x)
  {
    dolfin_error("EpetraVector.cpp",
                 "compute inner product with Epetra vector",
                 "Given vector is not initialized");
  }

  double a;
  const int err = _x->Dot(*(v._x), &a);
  if (err!= 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "compute inner product with Epetra vector",
                 "Did not manage to perform Epetra_Vector::Dot");
  }

  return a;
}
//-----------------------------------------------------------------------------
void EpetraVector::axpy(double a, const GenericVector& y)
{
  dolfin_assert(_x);

  const EpetraVector& _y = as_type<const EpetraVector>(y);
  if (!_y._x)
  {
    dolfin_error("EpetraVector.cpp",
                 "perform axpy operation with Epetra vector",
                 "Given vector is not initialized");
  }

  if (size() != _y.size())
  {
    dolfin_error("EpetraVector.cpp",
                 "perform axpy operation with Epetra vector",
                 "Vectors are not of the same size");
  }

  const int err = _x->Update(a, *(_y.vec()), 1.0);
  if (err != 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "perform axpy operation with Epetra vector",
                 "Did not manage to perform Epetra_Vector::Update");
  }
}
//-----------------------------------------------------------------------------
void EpetraVector::abs()
{
  dolfin_assert(_x);
  _x->Abs(*_x);
}
//-----------------------------------------------------------------------------
GenericLinearAlgebraFactory& EpetraVector::factory() const
{
  return EpetraFactory::instance();
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator= (const GenericVector& v)
{
  *this = as_type<const EpetraVector>(v);
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator= (double a)
{
  dolfin_assert(_x);
  _x->PutScalar(a);
  return *this;
}
//-----------------------------------------------------------------------------
void EpetraVector::update_ghost_values()
{
  dolfin_assert(_x);
  dolfin_assert(x_ghost);
  dolfin_assert(x_ghost->MyLength()
                == (dolfin::la_index) ghost_global_to_local.size());

  // Create importer
  Epetra_Import importer(x_ghost->Map(), _x->Map());

  // Import into ghost vector
  x_ghost->Import(*_x, importer, Insert);
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator= (const EpetraVector& v)
{
  // Check that vector lengths are equal
  if (size() != v.size())
  {
    dolfin_error("EpetraVector.cpp",
                 "assign one vector to another",
                 "Vectors must be of the same length when assigning. "
                 "Consider using the copy constructor instead");
  }

  // Check that maps (parallel layout) are the same
  dolfin_assert(_x);
  dolfin_assert(v._x);
  if (!_x->Map().SameAs(v._x->Map()))
  {
    dolfin_error("EpetraVector.cpp",
                 "assign one vector to another",
                 "Vectors must have the same parallel layout when assigning. "
                 "Consider using the copy constructor instead");
  }

  // Assign values
  *_x = *v._x;

  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator+= (const GenericVector& y)
{
  axpy(1.0, y);
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator+= (double a)
{
  dolfin_assert(_x);
  Epetra_FEVector y(*_x);
  y.PutScalar(a);
  _x->Update(1.0, y, 1.0);
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator-= (const GenericVector& y)
{
  axpy(-1.0, y);
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator-= (double a)
{
  dolfin_assert(_x);
  Epetra_FEVector y(*_x);
  y.PutScalar(-a);
  _x->Update(1.0, y, 1.0);
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator*= (double a)
{
  dolfin_assert(_x);
  const int err = _x->Scale(a);
  if (err!= 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "multiply Epetra vector by scalar",
                 "Did not manage to perform Epetra_Vector::Scale");
  }
  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator*= (const GenericVector& y)
{
  dolfin_assert(_x);
  const EpetraVector& v = as_type<const EpetraVector>(y);

  if (!v._x)
  {
    dolfin_error("EpetraVector.cpp",
                 "perform point-wise multiplication with Epetra vector",
                 "Given vector is not initialized");
  }

  if (size() != v.size())
  {
    dolfin_error("EpetraVector.cpp",
                 "perform point-wise multiplication with Epetra vector",
                 "Vectors are not of the same size");
  }

  const int err = _x->Multiply(1.0, *_x, *v._x, 0.0);
  if (err!= 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "perform point-wise multiplication with Epetra vector",
                 "Did not manage to perform Epetra_Vector::Multiply");
  }

  return *this;
}
//-----------------------------------------------------------------------------
const EpetraVector& EpetraVector::operator/=(double a)
{
  *this *= 1.0/a;
  return *this;
}
//-----------------------------------------------------------------------------
double EpetraVector::norm(std::string norm_type) const
{
  dolfin_assert(_x);

  double value = 0.0;
  int err = 0;
  if (norm_type == "l1")
    err = _x->Norm1(&value);
  else if (norm_type == "l2")
    err = _x->Norm2(&value);
  else
    err = _x->NormInf(&value);

  if (err != 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "compute norm of Epetra vector",
                 "Did not manage to perform Epetra_vector::Norm");
  }

  return value;
}
//-----------------------------------------------------------------------------
double EpetraVector::min() const
{
  dolfin_assert(_x);
  double value = 0.0;
  const int err = _x->MinValue(&value);
  if (err!= 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "compute minimum value of Epetra vector",
                 "Did not manage to perform Epetra_Vector::MinValue");
  }

  return value;
}
//-----------------------------------------------------------------------------
double EpetraVector::max() const
{
  dolfin_assert(_x);
  double value = 0.0;
  const int err = _x->MaxValue(&value);
  if (err != 0)
  {
    dolfin_error("EpetraVector.cpp",
                 "compute maximum value of Epetra vector",
                 "Did not manage to perform Epetra_Vector::MinValue");
  }

  return value;
}
//-----------------------------------------------------------------------------
double EpetraVector::sum() const
{
  dolfin_assert(_x);
  const std::size_t local_size = _x->MyLength();

  // Get local values
  std::vector<double> x_local;
  get_local(x_local);

  // Compute local sum
  double local_sum = 0.0;
  for (std::size_t i = 0; i < local_size; ++i)
    local_sum += x_local[i];

  // Compute global sum
  double global_sum = 0.0;
  _x->Comm().SumAll(&local_sum, &global_sum, 1);

  return global_sum;
}
//-----------------------------------------------------------------------------
double EpetraVector::sum(const Array<std::size_t>& rows) const
{
  dolfin_assert(_x);
  const std::size_t n0 = local_range().first;
  const std::size_t n1 = local_range().second;

  // Build sets of local and nonlocal entries
  Set<std::size_t> local_rows;
  Set<std::size_t> nonlocal_rows;
  for (std::size_t i = 0; i < rows.size(); ++i)
  {
    if (rows[i] >= n0 && rows[i] < n1)
      local_rows.insert(rows[i]);
    else
      nonlocal_rows.insert(rows[i]);
  }

  // Send nonlocal row indices to other processes
  const std::size_t num_processes = _x->Comm().NumProc();
  const std::size_t process_number = _x->Comm().MyPID();
  for (std::size_t i = 1; i < num_processes; ++i)
  {
    // Receive data from process p - i (i steps to the left), send data to
    // process p + i (i steps to the right)
    const std::size_t source
      = (process_number - i + num_processes) % num_processes;
    const std::size_t dest   = (process_number + i) % num_processes;

    // Send and receive data
    std::vector<std::size_t> received_nonlocal_rows;
    MPI::send_recv(mpi_comm(), nonlocal_rows.set(), dest,
                   received_nonlocal_rows, source);

    // Add rows which reside on this process
    for (std::size_t j = 0; j < received_nonlocal_rows.size(); ++j)
    {
      if (received_nonlocal_rows[j] >= n0 && received_nonlocal_rows[j] < n1)
        local_rows.insert(received_nonlocal_rows[j]);
    }
  }

  // Compute local sum
  double local_sum = 0.0;
  for (std::size_t i = 0; i < local_rows.size(); ++i)
    local_sum += (*_x)[0][local_rows[i] - n0];

  // Compute global sum
  double global_sum = 0.0;
  _x->Comm().SumAll(&local_sum, &global_sum, 1);

  return global_sum;
}
//-----------------------------------------------------------------------------
#endif
