#!/usr/bin/env py.test

"""Unit tests for the MultiMeshFunction class"""

# Copyright (C) 2016 Jorgen S. Dokken
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
#
# First added:  2016-06-11
# Last changed: 2016-08-12

import pytest
from dolfin import *
import ufl

from dolfin_utils.test import fixture, skip_in_parallel

@fixture
def multimesh():
    mesh_0 = RectangleMesh(Point(0,0), Point(0.6, 1), 40, 20)
    mesh_1 = RectangleMesh(Point(0.5,0),  Point(1,1),25,40)
    multimesh = MultiMesh()
    multimesh.add(mesh_0)
    multimesh.add(mesh_1)
    multimesh.build()
    return multimesh

@fixture
def V(multimesh):
    element = FiniteElement("Lagrange", triangle, 1)
    return MultiMeshFunctionSpace(multimesh, element)

@fixture
def v(V):
    return MultiMeshFunction(V)

@skip_in_parallel
def test_measure_mul(v, multimesh):
    assert isinstance(v*dX, ufl.form.Form)

@skip_in_parallel
def test_assemble_zero(v, multimesh):
    assert (assemble_multimesh(v*dX)<= 1e-12)

@skip_in_parallel
def test_assemble_area(v, multimesh):
    v.vector()[:] = 1
    assert (abs(assemble_multimesh(v*dX)-1)<= 1e-12)
