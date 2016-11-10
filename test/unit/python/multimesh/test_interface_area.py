#!/usr/bin/env py.test

"""Unit tests for multimesh volume computation"""

# Copyright (C) 2016 Anders Logg
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
# Modified by August Johansson 2016
#
# First added:  2016-11-02
# Last changed: 2016-11-10

from __future__ import print_function
import pytest

from dolfin import *
from dolfin_utils.test import skip_in_parallel

def compute_area_using_quadrature(multimesh):
    total_area = 0
    for part in range(multimesh.num_parts()):
        part_area = 0

        for cut_cell in multimesh.collision_map_cut_cells(part):
            local_qr = multimesh.quadrature_rule_interface_cut_cell(part, cut_cell)
            for weight in local_qr[1]:
                part_area += weight

        total_area += part_area
    return total_area

@skip_in_parallel
def test_meshes_on_diagonal():
    "Place meshes on the diagonal inside a background mesh and check the interface area"

    # Number of elements
    Nx = 3

    # Background mesh
    mesh_0 = UnitSquareMesh(Nx, Nx)

    # Create multimesh
    multimesh = MultiMesh()
    multimesh.add(mesh_0)

    # Mesh width (must be less than 1)
    width = 0.3
    assert width < 1

    # Mesh placement (must be less than the width)
    offset = 0.1111
    assert offset < width

    # Exact area for there are one mesh on top
    exact_area = 4*width

    num_parts = multimesh.num_parts()
    while num_parts*width + offset < 1:
        a = num_parts*offset
        b = a + width
        mesh_top = RectangleMesh(Point(a,a), Point(b,b), Nx, Nx)
        multimesh.add(mesh_top)
        if num_parts > 1:
            exact_area += 2*offset + 2*width
        num_parts = multimesh.num_parts()

    multimesh.build()

    area = compute_area_using_quadrature(multimesh)
    assert abs(area - exact_area) < DOLFIN_EPS_LARGE

@skip_in_parallel
def test_meshes_with_boundary_edge_overlap_2D():
    # start with boundary of mesh 1 overlapping edges of mesg 0
    mesh0 = UnitSquareMesh(4,4)
    mesh1 = UnitSquareMesh(1,1)

    mesh1_coords = mesh1.coordinates()
    mesh1_coords *= 0.5
    mesh1.translate(Point(0.25, 0.25))

    multimesh = MultiMesh()
    multimesh.add(mesh0)
    multimesh.add(mesh1)
    multimesh.build()

    exact_area = 2.0

    area = compute_area_using_quadrature(multimesh)
    assert  abs(area - exact_area) < DOLFIN_EPS_LARGE

    # next translate mesh 1 such that only the horizontal part of the boundary overlaps
    mesh1.translate(Point(0.1, 0.0))
    multimesh.build()
    area = compute_area_using_quadrature(multimesh)
    assert  abs(area - exact_area) < DOLFIN_EPS_LARGE

    # next translate mesh 1 such that no boundaries overlap with edges
    mesh1.translate(Point(0.0, 0.1))
    multimesh.build()
    area = compute_area_using_quadrature(multimesh)
    assert  abs(area - exact_area) < DOLFIN_EPS_LARGE



# FIXME: Temporary testing
if __name__ == "__main__":
    test_meshes_on_diagonal()
    test_meshes_with_boundary_edge_overlap_2D()
