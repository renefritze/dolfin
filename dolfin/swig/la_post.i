/* -*- C -*- */
// ===========================================================================
// SWIG directives for the DOLFIN la kernel module (post)
//
// The directives in this file are applied _after_ the header files of the
// modules has been loaded.
// ===========================================================================

// ---------------------------------------------------------------------------
// Instantiate uBLAS template classes
// ---------------------------------------------------------------------------
%template(uBLASSparseMatrix) dolfin::uBLASMatrix<dolfin::ublas_sparse_matrix>;
%template(uBLASDenseMatrix) dolfin::uBLASMatrix<dolfin::ublas_dense_matrix>;
%template(uBLASSparseFactory) dolfin::uBLASFactory<dolfin::ublas_sparse_matrix>;
%template(uBLASDenseFactory) dolfin::uBLASFactory<dolfin::ublas_dense_matrix>;

// ---------------------------------------------------------------------------
// Define names for uBLAS matrix types
// These are needed so returned uBLASMatrices from down_cast get correctly wrapped
// ---------------------------------------------------------------------------
%typedef dolfin::uBLASMatrix<dolfin::ublas_sparse_matrix> uBLASSparseMatrix;
%typedef dolfin::uBLASMatrix<dolfin::ublas_dense_matrix>  uBLASDenseMatrix;

// ---------------------------------------------------------------------------
// SLEPc specific extension code 
// ---------------------------------------------------------------------------
#ifdef HAS_SLEPC
%extend dolfin::SLEPcEigenSolver {

PyObject* get_eigenvalue(const int emode) {
    double err, ecc;
    self->get_eigenvalue(err, ecc, emode);

    PyObject* result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(err));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(ecc));
    Py_INCREF(result);
    return result;
}

PyObject* getEigenpair(dolfin::PETScVector& rr, dolfin::PETScVector& cc, const int emode) {
    double err, ecc;
    self->get_eigenpair(err, ecc, rr, cc, emode);

    PyObject* result = PyTuple_New(2);
    PyTuple_SET_ITEM(result, 0, PyFloat_FromDouble(err));
    PyTuple_SET_ITEM(result, 1, PyFloat_FromDouble(ecc));
    Py_INCREF(result);
    return result;
}

}
#endif

// ---------------------------------------------------------------------------
// C++ and Python extension code for BlockVector
// ---------------------------------------------------------------------------
%extend dolfin::BlockVector {
    Vector& getitem(int i) 
    { 
      return self->get(i);
    }
    void setitem(int i, Vector& v)
    {
      self->set(i, v); 
    }
    
  %pythoncode %{

    def __add__(self, v): 
      a = self.copy() 
      a += v
      return a

    def __sub__(self, v): 
      a = self.copy() 
      a -= v
      return a

    def __mul__(self, v): 
      a = self.copy() 
      a *= v
      return a

    def __rmul__(self, v):
      return self.__mul__(v)
  %}
}

// ---------------------------------------------------------------------------
// C++ and Python extension code for BlockMatrix
// ---------------------------------------------------------------------------
%extend dolfin::BlockMatrix {
%pythoncode
%{
    def __mul__(self, other):
      v = BlockVector(self.size(0))
      self.mult(other, v)
      return v
%}
}

%pythoncode
%{
  def BlockMatrix_get(self, t):
    i,j = t
    return self.get(i, j)

  def BlockMatrix_set(self, t, m): 
    i,j = t 
    return self.set(i, j, m)

  BlockMatrix.__getitem__ = BlockMatrix_get
  BlockMatrix.__setitem__ = BlockMatrix_set
%}

// ---------------------------------------------------------------------------
// Indices.i defines helper functions to extract C++ indices from Python 
// indices. These functions are not wrapped to the Python interface. They are
// only included in the C++ wrapper file.
//
// la_get_set_items.i defines helper functions that are wrapped to the
// Python. These are then used in the extended Python classes. See below.
// ---------------------------------------------------------------------------
%{
#include "Indices.i"
#include "la_get_set_items.i"
%}

%include "la_get_set_items.i"

