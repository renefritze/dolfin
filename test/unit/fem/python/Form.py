"""Unit tests for the fem interface"""

# Copyright (C) 2011 Johan Hake
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
# First added:  2011-12-02
# Last changed: 2011-12-02
#
# Modified by Marie E. Rognes (meg@simula.no), 2012

import unittest
import numpy
from dolfin import *

class FormTest(unittest.TestCase):

    def setUp(self):
        self.mesh = UnitSquareMesh(10, 10)
        self.V = FunctionSpace(self.mesh, "CG", 1)
        self.f = Expression("sin(pi*x[0]*x[1])")
        self.v = TestFunction(self.V)
        self.u = TrialFunction(self.V)

    def _test_assemble(self):
        ufl_form = self.f*self.u*self.v*dx
        dolfin_form = Form(ufl_form)
        ufc_form = dolfin_form._compiled_form
        A_ufl_norm = assemble(ufl_form).norm("frobenius")
        A_dolfin_norm = assemble(dolfin_form).norm("frobenius")
        A_ufc_norm = assemble(ufc_form, coefficients=[self.f],
                              function_spaces=[self.V, self.V]).norm("frobenius")
        self.assertAlmostEqual(A_ufl_norm, A_dolfin_norm)
        self.assertAlmostEqual(A_ufl_norm, A_ufc_norm)

class FormTestsOverManifolds(unittest.TestCase):

    def setUp(self):

        # 1D in 2D spaces
        self.square = UnitSquareMesh(2, 2)
        self.mesh1 = BoundaryMesh(self.square)
        self.V1 = FunctionSpace(self.mesh1, "CG", 1)
        self.VV1 = VectorFunctionSpace(self.mesh1, "CG", 1)
        self.Q1 = FunctionSpace(self.mesh1, "DG", 0)

        # 2D in 3D spaces
        self.cube = UnitCubeMesh(2, 2, 2)
        self.mesh2 = BoundaryMesh(self.cube)
        self.V2 = FunctionSpace(self.mesh2, "CG", 1)
        self.VV2 = VectorFunctionSpace(self.mesh2, "CG", 1)
        self.Q2 = FunctionSpace(self.mesh2, "DG", 0)

    def _test_assemble_functional(self):
        u = Function(self.V1)
        u.vector()[:] = 1.0
        surfacearea = assemble(u*dx)
        self.assertAlmostEqual(surfacearea, 4.0)

        u = Function(self.V2)
        u.vector()[:] = 1.0
        surfacearea = assemble(u*dx)
        self.assertAlmostEqual(surfacearea, 6.0)

        f = Expression("1.0")
        u = interpolate(f, self.V1)
        surfacearea = assemble(u*dx)
        self.assertAlmostEqual(surfacearea, 4.0)

        f = Expression("1.0")
        u = interpolate(f, self.V2)
        surfacearea = assemble(u*dx)
        self.assertAlmostEqual(surfacearea, 6.0)

    def _test_assemble_linear(self):
        u = Function(self.V1)
        w = TestFunction(self.Q1)
        u.vector()[:] = 0.5
        facetareas = assemble(u*w*dx).array().sum()
        self.assertAlmostEqual(facetareas, 2.0)

        u = Function(self.V2)
        w = TestFunction(self.Q2)
        u.vector()[:] = 0.5
        a = u*w*dx
        b = assemble(a)
        facetareas = assemble(u*w*dx).array().sum()
        self.assertAlmostEqual(facetareas, 3.0)

        mesh = UnitSquareMesh(8, 8)
        bdry = BoundaryMesh(mesh)
        V = FunctionSpace(mesh, "CG", 1)
        u = TrialFunction(V)
        v = TestFunction(V)

        BV = FunctionSpace(bdry, "CG", 1)
        bu = TrialFunction(BV)
        bv = TestFunction(BV)

        a = assemble(inner(u, v)*ds).array().sum()
        b = assemble(inner(bu, bv)*dx).array().sum()
        self.assertAlmostEqual(a, b)

    def _test_assemble_bilinear_1D_2D(self):

        V = FunctionSpace(self.square, 'CG', 1)
        u = TrialFunction(V)
        v = TestFunction(V)

        bu = TrialFunction(self.V1)
        bv = TestFunction(self.V1)

        a = assemble(inner(u, v)*ds).array().sum()
        b = assemble(inner(bu, bv)*dx).array().sum()
        self.assertAlmostEqual(a, b)

        bottom = compile_subdomains("near(x[1], 0.0)")
        foo = abs(assemble(inner(grad(u)[0], grad(v)[0])*ds,
                           exterior_facet_domains=bottom).array()).sum()
        BV = FunctionSpace(SubMesh(self.mesh1, bottom), "CG", 1)
        bu = TrialFunction(BV)
        bv = TestFunction(BV)
        bar = abs(assemble(inner(grad(bu), grad(bv))*dx).array()).sum()
        self.assertAlmostEqual(bar, foo)

    def _test_assemble_bilinear_2D_3D(self):

        V = FunctionSpace(self.cube, 'CG', 1)
        u = TrialFunction(V)
        v = TestFunction(V)

        bu = TrialFunction(self.V2)
        bv = TestFunction(self.V2)

        a = assemble(inner(u, v)*ds).array().sum()
        b = assemble(inner(bu, bv)*dx).array().sum()
        self.assertAlmostEqual(a, b)

        bottom = compile_subdomains("near(x[1], 0.0)")
        foo = abs(assemble(inner(grad(u)[0], grad(v)[0])*ds,
                           exterior_facet_domains=bottom).array()).sum()
        BV = FunctionSpace(SubMesh(self.mesh1, bottom), "CG", 1)
        bu = TrialFunction(BV)
        bv = TestFunction(BV)
        bar = abs(assemble(inner(grad(bu), grad(bv))*dx).array()).sum()
        self.assertAlmostEqual(bar, foo)

