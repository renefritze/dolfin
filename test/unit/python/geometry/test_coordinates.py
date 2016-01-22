#!/usr/bin/env py.test

"""Unit tests for coordinated interface"""

# Copyright (C) 2016 Jan Blechta
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

from __future__ import print_function
import pytest
import numpy as np

from dolfin import UnitIntervalMesh, UnitSquareMesh, UnitCubeMesh, UnitDiscMesh
from dolfin import FunctionSpace, VectorFunctionSpace, Function, mpi_comm_world
from dolfin import get_coordinates, set_coordinates, Mesh
from dolfin_utils.test import skip_in_parallel, fixture


@fixture
def meshes_p1():
    return UnitIntervalMesh(10), UnitSquareMesh(3, 3), UnitCubeMesh(2, 2, 2)


@fixture
def meshes_p2():
    return UnitDiscMesh(mpi_comm_world(), 1, 2, 2), UnitDiscMesh(mpi_comm_world(), 1, 2, 3)


def _test_get_set_coordinates(mesh):
    # Get coords
    V = FunctionSpace(mesh, mesh.ufl_coordinate_element())
    c = Function(V)
    get_coordinates(c, mesh.geometry())

    # Backup and zero coords
    coords = mesh.coordinates()
    coords_old = coords.copy()
    coords[:] = 0.0
    assert np.all(mesh.coordinates() == 0.0)

    # Set again to old value
    set_coordinates(mesh.geometry(), c)

    # Check
    assert np.all(mesh.coordinates() == coords_old)


def test_linear(meshes_p1):
    for mesh in meshes_p1:
        _test_get_set_coordinates(mesh)


@skip_in_parallel(reason="FunctionSpace(UnitDiscMesh) not working in parallel")
def test_higher_order(meshes_p2):
    for mesh in meshes_p2:
        _test_get_set_coordinates(mesh)


def test_raises(meshes_p1):
    mesh1, mesh2 = meshes_p1[:2]

    # Non-matching meshes (different address of mesh2.geometry()
    # and c.function_space().mesh().geometry())
    mesh_another = Mesh(mesh2)
    V = FunctionSpace(mesh_another, mesh_another.ufl_coordinate_element())
    c = Function(V)
    with pytest.raises(RuntimeError):
        get_coordinates(c, mesh2.geometry())
    with pytest.raises(RuntimeError):
        set_coordinates(mesh2.geometry(), c)

    # Wrong FE family
    V = VectorFunctionSpace(mesh2, "Discontinuous Lagrange", 1)
    c = Function(V)
    with pytest.raises(RuntimeError):
        get_coordinates(c, mesh2.geometry())
    with pytest.raises(RuntimeError):
        set_coordinates(mesh2.geometry(), c)

    # Wrong value rank
    V = FunctionSpace(mesh2, "Lagrange", 1)
    c = Function(V)
    with pytest.raises(RuntimeError):
        get_coordinates(c, mesh2.geometry())
    with pytest.raises(RuntimeError):
        set_coordinates(mesh2.geometry(), c)

    # Wrong value shape (incompatible for getting,
    # too small (< tdim) for setting)
    V = VectorFunctionSpace(mesh2, "Lagrange", mesh2.geometry().degree(),
            dim=mesh2.geometry().dim() - 1)
    c = Function(V)
    with pytest.raises(RuntimeError):
        get_coordinates(c, mesh2.geometry())
    with pytest.raises(RuntimeError):
        set_coordinates(mesh2.geometry(), c)

    # Non-matching degree (for getting), ok for setting
    V = VectorFunctionSpace(mesh2, "Lagrange", mesh2.geometry().degree() + 1)
    c = Function(V)
    with pytest.raises(RuntimeError):
        get_coordinates(c, mesh2.geometry())
    set_coordinates(mesh2.geometry(), c)

    # Can change gdim and degree by setting
    V = VectorFunctionSpace(mesh2, "Lagrange", 1,
            dim=mesh2.geometry().dim() + 1)
    set_coordinates(mesh2.geometry(), c)
