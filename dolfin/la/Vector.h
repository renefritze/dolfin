// Copyright (C) 2007 Garth N. Wells
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
// Modified by Anders Logg, 2007-2010.
// Modified by Kent-Andre Mardal, 2008.
// Modified by Ola Skavhaug, 2008.
// Modified by Martin Sandve Alnes, 2008.
//
// First added:  2007-07-03
// Last changed: 2011-01-14

#ifndef __DOLFIN_VECTOR_H
#define __DOLFIN_VECTOR_H

#include <string>
#include <utility>
#include <boost/shared_ptr.hpp>
#include <dolfin/common/types.h>
#include "DefaultFactory.h"
#include "GenericVector.h"

namespace dolfin
{

  template<typename T> class Array;

  /// This class provides the default DOLFIN vector class,
  /// based on the default DOLFIN linear algebra backend.

  class Vector : public GenericVector
  {

    using GenericVector::add;
    using GenericVector::local_range;
    using GenericVector::resize;
    using GenericVector::set;
    using GenericVector::size;

  public:

    /// Create empty vector
    Vector()
    {
      DefaultFactory factory;
      vector = factory.create_vector();
    }

    /// Create vector of size N
    explicit Vector(std::size_t N)
    {
      DefaultFactory factory;
      vector = factory.create_vector();
      vector->resize(N);
    }

    /// Copy constructor
    Vector(const Vector& x) : vector(x.vector->copy()) {}

    /// Create a Vector from a GenericVetor
    Vector(const GenericVector& x) : vector(x.copy()) {}

    //--- Implementation of the GenericTensor interface ---

    /// Return copy of vector
    virtual boost::shared_ptr<GenericVector> copy() const
    {
      boost::shared_ptr<Vector> x(new Vector(*this));
      return x;
    }

    /// Set all entries to zero and keep any sparse structure
    virtual void zero()
    { vector->zero(); }

    /// Finalize assembly of tensor
    virtual void apply(std::string mode)
    { vector->apply(mode); }

    /// Return informal string representation (pretty-print)
    virtual std::string str(bool verbose) const
    { return "<Vector wrapper of " + vector->str(verbose) + ">"; }

    //--- Implementation of the GenericVector interface ---

    /// Resize vector to size N
    virtual void resize(std::size_t N)
    { vector->resize(N); }

    /// Resize vector with given ownership range
    virtual void resize(std::pair<std::size_t, std::size_t> range)
    { vector->resize(range); }

    /// Resize vector with given ownership range and with ghost values
    virtual void resize(std::pair<std::size_t, std::size_t> range, const std::vector<std::size_t>& ghost_indices)
    { vector->resize(range, ghost_indices); }

    /// Return true if vector is empty
    virtual bool empty() const
    { return vector->empty(); }

    /// Return size of vector
    virtual std::size_t size() const
    { return vector->size(); }

    /// Return local size of vector
    virtual std::size_t local_size() const
    { return vector->local_size(); }

    /// Return local ownership range of a vector
    virtual std::pair<std::size_t, std::size_t> local_range() const
    { return vector->local_range(); }

    /// Determine whether global vector index is owned by this process
    virtual bool owns_index(std::size_t i) const
    { return vector->owns_index(i); }

    /// Get block of values (values must all live on the local process)
    virtual void get_local(double* block, std::size_t m, const dolfin::la_index* rows) const
    { vector->get_local(block,m,rows); }

    /// Set block of values
    virtual void set(const double* block, std::size_t m, const dolfin::la_index* rows)
    { vector->set(block, m, rows); }

    /// Add block of values
    virtual void add(const double* block, std::size_t m, const dolfin::la_index* rows)
    { vector->add(block, m, rows); }

    /// Get all values on local process
    virtual void get_local(std::vector<double>& values) const
    { vector->get_local(values); }

    /// Set all values on local process
    virtual void set_local(const std::vector<double>& values)
    { vector->set_local(values); }

    /// Add values to each entry on local process
    virtual void add_local(const Array<double>& values)
    { vector->add_local(values); }

    /// Gather entries into local vector x
    virtual void gather(GenericVector& x, const std::vector<dolfin::la_index>& indices) const
    { vector->gather(x, indices); }

    /// Gather entries into x
    virtual void gather(std::vector<double>& x, const std::vector<dolfin::la_index>& indices) const
    { vector->gather(x, indices); }

    /// Gather all entries into x on process 0
    virtual void gather_on_zero(std::vector<double>& x) const
    { vector->gather_on_zero(x); }

    /// Add multiple of given vector (AXPY operation)
    virtual void axpy(double a, const GenericVector& x)
    { vector->axpy(a, x); }

    /// Replace all entries in the vector by their absolute values
    virtual void abs()
    { vector->abs(); }

    /// Return inner product with given vector
    virtual double inner(const GenericVector& x) const
    { return vector->inner(x); }

    /// Return norm of vector
    virtual double norm(std::string norm_type) const
    { return vector->norm(norm_type); }

    /// Return minimum value of vector
    virtual double min() const
    { return vector->min(); }

    /// Return maximum value of vector
    virtual double max() const
    { return vector->max(); }

    /// Return sum of values of vector
    virtual double sum() const
    { return vector->sum(); }

    virtual double sum(const Array<std::size_t>& rows) const
    { return vector->sum(rows); }

    /// Multiply vector by given number
    virtual const Vector& operator*= (double a)
    { *vector *= a; return *this; }

    /// Multiply vector by another vector pointwise
    virtual const Vector& operator*= (const GenericVector& x)
    { *vector *= x; return *this; }

    /// Divide vector by given number
    virtual const Vector& operator/= (double a)
    { *this *= 1.0 / a; return *this; }

    /// Add given vector
    virtual const Vector& operator+= (const GenericVector& x)
    { axpy(1.0, x); return *this; }

    /// Add number to all components of a vector
    virtual const GenericVector& operator+= (double a)
    { *vector += a; return *this; }

    /// Subtract given vector
    virtual const Vector& operator-= (const GenericVector& x)
    { axpy(-1.0, x); return *this; }

    /// Subtract number from all components of a vector
    virtual const GenericVector& operator-= (double a)
    { *vector -= a; return *this; }

    /// Assignment operator
    virtual const GenericVector& operator= (const GenericVector& x)
    { *vector = x; return *this; }

    /// Assignment operator
    const Vector& operator= (double a)
    { *vector = a; return *this; }

    /// Return pointer to underlying data (const version)
    virtual const double* data() const
    { return vector->data(); }

    /// Return pointer to underlying data
    virtual double* data()
    { return vector->data(); }

    /// Update ghost values
    virtual void update_ghost_values()
    {
      vector->update_ghost_values();
    }
    //--- Special functions ---

    /// Return linear algebra backend factory
    virtual GenericLinearAlgebraFactory& factory() const
    { return vector->factory(); }

    //--- Special functions, intended for library use only ---

    /// Return concrete instance / unwrap (const version)
    virtual const GenericVector* instance() const
    { return vector.get(); }

    /// Return concrete instance / unwrap (non-const version)
    virtual GenericVector* instance()
    { return vector.get(); }

    virtual boost::shared_ptr<const LinearAlgebraObject> shared_instance() const
    { return vector; }

    virtual boost::shared_ptr<LinearAlgebraObject> shared_instance()
    { return vector; }

    //--- Special Vector functions ---

    /// Assignment operator
    const Vector& operator= (const Vector& x)
    { *vector = *x.vector; return *this; }

  private:

    // Pointer to concrete implementation
    boost::shared_ptr<GenericVector> vector;

  };

}

#endif
