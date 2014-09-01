#!/usr/bin/env py.test

# Copyright (C) 2012 Garth N. Wells
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
# First added:  2012-09-14
# Last changed: 2013-03-05

import pytest
import os
from dolfin import *

# create an output folder
@pytest.fixture(scope="module")
def temppath():
    filedir = os.path.dirname(os.path.abspath(__file__))
    basename = os.path.basename(__file__).replace(".py", "_data")
    temppath = os.path.join(filedir, basename, "")
    if not os.path.exists(temppath):
        os.mkdir(temppath)
    return temppath

if has_hdf5():
    def test_save_and_load_1d_mesh(temppath):
        mesh = UnitIntervalMesh(32)
        File(os.path.join(temppath, "mesh.xdmf")) << mesh
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mesh.xdmf")) << mesh
        mesh2 = Mesh(os.path.join(temppath, "mesh.xdmf"))
        assert mesh.size_global(0) == mesh2.size_global(0)
        dim = mesh.topology().dim()
        assert mesh.size_global(dim) == mesh2.size_global(dim)

    def test_save_and_load_2d_mesh(temppath):
        mesh = UnitSquareMesh(32, 32)
        File(os.path.join(temppath, "mesh_2D.xdmf")) << mesh
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mesh_2D.xdmf")) << mesh
        mesh2 = Mesh(os.path.join(temppath, "mesh_2D.xdmf"))
        assert mesh.size_global(0) == mesh2.size_global(0)
        dim = mesh.topology().dim()
        assert mesh.size_global(dim) == mesh2.size_global(dim)

    def test_save_and_load_3d_mesh(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        File(os.path.join(temppath, "mesh_3D.xdmf")) << mesh
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mesh_3D.xdmf")) << mesh
        mesh2 = Mesh(os.path.join(temppath, "mesh_3D.xdmf"))
        assert mesh.size_global(0) == mesh2.size_global(0)
        dim = mesh.topology().dim()
        assert mesh.size_global(dim) == mesh2.size_global(dim)

    def test_save_1d_scalar(temppath):
        mesh = UnitIntervalMesh(32)
        u = Function(FunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File(file_path_u +".xdmf") << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u

    def test_save_2d_scalar(temppath):
        mesh = UnitSquareMesh(16, 16)
        u = Function(FunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u

    def test_save_3d_scalar(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        u = Function(FunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u

    def test_save_2d_vector(temppath):
        mesh = UnitSquareMesh(16, 16)
        u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
        c = Constant((1.0, 2.0))
        u.interpolate(c)
        File(mesh.mpi_comm(), os.path.join(temppath, "u_2dv.xdmf")) << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u

    def test_save_3d_vector(temppath):
        mesh = UnitCubeMesh(1, 1, 1)
        u = Function(VectorFunctionSpace(mesh, "Lagrange", 1))
        c = Constant((1.0, 2.0, 3.0))
        u.interpolate(c)
        File(mesh.mpi_comm(), os.path.join(temppath, "u_3Dv.xdmf")) << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u

    def test_save_3d_vector_series(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        u = Function(VectorFunctionSpace(mesh, "Lagrange", 2))
        file = File(mesh.mpi_comm(), os.path.join(temppath, "u_3D.xdmf"))

        u.vector()[:] = 1.0
        file << (u, 0.1)

        u.vector()[:] = 2.0
        file << (u, 0.2)

        u.vector()[:] = 3.0
        file << (u, 0.3)
        del file

        file = XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u_3D.xdmf"))

        u.vector()[:] = 1.0
        file << (u, 0.1)

        u.vector()[:] = 2.0
        file << (u, 0.2)

        u.vector()[:] = 3.0
        file << (u, 0.3)

    def test_save_2d_tensor(temppath):
        mesh = UnitSquareMesh(16, 16)
        u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File(mesh.mpi_comm(), os.path.join(tensor, "tensor.xdmf")) << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "tensor.xdmf")) << u

    def test_save_3d_tensor(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        u = Function(TensorFunctionSpace(mesh, "Lagrange", 2))
        u.vector()[:] = 1.0
        File(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "u.xdmf")) << u

    def test_save_1d_mesh(temppath):
        mesh = UnitIntervalMesh(32)
        mf = CellFunction("size_t", mesh)
        for cell in cells(mesh):
            mf[cell] = cell.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_1D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_1D.xdmf")) << mf

    def test_save_2D_cell_function(temppath):
        mesh = UnitSquareMesh(32, 32)
        mf = CellFunction("size_t", mesh)
        for cell in cells(mesh):
            mf[cell] = cell.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_2D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_2D.xdmf")) << mf

    def test_save_3D_cell_function(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        mf = CellFunction("size_t", mesh)
        for cell in cells(mesh):
            mf[cell] = cell.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_3D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_3D.xdmf")) << mf

    def test_save_2D_facet_function(temppath):
        mesh = UnitSquareMesh(32, 32)
        mf = FacetFunction("size_t", mesh)
        for facet in facets(mesh):
            mf[facet] = facet.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_facet_2D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_facet_2D.xdmf")) << mf

    def test_save_3D_facet_function(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        mf = FacetFunction("size_t", mesh)
        for facet in facets(mesh):
            mf[facet] = facet.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_facet_3D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_facet_3D.xdmf")) << mf

    def test_save_3D_edge_function(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        mf = EdgeFunction("size_t", mesh)
        for edge in edges(mesh):
            mf[edge] = edge.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_edge_3D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_edge_3D.xdmf")) << mf

    def test_save_2D_vertex_function(temppath):
        mesh = UnitSquareMesh(32, 32)
        mf = VertexFunction("size_t", mesh)
        for vertex in vertices(mesh):
            mf[vertex] = vertex.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_vertex_2D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_vertex_2D.xdmf")) << mf

    def test_save_3D_vertex_function(temppath):
        mesh = UnitCubeMesh(8, 8, 8)
        mf = VertexFunction("size_t", mesh)
        for vertex in vertices(mesh):
            mf[vertex] = vertex.index()
        File(mesh.mpi_comm(), os.path.join(temppath, "mf_vertex_3D.xdmf")) << mf
        XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "mf_vertex_3D.xdmf")) << mf

    def test_save_points_2D(temppath):
        import numpy
        mesh = UnitSquareMesh(16, 16)
        points, values = [], []
        for v in vertices(mesh):
            points.append(v.point())
            values.append(v.point().norm())
        vals = numpy.array(values)

        file = XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "points_2D.xdmf"))
        file.write(points)

        file = XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "points_values_2D.xdmf"))
        file.write(points, vals)

    def test_save_points_3D(temppath):
        import numpy
        mesh = UnitCubeMesh(4, 4, 4)
        points, values = [], []
        for v in vertices(mesh):
            points.append(v.point())
            values.append(v.point().norm())
        vals = numpy.array(values)

        file = XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "points_3D.xdmf"))
        file.write(points)

        file = XDMFFile(mesh.mpi_comm(), os.path.join(temppath, "points_values_3D.xdmf"))
        file.write(points, vals)
