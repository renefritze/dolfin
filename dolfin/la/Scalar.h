// Copyright (C) 2007-2011 Anders Logg
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
// Modified by Garth N. Wells, 2007-2011.
// Modified by Ola Skavhaug, 2007.
//
// First added:  2007-03-15
// Last changed: 2011-11-11

#ifndef __SCALAR_H
#define __SCALAR_H

#include <string>
#include <vector>
#include <dolfin/common/MPI.h>
#include <dolfin/common/SubSystemsManager.h>
#include <dolfin/common/types.h>
#include "DefaultFactory.h"
#include "GenericTensor.h"
#include "TensorLayout.h"

namespace dolfin
{

  class TensorLayout;

  /// This class represents a real-valued scalar quantity and
  /// implements the GenericTensor interface for scalars.

  class Scalar : public GenericTensor
  {
  public:

    /// Create zero scalar
    Scalar() : GenericTensor(), _value(0.0), _mpi_comm(MPI_COMM_WORLD)
    { SubSystemsManager::init_mpi(); }

    /// Destructor
    virtual ~Scalar() {}

    //--- Implementation of the GenericTensor interface ---

    /// Initialize zero tensor using sparsity pattern
    void init(const TensorLayout& tensor_layout)
    {
      _value = 0.0;
      _mpi_comm = tensor_layout.mpi_comm();
    }

    /// Return true if empty
    bool empty() const
    { return false; }

    /// Return tensor rank (number of dimensions)
    std::size_t rank() const
    { return 0; }

    /// Return size of given dimension
    std::size_t size(std::size_t dim) const
    {
      if (dim != 0)
      {
        dolfin_error("Scalar.h",
                     "get size of scalar",
                     "Dim must be equal to zero.");
      }

      return 0;
    }

    /// Return local ownership range
    virtual std::pair<std::size_t, std::size_t>
      local_range(std::size_t dim) const
    {
      dolfin_error("Scalar.h",
                   "get local range of scalar",
                   "The local_range() function is not available for scalars");
      return std::make_pair(0, 0);
    }

    /// Get block of values
    void get(double* block, const dolfin::la_index* num_rows,
             const dolfin::la_index * const * rows) const
    { block[0] = _value; }

    /// Set block of values using global indices
    void set(const double* block, const dolfin::la_index* num_rows,
             const dolfin::la_index * const * rows)
    { _value = block[0]; }

    /// Set block of values using local indices
    void set_local(const double* block, const dolfin::la_index* num_rows,
                   const dolfin::la_index * const * rows)
    { _value = block[0]; }

    /// Add block of values using global indices
    void add(const double* block, const dolfin::la_index* num_rows,
             const dolfin::la_index * const * rows)
    { add_local(block, num_rows, rows); }

    /// Add block of values using local indices
    void add_local(const double* block, const dolfin::la_index* num_rows,
                   const dolfin::la_index * const * rows)
    {
      dolfin_assert(block);
      _value += block[0];
    }

    /// Add block of values using global indices
    void add(const double* block,
             const std::vector<const std::vector<dolfin::la_index>* >& rows)
    {
      dolfin_assert(block);
      _value += block[0];
    }

    /// Add block of values using local indices
    void add_local(const double* block,
             const std::vector<const std::vector<dolfin::la_index>* >& rows)
    {
      dolfin_assert(block);
      _value += block[0];
    }

    /// Add block of values using global indices
    void add(const double* block,
             const std::vector<std::vector<dolfin::la_index> >& rows)
    {
      dolfin_assert(block);
      _value += block[0];
    }

    /// Add block of values using local indices
    void add_local(const double* block,
             const std::vector<std::vector<dolfin::la_index> >& rows)
    {
      dolfin_assert(block);
      _value += block[0];
    }

    /// Set all entries to zero and keep any sparse structure
    void zero()
    { _value = 0.0; }

    /// Finalize assembly of tensor
    void apply(std::string mode)
    { _value = MPI::sum(_mpi_comm, _value); }

    /// Return MPI communicator
    MPI_Comm mpi_comm() const
    { return _mpi_comm; }

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const
    {
      std::stringstream s;
      s << "<Scalar value " << _value << ">";
      return s.str();
    }

    //--- Scalar interface ---

    /// Return copy of scalar
    virtual std::shared_ptr<Scalar> copy() const
    {
      std::shared_ptr<Scalar> s(new Scalar);
      s->_value = _value;
      return s;
    }

    /// Cast to double
    operator double() const
    { return _value; }

    /// Assignment from double
    const Scalar& operator= (double value)
    { _value = value; return *this; }

    //--- Special functions

    /// Return a factory for the default linear algebra backend
    GenericLinearAlgebraFactory& factory() const
    {
      DefaultFactory f;
      return f.factory();
    }
    //{ return dolfin::uBLASFactory<>::instance(); }

    /// Get value
    double getval() const
    { return _value; }

  private:

    // Value of scalar
    double _value;

    // MPI communicator
     MPI_Comm _mpi_comm;

  };

}

#endif
