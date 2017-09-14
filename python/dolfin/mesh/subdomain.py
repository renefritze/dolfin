import hashlib
import types
import dijitso
import ffc
import dolfin.cpp as cpp
from dolfin.jit.jit import compile_class, _math_header


def jit_generate(class_data, module_name, signature, parameters):

    template_code = """
// Based on https://gcc.gnu.org/wiki/Visibility
#if defined _WIN32 || defined __CYGWIN__
    #ifdef __GNUC__
        #define DLL_EXPORT __attribute__ ((dllexport))
    #else
        #define DLL_EXPORT __declspec(dllexport)
    #endif
#else
    #define DLL_EXPORT __attribute__ ((visibility ("default")))
#endif

#include <dolfin/common/Array.h>
#include <dolfin/math/basic.h>
#include <dolfin/mesh/SubDomain.h>
#include <Eigen/Dense>

{math_header}

namespace dolfin
{{
  class {classname} : public SubDomain
  {{
     public:
       {members}

       {classname}()
          {{
            {constructor}
          }}

       // Return true for points inside the sub domain
       bool inside(const Eigen::Ref<const Eigen::VectorXd> x, bool on_boundary) const final
       {{
         return {inside};
       }}

       void set_property(std::string name, double value)
       {{
{set_props}
       }}

       double get_property(std::string name) const
       {{
{get_props}
         return 0.0;
       }}

  }};
}}

extern "C" DLL_EXPORT dolfin::SubDomain * create_{classname}()
{{
  return new dolfin::{classname};
}}

"""
    _set_prop = """ if (name == "{name}") {name} = value;\n"""
    _get_prop = """ if (name == "{name}") return {name};\n"""

    inside_code = class_data['statements'][0]

    members = ""
    get_props = ""
    set_props = ""
    for k in class_data['properties']:
        members += " double " + k + ";\n"
        get_props += _get_prop.format(name = k)
        set_props += _set_prop.format(name = k)

    classname = signature
    code_c = template_code.format(inside=inside_code,
                                  classname=classname,
                                  math_header=_math_header,
                                  members=members, constructor="",
                                  get_props=get_props,
                                  set_props=set_props)
    code_h = ""
    depends = []

    return code_h, code_c, depends


def compile_subdomain(inside_code, properties):

    cpp_data = {'statements': inside_code, 'properties': properties,
                'name': 'subdomain', 'jit_generate': jit_generate}

    subdomain = compile_class(cpp_data)
    return subdomain


class CompiledSubDomain(cpp.mesh.SubDomain):
    def __new__(cls, inside_code, **kwargs):
        properties = kwargs
        return compile_subdomain(inside_code, properties)
