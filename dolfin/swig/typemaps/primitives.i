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
// Last changed: 2011-05-02

//=============================================================================
// General typemaps for PyDOLFIN
//=============================================================================

// Make sure Python int from std::size_t can be constructed
// It looks like SWIG_From_size_t is available but not SWIG_From_std_size_t
%fragment("SWIG_From_std_size_t", "header") {
  SWIGINTERNINLINE PyObject * SWIG_From_std_size_t  (std::size_t value)
  {
    return SWIG_From_unsigned_SS_long (static_cast< unsigned long >(value));
  }
}

// Make sure Python int from DolfinIndex can be constructed
%fragment("SWIG_From_dolfin_DolfinIndex", "header") {
  SWIGINTERNINLINE PyObject * SWIG_From_dolfin_DolfinIndex  (dolfin::DolfinIndex value)
  {
    return SWIG_From_unsigned_SS_long (static_cast< unsigned long >(value));
  }
}

//-----------------------------------------------------------------------------
// A home brewed type check for checking integers
// Needed due to problems with PyInt_Check from python 2.6 and NumPy
//-----------------------------------------------------------------------------
%fragment("PyInteger_Check", "header") {
  SWIGINTERNINLINE bool PyInteger_Check(PyObject* in)
  {
    return  PyInt_Check(in) || (PyArray_CheckScalar(in) &&
				PyArray_IsScalar(in,Integer));
  }
}

//-----------------------------------------------------------------------------
// Home brewed versions of the SWIG provided SWIG_AsVal(Type). These are needed
// as long as we need the PyInteger_Check. Whenever Python 2.6 is not supported
// we can scrap them.
//-----------------------------------------------------------------------------
#define Py_convert_frag(Type) "Py_convert_" {Type}

%fragment("Py_convert_std_size_t", "header", fragment="PyInteger_Check") {
  // A check for integer
  SWIGINTERNINLINE bool Py_convert_std_size_t(PyObject* in, std::size_t& value)
  {
    if (!(PyInteger_Check(in) && PyInt_AS_LONG(in)>=0))
      return false;
    value = static_cast<std::size_t>(PyInt_AS_LONG(in));
    return true;
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
    if (!PyInteger_Check(in))
      return false;
    value = static_cast<int>(PyInt_AS_LONG(in));
    return true;
  }
}

%fragment("Py_convert_uint", "header", fragment="PyInteger_Check") {
  // A check for int and converter to uint
  SWIGINTERNINLINE bool Py_convert_uint(PyObject* in, unsigned int& value)
  {
    if (!(PyInteger_Check(in) && PyInt_AS_LONG(in)>=0))
      return false;
    value = static_cast<unsigned int>(PyInt_AS_LONG(in));
    return true;
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
  $result = PyInt_FromLong(static_cast< long >($1));
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
  // Typemap unsigned int
  $result = PyInt_FromLong(static_cast< long >($1));
  // NOTE: From SWIG 2.0.5 does this macro return a Python long,
  // NOTE: which we do not want
  // NOTE: Fixed in 2.0.7, but keep the above fix for now
  // $result = SWIG_From(std::size_t)($1);
}

//-----------------------------------------------------------------------------
// Typecheck and in typemap (unsigned int)
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_INTEGER) unsigned int
{
  $1 = PyInteger_Check($input) ? 1 : 0;
}

%typemap(in, fragment="Py_convert_uint") unsigned int
{
  if (!Py_convert_uint($input, $1))
    SWIG_exception(SWIG_TypeError, "(a) expected positive 'int' for argument $argnum");
}

//-----------------------------------------------------------------------------
// Typecheck and in typemap (std::size_t)
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_INTEGER) std::size_t
{
  $1 = PyInteger_Check($input) ? 1 : 0;
}

%typemap(in, fragment="Py_convert_std_size_t") std::size_t
{
  if (!Py_convert_std_size_t($input, $1))
    SWIG_exception(SWIG_TypeError, "(b) expected positive 'int' for argument $argnum");
}

//-----------------------------------------------------------------------------
// The typecheck (int)
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_INTEGER) int
{
  $1 =  PyInteger_Check($input) ? 1 : 0;
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
// Ensure typefragments
//-----------------------------------------------------------------------------
%fragment(SWIG_From_frag(unsigned long));
%fragment(SWIG_From_frag(double));
%fragment(SWIG_From_frag(unsigned int));
%fragment(SWIG_From_frag(int));
%fragment(SWIG_From_frag(std::size_t));
%fragment(SWIG_From_frag(dolfin::DolfinIndex));
