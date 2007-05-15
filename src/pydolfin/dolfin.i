%module(directors="1") dolfin

%feature("autodoc", "1");

%{
#include <dolfin.h>
#include <numpy/arrayobject.h>
using namespace dolfin;
%}

%init%{
  import_array();
%}

// Ignores
%include "ignores.i"

// Renames
%include "renames.i"

// Typemaps
%include "typemaps.i"

// Directors
%include "directors.i"

// Exceptions
%include "dolfin_exceptions.i"

// FIXME: what are these doing?
namespace dolfin { class ublas_vector {}; }
%include "cpointer.i"
%include "std_string.i"
%include "std_vector.i"
%include "stl.i"
%include "carrays.i"
%array_functions(dolfin::real, realArray);
%array_functions(int, intArray);
%pointer_class(int, intp);
%pointer_class(double, doublep);

// Fixes for specific kernel modules (pre)
%include "dolfin_la_pre.i"
%include "dolfin_mesh_pre.i"
%include "dolfin_log_pre.i"

// DOLFIN interface
%import "dolfin/constants.h"
%include "dolfin_headers.h"

// FIXME: Move to dolfin_common_post.i
// common post
%template(STLVectorFunctionPtr) std::vector<dolfin::Function *>;
%template(ArrayFunctionPtr) dolfin::Array<dolfin::Function *>;
%template(STLVectorUInt) std::vector<unsigned int>;
%template(ArrayUInt) dolfin::Array<unsigned int>;

// Fixes for specific kernel modules (post)
%include "dolfin_la_post.i"
%include "dolfin_mesh_post.i"