// ---------------------------------------------------------------------------
// Macro with C++ and Python extension code for GenericVector types in PyDOLFIN
// ---------------------------------------------------------------------------
%define LA_POST_VEC_INTERFACE(VEC_TYPE)
%extend dolfin::VEC_TYPE
{
  void _scale(double a)
  {
    (*self)*=a;
  }
  
  void _vec_mul(const GenericVector& other)
  {
    (*self)*=other;
  }
// ---------------------------------------------------------------------------
  %pythoncode
  %{
    def __in_parallel(self):
        first, last = self.local_range()
        return first > 0 or len(self) > last
        
       
    def __is_compatibable(self, other):
        "Returns True if self, and other are compatible Vectors"
        if not isinstance(other, GenericVector):
            return False
        self_type = get_tensor_type(self)
        return self_type == get_tensor_type(other)
    
    def array(self):
        " Return a numpy array representation of Vector"
        from numpy import zeros, arange, uint0
        v = zeros(self.size())
        self.get_local(v)
        return v
    
    def __contains__(self, value):
        if not isinstance(value, (int, float)):
            raise TypeError, "expected scalar"
        return _contains(self,value)

    def __gt__(self, value):
        if isinstance(value, (int, float)):
            return _compare_vector_with_value(self, value, dolfin_gt)
        if isinstance(value,GenericVector):
            return _compare_vector_with_vector(self, value, dolfin_gt)
        return NotImplemented
    
    def __ge__(self,value):
        if isinstance(value, (int, float)):
            return _compare_vector_with_value(self, value, dolfin_ge)
        if isinstance(value, GenericVector):
            return _compare_vector_with_vector(self, value, dolfin_ge)
        return NotImplemented
    
    def __lt__(self,value):
        if isinstance(value, (int, float)):
            return _compare_vector_with_value(self, value, dolfin_lt)
        if isinstance(value, GenericVector):
            return _compare_vector_with_vector(self, value, dolfin_lt)
        return NotImplemented
    
    def __le__(self,value):
        if isinstance(value, (int, float)):
            return _compare_vector_with_value(self, value, dolfin_le)
        if isinstance(value, GenericVector):
            return _compare_vector_with_vector(self, value, dolfin_le)
        return NotImplemented
    
    def __eq__(self,value):
        if isinstance(value, (int, float)):
            return _compare_vector_with_value(self, value, dolfin_eq)
        if isinstance(value, GenericVector):
            return _compare_vector_with_vector(self, value, dolfin_eq)
        return NotImplemented
    
    def __neq__(self,value):
        if isinstance(value, (int, float)):
            return _compare_vector_with_value(self, value, dolfin_neq)
        if isinstance(value, GenericVector):
            return _compare_vector_with_vector(self, value, dolfin_neq)
        return NotImplemented

    def __neg__(self):
        ret = self.copy()
        ret *= -1
        return ret

    def __delitem__(self):
        raise ValueError, "cannot delete Vector elements"
    
    def __delslice__(self):
        raise ValueError, "cannot delete Vector elements"
    
    def __setslice__(self,i,j,values):
        if i == 0 and (j >= len(self) or j == -1) and isinstance(values, (float, int, GenericVector)):
            if isinstance(values, (float, int)) or len(values) == len(self):
                self.assign(values)
                return
            else:
                raise ValueError, "dimension error"
        self.__setitem__(slice(i, j, 1), values)
    
    def __getslice__(self, i, j):
        if i == 0 and (j >= len(self) or j == -1):
            return self.copy()
        return self.__getitem__(slice(i, j, 1))
    
    def __getitem__(self, indices):
        from numpy import ndarray
        from types import SliceType
        if isinstance(indices, int):
            return _get_vector_single_item(self, indices)
        elif isinstance(indices, (SliceType, ndarray, list) ):
            return down_cast(_get_vector_sub_vector(self, indices))
        else:
            raise TypeError, "expected an int, slice, list or numpy array of integers"

    def __setitem__(self, indices, values):
        from numpy import ndarray
        from types import SliceType
        if isinstance(indices, int):
            if isinstance(values,(float, int)):
                return _set_vector_items_value(self, indices, values)
            else:
                raise TypeError, "provide a scalar to set single item"
        elif isinstance(indices, (SliceType, ndarray, list)):
            if isinstance(values, (float, int)):
                _set_vector_items_value(self, indices, values)
            elif isinstance(values, GenericVector):
                _set_vector_items_vector(self, indices, values)
            elif isinstance(values, ndarray):
                _set_vector_items_array_of_float(self, indices, values)
            else:
                raise TypeError, "provide a scalar, GenericVector or numpy array of float to set items in Vector"
        else:
            raise TypeError, "index must be an int, slice or a list or numpy array of integers"
        
    def __len__(self):
        return self.size()

    def __iter__(self):
        for i in xrange(self.size()):
            yield _get_vector_single_item(self, i)

    def __add__(self,other):
        """x.__add__(y) <==> x+y"""
        if self.__is_compatibable(other):
            ret = self.copy()
            ret.axpy(1.0, other)
            return ret
        return NotImplemented
    
    def __sub__(self,other):
        """x.__sub__(y) <==> x-y"""
        if self.__is_compatibable(other):
            ret = self.copy()
            ret.axpy(-1.0,other)
            return ret
        return NotImplemented
    
    def __mul__(self,other):
        """x.__mul__(y) <==> x*y"""
        if isinstance(other,(int,float)):
            ret = self.copy()
            ret._scale(other)
            return ret
        if isinstance(other,GenericVector):
            ret = self.copy()
            ret._vec_mul(other)
            return ret
        return NotImplemented
    
    def __div__(self,other):
        """x.__div__(y) <==> x/y"""
        if isinstance(other,(int,float)):
            ret = self.copy()
            ret._scale(1.0/other)
            return ret
        return NotImplemented
    
    def __radd__(self,other):
        """x.__radd__(y) <==> y+x"""
        return self.__add__(other)
    
    def __rsub__(self,other):
        """x.__rsub__(y) <==> y-x"""
        return self.__sub__(other)
    
    def __rmul__(self,other):
        """x.__rmul__(y) <==> y*x"""
        if isinstance(other,(int,float)):
            ret = self.copy()
            ret._scale(other)
            return ret
        return NotImplemented
    
    def __rdiv__(self,other):
        """x.__rdiv__(y) <==> y/x"""
        return NotImplemented
    
    def __iadd__(self,other):
        """x.__iadd__(y) <==> x+y"""
        if self.__is_compatibable(other):
            self.axpy(1.0, other)
            return self
        return NotImplemented
    
    def __isub__(self,other):
        """x.__isub__(y) <==> x-y"""
        if self.__is_compatibable(other):
            self.axpy(-1.0, other)
            return self
        return NotImplemented
    
    def __imul__(self,other):
        """x.__imul__(y) <==> x*y"""
        if isinstance(other,(float,int)):
            self._scale(other)
            return self
        if isinstance(other,GenericVector):
            self._vec_mul(other)
            return self
        return NotImplemented

    def __idiv__(self,other):
        """x.__idiv__(y) <==> x/y"""
        if isinstance(other, (float, int)):
            self._scale(1.0/other)
            return self
        return NotImplemented
    
  %}
}
%enddef

