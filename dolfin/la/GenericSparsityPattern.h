// Copyright (C) 2007 Ola Skavhaug
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
// Modified by Magnus Vikstrom, 2008.
// Modified by Anders Logg, 2009.
// Modified by Garth N. Wells, 2010.
//
// First added:  2007-11-30
// Last changed: 2010-12-30

#ifndef __GENERIC_SPARSITY_PATTERN_H
#define __GENERIC_SPARSITY_PATTERN_H

#include <utility>
#include <unordered_map>
#include <vector>

#include <dolfin/common/types.h>
#include <dolfin/common/MPI.h>
#include <dolfin/common/Variable.h>

namespace dolfin
{

  /// Base class (interface) for generic tensor sparsity patterns.
  /// Currently, this interface is mostly limited to matrices.

  class GenericSparsityPattern : public Variable
  {
  public:

    enum Type {sorted, unsorted};

    /// Create empty sparsity pattern
    GenericSparsityPattern(std::size_t primary_dim)
      : _primary_dim(primary_dim) {}

    /// Destructor
    virtual ~GenericSparsityPattern() {}

    /// Initialize sparsity pattern for a generic tensor
    virtual void
      init(const MPI_Comm mpi_comm,
           const std::vector<std::size_t>& dims,
           const std::vector<std::pair<std::size_t, std::size_t> >& local_range,
           const std::vector<const std::vector<std::size_t>* > local_to_global,
           const std::vector<const std::vector<int>* > off_process_owner,
           const std::size_t block_size) = 0;

    /// Insert non-zero entries using global indices
    virtual void insert_global(const std::vector<
                        const std::vector<dolfin::la_index>* >& entries) = 0;

    /// Insert non-zero entries using local (process-wise) entries
    virtual void insert_local(const std::vector<
                        const std::vector<dolfin::la_index>* >& entries) = 0;

    /// Return rank
    virtual std::size_t rank() const = 0;

    /// Return primary dimension (e.g., 0=row partition, 1=column
    /// partition)
    std::size_t primary_dim() const
    { return _primary_dim; }

    /// Return local range for dimension dim
    virtual std::pair<std::size_t, std::size_t>
      local_range(std::size_t dim) const = 0;

    /// Return total number of nonzeros in local_range
    virtual std::size_t num_nonzeros() const = 0;

    /// Fill vector with number of nonzeros for diagonal block in
    /// local_range for primary dimension
    virtual void
      num_nonzeros_diagonal(std::vector<std::size_t>& num_nonzeros) const = 0;

    /// Fill vector with number of nonzeros for off-diagonal block in
    /// local_range for primary dimension
    virtual void num_nonzeros_off_diagonal(
      std::vector<std::size_t>& num_nonzeros) const = 0;

    /// Fill vector with number of nonzeros in local_range for primary
    /// dimension
    virtual void
      num_local_nonzeros(std::vector<std::size_t>& num_nonzeros) const = 0;

    /// Return underlying sparsity pattern (diagonal). Options are
    /// 'sorted' and 'unsorted'.
    virtual std::vector<std::vector<std::size_t> >
      diagonal_pattern(Type type) const = 0;

    /// Return underlying sparsity pattern (off-diagonal). Options
    /// are 'sorted' and 'unsorted'.
    virtual std::vector<std::vector<std::size_t> >
      off_diagonal_pattern(Type type) const = 0;

    /// Finalize sparsity pattern
    virtual void apply() = 0;

    /// Return MPI communicator
    virtual MPI_Comm mpi_comm() const = 0;

   private:

    // Primary sparsity pattern storage dimension (e.g., 0=row
    // partition, 1=column partition)
    const std::size_t _primary_dim;

  };

}

#endif
