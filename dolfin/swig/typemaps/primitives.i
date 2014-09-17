/* -*- C -*- */
// Copyright (C) 2007-2009 Anders Logg
//
// This file is part of DOLFIN.
//
// DOLFIN is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// DOLFIN is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
//
// Modified by Ola Skavhaug, 2007-2009.
// Modified by Garth N. Wells, 2007.
// Modified by Johan Hake, 2008-2011.
//
// First added:  2006-04-16
// Last changed: 2013-05-28

//=============================================================================
// General typemaps for PyDOLFIN
//=============================================================================

//-----------------------------------------------------------------------------
// Fragment for bool (after SWIG 3.0)
// Allow Python bool, scalar numpy booleans and 1 sized boolean arrays
//-----------------------------------------------------------------------------
%fragment(SWIG_AsVal_frag(bool2),"header",
          fragment=SWIG_AsVal_frag(long)) {
SWIGINTERN int
SWIG_AsVal_dec(bool2)(PyObject *obj, bool *val)
{
  int r;
  if (!(PyBool_Check(obj)||PyArray_IsScalar(obj, Bool)||(PyArray_Check(obj)&&PyArray_SIZE(reinterpret_cast<PyArrayObject*>(obj))==1)&&PyArray_TYPE(reinterpret_cast<PyArrayObject*>(obj))==NPY_BOOL))
    return SWIG_ERROR;
  r = PyObject_IsTrue(obj);
  if (r == -1)
    return SWIG_ERROR;
  if (val) *val = r ? true : false;
  return SWIG_OK;
}
}

// Include limit header
%fragment("limits_header", "header")
{
  // Include the limits header
  %#include <limits>
}

// Make sure Python int from std::size_t can be constructed
// It looks like SWIG_From_size_t is available but not SWIG_From_std_size_t
%fragment("SWIG_From_std_size_t", "header", fragment=SWIG_From_frag(size_t),
          fragment="limits_header")
{
  SWIGINTERNINLINE PyObject * SWIG_From_std_size_t(std::size_t value)
  {
    return SWIG_From_unsigned_SS_long (static_cast< unsigned long >(value));
    return PyInt_FromSize_t(value);
  }
}

// Make sure Python int from dolfin::la_index can be constructed
%fragment("SWIG_From_dolfin_la_index", "header")
{
  SWIGINTERNINLINE PyObject * SWIG_From_dolfin_la_index(dolfin::la_index value)
  {
    return SWIG_From_unsigned_SS_long (static_cast< unsigned long >(value));
    return PyInt_FromSize_t(static_cast<std::size_t>(value));
  }
}

//-----------------------------------------------------------------------------
// A home brewed type check for checking integers
// Needed due to problems with PyInt_Check from python 2.6 and NumPy
//-----------------------------------------------------------------------------
%fragment("PyInteger_Check", "header")
{
// Homebrewed Integer enumerate
typedef enum {
  _NO_PYTHON_INTEGER_TYPE=-1,
  _INT_PYTHON_INTEGER_TYPE,
  _LONG_PYTHON_INTEGER_TYPE,
  _NPY_PYTHON_INTEGER_TYPE
} PYTHON_INTEGER_TYPES;

SWIGINTERNINLINE PYTHON_INTEGER_TYPES PyInteger_Check(PyObject* in)
{
  if (PyInt_Check(in))
    return _INT_PYTHON_INTEGER_TYPE;
  if (PyLong_Check(in))
    return _LONG_PYTHON_INTEGER_TYPE;
  if (PyArray_CheckScalar(in) && PyArray_IsScalar(in,Integer))
    return _NPY_PYTHON_INTEGER_TYPE;

  // No integer type
  return _NO_PYTHON_INTEGER_TYPE;
}
}

//-----------------------------------------------------------------------------
// Home brewed versions of the SWIG provided SWIG_AsVal(Type). These are needed
// as long as we need the PyInteger_Check. Whenever Python 2.6 is not supported
// we can scrap them.
//-----------------------------------------------------------------------------
#define Py_convert_frag(Type) "Py_convert_" {Type}

