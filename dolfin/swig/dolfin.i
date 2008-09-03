%module(directors="1") dolfin

%feature("autodoc", "1");

%{
#define protected public

#include <dolfin/dolfin.h>
#include <numpy/arrayobject.h>
using namespace dolfin;
%}


%init%{
  import_array();
%}

// Renames
%include "renames.i"

// Ignores
%include "ignores.i"

// Typemaps
%include "typemaps.i"

// Directors
%include "directors.i"

// Exceptions
%include "dolfin_exceptions.i"

// FIXME: what are these doing?
%include "cpointer.i"
%include "std_sstream.i"
%include "std_string.i"
%include "std_vector.i"
//%include "std_map.i" // FIXME: Make this work
%include "stl.i"
%include "carrays.i"
%array_functions(dolfin::real, realArray);
%array_functions(int, intArray);
%pointer_class(int, intp);
%pointer_class(double, doublep);

// Fixes for specific kernel modules (pre)
%include "dolfin_la_pre.i"
%include "dolfin_mesh_pre.i"
%include "dolfin_fem_pre.i"
%include "dolfin_function_pre.i"
%include "dolfin_pde_pre.i"

// DOLFIN interface
%import "dolfin/common/types.h"
%include "dolfin_headers.i"

// Fixes for specific kernel modules (post)
%include "dolfin_la_post.i"
%include "dolfin_mesh_post.i"
%include "dolfin_log_post.i"
%include "dolfin_common_post.i"
%include "dolfin_function_post.i"

//%typedef         std::map<dolfin::uint, dolfin::uint> iimap; //FIXME: Make this work
//%template(iimap) std::map<dolfin::uint, dolfin::uint>; //FIXME: Make this work
