"""Unit tests for the RKSolver interface"""

# Copyright (C) 2013 Johan Hake
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
# First added:  2013-02-20
# Last changed: 2013-02-20

from __future__ import print_function
import unittest
from dolfin import *

parameters.form_compiler.optimize=True

import numpy as np

def convergence_order(errors, base = 2):
    import math
    orders = [0.0] * (len(errors)-1)
    for i in range(len(errors)-1):
        try:
            orders[i] = math.log(errors[i]/errors[i+1], base)
        except ZeroDivisionError:
            orders[i] = np.nan

    return orders

def function_closure(Scheme):

    def scalar_time(self):

        mesh = UnitSquareMesh(10, 10)
        V = FunctionSpace(mesh, "CG", 1)
        v = TestFunction(V)
        time = Constant(0.0)
        u0=10.0
        tstop = 1.0
        weight = Constant(2)
        u_true = Expression("u0 + 2*t + pow(t, 2)/2. + weight*pow(t, 3)/3. - "\
                            "pow(t, 5)/5.", t=tstop, u0=u0, weight=weight)

        u = Function(V)
        compound_time_expr = Expression("weight*time*time", weight=weight, \
                                        element=time.element(), time=time)
        form = (2+time+compound_time_expr-time**4)*v*dP
        
        scheme = Scheme(form, u, time)
        info(scheme)
        solver = PointIntegralSolver(scheme)
        u_errors = []
        for dt in [0.05, 0.025, 0.0125]:
            solver.reset_newton_solver()
            solver.reset_stage_solutions()
            u.interpolate(Constant(u0))
            solver.step_interval(0., tstop, dt)
            u_errors.append(errornorm(u_true, u))

        self.assertTrue(scheme.order()-min(convergence_order(u_errors))<0.1)

    def scalar(self):

        mesh = UnitSquareMesh(10, 10)
        V = FunctionSpace(mesh, "CG", 1)
        v = TestFunction(V)

        tstop = 1.0
        u_true = Expression("exp(t)", t=tstop)

        u = Function(V)
        form = u*v*dP

        scheme = Scheme(form, u)
        info(scheme)
        u_errors = []
        solver = PointIntegralSolver(scheme)
        for dt in [0.05, 0.025, 0.0125]:
            solver.reset_newton_solver()
            solver.reset_stage_solutions()
            u.interpolate(Constant(1.0))
            solver.step_interval(0., tstop, dt)
            u_errors.append(errornorm(u_true, u))
        self.assertTrue(scheme.order()-min(convergence_order(u_errors))<0.1)

    def vector(self):

        mesh = UnitSquareMesh(10, 10)
        V = VectorFunctionSpace(mesh, "CG", 1, dim=2)
        v = TestFunction(V)
        tstop = 1.0
        u_true = Expression(("cos(t)", "sin(t)"), t=tstop)

        u = Function(V)
        form = inner(as_vector((-u[1], u[0])), v)*dP

        scheme = Scheme(form, u)
        info(scheme)
        solver = PointIntegralSolver(scheme)
        u_errors = []
        for dt in [0.05, 0.025, 0.0125]:
            solver.reset_newton_solver()
            solver.reset_stage_solutions()
            u.interpolate(Constant((1.0, 0.0)))
            scheme.t().assign(0.0)
            next_dt = dt
            while float(scheme.t()) + next_dt <= tstop:
                if next_dt < 1000*DOLFIN_EPS:
                    break

                solver.step(next_dt)
                next_dt = min(tstop-float(scheme.t()), dt)

            u_errors.append(errornorm(u_true, u))

        self.assertTrue(scheme.order()-min(convergence_order(u_errors))<0.1)

    return scalar_time, scalar, vector

class PointIntegralSolverTest(unittest.TestCase):
    pass

# Build test methods using function closure so 1 test is generated per Scheme and
# test case
for Scheme in [ForwardEuler, ExplicitMidPoint, RK4,
               BackwardEuler, CN2, ESDIRK3, ESDIRK4]:

    scalar_time, scalar, vector = function_closure(Scheme)
    
    setattr(PointIntegralSolverTest,
            "test_butcher_schemes_scalar_time_{0}".format(Scheme), scalar_time)

    setattr(PointIntegralSolverTest,
            "test_butcher_schemes_scalar_{0}".format(Scheme), scalar)

    setattr(PointIntegralSolverTest,
            "test_butcher_schemes_vector_{0}".format(Scheme), vector)

if __name__ == "__main__":
    print("")
    print("Testing PyDOLFIN PointIntegralSolver operations")
    print("------------------------------------------------")
    unittest.main()
    cpp.set_log_level(INFO)
    list_timings()