%fragment("Py_convert_std_size_t", "header", fragment="PyInteger_Check")
{

SWIGINTERNINLINE bool Py_convert_std_size_t(PyObject* in, std::size_t& value)
{

  // Get integer type
  PYTHON_INTEGER_TYPES int_type = PyInteger_Check(in);

  // No integer type?
  if (int_type == _NO_PYTHON_INTEGER_TYPE)
  {
    return false;
  }

  // Conversion if python int
  if (int_type == _INT_PYTHON_INTEGER_TYPE)
  {
%#if PY_MAJOR_VERSION >= 3
    const long signed_value = PyLong_AS_LONG(in);
%#else
    const long signed_value = PyInt_AS_LONG(in);
%#endif
    value = static_cast<std::size_t>(signed_value);
    return signed_value>=0;
  }

  // Conversion if numpy scalar
  if (int_type == _NPY_PYTHON_INTEGER_TYPE)
  {
    return PyArray_CastScalarToCtype(in, &value, PyArray_DescrFromType(NPY_UINTP))==0;
  }

  // Conversion if python long
  if (int_type == _LONG_PYTHON_INTEGER_TYPE)
  {
%#if PY_MAJOR_VERSION >= 3
    value = PyLong_AsSize_t(in);
%#else
    value = static_cast<std::size_t>(PyLong_AsUnsignedLongLong(in));
%#endif
    return !PyErr_Occurred();
  }

  // Should never reach this point
  return false;
}
}

%fragment("Py_convert_double", "header") {
  // A check for float and converter for double
  SWIGINTERNINLINE bool Py_convert_double(PyObject* in, double& value)
  {
    return SWIG_AsVal(double)(in, &value);
  }
}

%fragment("Py_convert_int", "header", fragment="PyInteger_Check") {
  // A check for int and converter for int
SWIGINTERNINLINE bool Py_convert_int(PyObject* in, int& value)
{

  // Get integer type
  PYTHON_INTEGER_TYPES int_type = PyInteger_Check(in);

  // No integer type?
  if (int_type == _NO_PYTHON_INTEGER_TYPE)
  {
    return false;
  }

  // Conversion if python int or numpy type
  if (int_type == _INT_PYTHON_INTEGER_TYPE || int_type == _NPY_PYTHON_INTEGER_TYPE)
  {
    value = static_cast<int>(PyInt_AsLong(in));
    return true;
  }

  // Conversion if python long
  if (int_type == _LONG_PYTHON_INTEGER_TYPE)
  {
    long long_value = PyLong_AsLong(in);
    value = static_cast<int>(long_value);
    return true;
  }

  // Should never reach this point
  return false;
}

}

%fragment("Py_convert_uint", "header", fragment="PyInteger_Check") {
  // A check for int and converter to uint
  SWIGINTERNINLINE bool Py_convert_uint(PyObject* in, unsigned int& value)
{

  // Get integer type
  PYTHON_INTEGER_TYPES int_type = PyInteger_Check(in);

  // No integer type?
  if (int_type == _NO_PYTHON_INTEGER_TYPE)
  {
    return false;
  }

  // Conversion if python int or numpy int
  if (int_type == _INT_PYTHON_INTEGER_TYPE || int_type == _NPY_PYTHON_INTEGER_TYPE)
  {
%#if PY_MAJOR_VERSION >= 3
    const long signed_value = PyLong_AS_LONG(in);
%#else
    const long signed_value = PyInt_AS_LONG(in);
%#endif
    value = static_cast<unsigned int>(signed_value);
    return signed_value>=0;
  }

  // Conversion if python long
  if (int_type == _LONG_PYTHON_INTEGER_TYPE)
  {
    const npy_longlong signed_value = PyLong_AsLong(in);
    value = static_cast<unsigned int>(signed_value);
    return signed_value>=0;
  }

  // Should never reach this point
  return false;
}
}

