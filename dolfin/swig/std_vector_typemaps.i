/* -*- C -*- */
// Copyright (C) 2009 Johan Hake
// Licensed under the GNU LGPL Version 2.1.
//
// First added:  2009-08-31
// Last changed: 2009-09-02

//=============================================================================
// In this file we declare what types that should be able to be passed using a
// std::vector typemap. 
// 
// We want to avoid using SWIGs own typemaps in std_vector.i,
// as we really just want to be able to pass argument, in and a out, using 
// std::vector. We do not wnat to work with a proxy type of std::vector<Foo>, 
// as the interface reflex the C++ type and is hence not 'pythonic'. 
//=============================================================================

//-----------------------------------------------------------------------------
// Declare a dummy vector class
// This makes SWIG aware of the template type
//-----------------------------------------------------------------------------
namespace std
{
  template <class T> class vector 
  {
  };
}

//-----------------------------------------------------------------------------
// User macro for defineing in typmaps for std::vector of pointers to some 
// DOLFIN type
//-----------------------------------------------------------------------------
%define IN_TYPEMAPS_STD_VECTOR_OF_POINTERS(TYPE)

//-----------------------------------------------------------------------------
// Make SWIG aware of the shared_ptr version of TYPE
//-----------------------------------------------------------------------------
%types(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr<TYPE>*);

//-----------------------------------------------------------------------------
// Run the macros for the combination of const and no const of
// {const} std::vector<{const} dolfin::TYPE *>
//-----------------------------------------------------------------------------
//IN_TYPEMAP_STD_VECTOR_OF_POINTERS(TYPE,,)
IN_TYPEMAP_STD_VECTOR_OF_POINTERS(TYPE,const,)
IN_TYPEMAP_STD_VECTOR_OF_POINTERS(TYPE,,const)
IN_TYPEMAP_STD_VECTOR_OF_POINTERS(TYPE,const,const)

%enddef

//-----------------------------------------------------------------------------
// Macro for defining in typemaps for
// {const} std::vector<{const} dolfin::TYPE *>
// using a Python List of TYPE
//-----------------------------------------------------------------------------
%define IN_TYPEMAP_STD_VECTOR_OF_POINTERS(TYPE,CONST,CONST_VECTOR)

//-----------------------------------------------------------------------------
// The typecheck
//-----------------------------------------------------------------------------
%typecheck(SWIG_TYPECHECK_POINTER) CONST_VECTOR std::vector<CONST dolfin::TYPE *> &
{
  $1 = PyList_Check($input) ? 1 : 0;
}

//-----------------------------------------------------------------------------
// Instantiate the dummy template, making SWIG aware of the type
// FIXME: Is this needed?
//-----------------------------------------------------------------------------
//%template () std::vector<dolfin::TYPE *>;

//-----------------------------------------------------------------------------
// The typemap
//-----------------------------------------------------------------------------
%typemap (in) CONST_VECTOR std::vector<CONST dolfin::TYPE *> &(std::vector<CONST dolfin::TYPE *> tmp_vec)
{
  if (PyList_Check($input))
  {  
    int size = PyList_Size($input);
    int res = 0;
    PyObject * py_item = 0;
    void * itemp = 0;
    int newmem = 0;    
    tmp_vec.reserve(size);
    for (int i = 0; i < size; i++)
    {
      py_item = PyList_GetItem($input,i);
      res = SWIG_ConvertPtrAndOwn(py_item, &itemp, $descriptor(dolfin::TYPE *), 0, &newmem);
      if (SWIG_IsOK(res)) {
	tmp_vec.push_back(reinterpret_cast<dolfin::TYPE *>(itemp));
      }
      else
      {
	// If failed with normal pointer conversion then 
	// try with shared_ptr conversion
	newmem = 0;
	res = SWIG_ConvertPtrAndOwn(py_item, &itemp, $descriptor(SWIG_SHARED_PTR_QNAMESPACE::shared_ptr< dolfin::TYPE > *), 0, &newmem);
	if (SWIG_IsOK(res)) 
	{
	  tmp_vec.push_back(reinterpret_cast<SWIG_SHARED_PTR_QNAMESPACE::shared_ptr<dolfin::TYPE> *>(itemp)->get() );
	}
	else
	{
	  SWIG_exception(SWIG_TypeError, "list of TYPE expected (Bad conversion)");
	}
      }
    }
    $1 = &tmp_vec;
  }
  else
  {
    SWIG_exception(SWIG_TypeError, "list of TYPE expected");
  }
}

