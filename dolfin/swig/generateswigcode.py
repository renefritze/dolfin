#!/usr/bin/env python
#
# Generate SWIG files for Python interface of DOLFIN
#
# Copyright (C) 2012 Johan Hake
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
# First added:  2012-01-17
# Last changed: 2012-07-03

# System imports
import os
import re
import glob
import time
import sys

from dolfin_utils.cppparser import *

# Local imports
from codesnippets import *
from documentation import generate_docstrings

# Create time info for labeling generated code
_local_time = time.localtime()
_date_form = dict(year = _local_time.tm_year,
                  month = _local_time.tm_mon,
                  day = _local_time.tm_mday)

# Create form for copyright statement to a SWIG interface file
copyright_form_swig = dict(comment = r"//", holder="Johan Hake")

# reg exp pattern
_header_pattern = re.compile("#include +<(dolfin/\w+/\w+\.h)>")

def extract_module_header_files(submodule, excludes):
    """
    Extract header files for a submodule
    """

    # Read dolfin submodule include file
    code = open(os.path.join(os.path.pardir, submodule, \
                             "dolfin_%s.h" % submodule)).read()

    # Extract all headers
    all_headers = re.findall(_header_pattern, code)

    # Filter with excludes
    return [header for header in all_headers \
            if header.split("/")[-1] not in excludes]
    
def generate_submodule_info(module_to_submodules, excludes):
    """
    Check the passed module to submodule mapping and sort it
    Creates the reverse mapping together with additional info
    about each submodule
    """

    # Extract original modules from dolfin.h
    # NOTE: We need these, in particular the order
    original_submodules = []
    f = open(os.path.join(os.path.pardir, "dolfin.h"))
    for line in f:
        if "#include <dolfin/" in line and line[:17] == "#include <dolfin/":
            module = line.split("/")[1]
            original_submodules += [module]
    f.close()

    # Check that the directory structure of the combined modules
    # corresponds to the above dict
    module_dirs = []
    for module_dir in glob.glob("modules/*"):
        module_dirs.append(module_dir.split(os.path.sep)[-1])
    
    # Some sanity checks
    for module_dir in module_dirs:
        if module_dir not in module_to_submodules:
            raise RuntimeError("Found a subdirectory: '%s' under the 'modules' "\
                               "directory, which is not listed as a combined "\
                               "module." % module_dir)
    
    for combined_module, modules in module_to_submodules.items():
        if combined_module not in module_dirs:
            raise RuntimeError("Found a combined module: '%s' which is not a "\
                               "subdirectory under the 'modules' directory." % \
                               combined_module)
        for module in modules:
            if module not in original_submodules:
                raise RuntimeError("Found a module: '%s' listed in the '%s' "\
                                   "combined module, which is not part of the "\
                                   "original DOLFIN modules." % \
                                   (module, combined_module))
    
    # Create a map from original modules to the combined
    submodule_info = OrderedDict()
    not_included = []
    
    for submodule in original_submodules:
        for module, submodules in module_to_submodules.items():
            if submodule in submodules:
                submodule_info[submodule] = dict(\
                    module=module,
                    has_pre=os.path.isfile(\
                        os.path.join(submodule, "pre.i")),
                    has_post=os.path.isfile(\
                        os.path.join(submodule, "post.i")),
                    headers=extract_module_header_files(submodule, excludes))
                break
        else:
            not_included.append(submodule)
    
    # Remove not used submodules
    for submodule in not_included:
        original_submodules.remove(submodule)
        
    # Sort the submodules according to order in original_submodules
    for module, submodules in module_to_submodules.items():
        ordered_submodules = []
        for submodule in original_submodules:
            if submodule in submodules:
                ordered_submodules.append(submodule)
        module_to_submodules[module] = ordered_submodules

    # Return ordered submodule info
    return submodule_info
    
