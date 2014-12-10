#!/usr/bin/env py.test

"""Unit tests for the io library"""

# Copyright (C) 2007 Anders Logg
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
import os
from dolfin import *
from dolfin_utils.test import *

# FIXME: Not a proper unit test. When LocalMeshData has a public interface
# FIXME: we can expand on these

def test_read_local_mesh_data(datadir):
    file = File(os.path.join(datadir, "snake.xml.gz"))
    localdata = cpp.LocalMeshData(mpi_comm_world())
    file >> localdata
