#!/usr/bin/env py.test

"""Unit tests for the IntersectionTriangulation class"""

# Copyright (C) 2014 Anders Logg and August Johansson
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

import pytest
import numpy
from dolfin import *
from six.moves import xrange as range
from dolfin_utils.test import skip_in_parallel

def triangulation_to_mesh_2d(triangulation):
    editor = MeshEditor()
    mesh = Mesh()
    editor.open(mesh, 'triangle', 2, 2)
    num_cells = len(triangulation) // 6
    num_vertices = len(triangulation) // 2
    editor.init_cells(num_cells)
    editor.init_vertices(num_vertices)
    for i in range(num_cells):
        editor.add_cell(i, numpy.array([3*i, 3*i + 1, 3*i + 2], dtype='uint'))
    for i in range(num_vertices):
        editor.add_vertex(i, numpy.array([triangulation[2*i], triangulation[2*i + 1]], dtype='float'))
    editor.close()
    return mesh

def triangulation_to_mesh_2d_3d(triangulation):
    editor = MeshEditor()
    mesh = Mesh()
    editor.open(mesh, 'triangle', 2,3)
    num_cells = len(triangulation) // 9
    num_vertices = len(triangulation) // 3
    editor.init_cells(num_cells)
    editor.init_vertices(num_vertices)
    for i in range(num_cells):
        editor.add_cell(i, numpy.array([3*i, 3*i+1, 3*i+2], dtype='uint'))
    for i in range(num_vertices):
        editor.add_vertex(i, numpy.array([triangulation[3*i], triangulation[3*i+1], triangulation[3*i+2]], dtype='float'))
    editor.close()
    return mesh

def triangulation_to_mesh_3d(triangulation):
    editor = MeshEditor()
    mesh = Mesh()
    editor.open(mesh, 'tetrahedron', 3, 3)
    num_cells = len(triangulation) // 12
    num_vertices = len(triangulation) // 3
    editor.init_cells(num_cells)
    editor.init_vertices(num_vertices)
    for i in range(num_cells):
        editor.add_cell(i, numpy.array([4*i, 4*i+1, 4*i+2, 4*i+3], dtype='uint'))
    for i in range(num_vertices):
        editor.add_vertex(i, numpy.array([triangulation[3*i], triangulation[3*i+1], triangulation[3*i+2]], dtype='float'))
    editor.close()
    return mesh

@skip_in_parallel
def test_triangulate_intersection_2d():

    # Create two meshes of the unit square
    mesh_0 = UnitSquareMesh(1, 1)
    mesh_1 = UnitSquareMesh(1, 1)

    # Translate second mesh randomly
    #dx = Point(numpy.random.rand(),numpy.random.rand())
    dx = Point(0.278498, 0.546881)
    mesh_1.translate(dx)

    exactvolume = (1 - abs(dx[0]))*(1 - abs(dx[1]))

    # Compute triangulation
    volume = 0
    for c0 in cells(mesh_0):
        for c1 in cells(mesh_1):
            triangulation = c0.triangulate_intersection(c1)
            if (len(triangulation) > 0):
                tmesh = triangulation_to_mesh_2d(triangulation)
                for t in cells(tmesh):
                    volume += t.volume()

    errorstring = "translation=" + str(dx[0]) + str(" ") + str(dx[1])
    assert round(volume - exactvolume, 7) == 0, errorstring

@skip_in_parallel
def test_triangulate_intersection_2d_3d():

    # Note: this test will fail if the triangle mesh is aligned
    # with the tetrahedron mesh

    # Create a unit cube
    mesh_0 = UnitCubeMesh(1,1,1)

    # Create a 3D surface mesh
    editor = MeshEditor()
    mesh_1 = Mesh()
    editor.open(mesh_1, 'triangle', 2, 3)
    editor.init_cells(2)
    editor.init_vertices(4)
    # add cells
    editor.add_cell(0, numpy.array([0,1,2], dtype='uint'))
    editor.add_cell(1, numpy.array([1,2,3], dtype='uint'))
    # add vertices
    editor.add_vertex(0, numpy.array([0,0,0.5], dtype='float'))
    editor.add_vertex(1, numpy.array([1,0,0.5], dtype='float'))
    editor.add_vertex(2, numpy.array([0,1,0.5], dtype='float'))
    editor.add_vertex(3, numpy.array([1,1,0.5], dtype='float'))
    editor.close()

    # Rotate the triangle mesh around y axis a random angle in
    # (0,90) degrees
    #angle = numpy.random.rand()*90
    angle = 23.46354
    mesh_1.rotate(angle,1)

    # Exact area
    exactvolume = 1

    # Compute triangulation
    volume = 0
    for c0 in cells(mesh_0):
        for c1 in cells(mesh_1):
            triangulation = c0.triangulate_intersection(c1)
            if (len(triangulation) > 0):
                tmesh = triangulation_to_mesh_2d_3d(triangulation)
                for t in cells(tmesh):
                    volume += t.volume()

    errorstring = "rotation angle = " + str(angle)
    assert round(volume - exactvolume, 7) == 0, errorstring


@skip_in_parallel
def test_triangulate_intersection_3d():

    # Create two meshes of the unit cube
    mesh_0 = UnitCubeMesh(1, 1, 1)
    mesh_1 = UnitCubeMesh(1, 1, 1)

    # Translate second mesh
    # dx = Point(numpy.random.rand(),numpy.random.rand(),numpy.random.rand())
    dx = Point(0.913375, 0.632359, 0.097540)

    mesh_1.translate(dx)
    exactvolume = (1 - abs(dx[0]))*(1 - abs(dx[1]))*(1 - abs(dx[2]))

    # Compute triangulation
    volume = 0
    for c0 in cells(mesh_0):
        for c1 in cells(mesh_1):
            triangulation = c0.triangulate_intersection(c1)
            if (len(triangulation) > 0):
                tmesh = triangulation_to_mesh_3d(triangulation)
                for t in cells(tmesh):
                    volume += t.volume()

    errorstring = "translation="
    errorstring += str(dx[0])+" "+str(dx[1])+" "+str(dx[2])
    assert round(volume - exactvolume, 7) == 0, errorstring
