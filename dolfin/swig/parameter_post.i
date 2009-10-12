/* -*- C -*- */
// Copyright (C) 2006-2009 Johan Hake
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-05-12
// Last changed: 2009-10-12
//
// ===========================================================================
// SWIG directives for the DOLFIN parameter kernel module (post)
//
// The directives in this file are applied _after_ the header files of the
// modules has been loaded.
// ===========================================================================

// ---------------------------------------------------------------------------
// Modifications of Parameter interface
// ---------------------------------------------------------------------------
%extend dolfin::Parameter
{
%pythoncode%{
def value(self):
    val_type = self.type_str()
    if val_type == "string":
        return str(self)
    elif  val_type == "int":
        return int(self)
    elif val_type == "bool":
        return bool(self)
    elif val_type == "real":
        if has_gmp():
            # FIXME: Is it possible to convert real to some high-precision Python type?
            print "Warning: Converting real-valued parameter to double, might loose precision."
        return float(self)
    else:
        raise TypeError, "unknown value type '%s' of parameter '%s'"%(val_type,key)

def get_range(self):
    val_type = self.type_str()
    if val_type == "string":
        local_range = self._get_string_range()
        if len(local_range) == 0:
            return
        return local_range
    elif  val_type == "int":
        local_range = self._get_int_range()
        if local_range[0] == 0 and local_range[0] == local_range[0]:
            return
        return local_range
    elif val_type == "bool":
        return 
    elif val_type == "real":
        if has_gmp():
            # FIXME: Is it possible to convert real to some high-precision Python type?
            print "Warning: Converting real-valued parameter to double, might loose precision."
        local_range = self._get_real_range()
        if local_range[0] == 0 and local_range[0] == local_range[0]:
            return
        return local_range
    else:
        raise TypeError, "unknown value type '%s' of parameter '%s'"%(val_type,key)

def data(self):
    return self.value(), self.get_range(), self.access_count(), self.change_count()
%}

}

