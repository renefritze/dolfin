# -*- coding: utf-8 -*-

import os, os.path, sys
import warnings

# Make sure that we have a good scons-version
EnsureSConsVersion(0, 98, 5)

# Import the local 'scons'
sys.path.insert(0, os.path.abspath(os.path.join("scons", "simula-scons")))
import simula_scons as scons
 
# Import local exceptions
from simula_scons.Errors import PkgconfigError, PkgconfigMissing

# Create a SCons Environment based on the main os environment
env = scons.ExtendedEnvironment(ENV=os.environ)

# Set a projectname. Used in some places, like pkg-config generator
env["projectname"] = "dolfin"

# Set version
env["PACKAGE_VERSION"] = "0.9.2"

scons.setDefaultEnv(env)

# Specify a file where SCons store file signatures, used to figure out what is
# changed. We store it in the 'scons' subdirectory to avoid mixing signatures
# with the files in this project. 
project_sconsignfile = os.path.abspath(os.path.join("scons", ".sconsign"))
env.SConsignFile(project_sconsignfile)

# -----------------------------------------------------------------------------
# Command line option handling
# -----------------------------------------------------------------------------

DefaultPackages = ""

if env["PLATFORM"].startswith("win"):
  default_prefix = r"c:\local"
else:
  default_prefix = os.path.join(os.path.sep,"usr","local")

# Build the commandline options for SCons:
options = [
    # configurable options for installation:
    scons.PathOption("prefix", "Installation prefix", default_prefix),
    scons.PathOption("DESTDIR",
                     "Prepend DESTDIR to each installed target file", None),
    scons.PathOption("binDir", "Binary installation directory",
                     os.path.join("$prefix","bin")),
    scons.PathOption("manDir", "Manual page installation directory",
                     os.path.join("$prefix", "share", "man")),
    scons.PathOption("libDir", "Library installation directory",
                     os.path.join("$prefix","lib")),
    scons.PathOption("pkgConfDir", "Directory for installation of pkg-config files",
                     os.path.join("$prefix","lib","pkgconfig")),
    scons.PathOption("includeDir", "C/C++ header installation directory",
                     os.path.join("$prefix","include")),
    scons.PathOption("pythonModuleDir", "Python module installation directory", 
                     scons.defaultPythonLib(prefix="$prefix")),
    scons.PathOption("pythonExtDir", "Python extension module installation directory", 
                     scons.defaultPythonLib(prefix="$prefix", plat_specific=True)),
    # configurable options for how we want to build:
    BoolVariable("enableDebug", "Build with debug information", 1),
    BoolVariable("enableDebugUblas", "Add some extra Ublas debug information", 0),
    BoolVariable("enableOptimize", "Compile with optimization", 0),
    BoolVariable("enableDocs", "Build documentation", 0),
    BoolVariable("enableDemos", "Build demos", 0),
    BoolVariable("enableTests", "Build tests", 0),
    BoolVariable("enableCodeCoverage", "Enable code coverage", 0),
    BoolVariable("enableResolveCompiler", "Run tests to verify compiler", 1),
    # Enable or disable external packages.
    # These will also be listed in scons.cfg files, but if they are 
    # disabled here, that will override scons.cfg. Remark that unless the
    # module is listed as OptDependencies in scons.cfg, the whole module
    # will be turned off.
    BoolVariable("enableMpi", "Compile with support for MPI", "yes"),
    BoolVariable("enablePetsc", "Compile with support for PETSc linear algebra", "yes"),
    BoolVariable("enableSlepc", "Compile with support for SLEPc", "yes"),
    BoolVariable("enableScotch", "Compile with support for SCOTCH graph partitioning", "yes"),
    BoolVariable("enableGts", "Compile with support for GTS", "yes"),
    BoolVariable("enableUmfpack", "Compile with support for UMFPACK", "yes"),
    BoolVariable("enableTrilinos", "Compile with support for Trilinos", "yes"),
    BoolVariable("enableCholmod", "Compile with support for CHOLMOD", "yes"),
    BoolVariable("enableMtl4", "Compile with support for MTL4", "yes"),
    BoolVariable("enableParmetis", "Compile with support for ParMETIS", "yes"),
    BoolVariable("enableGmp", "Compile with support for GMP", "no"),
    BoolVariable("enablePython", "Compile the Python wrappers", "yes"),
    BoolVariable("enablePydolfin", "Compile the Python wrappers of DOLFIN *deprecated*", "yes"),
    # some of the above may need extra options (like petscDir), should we
    # try to get that from pkg-config?
    # It may be neccessary to specify the installation path to the above packages.
    # One can either use the options below (with<Package>Dir) or define the
    # <PACKAGE>_DIR environment variable.
    PathOption("withPetscDir", "Specify path to PETSc", None),
    PathOption("withSlepcDir", "Specify path to SLEPc", None),
    PathOption("withScotchDir", "Specify path to SCOTCH", None),
    PathOption("withUmfpackDir", "Specify path to UMFPACK", None),
    PathOption("withTrilinosDir", "Specify path to Trilinos", None),
    PathOption("withCholmodDir", "Specify path to CHOLMOD", None),
    PathOption("withMtl4Dir", "Specify path to MTL4", None),
    PathOption("withParmetisDir", "Specify path to ParMETIS", None),
    PathOption("withGmpDir", "Specify path to GMP", None),
    PathOption("withBoostDir", "Specify path to Boost", None),
    PathOption("withLibxml2Dir", "Specify path to libXML2", None),
    #
    # a few more options originally from PyCC:
    #BoolVariable("autoFetch", "Automatically fetch datafiles from (password protected) SSH repository", 0),
    BoolVariable("cacheOptions", "Cache command-line options for later invocations", 1),
    BoolVariable("veryClean", "Remove the sconsign file during clean, must be set during regular build", 0),
    # maybe we should do this more cleverly. The default in dolfin now is
    # to use mpicxx if that is available...:
    #("CXX", "Set C++ compiler", scons.defaultCxxCompiler()),
    #("FORTRAN", "Set FORTRAN compiler",scons.defaultFortranCompiler()),
    ("customCxxFlags", "Customize compilation of C++ code", ""),
    ("customLinkFlags", "Customize linking of C++ code", ""),
    #("data", "Parameter to the 'fetch' target: comma-delimited list of directories/files to fetch, \
    #        relative to the `data' directory. An empty value means that everything will be fetched.", ""),
    #("sshUser", "Specify the user for the SSH connection used to retrieve data files", ""),
    #('usePackages','Override or add dependency packages, separate with comma', ""),
    #('customDefaultPackages','Override the default set of packages (%r), separate package names with commas' % (DefaultPackages,)),
    ("SSLOG", "Set Simula scons log file", os.path.join(os.getcwd(),"scons","simula_scons.log")),
    ]


