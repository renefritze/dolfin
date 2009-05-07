/* -*- C -*-  (not really, but good for syntax highlighting) */
// Type maps for PyDOLFIN

// Basic typemaps
%typemap(in)  dolfin::uint = int;
%typemap(out) dolfin::uint = int;

// Typemap for values (in Function)
%typemap(directorin) double* values {
  {
    // Compute size of value (number of entries in tensor value)
    dolfin::uint size = 1;
    for (dolfin::uint i = 0; i < this->function_space().element().value_rank(); i++)
      size *= this->function_space().element().value_dimension(i);

    npy_intp dims[1] = {size};
    $input = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, reinterpret_cast<char *>($1_name));
  }
}

// Typemap for coordinates (in Function and SubDomain)
%typemap(directorin) const double* x {
  {
    // Compute size of x
    npy_intp dims[1] = {this->geometric_dimension()};
    $input = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, reinterpret_cast<char *>(const_cast<double*>($1_name)));
  }
}

%typemap(directorin) double* y {
  {
    // Compute size of x
    npy_intp dims[1] = {this->geometric_dimension()};
    $input = PyArray_SimpleNewFromData(1, dims, NPY_DOUBLE, reinterpret_cast<char *>($1_name));
  }
}

//%apply const double* x { const double* y };

// Typemap check
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) double* {
    // General typemap
    $1 = PyArray_Check($input) ? 1 : 0;
}

// Typemap check
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) dolfin::uint* {
    // General typemap
    $1 = PyArray_Check($input) ? 1 : 0;
}

// Typemap check
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) dolfin::Array<double>& {
    // General typemap
    $1 = PySequence_Check($input) ? 1 : 0;
}

// Typemap check
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) dolfin::Array<dolfin::uint>& {
    // General typemap
    $1 = PySequence_Check($input) ? 1 : 0;
}

// Typemap for sending any sequence as input to functions expecting an Array of real
%typemap(in) const dolfin::Array<double>& (dolfin::Array<double> tmp) {
  int i;
  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"*** Error: Expected a sequence");
    return NULL;
  }
  int pyseq_length = PySequence_Size($input);
  if (!pyseq_length > 0){
    PyErr_SetString(PyExc_RuntimeError,"*** Error: Supply a sequence with length > 0");
    return NULL;
  }
  tmp.reserve(pyseq_length);
  for (i = 0; i < pyseq_length; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyNumber_Check(o)) {
      tmp.push_back(static_cast<double>(PyFloat_AsDouble(o)));
    } else {
      PyErr_SetString(PyExc_ValueError,"*** Error: Sequence elements must be numbers");
      return NULL;
    }

  }
  $1 = &tmp;
}

// Typemap for sending any sequence as input to functions expecting an Array of uint
%typemap(in) const dolfin::Array<dolfin::uint>& (dolfin::Array<dolfin::uint> tmp) {
  int i;
  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"*** Error: Expected a sequence");
    return NULL;
  }
  int pyseq_length = PySequence_Size($input);
  if (!pyseq_length > 0){
    PyErr_SetString(PyExc_RuntimeError,"*** Error: Supply a sequence with length > 0");
    return NULL;
  }
  tmp.reserve(pyseq_length);
  for (i = 0; i < pyseq_length; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyNumber_Check(o)) {
      tmp.push_back(static_cast<dolfin::uint>(PyInt_AsLong(o)));
    } else {
      PyErr_SetString(PyExc_ValueError,"*** Error: Sequence elements must be integers");
      return NULL;
    }
  }
  $1 = &tmp;
}

// Typemap for sending numpy arrays as input to functions expecting a C array of real
%typemap(in) double* {
    if PyArray_Check($input) {
        PyArrayObject *xa = reinterpret_cast<PyArrayObject*>($input);
        if ( PyArray_TYPE(xa) == NPY_DOUBLE )
            $1  = static_cast<double*>(PyArray_DATA(xa));
        else
            SWIG_exception(SWIG_ValueError, "numpy array of doubles expected");
    } else
        SWIG_exception(SWIG_ValueError, "numpy array expected");
}

// Typemap for sending numpy arrays as input to functions expecting a C array of uint
%typemap(in) dolfin::uint* {
    if PyArray_Check($input) {
        PyArrayObject *xa = (PyArrayObject*)($input);
        printf("*** Checking: xa->descr->type = %c\n", xa->descr->type);
        if (xa->descr->type == 'L')
            $1 = (dolfin::uint *)(*xa).data;
        else
            SWIG_exception(SWIG_ValueError, "numpy array of unsigned integers expected");
    } else
        SWIG_exception(SWIG_ValueError, "numpy array expected");
}

// Typecheck for Parameter typemaps
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) dolfin::Parameter {
    bool pass = 0;
    pass = PyString_Check($input) ? 1 : pass;
    pass = PyInt_Check($input)    ? 1 : pass;
    pass = PyFloat_Check($input)  ? 1 : pass;
    pass = PyBool_Check($input)  ? 1 : pass;
    $1 = pass;
}

// Typemap for Parameter (in)
%typemap(in) dolfin::Parameter {
    if PyString_Check($input) {
        std::string input = PyString_AsString($input);
        dolfin::Parameter tmp(input);
        $1 = tmp;
    }
    else if PyBool_Check($input) {
        int val = PyInt_AsLong($input);
        if (val == 1) {
            dolfin::Parameter tmp(true);
            $1 = tmp;
        }
        else {
            dolfin::Parameter tmp(false);
            $1 = tmp;
        }
    }
    else if PyInt_Check($input) {
        int val = PyInt_AsLong($input);
        dolfin::Parameter tmp(val);
        $1 = tmp;
    }
    else if PyFloat_Check($input) {
        dolfin::Parameter tmp(PyFloat_AsDouble($input));
        $1 = tmp;
    }


}

// Typemap for Parameter (out)
%typemap(out) dolfin::Parameter {
  {
    // Custom typemap
    // std::string tmp;

    switch ( $1.type() )
    {
    case Parameter::type_double:

      $result = SWIG_From_double(*&($1));
      break;

    case Parameter::type_int:

      $result = SWIG_From_int((int)*&($1));
      break;

    case Parameter::type_bool:

      $result = SWIG_From_bool(*&($1));
      break;

    case Parameter::type_string:

      //$result = SWIG_From_std_string(*&($1));
      //tmp = (std::string)*&($1);
      //$result = SWIG_FromCharPtrAndSize(tmp.c_str(), tmp.size());
      $result = SWIG_FromCharPtrAndSize(((std::string)*&($1)).c_str(), ((std::string)*&($1)).size());

      break;

    default:
      error("Unknown type for parameter.");
    }
  }
}

%include numpy_typemaps.i