def extract_swig_modules_dependencies(module_to_submodules, submodule_info):
    """
    Extracts the file dependencies of the SWIG modules
    """

    # OrderedDict of all external dependencies for each SWIG module
    module_info = OrderedDict((module, dict(submodules=submodules,\
                                            declared_types=[],\
                                            used_types=set(),\
                                            dependencies={}
                                            ))\
                for module, submodules in module_to_submodules.items())

    # dict of where each dolfin type is declared and in what module and
    # submodule it is included in
    dolfin_type_def = OrderedDict()

    # Add UFC Function
    # FIXME: ufc inheritance is not used for now. The ufc module information
    # FIXME: is globally imported in shared_ptr_classes.i
    dolfin_type_def["ufc::function"] = dict(\
        module="",
        submodule="",
        header="ufc.h",
        bases=set(), derived=set())

    # dict mapping submodules to included files
    submodule_files = {}

    # Derived classes (Used if a found base class is not registered
    # when first collected)
    derived_classes = {}

    # Iterate over all SWIG modules 
    for module, submodules in module_to_submodules.items():

        print "Parsing headers for SWIG module:", module
        # Iterate over all submodules in each SWIG module
        for submodule in submodules:

            # Iterate over header files and collect info
            for header_file in submodule_info[submodule]["headers"]:

                # Read code
                code = open("../../%s" % header_file).read()

                try:
                    # Extract type info
                    used_types, declared_types = parse_and_extract_type_info(code)
                except Exception, e:
                    print
                    print "###################"
                    print "ERROR while parsing:", header_file
                    print "###################"
                    print e
                    print
                    continue

                # Store type info
                for dolfin_type, bases in declared_types.items():
                    
                    # Store type information
                    dolfin_type_def[dolfin_type] = dict(\
                        module=module,
                        submodule=submodule,
                        header=header_file,
                        bases=bases, derived=set())

                    # Register the class
                    module_info[module]["declared_types"].append(dolfin_type)
                    
                    # Store derived classes
                    for base in bases:
                        # Check if base class has been regisred
                        # (should be as the types are traversed in order of
                        #  declaration )
                        dolfin_type_info = dolfin_type_def.get(base)
                        if dolfin_type_info is None:
                            if base not in derived_classes:
                                derived_classes[base] = set()
                            derived_classes[base].add(dolfin_type)
                            
                        else:
                            # Add derived class
                            dolfin_type_info["derived"].add(dolfin_type)

                # Add collected types to module
                module_info[module]["used_types"].update(\
                    used_types)

    # If for one reason or the other a base class was detected which was not
    # registered we add derived information here
    for dolfin_type_name, derived in derived_classes.items():
        dolfin_type = dolfin_type_def.get(dolfin_type_name)
        if dolfin_type is not None:
            dolfin_type["derived"].update(derived)

    # Help functions to recursevily add base and derived information to types
    def add_bases(bases, commulative_bases):
        for base in bases:
            # Check if bas is a dolfin type
            if base in dolfin_type_def:
                commulative_bases.add(base)
                add_bases(dolfin_type_def[base]["bases"], commulative_bases)

    def add_derived(derived_set, commulative_derived):
        for derived in derived_set:
            # All derived should be a dolfin type...
            if derived in dolfin_type_def:
                commulative_derived.add(derived)
                add_derived(dolfin_type_def[derived]["derived"], commulative_derived)

    # Build class hierarchy (We need to import all base and derived classes)
    # First extract all bases to a type
    for dolfin_type in dolfin_type_def:

        # Recursively add base and derived classes
        if dolfin_type_def[dolfin_type]["bases"]:

            new_bases = set()
            add_bases(dolfin_type_def[dolfin_type]["bases"], new_bases)
            dolfin_type_def[dolfin_type]["bases"] = list(new_bases)

        if dolfin_type_def[dolfin_type]["derived"]:
            new_derived = set()
            add_derived(dolfin_type_def[dolfin_type]["derived"], new_derived)
            dolfin_type_def[dolfin_type]["derived"] = list(new_derived)
            
    # Collect used dolfin types in each module
    used_dolfin_types = dict((module, set()) for module in module_info)

    # Filter out dolfin types and add derived and bases for each type
    for dolfin_type in dolfin_type_def:

        # Turn all set data into lists
        if isinstance(dolfin_type_def[dolfin_type]["bases"], set):
            dolfin_type_def[dolfin_type]["bases"] = \
                        list(dolfin_type_def[dolfin_type]["bases"])
        if isinstance(dolfin_type_def[dolfin_type]["derived"], set):
            dolfin_type_def[dolfin_type]["derived"] = \
                        list(dolfin_type_def[dolfin_type]["derived"])
        
        for module in module_info:
            for used_type in module_info[module]["used_types"]:
                if dolfin_type in used_type:
                    used_dolfin_types[module].add(dolfin_type)

                    # Add bases and derived types
                    used_dolfin_types[module].update(\
                        dolfin_type_def[dolfin_type]["bases"])

                    break

    # Over write old used type
    for module in module_info:
        # Update dependencies
        module_info[module]["used_types"] = used_dolfin_types[module]

    # Check external module dependencies
    for present_module in module_info:
        for dependent_module in module_info:
            
            # If same module no external dependencies
            if present_module == dependent_module:
                continue
    
            # Iterate over all dolfin types in dependent modules and check it they
            # are present in the present module
            for dolfin_type in module_info[dependent_module]["declared_types"]:
                
                # Check for dependency 
                if dolfin_type in module_info[present_module]["used_types"]:

                    # Register the dependency
                    dependencies = module_info[present_module]["dependencies"]
                    submodule = dolfin_type_def[dolfin_type]["submodule"]
                    if submodule not in dependencies:
                        dependencies[submodule] = set()
                    dependencies[submodule].add(\
                        dolfin_type_def[dolfin_type]["header"])

        # Need special treatment for template definitions in function/pre.i
        if "function" in module_info[present_module]["dependencies"]:
            for dolfin_type in ["FunctionSpace", "Function"]:
                module_info[present_module]["dependencies"]["function"].add(\
                    dolfin_type_def[dolfin_type]["header"])
                
        # Over write old submodules dependencies with sorted version
        module_info[present_module]["dependencies"] = \
            sort_submodule_dependencies(\
            module_info[present_module]["dependencies"], submodule_info)
        
    # Return data structures
    return module_info, dolfin_type_def

