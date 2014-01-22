# -*- coding: utf-8 -*-
"""Unit tests for the Exodus io library"""

# Copyright (C) 2013 Nico Schlömer
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
# First added:  2013-02-27
# Last changed: 2013-03-11

import unittest
from dolfin import *

if has_exodus():
    class Exodus_Mesh_Output(unittest.TestCase):
        """Test output of Meshes to Exodus files"""

        def test_save_1d_mesh(self):
            mesh = UnitIntervalMesh(32)
            File("mesh.e") << mesh

        def test_save_2d_mesh(self):
            mesh = UnitSquareMesh(32, 32)
            File("mesh.e") << mesh

        def test_save_3d_mesh(self):
            mesh = UnitCubeMesh(8, 8, 8)
            File("mesh.e") << mesh


    class Exodus_Point_Function_Output(unittest.TestCase):
        """Test output of point-based Functions to Exodus files"""

        def test_save_1d_scalar(self):
            mesh = UnitIntervalMesh(32)
            u = Function(FunctionSpace(mesh, "Lagrange", 2))
            u.vector()[:] = 1.0
            File("u.e") << u

        def test_save_2d_scalar(self):
            mesh = UnitSquareMesh(16, 16)
            u = Function(FunctionSpace(mesh, "Lagrange", 2))
            u.vector()[:] = 1.0
            File("u.e") << u

        def test_save_3d_scalar(self):
            mesh = UnitCubeMesh(8, 8, 8)
            u = Function(FunctionSpace(mesh, "Lagrange", 2))
            u.vector()[:] = 1.0
            File("u.e") << u

        def test_save_2d_vector(self):
            mesh = UnitSquareMesh(16, 16)
            u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
            u.vector()[:] = 1.0
            File("u.e") << u

        def test_save_3d_vector(self):
            mesh = UnitCubeMesh(8, 8, 8)
            u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
            u.vector()[:] = 1.0
            File("u.e") << u

        def test_save_2d_tensor(self):
            mesh = UnitSquareMesh(16, 16)
            u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
            u.vector()[:] = 1.0
            File("u.e") << u

        #def test_save_3d_tensor(self):
        #    mesh = UnitCubeMesh(8, 8, 8)
        #    u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
        #    u.vector()[:] = 1.0
        #    File("u.e") << u

if __name__ == "__main__":
    unittest.main()
