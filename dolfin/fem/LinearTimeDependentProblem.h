// Copyright (C) 2012 Anders Logg
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
// First added:  2012-08-17
// Last changed: 2012-08-17

#ifndef __LINEAR_TIME_DEPENDENT_PROBLEM_H
#define __LINEAR_TIME_DEPENDENT_PROBLEM_H

#include <boost/shared_ptr.hpp>
#include <dolfin/common/Hierarchical.h>

namespace dolfin
{

  /// This class represents a linear time-dependent variational problem:
  ///
  /// Find u in U = U_h (x) U_k such that
  ///
  ///     a(u, v) = L(v)  for all v in V = V_h (x) V_k,
  ///
  /// where U is a tensor-product trial space and V is a tensor-product
  /// test space.

  class LinearTimeDependentProblem : public Hierarchical<LinearTimeDependentProblem>
  {
  public:

    /// Create linear variational problem without boundary conditions
    LinearTimeDependentProblem(const TensorProductForm& a,
                             const TensorProductForm& L,
                             Function& u);

    /// Create linear variational problem with a single boundary condition
    LinearTimeDependentProblem(const TensorProductForm& a,
                             const TensorProductForm& L,
                             Function& u,
                             const BoundaryCondition& bc);

    /// Create linear variational problem with a list of boundary conditions
    LinearTimeDependentProblem(const TensorProductForm& a,
                             const TensorProductForm& L,
                             Function& u,
                             std::vector<const BoundaryCondition*> bcs);

    /// Create linear variational problem with a list of boundary conditions
    /// (shared pointer version)
    LinearTimeDependentProblem(boost::shared_ptr<const TensorProductForm> a,
                               boost::shared_ptr<const TensorProductForm> L,
                               boost::shared_ptr<Function> u,
                               std::vector<boost::shared_ptr<const BoundaryCondition> > bcs);

    /// Return bilinear form
    boost::shared_ptr<const TensorProductForm> bilinear_form() const;

    /// Return linear form
    boost::shared_ptr<const TensorProductForm> linear_form() const;

    /// Return solution variable
    boost::shared_ptr<Function> solution();

    /// Return solution variable (const version)
    boost::shared_ptr<const Function> solution() const;

    /// Return boundary conditions
    std::vector<boost::shared_ptr<const BoundaryCondition> > bcs() const;

    /// Return trial space
    boost::shared_ptr<const FunctionSpace> trial_space() const;

    /// Return test space
    boost::shared_ptr<const FunctionSpace> test_space() const;

  private:

    // Check forms
    void check_forms() const;

    // The bilinear form
    boost::shared_ptr<const TensorProductForm> _a;

    // The linear form
    boost::shared_ptr<const TensorProductForm> _L;

    // The solution
    boost::shared_ptr<Function> _u;

    // The boundary conditions
    std::vector<boost::shared_ptr<const BoundaryCondition> > _bcs;

  };

}

#endif
