"""Unit tests for the VTK io library"""

# Copyright (C) 2011 Garth N. Wells
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
# First added:  2011-05-18
# Last changed:

import unittest
from dolfin import *

# VTK file options
file_options = ["ascii", "base64", "compressed"]
mesh_functions = [CellFunction, FacetFunction, FaceFunction, EdgeFunction, VertexFunction]
mesh_function_types = ["size_t", "int", "double", "bool"]
type_conv = dict(size_t=int, int=int, double=float, bool=bool)

class VTK_MeshFunction_Output(unittest.TestCase):
    """Test output of MeshFunctions to VTK files"""
    def test_save_1d_meshfunctions(self):
        mesh = UnitIntervalMesh(32)
        for F in mesh_functions:
            if F in [FaceFunction, EdgeFunction]: continue
            for t in mesh_function_types:
                mf = F(t, mesh, type_conv[t](1))
                File("mf.pvd") << mf
                f = File("mf.pvd")
                f << (mf, 0.)
                f << (mf, 1.)
                for file_option in file_options:
                    File("mf.pvd", file_option) << mf

    def test_save_2d_meshfunctions(self):
        mesh = UnitSquareMesh(32, 32)
        for F in mesh_functions:
            for t in mesh_function_types:
                mf = F(t, mesh, type_conv[t](1))
                File("mf.pvd") << mf
                f = File("mf.pvd")
                f << (mf, 0.)
                f << (mf, 1.)
                for file_option in file_options:
                    File("mf.pvd", file_option) << mf

    def test_save_3d_meshfunctions(self):
        mesh = UnitCubeMesh(8, 8, 8)
        for F in mesh_functions:
            for t in mesh_function_types:
                mf = F(t, mesh, type_conv[t](1))
                File("mf.pvd") << mf
                f = File("mf.pvd")
                f << (mf, 0.)
                f << (mf, 1.)
                for file_option in file_options:
                    File("mf.pvd", file_option) << mf

class VTK_Mesh_Output(unittest.TestCase):
    """Test output of Meshes to VTK files"""

    def test_save_1d_mesh(self):
        mesh = UnitIntervalMesh(32)
        File("mesh.pvd") << mesh
        f = File("mesh.pvd")
        f << (mesh, 0.)
        f << (mesh, 1.)
        for file_option in file_options:
            File("mesh.pvd", file_option) << mesh

    def test_save_2d_mesh(self):
        mesh = UnitSquareMesh(32, 32)
        File("mesh.pvd") << mesh
        f = File("mesh.pvd")
        f << (mesh, 0.)
        f << (mesh, 1.)
        for file_option in file_options:
            File("mesh.pvd", file_option) << mesh

    def test_save_3d_mesh(self):
        mesh = UnitCubeMesh(8, 8, 8)
        File("mesh.pvd") << mesh
        f = File("mesh.pvd")
        f << (mesh, 0.)
        f << (mesh, 1.)
        for file_option in file_options:
            File("mesh.pvd", file_option) << mesh


class VTK_Point_Function_Output(unittest.TestCase):
    """Test output of point-based Functions to VTK files"""

    def test_save_1d_scalar(self):
        mesh = UnitIntervalMesh(32)
        u = Function(FunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

    def test_save_2d_scalar(self):
        mesh = UnitSquareMesh(16, 16)
        u = Function(FunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

    def test_save_3d_scalar(self):
        mesh = UnitCubeMesh(8, 8, 8)
        u = Function(FunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

    # FFC fails for vector spaces in 1D
    #@unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
    #def test_save_1d_vector(self):
    #    mesh = UnitIntervalMesh(32)
    #    u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
    #    u.vector()[:] = 1.0
    #    File("u.pvd") << u
    #    for file_option in file_options:
    #        File("u.pvd", file_option) << u

    def test_save_2d_vector(self):
        mesh = UnitSquareMesh(16, 16)
        u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

    def test_save_3d_vector(self):
        mesh = UnitCubeMesh(8, 8, 8)
        u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

    # FFC fails for tensor spaces in 1D
    #@unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
    #def test_save_1d_tensor(self):
    #    mesh = UnitIntervalMesh(32)
    #    u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
    #    u.vector()[:] = 1.0
    #    File("u.pvd") << u
    #    for file_option in file_options:
    #        File("u.pvd", file_option) << u

    def test_save_2d_tensor(self):
        mesh = UnitSquareMesh(16, 16)
        u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

    def test_save_3d_tensor(self):
        mesh = UnitCubeMesh(8, 8, 8)
        u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File("u.pvd") << u
        f = File("u.pvd")
        f << (u, 0.)
        f << (u, 1.)
        for file_option in file_options:
            File("u.pvd", file_option) << u

if __name__ == "__main__":
    unittest.main()
