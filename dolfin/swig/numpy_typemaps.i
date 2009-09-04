/* -*- C -*- */
// Copyright (C) 2007-2009 Ola Skavhaug
// Licensed under the GNU LGPL Version 2.1.
//
// Modified by Johan Hake, 2008-2009.
// Modified by Anders logg, 2009.
//
// First added:  2007-12-16
// Last changed: 2009-09-02

//-----------------------------------------------------------------------------
// Add numpy typemaps and macro for numpy typemaps
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Macro for defining an unsafe in-typemap for NumPy arrays -> c arrays 
// 
// The typmaps defined by this macro just passes the pointer to the C array, 
// contained in the NumPy array to the function. The knowledge of the length
// of the incomming array is not used.
//
// TYPE       : The pointer type
// TYPE_UPPER : The SWIG specific name of the type used in the array type checks values
//              SWIG use: INT32 for integer, DOUBLE for double aso.
// NUMPY_TYPE : The NumPy type that is going to be checked for
// TYPE_NAME  : The name of the pointer type, 'double' for 'double', 'uint' for
//              'dolfin::uint'
// DESCR      : The char descriptor of the NumPy type
//-----------------------------------------------------------------------------
%define UNSAFE_NUMPY_TYPEMAPS(TYPE,TYPE_UPPER,NUMPY_TYPE,TYPE_NAME,DESCR)
%{
//-----------------------------------------------------------------------------
// Typemap function (Reducing wrapper code size)
//-----------------------------------------------------------------------------
static bool convert_numpy_to_ ## TYPE_NAME ## _array_no_check(PyObject* input, TYPE*& ret)
{
  if PyArray_Check(input) 
  {
    PyArrayObject *xa = reinterpret_cast<PyArrayObject*>(input);
    if ( PyArray_TYPE(xa) == NUMPY_TYPE )
    {
      ret  = static_cast<TYPE*>(PyArray_DATA(xa));
      return true;
    }
  }
  PyErr_SetString(PyExc_TypeError,"numpy array of 'TYPE_NAME' expected. Make sure that the numpy array use dtype='DESCR'.");
  return false;
}
%}

//-----------------------------------------------------------------------------
// The typecheck
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_ ## TYPE_UPPER) TYPE *
{
    $1 = PyArray_Check($input) ? 1 : 0;
}

//-----------------------------------------------------------------------------
// The typemap
//-----------------------------------------------------------------------------
%typemap(in) TYPE *
{
if (!convert_numpy_to_ ## TYPE_NAME ## _array_no_check($input,$1))
    return NULL;
}

//-----------------------------------------------------------------------------
// Apply the typemap on the TYPE* _array argument
//-----------------------------------------------------------------------------
%apply TYPE* {TYPE* _array}

%enddef

//-----------------------------------------------------------------------------
// Macro for defining an safe in-typemap for NumPy arrays -> c arrays 
// 
// Type       : The pointer type
// TYPE_UPPER : The SWIG specific name of the type used in the array type checks values
//              SWIG use: INT32 for integer, DOUBLE for double aso.
// NUMPY_TYPE : The NumPy type that is going to be checked for
// TYPE_NAME  : The name of the pointer type, 'double' for 'double', 'uint' for
//              'dolfin::uint'
// DESCR      : The char descriptor of the NumPy type
//-----------------------------------------------------------------------------
%define SAFE_NUMPY_TYPEMAPS(TYPE,TYPE_UPPER,NUMPY_TYPE,TYPE_NAME,DESCR)
%{
//-----------------------------------------------------------------------------
// Typemap function (Reducing wrapper code size)
//-----------------------------------------------------------------------------
static bool convert_numpy_to_ ## TYPE_NAME ## _array_with_check(PyObject* input, dolfin::uint& _array_dim, TYPE*& _array)
{
  if PyArray_Check(input) 
  {
    PyArrayObject *xa = reinterpret_cast<PyArrayObject*>(input);
    if ( PyArray_TYPE(xa) == NUMPY_TYPE )
    {
      _array  = static_cast<TYPE*>(PyArray_DATA(xa));
      _array_dim = static_cast<dolfin::uint>(PyArray_DIM(xa,0));
      return true;
    }
  }
  PyErr_SetString(PyExc_TypeError,"numpy array of 'TYPE_NAME' expected. Make sure that the numpy array use dtype='DESCR'.");
  return false;
}
%}

//-----------------------------------------------------------------------------
// The typecheck
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_ ## TYPE_UPPER ## _ARRAY) (dolfin::uint _array_dim, TYPE* _array)
{
  $1 = PyArray_Check($input) ? 1 : 0;
}

//-----------------------------------------------------------------------------
// The typemap
//-----------------------------------------------------------------------------
%typemap(in) (dolfin::uint _array_dim, TYPE* _array)
{
  if (!convert_numpy_to_ ## TYPE_NAME ## _array_with_check($input,$1,$2))
    return NULL;
}
%enddef

//-----------------------------------------------------------------------------
// Run the different macros and instantiate the typemaps
// NOTE: If a typemap is not used an error will be issued as the generated 
//       typemap function will not be used 
//-----------------------------------------------------------------------------
UNSAFE_NUMPY_TYPEMAPS(dolfin::uint,INT32,NPY_UINT,uint,I)
UNSAFE_NUMPY_TYPEMAPS(double,DOUBLE,NPY_DOUBLE,double,d)
//UNSAFE_NUMPY_TYPEMAPS(int,INT,NPY_INT,int,i)

SAFE_NUMPY_TYPEMAPS(dolfin::uint,INT32,NPY_UINT,uint,I)
//SAFE_NUMPY_TYPEMAPS(double,DOUBLE,NPY_DOUBLE,double,d)
//SAFE_NUMPY_TYPEMAPS(int,INT32,NPY_INT,int,i)

//-----------------------------------------------------------------------------
// Typecheck for function expecting two-dimensional NumPy arrays of double
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) (int _array_dim_0, int _array_dim_1, double* _array) 
{
    $1 = PyArray_Check($input) ? 1 : 0;
}

//-----------------------------------------------------------------------------
// Generic typemap to expand a two-dimensional NumPy arrays into three
// C++ arguments: _array_dim_0, _array_dim_1, _array
//-----------------------------------------------------------------------------
%typemap(in) (int _array_dim_0, int _array_dim_1, double* _array) {
    if PyArray_Check($input) {
        PyArrayObject *xa = reinterpret_cast<PyArrayObject*>($input);
        if ( PyArray_TYPE(xa) == NPY_DOUBLE ) {
            if ( PyArray_NDIM(xa) == 2 ) {
                $1 = PyArray_DIM(xa,0);
                $2 = PyArray_DIM(xa,1);
                $3  = static_cast<double*>(PyArray_DATA(xa));
            } else {
                SWIG_exception(SWIG_ValueError, "2d Array expected");
            }
        } else {
            SWIG_exception(SWIG_TypeError, "Array of doubles expected");
        }
    } else {
        SWIG_exception(SWIG_TypeError, "Array expected");
    }
}

//-----------------------------------------------------------------------------
// Typecheck for function expecting two-dimensional NumPy arrays of int
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_DOUBLE_ARRAY) (int _array_dim_0, int _array_dim_1, int* _array) 
{
    $1 = PyArray_Check($input) ? 1 : 0;
}

