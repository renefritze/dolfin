"""Unit tests for geometric multigrid via PETSc"""

# Copyright (C) 2016 Patrick E. Farrell and Garth N. Wells
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

from dolfin import *
from petsc4py import PETSc

SubSystemsManager.init_petsc()
PETSc.Sys.popErrorHandler()

from dolfin import *
import pytest
from dolfin_utils.test import skip_if_not_PETSc, skip_if_not_petsc4py, pushpop_parameters

@skip_if_not_petsc4py
def test_mg_solver_laplace(pushpop_parameters):

    parameters["linear_algebra_backend"] = "PETSc"

    mesh0 = UnitSquareMesh(5, 5)
    mesh1 = UnitSquareMesh(16, 16)
    mesh2 = UnitSquareMesh(32, 132)

    V0 = FunctionSpace(mesh0, "Lagrange", 1)
    V1 = FunctionSpace(mesh1, "Lagrange", 1)
    V2 = FunctionSpace(mesh2, "Lagrange", 1)

    u, v = TrialFunction(V2), TestFunction(V2)
    A = assemble(Constant(1.0)*u*v*dx)
    b = assemble(Constant(1.0)*v*dx)

    spaces = [V0, V1, V2]
    dm_collection = PETScDMCollection(spaces)

    solver = PETScKrylovSolver()
    solver.set_operator(A)

    PETScOptions.set("ksp_type", "richardson")
    PETScOptions.set("pc_type", "mg")
    PETScOptions.set("pc_mg_levels", 3)

    PETScOptions.set("pc_mg_galerkin")
    PETScOptions.set("ksp_monitor_true_residual")

    PETScOptions.set("ksp_atol", 1.0e-12)
    PETScOptions.set("ksp_rtol", 1.0e-12)
    solver.set_from_options()

    from petsc4py import PETSc
    ksp = solver.ksp()

    dm = dm_collection.dm()

    print("DM ref count (0)")
    print(type(dm))
    print(dm.refcount)

    ksp.setDM(dm)
    ksp.setDMActive(False)

    x = PETScVector()
    solver.solve(x, b)

    print("Solver ref count")
    print(ksp.refcount)
    del(solver)
    print(ksp.refcount)

    #print("DM ref count (1)")
    #print(dm.refcount)
    #del(dm)
    #print(dm.refcount)

    # Check multigrid solution against LU solver
    solver = LUSolver(A)
    x_lu = Vector()
    solver.solve(x_lu, b)
