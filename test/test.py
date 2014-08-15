"Run all tests"

# Copyright (C) 2007-2011 Anders Logg
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
# First added:  2007-06-09
# Last changed: 2011-11-14

from __future__ import print_function
import re, sys, os, subprocess
from instant import get_status_output

pwd = os.path.dirname(os.path.abspath(__file__))

# Tests to run
tests = ["unit", "regression", "system", "documentation", "codingstyle"]

# Check if we should enable memory testing
if len(sys.argv) == 2 and sys.argv[1] == "--enable-memory-test":
    tests.append("memory")

failed = []

# Command to run
command = [sys.executable, "test.py"] + sys.argv[1:]

# Run tests
for test in tests:
    print("Running tests: %s" % test)
    print("----------------------------------------------------------------------")
    os.chdir(os.path.join(pwd, test))
    status = subprocess.Popen(command, shell=False).wait()
    if status:
        failed.append(status)

sys.exit(len(failed))
