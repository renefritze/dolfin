// Copyright (C) 2007-2011 Garth N. Wells
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
// Modified by Anders Logg, 2007-2009.
//
// First added:  2007-03-13
// Last changed: 2011-01-02

#ifndef __SPARSITY_PATTERN_H
#define __SPARSITY_PATTERN_H

#include <string>
#include <utility>
#include <vector>

#include "dolfin/common/Set.h"
#include "dolfin/common/types.h"
#include "GenericSparsityPattern.h"
#include <boost/unordered_map.hpp>

namespace dolfin
{

  /// This class implements the GenericSparsityPattern interface.
  /// It is used by most linear algebra backends.

  class SparsityPattern : public GenericSparsityPattern
  {

    // NOTE: Do not change this typedef without performing careful
    //       performance profiling
    // Set type used for the rows of the sparsity pattern
    typedef dolfin::Set<std::size_t> set_type;

  public:

    /// Create empty sparsity pattern
    SparsityPattern(std::size_t primary_dim);

    /// Create sparsity pattern for a generic tensor
    SparsityPattern(const MPI_Comm mpi_comm,
                    const std::vector<std::size_t>& dims,
                    const std::vector<std::pair<std::size_t,
                    std::size_t> >& ownership_range,
                    const std::vector<const boost::unordered_map<std::size_t,
                    unsigned int>* > off_process_owner,
                    std::size_t primary_dim);

    /// Initialize sparsity pattern for a generic tensor
    void init(const MPI_Comm mpi_comm,
              const std::vector<std::size_t>& dims,
              const std::vector<std::pair<std::size_t,
              std::size_t> >& ownership_range,
              const std::vector<const boost::unordered_map<std::size_t,
              unsigned int>* > off_process_owner);

    /// Insert non-zero entries
    void
      insert(const std::vector<const std::vector<dolfin::la_index>* >& entries);

    /// Add edges (vertex = [index, owning process])
    void add_edges(const std::pair<dolfin::la_index, std::size_t>& vertex,
                   const std::vector<dolfin::la_index>& edges);

    /// Return rank
    std::size_t rank() const;

    /// Return local range for dimension dim
    std::pair<std::size_t, std::size_t> local_range(std::size_t dim) const;

    /// Return number of local nonzeros
    std::size_t num_nonzeros() const;

    /// Fill array with number of nonzeros for diagonal block in
    /// local_range for dimension 0. For matrices, fill array with number
    /// of nonzeros per local row for diagonal block
    void num_nonzeros_diagonal(std::vector<std::size_t>& num_nonzeros) const;

    /// Fill array with number of nonzeros for off-diagonal block in
    /// local_range for dimension 0. For matrices, fill array with number
    /// of nonzeros per local row for off-diagonal block
    void
      num_nonzeros_off_diagonal(std::vector<std::size_t>& num_nonzeros) const;

    /// Fill vector with number of nonzeros in local_range for dimension 0
    void num_local_nonzeros(std::vector<std::size_t>& num_nonzeros) const;

    /// Fill vector with edges for given vertex
    void get_edges(std::size_t vertex,
                   std::vector<dolfin::la_index>& edges) const;

    /// Finalize sparsity pattern
    void apply();

    // Return MPI communicator
    const MPI_Comm mpi_comm() const
    { return _mpi_comm; }

    /// Return informal string representation (pretty-print)
    std::string str(bool verbose) const;

    /// Return underlying sparsity pattern (diagonal). Options are
    /// 'sorted' and 'unsorted'.
    std::vector<std::vector<std::size_t> > diagonal_pattern(Type type) const;

    /// Return underlying sparsity pattern (off-diagional). Options are
    /// 'sorted' and 'unsorted'.
    std::vector<std::vector<std::size_t> >
      off_diagonal_pattern(Type type) const;

  private:

    // Print some useful information
    void info_statistics() const;

    // MPI communicator
    MPI_Comm _mpi_comm;

    // Ownership range for each dimension
    std::vector<std::pair<std::size_t, std::size_t> > _local_range;

    // Sparsity patterns for diagonal and off-diagonal blocks
    std::vector<set_type> diagonal;
    std::vector<set_type> off_diagonal;

    // Sparsity pattern for non-local entries stored as [i0, j0, i1, j1, ...]
    std::vector<std::size_t> non_local;

    // Map from non-local vertex to owning process index
    std::vector<boost::unordered_map<std::size_t, unsigned int> >
      _off_process_owner;

  };

}
#endif
