# Copyright (C) 2011 Marie E. Rognes
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
# First added:  2011-11-14
# Last changed: 2013-03-04

import sys
from instant import get_status_output

tests = ["assembly_derivatives.py", "form_operations.py"]

failed = []
for test in tests:
    command = "%s %s" % (sys.executable,test)
    print
    print "Running tests: %s" % test
    print "----------------------------------------------------------------------"
    fail, output = get_status_output(command)

    if fail:
        failed.append(fail)
        print "*** %s failed" % test
        print output
    else:
        print "OK"

sys.exit(len(failed))
