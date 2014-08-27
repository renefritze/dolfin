#!/usr/bin/env py.test

"Unit tests for XML input/output of MeshValueCollection"

# Copyright (C) 2011 Anders Logg
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
# First added:  2011-09-01
# Last changed: 2014-05-30

import pytest
from dolfin import *
import os

# create an output folder
filepath = os.path.abspath(__file__).split(str(os.path.sep))[:-1]
filepath = str(os.path.sep).join(filepath + ['output', ''])
if not os.path.exists(filepath):
    os.mkdir(filepath)

@pytest.mark.skipIf(MPI.size(mpi_comm_world()) > 1, 
                      reason="Skipping unit test(s) not working in parallel")
def test_insertion_extraction_io():
    "Test input/output via << and >>."

    # Create mesh
    mesh = UnitCubeMesh(5, 5, 5)

    # Create mesh value collection and add some data
    mesh = UnitCubeMesh(5, 5, 5)
    output_values = MeshValueCollection("size_t", mesh, 2)
    output_values.set_value(1,  1, 1);
    output_values.set_value(2,  1, 3);
    output_values.set_value(5,  1, 8);
    output_values.set_value(13, 1, 21);
    output_values.set_value(7,  2, 13);
    output_values.set_value(4,  2, 2);

    name = "test_mesh_value_collection"
    output_values.rename(name, "a MeshValueCollection")

    # Write to file
    output_file = File(filepath + "xml_mesh_value_collection_test_io.xml")
    output_file << output_values

    # Read from file
    input_file = File(filepath + "xml_mesh_value_collection_test_io.xml")
    input_values = MeshValueCollection("size_t", mesh)
    input_file >> input_values

    # Get some data and check that it matches
    assert input_values.size() == output_values.size()
    assert input_values.dim() == output_values.dim()
    assert input_values.name() == name

def test_constructor_input():
    "Test input via constructor."

    # Create mesh
    mesh = UnitCubeMesh(5, 5, 5)

    # Read from file
    input_values = MeshValueCollection("size_t", mesh, "xml_value_collection_ref.xml")

    # Check that size is correct
    assert MPI.sum(mesh.mpi_comm(), input_values.size()) == 6