// ---------------------------------------------------------------------------
// Macro with C++ and Python extension code for GenericMatrix types in PyDOLFIN
// ---------------------------------------------------------------------------
%define LA_POST_MAT_INTERFACE(MAT_TYPE)
%extend dolfin::MAT_TYPE
{
  void _scale(double a)
  {
    (*self)*=a;
  }
  
  PyObject* _data() {
    npy_intp rowdims[1];
    rowdims[0] = self->size(0)+1;
    
    PyArrayObject* rows = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNewFromData(1, rowdims, NPY_ULONG, (char *)(std::tr1::get<0>(self->data()))));
    if ( rows == NULL ) return NULL;
    
    npy_intp coldims[1];
    coldims[0] = std::tr1::get<3>(self->data());
    
    PyArrayObject* cols = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNewFromData(1, coldims, NPY_ULONG, (char *)(std::tr1::get<1>(self->data()))));
    if ( cols == NULL ) return NULL;
    
    npy_intp valuedims[1];
    valuedims[0] = std::tr1::get<3>(self->data());
    
    PyArrayObject* values = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNewFromData(1, valuedims, NPY_DOUBLE, (char *)(std::tr1::get<2>(self->data()))));
    if ( values == NULL ) return NULL;
    
    return Py_BuildValue("NNN",rows, cols, values);
  }