def write_module_interface_file(module, dependencies, submodule_info):
    """
    Write the main interface file for SWIG module
    """

    # Generate a form for code template
    module_form = dict(
        module=module,
        MODULE=module.upper(),
        )

    # Create import and include lines for each dependent file
    import_lines, headers_includes, file_dependencies = \
                  build_swig_import_info(dependencies, submodule_info)

    # Filter file dependencies
    file_dependencies = [f.replace("dolfin", "../../..") \
                         for f in file_dependencies]
    
    include_lines = []
    docstring_lines = []
    
    headers_includes.append("")
    headers_includes.append("// Include types from present module %s" % module)

    for submodule, submod_info in submodule_info.items():

        if submod_info["module"] != module:
            continue

        headers_includes.append("")
        headers_includes.append("// #include types from %s submodule" % submodule)

        include_lines.append("")
        include_lines.append("// %%include types from submodule %s" % submodule)
        
        # Add docstrings
        docstring_lines.append("%%include \"dolfin/swig/%s/docstrings.i\"" % submodule)

        # Check for pre includes
        if submod_info["has_pre"]:
            include_lines.append(\
                "%%include \"dolfin/swig/%s/pre.i\"" % submodule)
            file_dependencies.append("../../%s/pre.i" % submodule)

        # Add headers
        headers_includes.extend("#include \"%s\"" % header \
                                for header in submod_info["headers"])
        include_lines.extend("%%include \"%s\"" % header \
                             for header in submod_info["headers"])
        file_dependencies.extend(header.replace("dolfin", "../../..")\
                                 for header in submod_info["headers"])


        # Check for post includes
        if submod_info["has_post"]:
            include_lines.append(\
                "%%include \"dolfin/swig/%s/post.i\"" % submodule)
            file_dependencies.append("../../%s/post.i" % submodule)

    # Add imports and includes to form
    module_form["imports"] =    "\n".join(import_lines)
    module_form["includes"] =   "\n".join(include_lines)
    module_form["docstrings"] = "\n".join(docstring_lines)
    module_form["headers"] =    "\n".join(headers_includes)
    
    # Open file
    module_file = open(os.path.join("modules", module, "module.i"), "w")
    module_file.write(copyright_statement%(copyright_form_swig))

    # Write the generated code
    module_file.write(module_template % module_form)

    # Write swig interface file dependencies

    # Typemap files
    file_dependencies.extend("../../typemaps/" + \
                             interface_file.split(os.path.sep)[-1] \
                             for interface_file in \
                             glob.glob(os.path.join("typemaps","*.i")))

    file_dependencies.extend(["../../shared_ptr_classes.i",
                              "../../exceptions.i", "../../version.i",
                              "../../forwarddeclarations.i"])
                 
    dependency_file = open(os.path.join("modules", module,
                                        "dependencies.txt"), "w")
    dependency_file.write(";".join(sorted(file_dependencies)))