//-----------------------------------------------------------------------------
// Generic typemap to expand a two-dimensional NumPy arrays into three
// C++ arguments: _array_dim_0, _array_dim_1, _array
//-----------------------------------------------------------------------------
%typemap(in) (int _array_dim_0, int _array_dim_1, int* _array) 
{
    if PyArray_Check($input) {
        PyArrayObject *xa = reinterpret_cast<PyArrayObject*>($input);
        if ( PyArray_TYPE(xa) == NPY_INT ) {
            if ( PyArray_NDIM(xa) == 2 ) {
                $1 = PyArray_DIM(xa,0);
                $2 = PyArray_DIM(xa,1);
                $3  = static_cast<int*>(PyArray_DATA(xa));
            } else {
                SWIG_exception(SWIG_ValueError, "2d Array expected");
            }
        } else {
            SWIG_exception(SWIG_TypeError, "Array of integers expected");
        }
    } else {
        SWIG_exception(SWIG_TypeError, "Array expected");
    }
}

//-----------------------------------------------------------------------------
// Cleaner of temporary data when passing 2D NumPy arrays to C++ functions 
// expecting double **
//-----------------------------------------------------------------------------
%{
namespace __private {
  class DppDeleter {
  public:
    double** amat;
    DppDeleter () {amat = 0;}
    ~DppDeleter ()
    {
      free(amat);
    }
  };
}
%}

//-----------------------------------------------------------------------------
// Typemap for 2D NumPy arrays to C++ functions expecting double **
//-----------------------------------------------------------------------------
%typemap(in) double** (__private::DppDeleter tmp)
{
    if PyArray_Check($input) {
        PyArrayObject *xa = reinterpret_cast<PyArrayObject*>($input);
        if ( PyArray_TYPE(xa) == NPY_DOUBLE ) {
            if ( PyArray_NDIM(xa) == 2 ) {
                const int n = PyArray_DIM(xa,0);
                double **amat = static_cast<double**>(malloc(n*sizeof*amat));
                double *data = reinterpret_cast<double*>((*xa).data);
                for (int i=0;i<n;++i)
                    amat[i] = data + i*n;
                $1 = amat;
                tmp.amat = amat;
            } else {
                SWIG_exception(SWIG_ValueError, "2d Array expected");
            }
        } else {
            SWIG_exception(SWIG_TypeError, "Array of doubles expected");
        }
    } else {
        SWIG_exception(SWIG_TypeError, "Array expected");
    }
}

//-----------------------------------------------------------------------------
// Typemap for 2D NumPy arrays to C++ functions expecting double **
//-----------------------------------------------------------------------------
%typemap(in) (int _matrix_dim_0, int _matrix_dim_1, double** _matrix) (__private::DppDeleter tmp)
{
    if PyArray_Check($input) {
        PyArrayObject *xa = reinterpret_cast<PyArrayObject *>($input);
        if ( PyArray_TYPE(xa) == NPY_DOUBLE ) {
            if ( PyArray_NDIM(xa) == 2 ) {
                int n = PyArray_DIM(xa,0);
                int m = PyArray_DIM(xa,1);
                $1 = n;
                $2 = m;
                double **amat = static_cast<double **>(malloc(n*sizeof*amat));
                double *data = reinterpret_cast<double *>(PyArray_DATA(xa));
                for (int i=0;i<n;++i)
                    amat[i] = data + i*n;
                $3 = amat;
                tmp.amat = amat;
            } else {
                SWIG_exception(SWIG_ValueError, "2d Array expected");
            }
        } else {
            SWIG_exception(SWIG_TypeError, "Array of doubles expected");
        }
    } else {
        SWIG_exception(SWIG_TypeError, "Array expected");
    }
}

// vim:ft=cpp:
