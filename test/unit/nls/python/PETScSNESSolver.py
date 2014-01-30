"""Unit test for the SNES nonlinear solver"""
# Copyright (C) 2012 Patrick E. Farrell
#
# This file is part of DOLFIN.
#
# DOLFIN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DOLFIN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
#
# First added:  2012-10-17
# Last changed: 2013-11-22

"""Solve the Yamabe PDE which arises in the differential geometry of
general relativity. http://arxiv.org/abs/1107.0360.

The Yamabe equation is highly nonlinear and supports many
solutions. However, only one of these is of physical relevance -- the
positive solution.

This unit test demonstrates the capability of the SNES solver to
accept bounds on the resulting solution. The plain Newton method
converges to an unphysical negative solution, while the SNES solution
with {sign: nonnegative} converges to the physical positive solution.

An alternative interface to SNESVI allows the user to set explicitly
more complex bounds as GenericVectors or Function.
"""

from dolfin import *
import unittest

if has_petsc():
    try:
        parameters["linear_algebra_backend"] = "PETSc"
    except RuntimeError:
        import sys; sys.exit(0)

    parameters["form_compiler"]["quadrature_degree"] = 5

    mesh = Mesh("doughnut.xml.gz")
    V = FunctionSpace(mesh, "CG", 1)
    bcs = [DirichletBC(V, 1.0, "on_boundary")]

    u = Function(V)
    v = TestFunction(V)
    u.interpolate(Constant(-1000.0))

    r = sqrt(triangle.x[0]**2 + triangle.x[1]**2)
    rho = 1.0/r**3

    F = (8*inner(grad(u), grad(v))*dx + rho * inner(u**5, v)*dx \
         + (-1.0/8.0)*inner(u, v)*dx)

    du = TrialFunction(V)
    J = derivative(F,u,du)
    lb = Function(interpolate(Constant(0.),V))
    ub = Function(interpolate(Constant(100.),V))

    newton_solver_parameters = {"nonlinear_solver": "newton",
                                "newton_solver": {"linear_solver": "lu",
                                                  "maximum_iterations": 100,
                                                  "report": False}}

    snes_solver_parameters_sign = {"nonlinear_solver": "snes",
                                   "snes_solver": {"linear_solver": "lu",
                                                   "maximum_iterations": 100,
                                                   "sign": "nonnegative",
                                                   "report": True}}

    snes_solver_parameters_bounds = {"nonlinear_solver": "snes",
                                     "snes_solver": {"linear_solver": "lu",
                                                     "maximum_iterations": 100,
                                                     "sign": "default",
                                                     "report": True}}

class SNESSolverTester(unittest.TestCase):

    if has_petsc():

        def test_snes_solver(self):
            u.interpolate(Constant(-1000.0))
            solve(F == 0, u, bcs, solver_parameters=snes_solver_parameters_sign)
            self.assertTrue(u.vector().min() >= 0)

        def test_newton_solver(self):
            u.interpolate(Constant(-1000.0))
            solve(F == 0, u, bcs, solver_parameters=newton_solver_parameters)
            self.assertTrue(u.vector().min() < 0)

        def test_snes_solver_bound_functions(self):
            u.interpolate(Constant(-1000.0))
            problem = NonlinearVariationalProblem(F, u, bcs, J)
            solver  = NonlinearVariationalSolver(problem)
            solver.parameters.update(snes_solver_parameters_bounds)
            solver.solve(lb, ub)
            self.assertTrue(u.vector().min() >= 0)

        def test_snes_solver_bound_vectors(self):
            u.interpolate(Constant(-1000.0))
            problem = NonlinearVariationalProblem(F, u, bcs, J)
            solver  = NonlinearVariationalSolver(problem)
            solver.parameters.update(snes_solver_parameters_bounds)
            solver.solve(lb.vector(), ub.vector())
            self.assertTrue(u.vector().min() >= 0)


if __name__ == "__main__":
  # Turn off DOLFIN output
  set_log_active(False)

  print ""
  print "Testing DOLFIN nls/PETScSNESSolver interface"
  print "--------------------------------------------"
  unittest.main()