# Option enablePydolfin is now deprecated
if ARGUMENTS.has_key("enablePydolfin"):
  msg = "Option 'enablePydolfin' is deprecated and will be removed in " \
        "the future. Please use the option 'enablePython' instead to " \
        "enable/disable compiling of Python wrappers."
  warnings.warn(msg, DeprecationWarning)
  if not ARGUMENTS.has_key("enablePython"):
    ARGUMENTS["enablePython"] = ARGUMENTS["enablePydolfin"]
  del ARGUMENTS["enablePydolfin"]

# This Configure class handles both command-line options (which are merged into 
# the environment) and autoconf-style tests. A special feature is that it
# remembers configuration parameters (options and results from tests) between
# invocations so that they are re-used when cleaning after a previous build.
configure = scons.Configure(env, ARGUMENTS, options)

# Open log file for writing
scons.logOpen(env)
scons.log("=================== %s log ===================" % env["projectname"])
scons.logDate()

# Writing the simula_scons used to the log:
scons.log("Using simula_scons from: %s" % scons.__file__)

# Notify the user about that options from scons/options.cache are being used:
if not env.GetOption("clean"):
  try:
    optsCache = os.path.abspath(os.path.join("scons", "options.cache"))
    lines = file(optsCache).readlines()
    if lines:
      print "Using options from %s" % optsCache
    # FIXME: this can be removed when option enablePydolfin is removed
    new_lines = []
    for line in lines:
      if line.startswith("enablePydolfin"):
        if not "enablePython" in lines:
          env["enablePython"] = eval(line.split('=')[1])
          new_lines.append("enablePython = %s\n" % env["enablePython"])
      else:
        new_lines.append(line)
    if new_lines != lines:
      file(optsCache, 'w').writelines(new_lines)
  except IOError, msg:
    pass

