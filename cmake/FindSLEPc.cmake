# - Try to find SLEPC
# Once done this will define
#
#  SLEPC_FOUND        - system has SLEPc
#  SLEPC_INCLUDE_DIR  - include directories for SLEPc
#  SLEPC_LIBARIES     - libraries for SLEPc
#  SLEPC_ROOT_DIR     - the SLEPc root directory
#
# Assumes that PETSC_ARCH has been set by alredy calling find_package(PETSc)

# Set debian_arches (PETSC_ARCH for Debian-style installations)
foreach (debian_arches linux kfreebsd)
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(DEBIAN_FLAVORS ${debian_arches}-gnu-c-debug ${debian_arches}-gnu-c-opt ${DEBIAN_FLAVORS})
  else()
    set(DEBIAN_FLAVORS ${debian_arches}-gnu-c-opt ${debian_arches}-gnu-c-debug ${DEBIAN_FLAVORS})
  endif()
endforeach()

# List of possible locations for SLEPC_DIR
set(slepc_dir_locations "")
list(APPEND slepc_dir_locations "/usr/lib/slepcdir/3.1")
list(APPEND slepc_dir_locations "/usr/lib/slepcdir/3.0.0")
list(APPEND slepc_dir_locations "$ENV{HOME}/petsc")

# FIXME: Use CMake list of library paths (and not LD_LIBRARY_PATH),
#        maybe ${CMAKE_SYSTEM_LIBRARY_PATH}?
# Add prefixes in LD_LIBRARY_PATH to possible locations
string(REGEX REPLACE ":" ";" libdirs $ENV{LD_LIBRARY_PATH})
foreach (libdir ${libdirs})
  get_filename_component(slepc_dir_location "${libdir}/" PATH)
  list(APPEND slepc_dir_locations ${slepc_dir_location})
endforeach()

# Try to figure out SLEPC_DIR by finding slepc.h
find_path(SLEPC_DIR include/slepc.h
  PATHS ENV SLEPC_DIR ${slepc_dir_locations}
  DOC "SLEPc directory")

# Report result of search for SLEPC_DIR
if (DEFINED SLEPC_DIR)
  set(SLEPC_ROOT_DIR ${SLEPC_DIR})
  message(STATUS "SLEPC_DIR is ${SLEPC_DIR}")
else()
  message(STATUS "SLEPC_DIR is empty")
endif()

if (SLEPC_DIR AND PETSC_ARCH)

  # Create a temporary Makefile to probe the SLEPcc configuration
  set(slepc_config_makefile ${PROJECT_BINARY_DIR}/Makefile.slepc)
  file(WRITE ${slepc_config_makefile}
"# This file was autogenerated by FindSLEPc.cmake
SLEPC_DIR  = ${SLEPC_DIR}
PETSC_ARCH = ${PETSC_ARCH}
include ${SLEPC_DIR}/conf/slepc_common
show :
	-@echo -n \${\${VARIABLE}}
")

  # Define macro for getting SLEPc variables from Makefile
  macro(SLEPC_GET_VARIABLE var name)
    set(${var} "NOTFOUND" CACHE INTERNAL "Cleared" FORCE)
    execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} -f ${slepc_config_makefile} show VARIABLE=${name}
      OUTPUT_VARIABLE ${var}
      RESULT_VARIABLE slepc_return)
  endmacro()

  # Call macro to get the SLEPc variables
  slepc_get_variable(SLEPC_INCLUDE_DIRS   CC_INCLUDES)
  slepc_get_variable(SLEPC_LIBRARIES      SLEPC_LIB)

  # Remove temporary Makefile
  file(REMOVE ${slepc_config_makefile})

  # Turn SLEPC_INCLUDE_DIRS into a semi-colon separated list
  string(REPLACE "-I" "" SLEPC_INCLUDE_DIRS "${SLEPC_INCLUDE_DIRS}")
  separate_arguments(SLEPC_INCLUDE_DIRS)

  # Set flags for building test program
  set(CMAKE_REQUIRED_INCLUDES ${SLEPC_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_LIBRARIES ${SLEPC_LIBRARIES})

  # Run SLEPc test program
  include(CheckCXXSourceRuns)
  check_cxx_source_runs("
#include \"petsc.h\"
#include \"slepc.h\"
int main()
{
  PetscErrorCode ierr;
  int argc = 0;
  char** argv = NULL;
  ierr = SlepcInitialize(&argc, &argv, PETSC_NULL, PETSC_NULL);
  EPS eps;
  ierr = EPSCreate(PETSC_COMM_SELF, &eps); CHKERRQ(ierr);
  ierr = EPSSetFromOptions(eps); CHKERRQ(ierr);
  ierr = EPSDestroy(eps); CHKERRQ(ierr);
  ierr = SlepcFinalize(); CHKERRQ(ierr);
  return 0;
}
" SLEPC_TEST_RUNS)

  if (SLEPC_TEST_RUNS)
    message(STATUS "SLEPc test runs")
  else()
    message(STATUS "SLEPc test failed")
  endif()

endif()

# Standard package handling
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SLEPc
  "SLEPc could not be found. Be sure to set SLEPC_DIR and PETSC_ARCH."
  SLEPC_ROOT_DIR SLEPC_INCLUDE_DIRS SLEPC_LIBRARIES)