class FormTestsOverFunnySpaces(unittest.TestCase):

    def setUp(self):

        n = 1
        bottom = compile_subdomains("near(x[1], 0.0)")
        self.square = UnitSquareMesh(n, n)
        self.square3d = SubMesh(BoundaryMesh(UnitCubeMesh(n, n, n)), bottom)

        # # Create mesh object and open editor
        # self.square = Mesh()
        # editor = MeshEditor()
        # editor.open(self.square, 2, 2)
        # editor.init_vertices(4)
        # editor.init_cells(2)

        # # Add vertices
        # editor.add_vertex(1, 0.0, 0.0)
        # editor.add_vertex(0, 1.0, 0.0)
        # editor.add_vertex(3, 0.0, 1.0)
        # editor.add_vertex(2, 1.0, 1.0)

        # # Add cell
        # editor.add_cell(0, 0, 1, 2)
        # editor.add_cell(1, 1, 2, 3)

        # # Close editor
        # editor.close()

        # # Create mesh object and open editor
        # self.square3d = Mesh()
        # editor = MeshEditor()
        # editor.open(self.square3d, 2, 3)
        # editor.init_vertices(4)
        # editor.init_cells(2)

        # # Add vertices
        # editor.add_vertex(1, 0.0, 0.0, 0.0)
        # editor.add_vertex(0, 1.0, 0.0, 0.0)
        # editor.add_vertex(3, 0.0, 1.0, 0.0)
        # editor.add_vertex(2, 1.0, 1.0, 0.0)

        # # Add cell
        # editor.add_cell(0, 0, 1, 2)
        # editor.add_cell(1, 1, 2, 3)

        # Create global_orientation
        mf = self.square3d.data().create_mesh_function("cell_orientation", 2)
        global_normal = numpy.array((0.0, 1.0, 0.0))

        for cell in cells(self.square3d):
            ind = [v.index() for v in vertices(cell)]
            v1 = self.square3d.coordinates()[ind[1], :] - self.square3d.coordinates()[ind[0], :]
            v2 = self.square3d.coordinates()[ind[2], :] - self.square3d.coordinates()[ind[0], :]
            local_normal = numpy.cross(v1, v2)
            orientation = numpy.inner(global_normal, local_normal)
            print "orientation = ", orientation
            if orientation > 0:
                mf[cell.index()] = 2
            elif orientation < 0:
                mf[cell.index()] = 1
            else:
                raise Exception, "Not expecting orthogonal local/global normal"

        #plot(mf, interactive=True)

        self.CG2 = VectorFunctionSpace(self.square, "CG", 1)
        self.CG3 = VectorFunctionSpace(self.square3d, "CG", 1)
        self.RT2 = FunctionSpace(self.square, "RT", 1)
        self.RT3 = FunctionSpace(self.square3d, "RT", 1)

    def test_basic_rt(self):

        f2 = Expression(("2.0", "0.0"))
        f3 = Expression(("0.0", "0.0", "2.0"))

        u2 = TrialFunction(self.RT2)
        u3 = TrialFunction(self.RT3)
        v2 = TestFunction(self.RT2)
        v3 = TestFunction(self.RT3)

        #w2 = project(f2, self.RT2)
        #w3 = project(f3, self.RT3)
        #info(w2.vector(), True)
        #info(w3.vector(), True)

        #exit()

        a2 = inner(u2, v2)*dx
        a3 = inner(u3, v3)*dx

        L2 = inner(f2, v2)*dx
        L3 = inner(f3, v3)*dx

        w2 = Function(self.RT2)
        w3 = Function(self.RT3)

        info_blue("Assembling std")
        A2 = assemble(a2)
        b2 = assemble(L2)

        info_blue("Assembling manifold")
        A3 = assemble(a3)
        b3 = assemble(L3)

        print "A2 - A3 = ", A2.array() - A3.array()
        print "b2 - b3 = ", b2.array() - b3.array()

        solve(A2, w2.vector(), b2)
        solve(A3, w3.vector(), b3)

        info(w2.vector(), True)
        info(w3.vector(), True)

        avg2 = assemble(inner(w2, w2)*dx)
        print "avg2 = ", avg2

        avg3 = assemble(inner(w3, w3)*dx)
        print "avg3 = ", avg3


if __name__ == "__main__":
    print ""
    print "Testing PyDOLFIN Form operations"
    print "------------------------------------------------"
    unittest.main()