// ---------------------------------------------------------------------------
// Modifications of Parameters interface
// ---------------------------------------------------------------------------
%extend dolfin::Parameters
{
  void _parse(PyObject *op)
  {
    if (PyList_Check(op))
    {
      int i;
      int argc = PyList_Size(op);
      char **argv = (char **) malloc((argc+1)*sizeof(char *));
      for (i = 0; i < argc; i++)
      {
        PyObject *o = PyList_GetItem(op,i);
        if (PyString_Check(o))
          argv[i] = PyString_AsString(o);
        else
        {
          free(argv);
          throw std::runtime_error("list must contain strings");
        }
      }
      argv[i] = 0;
      self->parse(argc, argv);
      free(argv);
    }
    else
     throw std::runtime_error("not a list");
  }

%pythoncode%{

def add(self,*args):
    """ Add a parameter to the parameter set"""
    if len(args) == 2 and isinstance(args[1],bool):
        self._add_bool(*args)
    else:
        self._add(*args)

def parse(self,argv=None):
    "Parse command line arguments"
    if argv is None:
        import sys
        argv = sys.argv
    self._parse(argv)

def keys(self):
    "Returns a list of the parameter keys"
    ret = self._get_parameter_keys()
    ret += self._get_parameter_set_keys()
    return ret

def iterkeys(self):
    "Returns an iterator for the parameter keys"
    for key in self.keys():
        yield key

def values(self):
    "Returns a list of the parameter values"
    return [self[key] for key in self.keys()]

def itervalues(self):
    "Returns an iterator to the parameter values"
    return (self[key] for key in self.keys())

def items(self):
    return zip(self.keys(),self.values())

def iteritems(self):
    "Returns an iterator over the (key, value) items of the Parameters"
    for key, value in self.items():
        yield key, value

def set_range(self, key, *arg):
    "Set the range for the given parameter"
    if key not in self._get_parameter_keys():
        raise KeyError, "no parameter with name '%s'"%key
    self._get_parameter(key).set_range(*arg)

def __getitem__(self, key):
    "Return the parameter corresponding to the given key"
    if key in self._get_parameter_keys():
        return self._get_parameter(key).value()
        
    if key in self._get_parameter_set_keys():
        return self._get_parameter_set(key)

    raise KeyError, "'%s'"%key

def __setitem__(self, key, value):
    "Set the parameter 'key', with given 'value'"
    if key not in self._get_parameter_keys():
        raise KeyError, "'%s' is not a parameter"%key
    if not isinstance(value,(int,str,float,bool)):
        raise TypeError, "can only set 'int', 'bool', 'float' and 'str' parameters"
    par = self._get_parameter(key)
    if isinstance(value,bool):
        par._assign_bool(value)
    else:
        par._assign(value)

def update(self, other):
    "A recursive update that handles parameter subsets correctly."
    if not isinstance(other,(type(self),dict)):
        raise TypeError, "expected a 'dict' or a '%s'"%type(self).__name__
    for key, other_value in other.iteritems():
        self_value  = self[key]
        if isinstance(self_value, type(self)):
            self_value.update(other_value)
        else:
            setattr(self, key, other_value)

def to_dict(self):
    """Convert the Parameters to a dict"""
    ret = {}
    for key, value in self.iteritems():
        if isinstance(value, type(self)):
            ret[key] = value.to_dict()
        else:
            ret[key] = value
    return ret

def copy(self):
    "Return a copy of it self"
    return type(self)(self)

def option_string(self):
    "Return an option string representation of the Parameters"
    def option_list(parent,basename):
        ret_list = []
        for key, value in parent.iteritems():
            if isinstance(value, type(parent)):
                ret_list.extend(option_list(value,basename + key + '.'))
            else:
                ret_list.append(basename + key + " " + str(value))
        return ret_list

    return " ".join(option_list(self,"--"))

def __str__(self):
    "p.__str__() <==> str(x)"
    return self.str(False)

__getattr__ = __getitem__
__setattr__ = __setitem__

def iterdata(self):
    """Returns an iterator of a tuple of a parameter key together with its value"""
    for key in self.iterkeys():
        yield key, self.get(key)

def get(self, key):
    """Return all data available for a certain parameter

    The data is returned in a tuple:
    value, range, access_count, change_count = parameters.get('name')
    """
    if key in self._get_parameter_keys():
        return self._get_parameter(key).data()
        
    if key in self._get_parameter_set_keys():
        return self._get_parameter_set(key)

    raise KeyError, "'%s'"%key
    
%}

}

%pythoncode%{
old_init = Parameters.__init__
def __new_Parameter_init__(self,*args,**kwargs):
    """ Initialize Parameters

    Usage:
    Parameters("parameters")
       returns an empty Parameters

    Parameters(other_parameters)
       returns a copy of the other_parameters

    Parameters("parameters",dim=3,tol=0.1,name='Name')
       returns a parameters with the given values

    Parameters("parameters",dim=(3,0,4),name=("Name",["Name","Blame"])
      returns a parameters with the given values and ranges
    """
    if len(args) == 1 and isinstance(args[0],(str,type(self))):
        old_init(self,args[0])
    else:
        raise TypeError, "expected a single optional argument of type 'str' or ''"%type(self).__name__
    if len(kwargs) == 0:
        return

    for key, value in kwargs.iteritems():
        if isinstance(value,type(self)):
            self.add(value)
        elif isinstance(value,tuple):
            if len(value) > 0 and ((isinstance(value[0],str) and len(value) == 2) or \
                                   (isinstance(value[0],(int,float)) and len(value) == 3)):
                if isinstance(value[0],(float,int)) or \
                    (isinstance(value[0],str) and isinstance(value[1],list)):
                    self.add(key,*value)
                else:
                    raise TypeError, "expected a list as second item of tuple, when first is a 'str'"
            else:
                raise TypeError,"expected a range tuple of size 2 for 'str' values and 3 for scalars"
        else:
            self.add(key,value)

Parameters.__init__ = __new_Parameter_init__

%}

// Expose the global variable parameters for the Python interface
%inline %{
extern dolfin::GlobalParameters dolfin::parameters;
%}

%pythoncode%{
parameters = _cpp.cvar.parameters
%}