# If we are in very-clean mode, remove the sconsign file. 
if env.GetOption("clean"):
  try:
    if env["veryClean"]:
      os.unlink("%s.dblite" % project_sconsignfile)
      os.unlink(os.path.join('scons', 'options.cache'))
      import glob
      for f in glob.glob(os.path.join('scons', 'pkgconfig', '*.pc')):
        os.unlink(f)
  except OSError, msg:
    scons.log("Error using 'veryClean' option:\n%s\n" % msg)

# Set default compiler and linker flags (defining CXXFLAGS/LINKFLAGS
# will override this)
env['CXXFLAGS'] = os.environ.get("CXXFLAGS", "-Wall -pipe -ansi -Werror")
env["LINKFLAGS"] = os.environ.get("LINKFLAGS", "")  # FIXME: "" OK as default?

# Default FORTRAN flags
#env["SHFORTRANFLAGS"] = "-Wall -pipe -fPIC"

# If Debug is enabled, add -g:
if env["enableDebug"]:
  env.Append(CXXFLAGS=" -DDEBUG -g")

if not env["enableDebugUblas"]:
  env.Append(CXXFLAGS=" -DNDEBUG")

# if Optimization is requested, use -O3
if env["enableOptimize"]:
  env.Append(CXXFLAGS=" -O3")
else:
  # FIXME: why are we optimizing when enableOptimize is False?
  env.Append(CXXFLAGS=" -O2")

# Not sure we need this - but lets leave it for completeness sake - if people
# use if for PyCC, and know that dolfin use the same system, they will expect
# it to be here. We should probably discuss whether that is a good argument or
# not. 
# Append whatever custom flags given
if env["customCxxFlags"]:
  env.Append(CXXFLAGS=" " + env["customCxxFlags"])

# Append custom linker flags
if env["customLinkFlags"]:
  env.Append(LINKFLAGS=" " + env["customLinkFlags"])

# Determine which compiler to be used:
cxx_compilers = ["c++", "g++", "CC"]
# Use CXX from os.environ if available:
cxx = os.environ.get("CXX", env.Detect(cxx_compilers))

# Set MPI compiler and add neccessary MPI flags if enableMpi is True:
if env["enableMpi"]:
  mpi_cxx_compilers = ["mpic++", "mpicxx", "mpiCC"]
  mpi_cxx = os.environ.get("CXX", env.Detect(mpi_cxx_compilers))
  # Several cases for mpi_cxx depending on CXX from os.environ:
  # CXX=                           - not OK
  # CXX=cxx_compiler               - not OK
  # CXX=/path/to/cxx_compiler      - not OK 
  # CXX=mpi_cxx_compiler           - OK
  # CXX=/path/to/mpi_cxx_compiler  - OK (use os.path.basename)
  # CXX="ccache cxx_compiler"      - OK (use mpi_cxx.split()[-1])
  # FIXME: Any other cases?
  if not env.Detect(["mpirun", "mpiexec", "orterun"]) or not \
         (mpi_cxx and \
          os.path.basename(mpi_cxx.split()[-1]) in mpi_cxx_compilers):
    print "MPI not found (might not work if PETSc uses MPI)."
    # revert back to cxx compiler
    env["CXX"] = cxx
  else:
    # Found MPI, so set HAS_MPI and IGNORE_CXX_SEEK (mpich2 bug)
    env.Append(CXXFLAGS=" -DHAS_MPI=1 -DMPICH_IGNORE_CXX_SEEK")
    env["CXX"] = mpi_cxx
else:
  env["CXX"] = cxx

if not env["CXX"]:
  print "Unable to find any valid C++ compiler."
  # try to use g++ as default:
  env["CXX"] = "g++"

# process list of packages to be included in allowed Dependencies.
# Do we need this any more? I think we rather pick up (external) packages from
# the scons.cfg files. Actually, I doubt usePackages is ever used?
#env["usePackages"] = scons.resolvePackages(env["usePackages"].split(','),\
#        env.get("customDefaultPackages", DefaultPackages).split(","))

# Figure out if we should fetch datafiles, and set an option in env. 
#doFetch = "fetch" in COMMAND_LINE_TARGETS or (env["autoFetch"]) and not env.GetOption("clean")
#env["doFetch"] = doFetch