def generate_runtime_config_file(dolfin_type_def, module_to_submodules,
                                 submodule_info):
    # Extract all shared_ptr stored classes and store them in a pyton module
    # and place that under dolfin.compilemodeuls.sharedptrclasses.py
    shared_ptr_classes = re.findall("%shared_ptr\(dolfin::(.+)\)", \
                                    open("shared_ptr_classes.i").read())

    def repr_ordered_dict(data):
        """
        Make a readable repr version of an OrderedDict
        """
        return "OrderedDict([\\\n%s])" % "\n".join(\
            "('%s', %s)," % (key, item) for key, item in data.items())
    
    #shared_ptr_classes = filter(lambda x: "NAME" not in x, shared_ptr_classes)
    runtime_file = '''"""
This module contains compiletime information about the dolfin python
library, which can be utelized at runtime.

The file is automatically generated by the generateswigcode.py script
in the dolfin/swig directory."""
    
try:
    from collections import OrderedDict
except ImportError:
    from dolfin_utils.ordereddict import OrderedDict

# A list of shared_ptr declared classes in dolfin
shared_ptr_classes = %s

# An OrderedDict of all dolfin declared and its meta info
dolfin_type_def = %s

# A map between modules and its submodules
module_to_submodules = %s

# A reverse map between submodules and modules
submodule_info = %s
''' % (repr(shared_ptr_classes), repr_ordered_dict(dolfin_type_def),\
       repr_ordered_dict(module_to_submodules), repr_ordered_dict(submodule_info))
    
    par = os.path.pardir
    open(os.path.join(par, par, "site-packages", "dolfin", \
                      "compilemodules", "swigimportinfo.py"), "w").write(\
        runtime_file)


def regenerate_swig_interface(module_to_submodules, excludes):
    """
    Regenerate the whole swig interface
    """
    
    # Check the submodule order and create a submodule to module mapping
    submodule_info = generate_submodule_info(module_to_submodules, excludes)    

    # Create dolfin type info and depdency structures to be used when
    # generating swig interface files
    module_info, dolfin_type_def = extract_swig_modules_dependencies(\
        module_to_submodules, submodule_info)

    # Iterate over the modules and create a swig interface file
    for module in module_to_submodules:
        write_module_interface_file(module, module_info[module]["dependencies"], \
                                    submodule_info)

    # Create a python module with generated type info to be used runtime
    generate_runtime_config_file(dolfin_type_def, module_to_submodules,
                                 submodule_info)
    

if __name__ == "__main__":
    # Generate all docstrings
    # FIXME: Might be integratable into write_module_code?
    generate_docstrings()

    # User defined definition of SWIG modules.
    # NOTE: Order of modules are important, but not order of submodules
    module_to_submodules = OrderedDict([\
        ("common", ["common", "parameter", "log"]),
        ("la", ["la", "nls"]),
        ("mesh", ["mesh", "intersection", "refinement", \
                  "ale", "graph", "generation"]),
        ("function", ["function", "math"]),
        ("fem", ["fem", "quadrature", "adaptivity"]),
        ("io", ["io", "plot"])])

    # User defined list of headers to exclude (add more here)
    excludes = ["LogStream.h", "IntersectionOperatorImplementation.h" ]

    # Regnerate SWIG interface
    regenerate_swig_interface(module_to_submodules, excludes)
    
