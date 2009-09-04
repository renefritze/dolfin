#!/usr/bin/env python
#
# Generate list of include files for SWIG interface file.
#
# Modified by <aasmund@simula.no>

__author__ = "Anders Logg (logg@simula.no)"
__date__ = "2007-04-12 -- 2007-05-02"
__copyright__ = "Copyright (C) 2007 Anders Logg"
__license__  = "GNU LGPL Version 2.1"

import os
import re

# List of headers to exclude (add more here)
excludes = ["plot.h", "ParameterSystem.h", "ParameterList.h", \
            "ConvectionMatrix.h", "MassMatrix.h", "StiffnessMatrix.h", "LoadVector.h"]

# Name of SWIG interface file to be generated
interface_file = "headers.i"

# Extract modules from dolfin.h
modules = []
f = open("../dolfin.h")
for line in f:
    if "#include <dolfin/" in line and line[:17] == "#include <dolfin/":
        module = line.split("/")[1]
        modules += [module]
f.close()

# Extract header files
headers = []
docstring_headers = [] # Added for docstring extraction
for module in modules:
    module_headers = []
    print "Processing dolfin_%s.h..." % module
    f = open("../%s/dolfin_%s.h" % (module, module))
    module_base = "../" + module + "/"  # Added for docstring extraction
    for line in f:
        if re.search("^#include ",line):
            header = line.split()[1].replace("<", "").replace(">", "")
            # get just the file name (after last /) and check against excludes:
            if not header.split("/")[-1] in excludes:
                module_headers += [header]
                docstring_headers += [module_base + os.path.basename(header)]  # Added for docstring extraction
    f.close()
    headers += [(module, module_headers)]

# Generate list of header files
print "Generating file %s" % interface_file
f = open(interface_file, "w")
f.write("// Generated list of include files for PyDOLFIN\n")
for (module, module_headers) in headers:
    f.write("\n// DOLFIN headers included from %s\n" % module)
    for header in module_headers:
        f.write("%%include \"%s\"\n" % header)
f.close()

# Added for docstring extraction
from generate_docstrings import DocstringGenerator

g = DocstringGenerator(header_files = docstring_headers, swig_directory = ".", docstring_file_base = "")
g.generate_doxygen_documentation()
g.generate_interface_file_from_index()
g.clean()
