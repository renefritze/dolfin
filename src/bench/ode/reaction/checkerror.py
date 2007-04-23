#!/usr/bin/python
#
# Copyright (C) 2005-2006 Anders Logg.
# Licensed under the GNU LGPL Version 2.1.
#
# Test script for checking the error of the
# multi-adaptive solution. Good for testing.

from os import system
from benchutil import *

system("./dolfin-ode-reaction mcg fixed-point 1e-6 1000 5.0 parameters-bench.xml")
e = check_error("solution.data", "reference_1000.data")

print ""
print "Error: %g" % e
print "Error should be 1.7708e-05 and take about 13.75 seconds to compute."