// ---------------------------------------------------------------------------
  %pythoncode
  %{
    def __is_compatibable(self,other):
        "Returns True if self, and other are compatible Vectors"
        if not isinstance(other,GenericMatrix):
            return False
        self_type = get_tensor_type(self)
        return self_type == get_tensor_type(other)
        
    def array(self):
        " Return a numpy array representation of Matrix"
        from numpy import zeros
        A = zeros((self.size(0), self.size(1)))
        for i in xrange(self.size(0)):
            column, values = self.getrow(i)
            A[i,column] = values
        return A

    def data(self):
        """ Return arrays to underlying compresssed row/column storage data """
        
        return self._data()
        
    def __getitem__(self,indices):
        from numpy import ndarray
        from types import SliceType
        if not (isinstance(indices, tuple) and len(indices) == 2):
            raise TypeError, "expected two indices"
        if not all(isinstance(ind,(int,SliceType,list,ndarray)) for ind in indices):
            raise TypeError, "an int, slice, list or numpy array as indices"
        
        if isinstance(indices[0],int):
            if isinstance(indices[1],int):
                return _get_matrix_single_item(self,indices[0],indices[1])
            return down_cast(_get_matrix_sub_vector(self,indices[0], indices[1], True))
        elif isinstance(indices[1],int):
            return down_cast(_get_matrix_sub_vector(self,indices[1], indices[0], False))
        else:
            same_indices = id(indices[0]) == id(indices[1])
            
            if not same_indices and ( type(indices[0]) == type(indices[1]) ):
                if isinstance(indices[0],(list,SliceType)):
                    same_indices = indices[0] == indices[1]
                else:
                    same_indices = (indices[0] == indices[1]).all()
            
            if same_indices:
                return down_cast(_get_matrix_sub_matrix(self, indices[0], None))
            else:
                return down_cast(_get_matrix_sub_matrix(self, indices[0], indices[1]))
            
    def __setitem__(self,indices, values):
        from numpy import ndarray
        from types import SliceType
        if not (isinstance(indices,tuple) and len(indices) == 2):
            raise TypeError, "expected two indices"
        if not all(isinstance(ind,(int,SliceType,list,ndarray)) for ind in indices):
            raise TypeError, "an int, slice, list or numpy array as indices"
        
        if isinstance(indices[0],int):
            if isinstance(indices[1],int):
                if not isinstance(values,(float,int)):
                    raise TypeError, "expected scalar for single value assigment"
                _set_matrix_single_item(self,indices[0],indices[1],values)
            else:
                raise NotImplementedError
                if isinstance(values,GenericVector):
                    _set_matrix_items_vector(self, indices[0], indices[1], values, True)
                elif isinstance(values,ndarray):
                    _set_matrix_items_array_of_float(self, indices[0], indices[1], values, True)
                else:
                    raise TypeError, "expected a GenericVector or numpy array of float"
        elif isinstance(indices[1],int):
            raise NotImplementedError
            if isinstance(values,GenericVector):
                _set_matrix_items_vector(self, indices[1], indices[0], values, False)
            elif isinstance(values,ndarray):
                _set_matrix_items_array_of_float(self, indices[1], indices[0], values, False)
            else:
                raise TypeError, "expected a GenericVector or numpy array of float"

        else:
            raise NotImplementedError
            same_indices = id(indices[0]) == id(indices[1])
            
            if not same_indices and ( type(indices[0]) == type(indices[1]) ):
                if isinstance(indices[0],(list,SliceType)):
                    same_indices = indices[0] == indices[1]
                else:
                    same_indices = (indices[0] == indices[1]).all()
            
            if same_indices:
                if isinstance(values,GenericMatrix):
                    _set_matrix_items_matrix(self, indices[0], None, values)
                elif isinstance(values,ndarray) and len(values.shape)==2:
                    _set_matrix_items_array_of_float(self, indices[0], None, values)
                else:
                    raise TypeError, "expected a GenericMatrix or 2D numpy array of float"
            else:
                if isinstance(values,GenericMatrix):
                    _set_matrix_items_matrix(self, indices[0], indices[1], values)
                elif isinstance(values,ndarray) and len(values.shape)==2:
                    _set_matrix_items_array_of_float(self, indices[0], indices[1], values)
                else:
                    raise TypeError, "expected a GenericMatrix or 2D numpy array of float"
            
    def __add__(self,other):
        """x.__add__(y) <==> x+y"""
        if self.__is_compatibable(other):
            ret = self.copy()
            ret.axpy(1.0,other)
            return ret
        return NotImplemented
    
    def __sub__(self,other):
        """x.__sub__(y) <==> x-y"""
        if self.__is_compatibable(other):
            ret = self.copy()
            ret.axpy(-1.0,other)
            return ret
        return NotImplemented
    
    def __mul__(self,other):
        """x.__mul__(y) <==> x*y"""
        from numpy import ndarray
        if isinstance(other,(int,float)):
            ret = self.copy()
            ret._scale(other)
            return ret
        elif isinstance(other,GenericVector):
            matrix_type = get_tensor_type(self)
            vector_type = get_tensor_type(other)
            if vector_type not in _matrix_vector_mul_map[matrix_type]:
                raise TypeError, "Provide a Vector which can be down_casted to ''"%vector_type.__name__
            if type(other) == Vector:
                ret = Vector(self.size(0))
            else:
                ret = vector_type(self.size(0))
            self.mult(other, ret)
            return ret
        elif isinstance(other,ndarray):
            if len(other.shape) !=1:
                raise ValueError, "Provide an 1D NumPy array"
            vec_size = other.shape[0]
            if vec_size != self.size(1):
                raise ValueError, "Provide a NumPy array with length %d"%self.size(1)
            vec_type = _matrix_vector_mul_map[get_tensor_type(self)][0]
            vec  = vec_type(vec_size)
            vec.set_local(other)
            result_vec = vec.copy()
            self.mult(vec, result_vec)
            ret = other.copy()
            result_vec.get_local(ret)
            return ret
        return NotImplemented
    
    def __div__(self,other):
        """x.__div__(y) <==> x/y"""
        if isinstance(other,(int,float)):
            ret = self.copy()
            ret._scale(1.0/other)
            return ret
        return NotImplemented
    
    def __radd__(self,other):
        """x.__radd__(y) <==> y+x"""
        return self.__add__(other)
    
    def __rsub__(self,other):
        """x.__rsub__(y) <==> y-x"""
        return self.__sub__(other)
    
    def __rmul__(self,other):
        """x.__rmul__(y) <==> y*x"""
        if isinstance(other,(int,float)):
            ret = self.copy()
            ret._scale(other)
            return ret
        return NotImplemented
    
    def __rdiv__(self,other):
        """x.__rdiv__(y) <==> y/x"""
        return NotImplemented
    
    def __iadd__(self,other):
        """x.__iadd__(y) <==> x+y"""
        if self.__is_compatibable(other):
            self.axpy(1.0,other)
            return self
        return NotImplemented
    
    def __isub__(self,other):
        """x.__isub__(y) <==> x-y"""
        if self.__is_compatibable(other):
            self.axpy(-1.0,other)
            return self
        return NotImplemented
    
    def __imul__(self,other):
        """x.__imul__(y) <==> x*y"""
        if isinstance(other,(float,int)):
            self._scale(other)
            return self
        return NotImplemented

    def __idiv__(self,other):
        """x.__idiv__(y) <==> x/y"""
        if isinstance(other,(float,int)):
            self._scale(1.0/other)
            return self
        return NotImplemented

  %}
}
%enddef