//-----------------------------------------------------------------------------
// Typemaps for unsigned int and int
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Out typemap (unsigned int)
//-----------------------------------------------------------------------------
%typemap(out, fragment=SWIG_From_frag(unsigned int)) unsigned int
{
  // Typemap unsigned int
  $result = PyInt_FromLong(static_cast<long>($1));
  // NOTE: From SWIG 2.0.5 does this macro return a Python long,
  // NOTE: which we do not want
  // NOTE: Fixed in 2.0.7, but keep the above fix for now
  // $result = SWIG_From(unsigned int)($1);
}

//-----------------------------------------------------------------------------
// Out typemap (std::size_t)
//-----------------------------------------------------------------------------
%typemap(out, fragment=SWIG_From_frag(std::size_t)) std::size_t
{
  if ($1<std::numeric_limits<long>::max())
%#if PY_MAJOR_VERSION >= 3
    $result = PyLong_FromSsize_t($1);
%#else
    $result = PyInt_FromSsize_t($1);
%#endif
  else
    $result = PyLong_FromUnsignedLongLong(static_cast<unsigned long long>($1));
}

//-----------------------------------------------------------------------------
// Typecheck and in typemap (unsigned int)
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_INTEGER) unsigned int
{
  $1 = PyInteger_Check($input) == _NO_PYTHON_INTEGER_TYPE ? 0 : 1;
}

%typemap(in, fragment="Py_convert_uint") unsigned int
{
  if (!Py_convert_uint($input, $1))
    SWIG_exception(SWIG_TypeError, "(uint) expected positive 'int' for argument $argnum");
}

//-----------------------------------------------------------------------------
// Typecheck and in typemap (std::size_t)
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_INTEGER) std::size_t
{
  $1 = PyInteger_Check($input) == _NO_PYTHON_INTEGER_TYPE ? 0 : 1;
}

%typemap(in, fragment="Py_convert_std_size_t") std::size_t
{
  if (!Py_convert_std_size_t($input, $1))
    SWIG_exception(SWIG_TypeError, "(size_t) expected positive 'int' for argument $argnum");
}

//-----------------------------------------------------------------------------
// The typecheck (int)
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_INTEGER) int
{
  $1 = PyInteger_Check($input) == _NO_PYTHON_INTEGER_TYPE ? 0 : 1;
}

//-----------------------------------------------------------------------------
// The typemap (int)
//-----------------------------------------------------------------------------
%typemap(in, fragment="Py_convert_int") int
{
  if (!Py_convert_int($input, $1))
    SWIG_exception(SWIG_TypeError, "expected 'int' for argument $argnum");
}
//-----------------------------------------------------------------------------
// The typecheck (bool) Needed after SWIG 3.0
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_BOOL) book
{
  $1 = PyBool_Check(obj)||PyArray_IsScalar(obj, Bool);
}

//-----------------------------------------------------------------------------
// The typemaps (bool)
//-----------------------------------------------------------------------------
%typemap(in, fragment=SWIG_AsVal_frag(bool2)) bool
{
  int swig_res = SWIG_AsVal_dec(bool2)($input, &$1);
  if (!SWIG_IsOK(swig_res))
    SWIG_exception(SWIG_TypeError, "expected 'bool' for argument $argnum");
}

%typemap(directorout, fragment=SWIG_AsVal_frag(bool2)) bool
{
  int swig_res = SWIG_AsVal_dec(bool2)($1, &$result);
  if (!SWIG_IsOK(swig_res))
    Swig::DirectorTypeMismatchException::raise(SWIG_ErrorType(SWIG_ArgError(swig_res)), "expected 'bool' as the output argument of '$symname'");
}

//-----------------------------------------------------------------------------
// Ensure typefragments
//-----------------------------------------------------------------------------
%fragment(SWIG_From_frag(unsigned long));
%fragment(SWIG_From_frag(double));
%fragment(SWIG_From_frag(unsigned int));
%fragment(SWIG_From_frag(int));
%fragment(SWIG_From_frag(std::size_t));
%fragment(SWIG_From_frag(dolfin::la_index));
%fragment("Py_convert_int");
%fragment("Py_convert_uint");
%fragment("Py_convert_std_size_t");