//-----------------------------------------------------------------------------
%enddef

//-----------------------------------------------------------------------------
// Macro for defining an argout typemap for a std::vector of primitives
// The typemaps makes a function returning a NumPy array of that primitive
//
// TYPE       : The primitive type
// TYPE_UPPER : The SWIG specific name of the type used in the array type checks
//              values SWIG use: INT32 for integer, DOUBLE for double aso.
// ARG_NAME   : The name of the argument that will be maped as an 'argout' argument
// NUMPY_TYPE : The type of the NumPy array that will be returned
//-----------------------------------------------------------------------------
%define ARGOUT_TYPEMAP_STD_VECTOR_OF_PRIMITIVES(TYPE, TYPE_UPPER, ARG_NAME, NUMPY_TYPE)

//-----------------------------------------------------------------------------
// In typemap removing the argument from the expected in list
//-----------------------------------------------------------------------------
%typemap (in,numinputs=0) std::vector<TYPE>& ARG_NAME (std::vector<TYPE> vec_temp)
{
  $1 = &vec_temp;
}

//-----------------------------------------------------------------------------
// Argout typemap, returning a NumPy array for the std::vector<TYPE>
//-----------------------------------------------------------------------------
%typemap(argout) std::vector<TYPE> & ARG_NAME
{
  PyObject* o0 = 0;
  PyObject* o1 = 0;
  PyObject* o2 = 0;
  npy_intp size = $1->size();
  PyArrayObject *ret = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNew(1, &size, NUMPY_TYPE));
  TYPE* data = static_cast<TYPE*>(PyArray_DATA(ret));
  for (int i = 0; i < size; ++i)
    data[i] = (*$1)[i];
  o0 = PyArray_Return(ret);
  // If the $result is not already set
  if ((!$result) || ($result == Py_None)) 
  {
    $result = o0;
  }
  // If the result is set by another out typemap build a tuple of arguments
  else
  {
    // If the the argument is set but is not a tuple make one and put the result in it
    if (!PyTuple_Check($result)) 
    {
      o1 = $result;
      $result = PyTuple_New(1);
      PyTuple_SetItem($result, 0, o1);
    }
    o2 = PyTuple_New(1);
    PyTuple_SetItem(o2, 0, o0);
    o1 = $result;
    $result = PySequence_Concat(o1, o2);
    Py_DECREF(o1);
    Py_DECREF(o2);
  }
}

%enddef

//-----------------------------------------------------------------------------
// Run the different macros and instantiate the typemaps
//-----------------------------------------------------------------------------
IN_TYPEMAPS_STD_VECTOR_OF_POINTERS(DirichletBC)
IN_TYPEMAPS_STD_VECTOR_OF_POINTERS(BoundaryCondition)
IN_TYPEMAPS_STD_VECTOR_OF_POINTERS(GenericFunction)
IN_TYPEMAPS_STD_VECTOR_OF_POINTERS(FunctionSpace)

ARGOUT_TYPEMAP_STD_VECTOR_OF_PRIMITIVES(dolfin::uint, INT32, cells, NPY_INT)
ARGOUT_TYPEMAP_STD_VECTOR_OF_PRIMITIVES(dolfin::uint, INT32, columns, NPY_INT)
ARGOUT_TYPEMAP_STD_VECTOR_OF_PRIMITIVES(double, DOUBLE, values, NPY_DOUBLE)