# -----------------------------------------------------------------------------
# Call the main SConscript in the project directory
# -----------------------------------------------------------------------------
try:
  # Invoke the SConscript as if it was situated in the build directory, this
  # tells SCons to build beneath this
  buildDataHash = env.SConscript(os.path.join(env["projectname"], "SConscript"), exports=["env", "configure"])
except PkgconfigError, err:
  sys.stderr.write("%s\n" % err)
  Exit(1)

# -----------------------------------------------------------------------------
# Set up build targets
# -----------------------------------------------------------------------------

# default build-targets: shared libs, extension modules, programs, demos, and
# documentation.
for n in buildDataHash["shlibs"] + buildDataHash["extModules"] + \
        buildDataHash["progs"] + buildDataHash["demos"], buildDataHash["docs"], buildDataHash["tests"]:
  env.Default(n)

# -----------------------------------------------------------------------------
# Set up installation targets
# -----------------------------------------------------------------------------

install_prefix = "$prefix"
if env.get("DESTDIR"):
  install_prefix = os.path.join("$DESTDIR", "$prefix")
binDir = env["binDir"].replace("$prefix", install_prefix)
includeDir = env["includeDir"].replace("$prefix", install_prefix)
libDir = env["libDir"].replace("$prefix", install_prefix)
pkgConfDir = env["pkgConfDir"].replace("$prefix", install_prefix)
pythonModuleDir = env["pythonModuleDir"].replace("$prefix", install_prefix)
pythonExtDir = env["pythonExtDir"].replace("$prefix", install_prefix)
manDir = env["manDir"].replace("$prefix", install_prefix)

# install dolfin-convert into binDir:
env.Install(binDir, os.path.join("misc","utils","convert","dolfin-convert"))

# install dolfin-convert manual page into manDir/man1:
env.Install(os.path.join(manDir, "man1"),
            os.path.join("doc", "man", "man1", "dolfin-convert.1.gz"))

# install dolfin-order into binDir
env.Install(binDir, os.path.join("misc","utils","order","dolfin-order"))

# install dolfin-order manual page into manDir/man1
env.Install(os.path.join(manDir, "man1"),
            os.path.join("doc", "man", "man1", "dolfin-order.1.gz"))

# shared libraries goes into our libDir:
for l in buildDataHash["shlibs"]:
  env.InstallVersionedSharedLibrary(libDir, l)

# install header files in the same structure as in the source tree, within
# includeDir/dolfin:
for h in buildDataHash["headers"]:
  # Get the module path relative to the "src" directory
  dpath = os.path.dirname(h.srcnode().path).split(os.path.sep, 1)[1]
  env.Install(os.path.join(includeDir, env["projectname"], dpath), h)
# Also, we want the special 'dolfin.h' file to be installed directly in the
# toplevel includeDir. 
if buildDataHash.has_key("dolfin_header") and buildDataHash["dolfin_header"] != "":
  env.Install(includeDir, buildDataHash["dolfin_header"])

## install python scripts in the bin directory
#for s in buildDataHash["pythonScripts"]:
#  env.Install(binDir, s)

if env["enablePython"]:
    # install python modules, usually in site-packages/<projectname>
    for m in buildDataHash["pythonModules"]:
        env.Install(os.path.join(pythonModuleDir, env["projectname"]), m)

if env["enablePython"]:
    # install extension modules, usually in site-packages
    for e in buildDataHash["extModules"]:
        env.Install(os.path.join(pythonExtDir, env["projectname"]), e)
    # install SWIG interface files in includeDir/swig
    for s in buildDataHash["swigfiles"]:
        env.Install(os.path.join(includeDir, env["projectname"], "swig"), s)

# install generated pkg-config files in $prefix/lib/pkgconfig or other
# specified place
for p in buildDataHash["pkgconfig"]:
  env.Install(pkgConfDir, p)

# grab installation prefix, substitute all scons construction variables
# (those prefixed with $...), and create a normalized path:
prefix = os.path.normpath(env.subst(install_prefix))
# add '/' (or similar) at the end of prefix if missing:
if not prefix[-1] == os.path.sep:
  prefix += os.path.sep

# not sure we need common.py for pydolfin.
#commonfile=os.path.join("site-packages", "pycc", "common.py")

#env = scons.installCommonFile(env, commonfile, prefix)

if env["enablePython"]:
    installfiles = scons.buildFileList(buildDataHash["pythonPackageDirs"])

    for f in installfiles:
        installpath=os.path.sep.join(os.path.dirname(f).split(os.path.sep)[1:])
        env.Install(os.path.join(pythonModuleDir,installpath), f)

_targetdir = os.path.join(prefix, "share", env["projectname"], "data")
if 'install' in COMMAND_LINE_TARGETS and not os.path.isdir(_targetdir):
  os.makedirs(_targetdir)
env = scons.addInstallTargets(env, sourcefiles=buildDataHash["data"],
                              targetdir=_targetdir)

## No tests to install for dolfin?
## Not sure tests should be installed, have to check that.
#_targetdir=os.path.join(prefix, "share", "pycc", "tests")
#if 'install' in COMMAND_LINE_TARGETS and not os.path.isdir(_targetdir):
#  os.makedirs(_targetdir)
#env = scons.addInstallTargets(env, sourcefiles=buildDataHash["tests"],
#                              targetdir=_targetdir)

_targetdir = os.path.join(prefix, "share", "doc", env["projectname"])
if 'install' in COMMAND_LINE_TARGETS and not os.path.isdir(_targetdir):
  os.makedirs(_targetdir)
env = scons.addInstallTargets(env, sourcefiles=buildDataHash["docs"],
                              targetdir=_targetdir)

# Instruct scons what to do when user requests 'install'
targets = [binDir, manDir, libDir, includeDir, pkgConfDir]
if env["enablePython"]:
    targets.append(pythonModuleDir)
    targets.append(pythonExtDir)
env.Alias("install", targets)

# _runTests used to use the global 'ret' (now buildDataHash). Therefore, we
# need to wrap _runTests in a closure, now that the functions is moved into
# 'scons'
_runTests = scons.gen_runTests(buildDataHash)

env.Command("runtests", buildDataHash["shlibs"] + buildDataHash["extModules"],
            Action(_runTests, scons._strRuntests))

# Create helper file for setting environment variables
scons.createHelperFile(env)

def help():
    from SCons.Script import GetBuildFailures
    build_failures = GetBuildFailures()
    if build_failures:
        for bf in build_failures:
            print "%s failed: %s" % (bf.node, bf.errstr)
        return
    msg = """---------------------------------------------------------
Compilation of DOLFIN finished. Now run

    scons install

to install DOLFIN on your system. Note that you may need
to be root in order to install. To specify an alternative
installation directory, run

    scons install prefix=<path>

You may also run ./scons.local for a local installation
in the DOLFIN source tree.

You can compile all the demo programs in the subdirectory
demo by running

   scons enableDemos=yes

---------------------------------------------------------"""
    print msg

def help_install():
    from SCons.Script import GetBuildFailures
    build_failures = GetBuildFailures()
    if build_failures:
        for bf in build_failures:
            print "%s failed: %s" % (bf.node, bf.errstr)
        return
    msg = """---------------------------------------------------------
DOLFIN successfully compiled and installed in\n\n  %s\n\n""" % prefix
    # Check that the installation directory is set up correctly
    if not os.path.join(env.subst(env["prefix"]),"bin") in os.environ["PATH"]:
        msg += """---------------------------------------------------------
Warning: Installation directory is not in PATH.

To compile a program against DOLFIN, you need to update
your environment variables to reflect the installation
directory you have chosen for DOLFIN. A simple way to do
this if you are using Bash-like shell is to source the
file dolfin.conf:

    source dolfin.conf

Windows users can simply run the file dolfin.bat:

    dolfin.bat

This will update the values for the environment variables
PATH, LD_LIBRARY_PATH, PKG_CONFIG_PATH and PYTHONPATH.
---------------------------------------------------------"""
    else:
        msg += "---------------------------------------------------------"
    print msg

# Print some help text at the end
if not env.GetOption("clean"):
    import atexit
    if 'install' in COMMAND_LINE_TARGETS:
        atexit.register(help_install)
    else:
        atexit.register(help)

# Close log file
scons.logClose()

# vim:ft=python ts=2 sw=2