// ---------------------------------------------------------------------------
// Macro with C++ and Python extension code for GenericVector types in PyDOLFIN
// that are able to return a pointer to the underlaying contigious data 
// only used for the uBLAS and MTL4 backends
// ---------------------------------------------------------------------------
%define LA_VEC_DATA_ACCESS(VEC_TYPE)
%extend dolfin::VEC_TYPE
{
  PyObject* _data()
  {
    npy_intp valuedims[1];
    valuedims[0] = self->size();
    PyArrayObject* values = reinterpret_cast<PyArrayObject*>(PyArray_SimpleNewFromData(1, valuedims, NPY_DOUBLE, (char *)(self->data())));
    if ( values == NULL ) return NULL;
    return reinterpret_cast<PyObject*>(values);
  }

  %pythoncode
  %{
    def data(self):
        " Return an array to the underlaying data"
        return self._data()
  %}
}
%enddef

// ---------------------------------------------------------------------------
// Macro with code for down casting GenericTensors
// ---------------------------------------------------------------------------
%define DOWN_CAST_MACRO(TENSOR_TYPE)
%inline %{
bool has_type_ ## TENSOR_TYPE(dolfin::GenericTensor & tensor)
{ return tensor.has_type<dolfin::TENSOR_TYPE>(); }

dolfin::TENSOR_TYPE & down_cast_ ## TENSOR_TYPE(dolfin::GenericTensor & tensor)
{ return tensor.down_cast<dolfin::TENSOR_TYPE>(); }
%}

%pythoncode %{
_has_type_map[TENSOR_TYPE] = has_type_ ## TENSOR_TYPE
_down_cast_map[TENSOR_TYPE] = down_cast_ ## TENSOR_TYPE
%}

