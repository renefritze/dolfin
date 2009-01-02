"""Run all unit tests."""

__author__ = "Anders Logg (logg@simula.no)"
__date__ = "2006-08-09 -- 2007-08-03"
__copyright__ = "Copyright (C) 2006-2007 Anders Logg"
__license__  = "GNU LGPL Version 2.1"

import sys
from os import system
from commands import getoutput
import re

# Tests to run
tests = ["function", "mesh", "meshconvert", "la", "io"]

# FIXME: Graph tests disabled for now since SCOTCH is now required

# Check if we should run only Python tests, use for quick testing
if len(sys.argv) == 2 and sys.argv[1] == "--only-python":
    only_python = True
else:
    only_python = False

# Run tests
failed = []
for test in tests:
    print "Running unit tests for %s" % test
    print "----------------------------------------------------------------------"

    print "C++:   ",
    if not only_python:
        output = getoutput("cd %s/cpp && ./test" % test)
        if "OK" in output:
            num_tests = int(re.search("OK \((\d+)\)", output).groups()[0])
            print "OK (%d tests)" % num_tests
        else:
            print "*** Failed"
            failed += [(test, "C++", output)]
    else:
        print "Skipping"
    
    print "Python:",
    output = getoutput("cd %s/python && python ./test.py" % test)
    if "OK" in output:
        num_tests = int(re.search("Ran (\d+) test", output).groups()[0])
        print "OK (%d tests)" % num_tests
    else:
        print "*** Failed"
        failed += [(test, "Python", output)]

    print ""

# Print output for failed tests
for (test, interface, output) in failed:
    print "One or more unit tests failed for %s (%s):" % (test, interface)
    print output

# Return error code if tests failed
sys.exit(len(failed) != 0)
