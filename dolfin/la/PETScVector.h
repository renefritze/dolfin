// Copyright (C) 2004-2010 Johan Hoffman, Johan Jansson and Anders Logg
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
// Modified by Garth N. Wells, 2005-2010.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Ola Skavhaug, 2008.
// Modified by Martin Alnæs, 2008.
// Modified by Fredrik Valdmanis, 2011.
//
// First added:  2004-01-01
// Last changed: 2011-09-29

#ifndef __PETSC_VECTOR_H
#define __PETSC_VECTOR_H

#ifdef HAS_PETSC

#include <map>
#include <string>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <petscsys.h>
#include <petscvec.h>

#include <dolfin/log/dolfin_log.h>
#include <dolfin/common/types.h>
#include "GenericVector.h"
#include "PETScObject.h"

namespace dolfin
{

  class PETScVectorDeleter
  {
  public:
    void operator() (Vec* x)
    {
      if (*x)
        VecDestroy(x);
      delete x;
    }
  };

  class GenericSparsityPattern;
  template<typename T> class Array;

  /// This class provides a simple vector class based on PETSc.
  /// It is a simple wrapper for a PETSc vector pointer (Vec)
  /// implementing the GenericVector interface.
  ///
  /// The interface is intentionally simple. For advanced usage,
  /// access the PETSc Vec pointer using the function vec() and
  /// use the standard PETSc interface.

  class PETScVector : public GenericVector, public PETScObject
  {
  public:

    /// Create empty vector
    //explicit PETScVector(bool use_gpu=false);
    explicit PETScVector();

    /// Create vector of size N
    PETScVector(MPI_Comm comm, std::size_t N, bool use_gpu=false);

    /// Create vector
    PETScVector(const GenericSparsityPattern& sparsity_pattern);

    /// Copy constructor
    PETScVector(const PETScVector& x);

    /// Create vector from given PETSc Vec pointer
    explicit PETScVector(boost::shared_ptr<Vec> x);

    /// Destructor
    virtual ~PETScVector();

    //--- Implementation of the GenericTensor interface ---

    /// Set all entries to zero and keep any sparse structure
    virtual void zero();

    /// Finalize assembly of tensor
    virtual void apply(std::string mode);

    /// Return MPI communicator
    virtual const MPI_Comm mpi_comm() const;

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const;

    //--- Implementation of the GenericVector interface ---

    /// Return copy of vector
    virtual boost::shared_ptr<GenericVector> copy() const;

    /// Resize vector to global size N
    virtual void resize(MPI_Comm comm, std::size_t N);

    /// Resize vector with given ownership range
    virtual void resize(MPI_Comm comm, std::pair<std::size_t, std::size_t> range);

    /// Resize vector with given ownership range and with ghost values
    virtual void resize(MPI_Comm comm, std::pair<std::size_t, std::size_t> range,
                        const std::vector<la_index>& ghost_indices);

    /// Return true if vector is empty
    virtual bool empty() const;

    /// Return size of vector
    virtual std::size_t size() const;

    /// Return local size of vector
    virtual std::size_t local_size() const;

    /// Return ownership range of a vector
    virtual std::pair<std::size_t, std::size_t> local_range() const;

    /// Determine whether global vector index is owned by this process
    virtual bool owns_index(std::size_t i) const;

    /// Get block of values (values must all live on the local process)
    virtual void get_local(double* block, std::size_t m, const dolfin::la_index* rows) const;

    /// Set block of values
    virtual void set(const double* block, std::size_t m, const dolfin::la_index* rows);

    /// Add block of values
    virtual void add(const double* block, std::size_t m, const dolfin::la_index* rows);

    /// Get all values on local process
    virtual void get_local(std::vector<double>& values) const;

    /// Set all values on local process
    virtual void set_local(const std::vector<double>& values);

    /// Add values to each entry on local process
    virtual void add_local(const Array<double>& values);

    /// Gather vector entries into a local vector
    virtual void gather(GenericVector& y, const std::vector<dolfin::la_index>& indices) const;

    /// Gather entries into x
    virtual void gather(std::vector<double>& x, const std::vector<dolfin::la_index>& indices) const;

    /// Gather all entries into x on process 0
    virtual void gather_on_zero(std::vector<double>& x) const;

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(double a, const GenericVector& x);

    /// Replace all entries in the vector by their absolute values
    virtual void abs();

    /// Return inner product with given vector
    virtual double inner(const GenericVector& v) const;

    /// Return norm of vector
    virtual double norm(std::string norm_type) const;

    /// Return minimum value of vector
    virtual double min() const;

    /// Return maximum value of vector
    virtual double max() const;

    /// Return sum of values of vector
    virtual double sum() const;

    /// Return sum of selected rows in vector
    virtual double sum(const Array<std::size_t>& rows) const;

    /// Multiply vector by given number
    virtual const PETScVector& operator*= (double a);

    /// Multiply vector by another vector pointwise
    virtual const PETScVector& operator*= (const GenericVector& x);

    /// Divide vector by given number
    virtual const PETScVector& operator/= (double a);

    /// Add given vector
    virtual const PETScVector& operator+= (const GenericVector& x);

    /// Add number to all components of a vector
    virtual const PETScVector& operator+= (double a);

    /// Subtract given vector
    virtual const PETScVector& operator-= (const GenericVector& x);

    /// Subtract number from all components of a vector
    virtual const PETScVector& operator-= (double a);

    /// Assignment operator
    virtual const GenericVector& operator= (const GenericVector& x);

    /// Assignment operator
    virtual const PETScVector& operator= (double a);

    virtual void update_ghost_values();

    //--- Special functions ---

    /// Reset data and PETSc vector object
    void reset();

    /// Return linear algebra backend factory
    virtual GenericLinearAlgebraFactory& factory() const;

    //--- Special PETSc functions ---

    /// Return shared_ptr to PETSc Vec object
    boost::shared_ptr<Vec> vec() const;

    /// Assignment operator
    const PETScVector& operator= (const PETScVector& x);

    friend class PETScBaseMatrix;
    friend class PETScMatrix;

  private:

    // Initialise PETSc vector
    void _init(MPI_Comm comm, std::pair<std::size_t, std::size_t> range,
               const std::vector<la_index>& ghost_indices);

    // Return true if vector is distributed
    bool distributed() const;

    // PETSc Vec pointer
    boost::shared_ptr<Vec> _x;

    // PETSc Vec pointer (local ghosted)
    mutable boost::shared_ptr<Vec> x_ghosted;

    // Global-to-local map for ghost values
    boost::unordered_map<std::size_t, std::size_t> ghost_global_to_local;

    // PETSc norm types
    static const std::map<std::string, NormType> norm_types;

    // PETSc vector architechture
    const bool _use_gpu;

  };

}

#endif

#endif