%enddef

// ---------------------------------------------------------------------------
// Define Python lookup maps for down_casting
// ---------------------------------------------------------------------------
%pythoncode %{
_has_type_map = {}
_down_cast_map = {}
# A map with matrix types as keys and list of possible vector types as values
_matrix_vector_mul_map = {}
%}

// ---------------------------------------------------------------------------
// Run the LS interface macros
// ---------------------------------------------------------------------------
LA_POST_VEC_INTERFACE(GenericVector)
LA_POST_MAT_INTERFACE(GenericMatrix)
//LA_VEC_DATA_ACCESS(GenericVector)

LA_POST_VEC_INTERFACE(Vector)
LA_POST_MAT_INTERFACE(Matrix)
LA_VEC_DATA_ACCESS(Vector)

LA_POST_VEC_INTERFACE(uBLASVector)
// NOTE: The uBLAS macros need to be run using the whole template type
// I have tried using the typmaped one from above but with no luck.
LA_POST_MAT_INTERFACE(uBLASMatrix<dolfin::ublas_sparse_matrix>)
LA_POST_MAT_INTERFACE(uBLASMatrix<dolfin::ublas_dense_matrix>)
LA_VEC_DATA_ACCESS(uBLASVector)

// ---------------------------------------------------------------------------
// Run the downcast macro
// ---------------------------------------------------------------------------
DOWN_CAST_MACRO(uBLASVector)
DOWN_CAST_MACRO(uBLASSparseMatrix)
DOWN_CAST_MACRO(uBLASDenseMatrix)

// ---------------------------------------------------------------------------
// Fill lookup map
// ---------------------------------------------------------------------------
%pythoncode %{
_matrix_vector_mul_map[uBLASSparseMatrix] = [uBLASVector]
_matrix_vector_mul_map[uBLASDenseMatrix]  = [uBLASVector]
%}

// ---------------------------------------------------------------------------
// Run backend specific macros
// ---------------------------------------------------------------------------
#ifdef HAS_PETSC
LA_POST_VEC_INTERFACE(PETScVector)
LA_POST_MAT_INTERFACE(PETScMatrix)

DOWN_CAST_MACRO(PETScVector)
DOWN_CAST_MACRO(PETScMatrix)

%pythoncode %{
_matrix_vector_mul_map[PETScMatrix] = [PETScVector]
%}
#endif

#ifdef HAS_TRILINOS
LA_POST_VEC_INTERFACE(EpetraVector)
LA_POST_MAT_INTERFACE(EpetraMatrix)

DOWN_CAST_MACRO(EpetraVector)
DOWN_CAST_MACRO(EpetraMatrix)

%pythoncode %{
_matrix_vector_mul_map[EpetraMatrix] = [EpetraVector]
%}
#endif

#ifdef HAS_MTL4
LA_POST_VEC_INTERFACE(MTL4Vector)
LA_POST_MAT_INTERFACE(MTL4Matrix)
LA_VEC_DATA_ACCESS(MTL4Vector)

DOWN_CAST_MACRO(MTL4Vector)
DOWN_CAST_MACRO(MTL4Matrix)

%pythoncode %{
_matrix_vector_mul_map[MTL4Matrix] = [MTL4Vector]
%}
#endif

// ---------------------------------------------------------------------------
// Dynamic wrappers for GenericTensor::down_cast and GenericTensor::has_type, 
// using dict of tensor types to select from C++ template instantiations
// ---------------------------------------------------------------------------
%pythoncode %{
def get_tensor_type(tensor):
    "Return the concrete subclass of tensor."
    for k,v in _has_type_map.items():
        if v(tensor):
            return k
    dolfin_error("Unregistered tensor type.")

def has_type(tensor, subclass):
    "Return wether tensor is of the given subclass."
    global _has_type_map
    assert _has_type_map
    assert subclass in _has_type_map
    return bool(_has_type_map[subclass](tensor))

def down_cast(tensor, subclass=None):
    "Cast tensor to the given subclass, passing the wrong class is an error."
    global _down_cast_map
    assert _down_cast_map
    if subclass is None:
        subclass = get_tensor_type(tensor)
    assert subclass in _down_cast_map
    ret = _down_cast_map[subclass](tensor)
    
    # Store the tensor to avoid garbage collection
    ret._org_upcasted_tensor = tensor
    return ret

%}
