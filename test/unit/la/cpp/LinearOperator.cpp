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
// First added:  2012-08-21
// Last changed: 2012-08-30
//
// Unit tests for matrix-free linear solvers (LinearOperator)

#include <dolfin.h>
#include <dolfin/common/unittest.h>
#include "forms/ReactionDiffusion.h"
#include "forms/ReactionDiffusionAction.h"

using namespace dolfin;

// Backends supporting the LinearOperator interface
std::vector<std::string> backends;

class TestLinearOperator : public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestLinearOperator);
  CPPUNIT_TEST(test_linear_operator);
  CPPUNIT_TEST_SUITE_END();

public:

  void test_linear_operator()
  {
    // Define linear operator
    class MyLinearOperator : public LinearOperator
    {
    public:

      MyLinearOperator(Form& a_action, Function& u)
        : LinearOperator(u.function_space()->dim(),
                         u.function_space()->dim()),
          a_action(a_action), u(u)
      {
        // Do nothing
      }

      void mult(const GenericVector& x, GenericVector& y) const
      {
        // Update coefficient vector
        *u.vector() = x;

        // Assemble action
        assemble(y, a_action, false);
      }

    private:

      Form& a_action;
      Function& u;

    };

    // Iterate over backends supporting linear operators
    for (dolfin::uint i = 0; i < backends.size(); i++)
    {
      // Set linear algebra backend
      parameters["linear_algebra_backend"] = backends[i];

      // Compute reference value by solving ordinary linear system
      UnitSquare mesh(8, 8);
      ReactionDiffusion::FunctionSpace V(mesh);
      ReactionDiffusion::BilinearForm a(V, V);
      Matrix A;
      Vector x;
      Vector b(V.dim());
      b = 1.0;
      assemble(A, a);
      solve(A, x, b, "gmres", "none");
      const double norm_ref = norm(x, "l2");

      // Solve using linear operator defined by form action
      ReactionDiffusionAction::LinearForm a_action(V);
      Function u(V);
      a_action.u = u;
      MyLinearOperator O(a_action, u);
      solve(O, x, b, "gmres", "none");
      const double norm_action = norm(x, "l2");

      // Check results
      CPPUNIT_ASSERT_DOUBLES_EQUAL(norm_ref, norm_action, 1e-10);
    }
  }

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestLinearOperator);

int main()
{
  // Add backends supporting the LinearOperator interface
  backends.push_back("PETSc");
  backends.push_back("uBLAS");

  DOLFIN_TEST;
}
