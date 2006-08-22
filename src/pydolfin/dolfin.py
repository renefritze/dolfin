# This file was created automatically by SWIG 1.3.27.
# Don't modify this file, modify the SWIG interface instead.

import _dolfin

# This file is compatible with both classic and new-style classes.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types


try:
    import weakref
    weakref_proxy = weakref.proxy
except:
    weakref_proxy = lambda x: x



def new_realArray(*args):
    """new_realArray(int nelements) -> real"""
    return _dolfin.new_realArray(*args)

def delete_realArray(*args):
    """delete_realArray(real ary)"""
    return _dolfin.delete_realArray(*args)

def realArray_getitem(*args):
    """realArray_getitem(real ary, int index) -> real"""
    return _dolfin.realArray_getitem(*args)

def realArray_setitem(*args):
    """realArray_setitem(real ary, int index, real value)"""
    return _dolfin.realArray_setitem(*args)

def new_intArray(*args):
    """new_intArray(int nelements) -> int"""
    return _dolfin.new_intArray(*args)

def delete_intArray(*args):
    """delete_intArray(int ary)"""
    return _dolfin.delete_intArray(*args)

def intArray_getitem(*args):
    """intArray_getitem(int ary, int index) -> int"""
    return _dolfin.intArray_getitem(*args)

def intArray_setitem(*args):
    """intArray_setitem(int ary, int index, int value)"""
    return _dolfin.intArray_setitem(*args)

def dolfin_init(*args):
    """dolfin_init(int argc, char argv)"""
    return _dolfin.dolfin_init(*args)

def sqr(*args):
    """sqr(real x) -> real"""
    return _dolfin.sqr(*args)

def ipow(*args):
    """ipow(uint a, uint n) -> uint"""
    return _dolfin.ipow(*args)

def rand(*args):
    """rand() -> real"""
    return _dolfin.rand(*args)

def seed(*args):
    """seed(unsigned int s)"""
    return _dolfin.seed(*args)
class TimeDependent(_object):
    """Proxy of C++ TimeDependent class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TimeDependent, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TimeDependent, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TimeDependent instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> TimeDependent
        __init__(self, real t) -> TimeDependent
        """
        _swig_setattr(self, TimeDependent, 'this', _dolfin.new_TimeDependent(*args))
        _swig_setattr(self, TimeDependent, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_TimeDependent):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def sync(*args):
        """sync(self, real t)"""
        return _dolfin.TimeDependent_sync(*args)

    def time(*args):
        """time(self) -> real"""
        return _dolfin.TimeDependent_time(*args)


class TimeDependentPtr(TimeDependent):
    def __init__(self, this):
        _swig_setattr(self, TimeDependent, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TimeDependent, 'thisown', 0)
        self.__class__ = TimeDependent
_dolfin.TimeDependent_swigregister(TimeDependentPtr)

class Variable(_object):
    """Proxy of C++ Variable class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Variable, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Variable, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Variable instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Variable
        __init__(self, string name, string label) -> Variable
        __init__(self, Variable variable) -> Variable
        """
        _swig_setattr(self, Variable, 'this', _dolfin.new_Variable(*args))
        _swig_setattr(self, Variable, 'thisown', 1)
    def rename(*args):
        """rename(self, string name, string label)"""
        return _dolfin.Variable_rename(*args)

    def name(*args):
        """name(self) -> string"""
        return _dolfin.Variable_name(*args)

    def label(*args):
        """label(self) -> string"""
        return _dolfin.Variable_label(*args)

    def number(*args):
        """number(self) -> int"""
        return _dolfin.Variable_number(*args)


class VariablePtr(Variable):
    def __init__(self, this):
        _swig_setattr(self, Variable, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Variable, 'thisown', 0)
        self.__class__ = Variable
_dolfin.Variable_swigregister(VariablePtr)


def suffix(*args):
    """suffix(char string, char suffix) -> bool"""
    return _dolfin.suffix(*args)

def remove_newline(*args):
    """remove_newline(char string)"""
    return _dolfin.remove_newline(*args)

def length(*args):
    """length(char string) -> int"""
    return _dolfin.length(*args)

def date(*args):
    """date() -> string"""
    return _dolfin.date(*args)

def delay(*args):
    """delay(real seconds)"""
    return _dolfin.delay(*args)

def tic(*args):
    """tic()"""
    return _dolfin.tic(*args)

def toc(*args):
    """toc() -> real"""
    return _dolfin.toc(*args)

def tocd(*args):
    """tocd() -> real"""
    return _dolfin.tocd(*args)

def dolfin_update(*args):
    """dolfin_update()"""
    return _dolfin.dolfin_update(*args)

def dolfin_quit(*args):
    """dolfin_quit()"""
    return _dolfin.dolfin_quit(*args)

def dolfin_finished(*args):
    """dolfin_finished() -> bool"""
    return _dolfin.dolfin_finished(*args)

def dolfin_segfault(*args):
    """dolfin_segfault()"""
    return _dolfin.dolfin_segfault(*args)

def dolfin_output(*args):
    """dolfin_output(char destination)"""
    return _dolfin.dolfin_output(*args)

def dolfin_log(*args):
    """dolfin_log(bool state)"""
    return _dolfin.dolfin_log(*args)
class Parameter(_object):
    """Proxy of C++ Parameter class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Parameter, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Parameter, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Parameter instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    type_real = _dolfin.Parameter_type_real
    type_int = _dolfin.Parameter_type_int
    type_bool = _dolfin.Parameter_type_bool
    type_string = _dolfin.Parameter_type_string
    def __init__(self, *args):
        """
        __init__(self, int value) -> Parameter
        __init__(self, uint value) -> Parameter
        __init__(self, real value) -> Parameter
        __init__(self, bool value) -> Parameter
        __init__(self, string value) -> Parameter
        __init__(self, char value) -> Parameter
        __init__(self, Parameter parameter) -> Parameter
        """
        _swig_setattr(self, Parameter, 'this', _dolfin.new_Parameter(*args))
        _swig_setattr(self, Parameter, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Parameter):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def type(*args):
        """type(self) -> int"""
        return _dolfin.Parameter_type(*args)


class ParameterPtr(Parameter):
    def __init__(self, this):
        _swig_setattr(self, Parameter, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Parameter, 'thisown', 0)
        self.__class__ = Parameter
_dolfin.Parameter_swigregister(ParameterPtr)

def dolfin_begin(*args):
    """
    dolfin_begin()
    dolfin_begin(char msg, v(...) ??)
    """
    return _dolfin.dolfin_begin(*args)

def dolfin_end(*args):
    """
    dolfin_end()
    dolfin_end(char msg, v(...) ??)
    """
    return _dolfin.dolfin_end(*args)

class File(_object):
    """Proxy of C++ File class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, File, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, File, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::File instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    xml = _dolfin.File_xml
    matlab = _dolfin.File_matlab
    matrixmarket = _dolfin.File_matrixmarket
    octave = _dolfin.File_octave
    opendx = _dolfin.File_opendx
    gid = _dolfin.File_gid
    tecplot = _dolfin.File_tecplot
    vtk = _dolfin.File_vtk
    python = _dolfin.File_python
    def __init__(self, *args):
        """
        __init__(self, string filename) -> File
        __init__(self, string filename, Type type) -> File
        """
        _swig_setattr(self, File, 'this', _dolfin.new_File(*args))
        _swig_setattr(self, File, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_File):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def __rshift__(*args):
        """
        __rshift__(self, Vector x)
        __rshift__(self, Matrix A)
        __rshift__(self, Mesh mesh)
        __rshift__(self, NewMesh mesh)
        __rshift__(self, Function u)
        __rshift__(self, Sample sample)
        __rshift__(self, FiniteElementSpec spec)
        __rshift__(self, ParameterList parameters)
        __rshift__(self, BLASFormData blas)
        """
        return _dolfin.File___rshift__(*args)

    def __lshift__(*args):
        """
        __lshift__(self, Vector x)
        __lshift__(self, Matrix A)
        __lshift__(self, Mesh mesh)
        __lshift__(self, NewMesh mesh)
        __lshift__(self, Function u)
        __lshift__(self, Sample sample)
        __lshift__(self, FiniteElementSpec spec)
        __lshift__(self, ParameterList parameters)
        __lshift__(self, BLASFormData blas)
        """
        return _dolfin.File___lshift__(*args)


class FilePtr(File):
    def __init__(self, this):
        _swig_setattr(self, File, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, File, 'thisown', 0)
        self.__class__ = File
_dolfin.File_swigregister(FilePtr)

class ublas_dense_matrix(_object):
    """Proxy of C++ ublas_dense_matrix class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ublas_dense_matrix, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ublas_dense_matrix, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::ublas_dense_matrix instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ublas_dense_matrixPtr(ublas_dense_matrix):
    def __init__(self, this):
        _swig_setattr(self, ublas_dense_matrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ublas_dense_matrix, 'thisown', 0)
        self.__class__ = ublas_dense_matrix
_dolfin.ublas_dense_matrix_swigregister(ublas_dense_matrixPtr)

class ublas_sparse_matrix(_object):
    """Proxy of C++ ublas_sparse_matrix class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ublas_sparse_matrix, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ublas_sparse_matrix, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::ublas_sparse_matrix instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)

class ublas_sparse_matrixPtr(ublas_sparse_matrix):
    def __init__(self, this):
        _swig_setattr(self, ublas_sparse_matrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ublas_sparse_matrix, 'thisown', 0)
        self.__class__ = ublas_sparse_matrix
_dolfin.ublas_sparse_matrix_swigregister(ublas_sparse_matrixPtr)

class GenericMatrix(_object):
    """Proxy of C++ GenericMatrix class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, GenericMatrix, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, GenericMatrix, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::GenericMatrix instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_GenericMatrix):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """
        init(self, uint M, uint N)
        init(self, uint M, uint N, uint nzmax)
        """
        return _dolfin.GenericMatrix_init(*args)

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.GenericMatrix_size(*args)

    def get(*args):
        """get(self, uint i, uint j) -> real"""
        return _dolfin.GenericMatrix_get(*args)

    def set(*args):
        """
        set(self, uint i, uint j, real value)
        set(self, real block, int rows, int m, int cols, int n)
        """
        return _dolfin.GenericMatrix_set(*args)

    def add(*args):
        """add(self, real block, int rows, int m, int cols, int n)"""
        return _dolfin.GenericMatrix_add(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.GenericMatrix_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.GenericMatrix_zero(*args)

    def ident(*args):
        """ident(self, int rows, int m)"""
        return _dolfin.GenericMatrix_ident(*args)

    def nzmax(*args):
        """nzmax(self) -> uint"""
        return _dolfin.GenericMatrix_nzmax(*args)


class GenericMatrixPtr(GenericMatrix):
    def __init__(self, this):
        _swig_setattr(self, GenericMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, GenericMatrix, 'thisown', 0)
        self.__class__ = GenericMatrix
_dolfin.GenericMatrix_swigregister(GenericMatrixPtr)

class GenericVector(_object):
    """Proxy of C++ GenericVector class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, GenericVector, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, GenericVector, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::GenericVector instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_GenericVector):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """init(self, uint N)"""
        return _dolfin.GenericVector_init(*args)

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.GenericVector_size(*args)

    def get(*args):
        """get(self, uint i) -> real"""
        return _dolfin.GenericVector_get(*args)

    def set(*args):
        """
        set(self, uint i, real value)
        set(self, real block, int pos, int n)
        """
        return _dolfin.GenericVector_set(*args)

    def add(*args):
        """add(self, real block, int pos, int n)"""
        return _dolfin.GenericVector_add(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.GenericVector_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.GenericVector_zero(*args)


class GenericVectorPtr(GenericVector):
    def __init__(self, this):
        _swig_setattr(self, GenericVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, GenericVector, 'thisown', 0)
        self.__class__ = GenericVector
_dolfin.GenericVector_swigregister(GenericVectorPtr)

class GMRES(_object):
    """Proxy of C++ GMRES class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, GMRES, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, GMRES, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::GMRES instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def solve(*args):
        """
        solve(PETScMatrix A, PETScVector x, PETScVector b, Preconditioner pc=default_pc) -> uint
        solve(PETScMatrix A, PETScVector x, PETScVector b) -> uint
        solve(PETScKrylovMatrix A, PETScVector x, PETScVector b, 
            Preconditioner pc=default_pc) -> uint
        solve(PETScKrylovMatrix A, PETScVector x, PETScVector b) -> uint
        solve(PETScMatrix A, PETScVector x, PETScVector b, PETScPreconditioner pc) -> uint
        solve(PETScKrylovMatrix A, PETScVector x, PETScVector b, 
            PETScPreconditioner pc) -> uint
        solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
            uBlasVector b, Preconditioner pc=default_pc) -> uint
        solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
            uBlasVector b) -> uint
        solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
            uBlasVector b, Preconditioner pc=default_pc) -> uint
        solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
            uBlasVector b) -> uint
        solve(uBlasKrylovMatrix A, uBlasVector x, uBlasVector b, 
            Preconditioner pc=default_pc) -> uint
        solve(uBlasKrylovMatrix A, uBlasVector x, uBlasVector b) -> uint
        solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
            uBlasVector b, uBlasPreconditioner pc) -> uint
        solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
            uBlasVector b, uBlasPreconditioner pc) -> uint
        solve(uBlasKrylovMatrix A, uBlasVector x, uBlasVector b, 
            uBlasPreconditioner pc) -> uint
        """
        return _dolfin.GMRES_solve(*args)

    if _newclass:solve = staticmethod(solve)
    __swig_getmethods__["solve"] = lambda x: solve

class GMRESPtr(GMRES):
    def __init__(self, this):
        _swig_setattr(self, GMRES, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, GMRES, 'thisown', 0)
        self.__class__ = GMRES
_dolfin.GMRES_swigregister(GMRESPtr)

def GMRES_solve(*args):
    """
    solve(PETScMatrix A, PETScVector x, PETScVector b, Preconditioner pc=default_pc) -> uint
    solve(PETScMatrix A, PETScVector x, PETScVector b) -> uint
    solve(PETScKrylovMatrix A, PETScVector x, PETScVector b, 
        Preconditioner pc=default_pc) -> uint
    solve(PETScKrylovMatrix A, PETScVector x, PETScVector b) -> uint
    solve(PETScMatrix A, PETScVector x, PETScVector b, PETScPreconditioner pc) -> uint
    solve(PETScKrylovMatrix A, PETScVector x, PETScVector b, 
        PETScPreconditioner pc) -> uint
    solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
        uBlasVector b, Preconditioner pc=default_pc) -> uint
    solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
        uBlasVector b) -> uint
    solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
        uBlasVector b, Preconditioner pc=default_pc) -> uint
    solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
        uBlasVector b) -> uint
    solve(uBlasKrylovMatrix A, uBlasVector x, uBlasVector b, 
        Preconditioner pc=default_pc) -> uint
    solve(uBlasKrylovMatrix A, uBlasVector x, uBlasVector b) -> uint
    solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
        uBlasVector b, uBlasPreconditioner pc) -> uint
    solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
        uBlasVector b, uBlasPreconditioner pc) -> uint
    GMRES_solve(uBlasKrylovMatrix A, uBlasVector x, uBlasVector b, 
        uBlasPreconditioner pc) -> uint
    """
    return _dolfin.GMRES_solve(*args)

class LU(_object):
    """Proxy of C++ LU class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, LU, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, LU, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::LU instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def solve(*args):
        """
        solve(PETScMatrix A, PETScVector x, PETScVector b)
        solve(PETScKrylovMatrix A, PETScVector x, PETScVector b)
        solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
            uBlasVector b)
        solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
            uBlasVector b)
        """
        return _dolfin.LU_solve(*args)

    if _newclass:solve = staticmethod(solve)
    __swig_getmethods__["solve"] = lambda x: solve

class LUPtr(LU):
    def __init__(self, this):
        _swig_setattr(self, LU, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, LU, 'thisown', 0)
        self.__class__ = LU
_dolfin.LU_swigregister(LUPtr)

def LU_solve(*args):
    """
    solve(PETScMatrix A, PETScVector x, PETScVector b)
    solve(PETScKrylovMatrix A, PETScVector x, PETScVector b)
    solve(uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
        uBlasVector b)
    LU_solve(uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
        uBlasVector b)
    """
    return _dolfin.LU_solve(*args)

class PETScKrylovMatrix(_object):
    """Proxy of C++ PETScKrylovMatrix class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScKrylovMatrix, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScKrylovMatrix, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScKrylovMatrix instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_PETScKrylovMatrix):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """
        init(self, PETScVector x, PETScVector y)
        init(self, int M, int N)
        """
        return _dolfin.PETScKrylovMatrix_init(*args)

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.PETScKrylovMatrix_size(*args)

    def mat(*args):
        """mat(self) -> Mat"""
        return _dolfin.PETScKrylovMatrix_mat(*args)

    def mult(*args):
        """mult(self, PETScVector x, PETScVector y)"""
        return _dolfin.PETScKrylovMatrix_mult(*args)

    def disp(*args):
        """
        disp(self, bool sparse=True, int precision=2)
        disp(self, bool sparse=True)
        disp(self)
        """
        return _dolfin.PETScKrylovMatrix_disp(*args)


class PETScKrylovMatrixPtr(PETScKrylovMatrix):
    def __init__(self, this):
        _swig_setattr(self, PETScKrylovMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScKrylovMatrix, 'thisown', 0)
        self.__class__ = PETScKrylovMatrix
_dolfin.PETScKrylovMatrix_swigregister(PETScKrylovMatrixPtr)

class PETScKrylovSolver(_object):
    """Proxy of C++ PETScKrylovSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScKrylovSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScKrylovSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScKrylovSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, KrylovMethod method=default_method, Preconditioner pc=default_pc) -> PETScKrylovSolver
        __init__(self, KrylovMethod method=default_method) -> PETScKrylovSolver
        __init__(self) -> PETScKrylovSolver
        __init__(self, Preconditioner pc) -> PETScKrylovSolver
        __init__(self, PETScPreconditioner PETScPreconditioner) -> PETScKrylovSolver
        __init__(self, KrylovMethod method, PETScPreconditioner PETScPreconditioner) -> PETScKrylovSolver
        """
        _swig_setattr(self, PETScKrylovSolver, 'this', _dolfin.new_PETScKrylovSolver(*args))
        _swig_setattr(self, PETScKrylovSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_PETScKrylovSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """
        solve(self, PETScMatrix A, PETScVector x, PETScVector b) -> uint
        solve(self, PETScKrylovMatrix A, PETScVector x, PETScVector b) -> uint
        """
        return _dolfin.PETScKrylovSolver_solve(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.PETScKrylovSolver_disp(*args)


class PETScKrylovSolverPtr(PETScKrylovSolver):
    def __init__(self, this):
        _swig_setattr(self, PETScKrylovSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScKrylovSolver, 'thisown', 0)
        self.__class__ = PETScKrylovSolver
_dolfin.PETScKrylovSolver_swigregister(PETScKrylovSolverPtr)

class PETScLinearSolver(_object):
    """Proxy of C++ PETScLinearSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScLinearSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScLinearSolver, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScLinearSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_PETScLinearSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(self, PETScMatrix A, PETScVector x, PETScVector b) -> uint"""
        return _dolfin.PETScLinearSolver_solve(*args)


class PETScLinearSolverPtr(PETScLinearSolver):
    def __init__(self, this):
        _swig_setattr(self, PETScLinearSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScLinearSolver, 'thisown', 0)
        self.__class__ = PETScLinearSolver
_dolfin.PETScLinearSolver_swigregister(PETScLinearSolverPtr)

class PETScLUSolver(PETScLinearSolver):
    """Proxy of C++ PETScLUSolver class"""
    __swig_setmethods__ = {}
    for _s in [PETScLinearSolver]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScLUSolver, name, value)
    __swig_getmethods__ = {}
    for _s in [PETScLinearSolver]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, PETScLUSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScLUSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> PETScLUSolver"""
        _swig_setattr(self, PETScLUSolver, 'this', _dolfin.new_PETScLUSolver(*args))
        _swig_setattr(self, PETScLUSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_PETScLUSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """
        solve(self, PETScMatrix A, PETScVector x, PETScVector b) -> uint
        solve(self, PETScKrylovMatrix A, PETScVector x, PETScVector b) -> uint
        """
        return _dolfin.PETScLUSolver_solve(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.PETScLUSolver_disp(*args)


class PETScLUSolverPtr(PETScLUSolver):
    def __init__(self, this):
        _swig_setattr(self, PETScLUSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScLUSolver, 'thisown', 0)
        self.__class__ = PETScLUSolver
_dolfin.PETScLUSolver_swigregister(PETScLUSolverPtr)

class PETScManager(_object):
    """Proxy of C++ PETScManager class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScManager, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScManager, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScManager instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def init(*args):
        """
        init()
        init(int argc, char argv)
        """
        return _dolfin.PETScManager_init(*args)

    if _newclass:init = staticmethod(init)
    __swig_getmethods__["init"] = lambda x: init

class PETScManagerPtr(PETScManager):
    def __init__(self, this):
        _swig_setattr(self, PETScManager, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScManager, 'thisown', 0)
        self.__class__ = PETScManager
_dolfin.PETScManager_swigregister(PETScManagerPtr)

def PETScManager_init(*args):
    """
    init()
    PETScManager_init(int argc, char argv)
    """
    return _dolfin.PETScManager_init(*args)

class PETScMatrix(GenericMatrix,Variable):
    """Proxy of C++ PETScMatrix class"""
    __swig_setmethods__ = {}
    for _s in [GenericMatrix,Variable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScMatrix, name, value)
    __swig_getmethods__ = {}
    for _s in [GenericMatrix,Variable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, PETScMatrix, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScMatrix instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    default_matrix = _dolfin.PETScMatrix_default_matrix
    spooles = _dolfin.PETScMatrix_spooles
    superlu = _dolfin.PETScMatrix_superlu
    umfpack = _dolfin.PETScMatrix_umfpack
    def __init__(self, *args):
        """
        __init__(self) -> PETScMatrix
        __init__(self, Type type) -> PETScMatrix
        __init__(self, Mat A) -> PETScMatrix
        __init__(self, uint M, uint N) -> PETScMatrix
        __init__(self, uint M, uint N, Type type) -> PETScMatrix
        __init__(self, PETScMatrix B) -> PETScMatrix
        """
        _swig_setattr(self, PETScMatrix, 'this', _dolfin.new_PETScMatrix(*args))
        _swig_setattr(self, PETScMatrix, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_PETScMatrix):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """
        init(self, uint M, uint N)
        init(self, uint M, uint N, uint nzmax)
        init(self, uint M, uint N, uint bs, uint nzmax)
        """
        return _dolfin.PETScMatrix_init(*args)

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.PETScMatrix_size(*args)

    def nz(*args):
        """nz(self, uint row) -> uint"""
        return _dolfin.PETScMatrix_nz(*args)

    def nzsum(*args):
        """nzsum(self) -> uint"""
        return _dolfin.PETScMatrix_nzsum(*args)

    def nzmax(*args):
        """nzmax(self) -> uint"""
        return _dolfin.PETScMatrix_nzmax(*args)

    def get(*args):
        """get(self, uint i, uint j) -> real"""
        return _dolfin.PETScMatrix_get(*args)

    def set(*args):
        """
        set(self, uint i, uint j, real value)
        set(self, real block, int rows, int m, int cols, int n)
        """
        return _dolfin.PETScMatrix_set(*args)

    def add(*args):
        """
        add(self, uint i, uint j, real value)
        add(self, real block, int rows, int m, int cols, int n)
        """
        return _dolfin.PETScMatrix_add(*args)

    def getRow(*args):
        """getRow(self, uint i, int ncols, dolfin::Array<(int)> columns, dolfin::Array<(dolfin::real)> values)"""
        return _dolfin.PETScMatrix_getRow(*args)

    def ident(*args):
        """ident(self, int rows, int m)"""
        return _dolfin.PETScMatrix_ident(*args)

    def mult(*args):
        """
        mult(self, PETScVector x, PETScVector Ax)
        mult(self, PETScVector x, uint row) -> real
        mult(self, real x, uint row) -> real
        """
        return _dolfin.PETScMatrix_mult(*args)

    def lump(*args):
        """lump(self, PETScVector m)"""
        return _dolfin.PETScMatrix_lump(*args)

    l1 = _dolfin.PETScMatrix_l1
    linf = _dolfin.PETScMatrix_linf
    frobenius = _dolfin.PETScMatrix_frobenius
    def norm(*args):
        """
        norm(self, Norm type=l1) -> real
        norm(self) -> real
        """
        return _dolfin.PETScMatrix_norm(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.PETScMatrix_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.PETScMatrix_zero(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.PETScMatrix_type(*args)

    def mat(*args):
        """mat(self) -> Mat"""
        return _dolfin.PETScMatrix_mat(*args)

    def disp(*args):
        """
        disp(self, bool sparse=True, int precision=2)
        disp(self, bool sparse=True)
        disp(self)
        """
        return _dolfin.PETScMatrix_disp(*args)

    def __call__(*args):
        """
        __call__(self, uint i, uint j) -> real
        __call__(self, uint i, uint j) -> PETScMatrixElement
        """
        return _dolfin.PETScMatrix___call__(*args)


class PETScMatrixPtr(PETScMatrix):
    def __init__(self, this):
        _swig_setattr(self, PETScMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScMatrix, 'thisown', 0)
        self.__class__ = PETScMatrix
_dolfin.PETScMatrix_swigregister(PETScMatrixPtr)

class PETScMatrixElement(_object):
    """Proxy of C++ PETScMatrixElement class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScMatrixElement, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScMatrixElement, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScMatrixElement instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, uint i, uint j, PETScMatrix A) -> PETScMatrixElement
        __init__(self, PETScMatrixElement e) -> PETScMatrixElement
        """
        _swig_setattr(self, PETScMatrixElement, 'this', _dolfin.new_PETScMatrixElement(*args))
        _swig_setattr(self, PETScMatrixElement, 'thisown', 1)
    def __iadd__(*args):
        """__iadd__(self, real a) -> PETScMatrixElement"""
        return _dolfin.PETScMatrixElement___iadd__(*args)

    def __isub__(*args):
        """__isub__(self, real a) -> PETScMatrixElement"""
        return _dolfin.PETScMatrixElement___isub__(*args)

    def __imul__(*args):
        """__imul__(self, real a) -> PETScMatrixElement"""
        return _dolfin.PETScMatrixElement___imul__(*args)


class PETScMatrixElementPtr(PETScMatrixElement):
    def __init__(self, this):
        _swig_setattr(self, PETScMatrixElement, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScMatrixElement, 'thisown', 0)
        self.__class__ = PETScMatrixElement
_dolfin.PETScMatrixElement_swigregister(PETScMatrixElementPtr)

class PETScPreconditioner(_object):
    """Proxy of C++ PETScPreconditioner class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScPreconditioner, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScPreconditioner, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScPreconditioner instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_PETScPreconditioner):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def setup(*args):
        """setup(KSP ksp, PETScPreconditioner pc)"""
        return _dolfin.PETScPreconditioner_setup(*args)

    if _newclass:setup = staticmethod(setup)
    __swig_getmethods__["setup"] = lambda x: setup
    def solve(*args):
        """solve(self, PETScVector x, PETScVector b)"""
        return _dolfin.PETScPreconditioner_solve(*args)


class PETScPreconditionerPtr(PETScPreconditioner):
    def __init__(self, this):
        _swig_setattr(self, PETScPreconditioner, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScPreconditioner, 'thisown', 0)
        self.__class__ = PETScPreconditioner
_dolfin.PETScPreconditioner_swigregister(PETScPreconditionerPtr)

def PETScPreconditioner_setup(*args):
    """PETScPreconditioner_setup(KSP ksp, PETScPreconditioner pc)"""
    return _dolfin.PETScPreconditioner_setup(*args)

class PETScVector(GenericVector,Variable):
    """Proxy of C++ PETScVector class"""
    __swig_setmethods__ = {}
    for _s in [GenericVector,Variable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScVector, name, value)
    __swig_getmethods__ = {}
    for _s in [GenericVector,Variable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, PETScVector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScVector instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> PETScVector
        __init__(self, uint size) -> PETScVector
        __init__(self, Vec x) -> PETScVector
        __init__(self, PETScVector x) -> PETScVector
        """
        _swig_setattr(self, PETScVector, 'this', _dolfin.new_PETScVector(*args))
        _swig_setattr(self, PETScVector, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_PETScVector):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """init(self, uint size)"""
        return _dolfin.PETScVector_init(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.PETScVector_clear(*args)

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.PETScVector_size(*args)

    def vec(*args):
        """vec(self) -> Vec"""
        return _dolfin.PETScVector_vec(*args)

    def array(*args):
        """
        array(self) -> real
        array(self) -> real
        """
        return _dolfin.PETScVector_array(*args)

    def restore(*args):
        """
        restore(self, real data)
        restore(self, real data)
        """
        return _dolfin.PETScVector_restore(*args)

    def axpy(*args):
        """axpy(self, real a, PETScVector x)"""
        return _dolfin.PETScVector_axpy(*args)

    def div(*args):
        """div(self, PETScVector x)"""
        return _dolfin.PETScVector_div(*args)

    def mult(*args):
        """mult(self, PETScVector x)"""
        return _dolfin.PETScVector_mult(*args)

    def set(*args):
        """
        set(self, uint i, real value)
        set(self, real block, int pos, int n)
        """
        return _dolfin.PETScVector_set(*args)

    def add(*args):
        """
        add(self, uint i, real value)
        add(self, real block, int pos, int n)
        """
        return _dolfin.PETScVector_add(*args)

    def get(*args):
        """
        get(self, uint i) -> real
        get(self, real block, int cols, int n)
        """
        return _dolfin.PETScVector_get(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.PETScVector_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.PETScVector_zero(*args)

    def __call__(*args):
        """
        __call__(self, uint i) -> PETScVectorElement
        __call__(self, uint i) -> real
        """
        return _dolfin.PETScVector___call__(*args)

    def copy(*args):
        """
        copy(self, PETScVector x) -> PETScVector
        copy(self, real a) -> PETScVector
        """
        return _dolfin.PETScVector_copy(*args)

    def __iadd__(*args):
        """__iadd__(self, PETScVector x) -> PETScVector"""
        return _dolfin.PETScVector___iadd__(*args)

    def __isub__(*args):
        """__isub__(self, PETScVector x) -> PETScVector"""
        return _dolfin.PETScVector___isub__(*args)

    def __imul__(*args):
        """__imul__(self, real a) -> PETScVector"""
        return _dolfin.PETScVector___imul__(*args)

    def __idiv__(*args):
        """__idiv__(self, real a) -> PETScVector"""
        return _dolfin.PETScVector___idiv__(*args)

    def __mul__(*args):
        """__mul__(self, PETScVector x) -> real"""
        return _dolfin.PETScVector___mul__(*args)

    l1 = _dolfin.PETScVector_l1
    l2 = _dolfin.PETScVector_l2
    linf = _dolfin.PETScVector_linf
    def norm(*args):
        """
        norm(self, NormType type=l2) -> real
        norm(self) -> real
        """
        return _dolfin.PETScVector_norm(*args)

    def sum(*args):
        """sum(self) -> real"""
        return _dolfin.PETScVector_sum(*args)

    def max(*args):
        """max(self) -> real"""
        return _dolfin.PETScVector_max(*args)

    def min(*args):
        """min(self) -> real"""
        return _dolfin.PETScVector_min(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.PETScVector_disp(*args)

    def createScatterer(*args):
        """createScatterer(PETScVector x1, PETScVector x2, int offset, int size) -> VecScatter"""
        return _dolfin.PETScVector_createScatterer(*args)

    if _newclass:createScatterer = staticmethod(createScatterer)
    __swig_getmethods__["createScatterer"] = lambda x: createScatterer
    def gather(*args):
        """gather(PETScVector x1, PETScVector x2, VecScatter x1sc)"""
        return _dolfin.PETScVector_gather(*args)

    if _newclass:gather = staticmethod(gather)
    __swig_getmethods__["gather"] = lambda x: gather
    def scatter(*args):
        """scatter(PETScVector x1, PETScVector x2, VecScatter x1sc)"""
        return _dolfin.PETScVector_scatter(*args)

    if _newclass:scatter = staticmethod(scatter)
    __swig_getmethods__["scatter"] = lambda x: scatter
    def fromArray(*args):
        """fromArray(real u, PETScVector x, uint offset, uint size)"""
        return _dolfin.PETScVector_fromArray(*args)

    if _newclass:fromArray = staticmethod(fromArray)
    __swig_getmethods__["fromArray"] = lambda x: fromArray
    def toArray(*args):
        """toArray(real y, PETScVector x, uint offset, uint size)"""
        return _dolfin.PETScVector_toArray(*args)

    if _newclass:toArray = staticmethod(toArray)
    __swig_getmethods__["toArray"] = lambda x: toArray

class PETScVectorPtr(PETScVector):
    def __init__(self, this):
        _swig_setattr(self, PETScVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScVector, 'thisown', 0)
        self.__class__ = PETScVector
_dolfin.PETScVector_swigregister(PETScVectorPtr)

def PETScVector_createScatterer(*args):
    """PETScVector_createScatterer(PETScVector x1, PETScVector x2, int offset, int size) -> VecScatter"""
    return _dolfin.PETScVector_createScatterer(*args)

def PETScVector_gather(*args):
    """PETScVector_gather(PETScVector x1, PETScVector x2, VecScatter x1sc)"""
    return _dolfin.PETScVector_gather(*args)

def PETScVector_scatter(*args):
    """PETScVector_scatter(PETScVector x1, PETScVector x2, VecScatter x1sc)"""
    return _dolfin.PETScVector_scatter(*args)

def PETScVector_fromArray(*args):
    """PETScVector_fromArray(real u, PETScVector x, uint offset, uint size)"""
    return _dolfin.PETScVector_fromArray(*args)

def PETScVector_toArray(*args):
    """PETScVector_toArray(real y, PETScVector x, uint offset, uint size)"""
    return _dolfin.PETScVector_toArray(*args)

class PETScVectorElement(_object):
    """Proxy of C++ PETScVectorElement class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, PETScVectorElement, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, PETScVectorElement, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::PETScVectorElement instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, uint i, PETScVector x) -> PETScVectorElement
        __init__(self, PETScVectorElement e) -> PETScVectorElement
        """
        _swig_setattr(self, PETScVectorElement, 'this', _dolfin.new_PETScVectorElement(*args))
        _swig_setattr(self, PETScVectorElement, 'thisown', 1)
    def __iadd__(*args):
        """__iadd__(self, real a) -> PETScVectorElement"""
        return _dolfin.PETScVectorElement___iadd__(*args)

    def __isub__(*args):
        """__isub__(self, real a) -> PETScVectorElement"""
        return _dolfin.PETScVectorElement___isub__(*args)

    def __imul__(*args):
        """__imul__(self, real a) -> PETScVectorElement"""
        return _dolfin.PETScVectorElement___imul__(*args)


class PETScVectorElementPtr(PETScVectorElement):
    def __init__(self, this):
        _swig_setattr(self, PETScVectorElement, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, PETScVectorElement, 'thisown', 0)
        self.__class__ = PETScVectorElement
_dolfin.PETScVectorElement_swigregister(PETScVectorElementPtr)

class uBlasDummyPreconditioner(_object):
    """Proxy of C++ uBlasDummyPreconditioner class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasDummyPreconditioner, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasDummyPreconditioner, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasDummyPreconditioner instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> uBlasDummyPreconditioner"""
        _swig_setattr(self, uBlasDummyPreconditioner, 'this', _dolfin.new_uBlasDummyPreconditioner(*args))
        _swig_setattr(self, uBlasDummyPreconditioner, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasDummyPreconditioner):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(self, uBlasVector x, uBlasVector b)"""
        return _dolfin.uBlasDummyPreconditioner_solve(*args)


class uBlasDummyPreconditionerPtr(uBlasDummyPreconditioner):
    def __init__(self, this):
        _swig_setattr(self, uBlasDummyPreconditioner, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasDummyPreconditioner, 'thisown', 0)
        self.__class__ = uBlasDummyPreconditioner
_dolfin.uBlasDummyPreconditioner_swigregister(uBlasDummyPreconditionerPtr)

class uBlasKrylovMatrix(_object):
    """Proxy of C++ uBlasKrylovMatrix class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasKrylovMatrix, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasKrylovMatrix, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasKrylovMatrix instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_uBlasKrylovMatrix):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.uBlasKrylovMatrix_size(*args)

    def mult(*args):
        """mult(self, uBlasVector x, uBlasVector y)"""
        return _dolfin.uBlasKrylovMatrix_mult(*args)


class uBlasKrylovMatrixPtr(uBlasKrylovMatrix):
    def __init__(self, this):
        _swig_setattr(self, uBlasKrylovMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasKrylovMatrix, 'thisown', 0)
        self.__class__ = uBlasKrylovMatrix
_dolfin.uBlasKrylovMatrix_swigregister(uBlasKrylovMatrixPtr)

class uBlasKrylovSolver(_object):
    """Proxy of C++ uBlasKrylovSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasKrylovSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasKrylovSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasKrylovSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, KrylovMethod method=default_method, Preconditioner pc=default_pc) -> uBlasKrylovSolver
        __init__(self, KrylovMethod method=default_method) -> uBlasKrylovSolver
        __init__(self) -> uBlasKrylovSolver
        __init__(self, Preconditioner pc) -> uBlasKrylovSolver
        __init__(self, uBlasPreconditioner pc) -> uBlasKrylovSolver
        __init__(self, KrylovMethod method, uBlasPreconditioner preconditioner) -> uBlasKrylovSolver
        """
        _swig_setattr(self, uBlasKrylovSolver, 'this', _dolfin.new_uBlasKrylovSolver(*args))
        _swig_setattr(self, uBlasKrylovSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasKrylovSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """
        solve(self, uBlasMatrix<(dolfin::ublas_dense_matrix)> A, uBlasVector x, 
            uBlasVector b) -> uint
        solve(self, uBlasMatrix<(dolfin::ublas_sparse_matrix)> A, uBlasVector x, 
            uBlasVector b) -> uint
        solve(self, uBlasKrylovMatrix A, uBlasVector x, uBlasVector b) -> uint
        """
        return _dolfin.uBlasKrylovSolver_solve(*args)


class uBlasKrylovSolverPtr(uBlasKrylovSolver):
    def __init__(self, this):
        _swig_setattr(self, uBlasKrylovSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasKrylovSolver, 'thisown', 0)
        self.__class__ = uBlasKrylovSolver
_dolfin.uBlasKrylovSolver_swigregister(uBlasKrylovSolverPtr)

class uBlasLinearSolver(_object):
    """Proxy of C++ uBlasLinearSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasLinearSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasLinearSolver, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasLinearSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_uBlasLinearSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """
        solve(self, uBlasDenseMatrix A, uBlasVector x, uBlasVector b) -> uint
        solve(self, uBlasSparseMatrix A, uBlasVector x, uBlasVector b) -> uint
        """
        return _dolfin.uBlasLinearSolver_solve(*args)


class uBlasLinearSolverPtr(uBlasLinearSolver):
    def __init__(self, this):
        _swig_setattr(self, uBlasLinearSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasLinearSolver, 'thisown', 0)
        self.__class__ = uBlasLinearSolver
_dolfin.uBlasLinearSolver_swigregister(uBlasLinearSolverPtr)

class uBlasLUSolver(uBlasLinearSolver):
    """Proxy of C++ uBlasLUSolver class"""
    __swig_setmethods__ = {}
    for _s in [uBlasLinearSolver]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasLUSolver, name, value)
    __swig_getmethods__ = {}
    for _s in [uBlasLinearSolver]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasLUSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasLUSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> uBlasLUSolver"""
        _swig_setattr(self, uBlasLUSolver, 'this', _dolfin.new_uBlasLUSolver(*args))
        _swig_setattr(self, uBlasLUSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasLUSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """
        solve(self, uBlasDenseMatrix A, uBlasVector x, uBlasVector b) -> uint
        solve(self, uBlasSparseMatrix A, uBlasVector x, uBlasVector b) -> uint
        solve(self, uBlasKrylovMatrix A, uBlasVector x, uBlasVector b)
        """
        return _dolfin.uBlasLUSolver_solve(*args)

    def solveInPlaceUBlas(*args):
        """solveInPlaceUBlas(self, uBlasDenseMatrix A, uBlasVector x, uBlasVector b) -> uint"""
        return _dolfin.uBlasLUSolver_solveInPlaceUBlas(*args)


class uBlasLUSolverPtr(uBlasLUSolver):
    def __init__(self, this):
        _swig_setattr(self, uBlasLUSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasLUSolver, 'thisown', 0)
        self.__class__ = uBlasLUSolver
_dolfin.uBlasLUSolver_swigregister(uBlasLUSolverPtr)

class uBlasILUPreconditioner(_object):
    """Proxy of C++ uBlasILUPreconditioner class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasILUPreconditioner, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasILUPreconditioner, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasILUPreconditioner instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> uBlasILUPreconditioner
        __init__(self, uBlasSparseMatrix A) -> uBlasILUPreconditioner
        """
        _swig_setattr(self, uBlasILUPreconditioner, 'this', _dolfin.new_uBlasILUPreconditioner(*args))
        _swig_setattr(self, uBlasILUPreconditioner, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasILUPreconditioner):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(self, uBlasVector x, uBlasVector b)"""
        return _dolfin.uBlasILUPreconditioner_solve(*args)


class uBlasILUPreconditionerPtr(uBlasILUPreconditioner):
    def __init__(self, this):
        _swig_setattr(self, uBlasILUPreconditioner, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasILUPreconditioner, 'thisown', 0)
        self.__class__ = uBlasILUPreconditioner
_dolfin.uBlasILUPreconditioner_swigregister(uBlasILUPreconditionerPtr)

class uBlasPreconditioner(_object):
    """Proxy of C++ uBlasPreconditioner class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasPreconditioner, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasPreconditioner, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasPreconditioner instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_uBlasPreconditioner):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """
        init(self, uBlasDenseMatrix A)
        init(self, uBlasSparseMatrix A)
        init(self, uBlasKrylovMatrix A)
        """
        return _dolfin.uBlasPreconditioner_init(*args)

    def solve(*args):
        """solve(self, uBlasVector x, uBlasVector b)"""
        return _dolfin.uBlasPreconditioner_solve(*args)


class uBlasPreconditionerPtr(uBlasPreconditioner):
    def __init__(self, this):
        _swig_setattr(self, uBlasPreconditioner, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasPreconditioner, 'thisown', 0)
        self.__class__ = uBlasPreconditioner
_dolfin.uBlasPreconditioner_swigregister(uBlasPreconditionerPtr)

class uBlasVector(GenericVector,Variable):
    """Proxy of C++ uBlasVector class"""
    __swig_setmethods__ = {}
    for _s in [GenericVector,Variable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasVector, name, value)
    __swig_getmethods__ = {}
    for _s in [GenericVector,Variable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasVector, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasVector instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> uBlasVector
        __init__(self, uint N) -> uBlasVector
        """
        _swig_setattr(self, uBlasVector, 'this', _dolfin.new_uBlasVector(*args))
        _swig_setattr(self, uBlasVector, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasVector):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """init(self, uint N)"""
        return _dolfin.uBlasVector_init(*args)

    def copy(*args):
        """copy(self, real a) -> uBlasVector"""
        return _dolfin.uBlasVector_copy(*args)

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.uBlasVector_size(*args)

    def __call__(*args):
        """
        __call__(self, uint i) -> real
        __call__(self, uint i) -> real
        """
        return _dolfin.uBlasVector___call__(*args)

    def set(*args):
        """
        set(self, uint i, real value)
        set(self, real block, int pos, int n)
        """
        return _dolfin.uBlasVector_set(*args)

    def add(*args):
        """add(self, real block, int pos, int n)"""
        return _dolfin.uBlasVector_add(*args)

    def get(*args):
        """
        get(self, uint i) -> real
        get(self, real block, int pos, int n)
        """
        return _dolfin.uBlasVector_get(*args)

    l1 = _dolfin.uBlasVector_l1
    l2 = _dolfin.uBlasVector_l2
    linf = _dolfin.uBlasVector_linf
    def norm(*args):
        """
        norm(self, NormType type=l2) -> real
        norm(self) -> real
        """
        return _dolfin.uBlasVector_norm(*args)

    def sum(*args):
        """sum(self) -> real"""
        return _dolfin.uBlasVector_sum(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.uBlasVector_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.uBlasVector_zero(*args)

    def disp(*args):
        """
        disp(self, uint precision=2)
        disp(self)
        """
        return _dolfin.uBlasVector_disp(*args)


class uBlasVectorPtr(uBlasVector):
    def __init__(self, this):
        _swig_setattr(self, uBlasVector, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasVector, 'thisown', 0)
        self.__class__ = uBlasVector
_dolfin.uBlasVector_swigregister(uBlasVectorPtr)

class uBlasSparseMatrix(Variable,GenericMatrix,ublas_sparse_matrix):
    """Proxy of C++ uBlasSparseMatrix class"""
    __swig_setmethods__ = {}
    for _s in [Variable,GenericMatrix,ublas_sparse_matrix]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasSparseMatrix, name, value)
    __swig_getmethods__ = {}
    for _s in [Variable,GenericMatrix,ublas_sparse_matrix]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasSparseMatrix, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasMatrix<dolfin::ublas_sparse_matrix > instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> uBlasSparseMatrix
        __init__(self, uint M, uint N) -> uBlasSparseMatrix
        """
        _swig_setattr(self, uBlasSparseMatrix, 'this', _dolfin.new_uBlasSparseMatrix(*args))
        _swig_setattr(self, uBlasSparseMatrix, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasSparseMatrix):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.uBlasSparseMatrix_size(*args)

    def get(*args):
        """get(self, uint i, uint j) -> real"""
        return _dolfin.uBlasSparseMatrix_get(*args)

    def getRow(*args):
        """getRow(self, uint i, int ncols, dolfin::Array<(int)> columns, dolfin::Array<(dolfin::real)> values)"""
        return _dolfin.uBlasSparseMatrix_getRow(*args)

    def lump(*args):
        """lump(self, uBlasVector m)"""
        return _dolfin.uBlasSparseMatrix_lump(*args)

    def solve(*args):
        """solve(self, uBlasVector x, uBlasVector b)"""
        return _dolfin.uBlasSparseMatrix_solve(*args)

    def invert(*args):
        """invert(self)"""
        return _dolfin.uBlasSparseMatrix_invert(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.uBlasSparseMatrix_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.uBlasSparseMatrix_zero(*args)

    def ident(*args):
        """ident(self, int rows, int m)"""
        return _dolfin.uBlasSparseMatrix_ident(*args)

    def mult(*args):
        """mult(self, uBlasVector x, uBlasVector y)"""
        return _dolfin.uBlasSparseMatrix_mult(*args)

    def disp(*args):
        """
        disp(self, uint precision=2)
        disp(self)
        """
        return _dolfin.uBlasSparseMatrix_disp(*args)

    def init(*args):
        """
        init(self, uint M, uint N)
        init(self, uint M, uint N, uint nzmax)
        """
        return _dolfin.uBlasSparseMatrix_init(*args)

    def set(*args):
        """
        set(self, uint i, uint j, real value)
        set(self, real block, int rows, int m, int cols, int n)
        """
        return _dolfin.uBlasSparseMatrix_set(*args)

    def add(*args):
        """add(self, real block, int rows, int m, int cols, int n)"""
        return _dolfin.uBlasSparseMatrix_add(*args)

    def nzmax(*args):
        """nzmax(self) -> uint"""
        return _dolfin.uBlasSparseMatrix_nzmax(*args)


class uBlasSparseMatrixPtr(uBlasSparseMatrix):
    def __init__(self, this):
        _swig_setattr(self, uBlasSparseMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasSparseMatrix, 'thisown', 0)
        self.__class__ = uBlasSparseMatrix
_dolfin.uBlasSparseMatrix_swigregister(uBlasSparseMatrixPtr)

class uBlasDenseMatrix(Variable,GenericMatrix,ublas_dense_matrix):
    """Proxy of C++ uBlasDenseMatrix class"""
    __swig_setmethods__ = {}
    for _s in [Variable,GenericMatrix,ublas_dense_matrix]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, uBlasDenseMatrix, name, value)
    __swig_getmethods__ = {}
    for _s in [Variable,GenericMatrix,ublas_dense_matrix]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, uBlasDenseMatrix, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::uBlasMatrix<dolfin::ublas_dense_matrix > instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> uBlasDenseMatrix
        __init__(self, uint M, uint N) -> uBlasDenseMatrix
        """
        _swig_setattr(self, uBlasDenseMatrix, 'this', _dolfin.new_uBlasDenseMatrix(*args))
        _swig_setattr(self, uBlasDenseMatrix, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_uBlasDenseMatrix):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.uBlasDenseMatrix_size(*args)

    def get(*args):
        """get(self, uint i, uint j) -> real"""
        return _dolfin.uBlasDenseMatrix_get(*args)

    def getRow(*args):
        """getRow(self, uint i, int ncols, dolfin::Array<(int)> columns, dolfin::Array<(dolfin::real)> values)"""
        return _dolfin.uBlasDenseMatrix_getRow(*args)

    def lump(*args):
        """lump(self, uBlasVector m)"""
        return _dolfin.uBlasDenseMatrix_lump(*args)

    def solve(*args):
        """solve(self, uBlasVector x, uBlasVector b)"""
        return _dolfin.uBlasDenseMatrix_solve(*args)

    def invert(*args):
        """invert(self)"""
        return _dolfin.uBlasDenseMatrix_invert(*args)

    def apply(*args):
        """apply(self)"""
        return _dolfin.uBlasDenseMatrix_apply(*args)

    def zero(*args):
        """zero(self)"""
        return _dolfin.uBlasDenseMatrix_zero(*args)

    def ident(*args):
        """ident(self, int rows, int m)"""
        return _dolfin.uBlasDenseMatrix_ident(*args)

    def mult(*args):
        """mult(self, uBlasVector x, uBlasVector y)"""
        return _dolfin.uBlasDenseMatrix_mult(*args)

    def disp(*args):
        """
        disp(self, uint precision=2)
        disp(self)
        """
        return _dolfin.uBlasDenseMatrix_disp(*args)

    def init(*args):
        """
        init(self, uint M, uint N)
        init(self, uint M, uint N, uint nzmax)
        """
        return _dolfin.uBlasDenseMatrix_init(*args)

    def set(*args):
        """
        set(self, uint i, uint j, real value)
        set(self, real block, int rows, int m, int cols, int n)
        """
        return _dolfin.uBlasDenseMatrix_set(*args)

    def add(*args):
        """add(self, real block, int rows, int m, int cols, int n)"""
        return _dolfin.uBlasDenseMatrix_add(*args)

    def nzmax(*args):
        """nzmax(self) -> uint"""
        return _dolfin.uBlasDenseMatrix_nzmax(*args)


class uBlasDenseMatrixPtr(uBlasDenseMatrix):
    def __init__(self, this):
        _swig_setattr(self, uBlasDenseMatrix, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, uBlasDenseMatrix, 'thisown', 0)
        self.__class__ = uBlasDenseMatrix
_dolfin.uBlasDenseMatrix_swigregister(uBlasDenseMatrixPtr)

# Explicit typedefs
Vector = PETScVector
Matrix = PETScMatrix
KrylovSolver = PETScKrylovSolver

# Explicit typedefs
DenseVector = uBlasVector

def __getitem__(self, i):
    return self.get(i)
def __setitem__(self, i, val):
    self.set(i, val)

GenericVector.__getitem__ = __getitem__
GenericVector.__setitem__ = __setitem__

def __getitem__(self, i):
    return self.get(i[0], i[1])
def __setitem__(self, i, val):
    self.set(i[0], i[1], val)

GenericMatrix.__getitem__ = __getitem__
GenericMatrix.__setitem__ = __setitem__

class Function(Variable,TimeDependent):
    """Proxy of C++ Function class"""
    __swig_setmethods__ = {}
    for _s in [Variable,TimeDependent]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Function, name, value)
    __swig_getmethods__ = {}
    for _s in [Variable,TimeDependent]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Function, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Function instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, real value) -> Function
        __init__(self, uint vectordim=1) -> Function
        __init__(self) -> Function
        __init__(self, FunctionPointer fp, uint vectordim=1) -> Function
        __init__(self, FunctionPointer fp) -> Function
        __init__(self, Vector x) -> Function
        __init__(self, Vector x, Mesh mesh) -> Function
        __init__(self, Vector x, Mesh mesh, FiniteElement element) -> Function
        __init__(self, Mesh mesh, FiniteElement element) -> Function
        __init__(self, Function f) -> Function
        """
        if self.__class__ == Function:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, Function, 'this', _dolfin.new_Function(*args))
        _swig_setattr(self, Function, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Function):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def eval(*args):
        """
        eval(self, Point p, uint i=0) -> real
        eval(self, Point p) -> real
        """
        return _dolfin.Function_eval(*args)

    def __call__(*args):
        """
        __call__(self, Point p, uint i=0) -> real
        __call__(self, Point p) -> real
        __call__(self, Vertex vertex, uint i=0) -> real
        __call__(self, Vertex vertex) -> real
        """
        return _dolfin.Function___call__(*args)

    def __getitem__(*args):
        """__getitem__(self, uint i) -> Function"""
        return _dolfin.Function___getitem__(*args)

    def interpolate(*args):
        """interpolate(self, real coefficients, AffineMap map, FiniteElement element)"""
        return _dolfin.Function_interpolate(*args)

    def vectordim(*args):
        """vectordim(self) -> uint"""
        return _dolfin.Function_vectordim(*args)

    def vector(*args):
        """vector(self) -> Vector"""
        return _dolfin.Function_vector(*args)

    def mesh(*args):
        """mesh(self) -> Mesh"""
        return _dolfin.Function_mesh(*args)

    def element(*args):
        """element(self) -> FiniteElement"""
        return _dolfin.Function_element(*args)

    def attach(*args):
        """
        attach(self, Vector x, bool local=False)
        attach(self, Vector x)
        attach(self, Mesh mesh, bool local=False)
        attach(self, Mesh mesh)
        attach(self, FiniteElement element, bool local=False)
        attach(self, FiniteElement element)
        """
        return _dolfin.Function_attach(*args)

    def init(*args):
        """init(self, Mesh mesh, FiniteElement element)"""
        return _dolfin.Function_init(*args)

    constant = _dolfin.Function_constant
    user = _dolfin.Function_user
    functionpointer = _dolfin.Function_functionpointer
    discrete = _dolfin.Function_discrete
    def type(*args):
        """type(self) -> int"""
        return _dolfin.Function_type(*args)

    def __disown__(self):
        self.thisown = 0
        _dolfin.disown_Function(self)
        return weakref_proxy(self)

class FunctionPtr(Function):
    def __init__(self, this):
        _swig_setattr(self, Function, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Function, 'thisown', 0)
        self.__class__ = Function
_dolfin.Function_swigregister(FunctionPtr)

class Form(_object):
    """Proxy of C++ Form class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Form, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Form, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Form instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint num_functions) -> Form"""
        _swig_setattr(self, Form, 'this', _dolfin.new_Form(*args))
        _swig_setattr(self, Form, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Form):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def update(*args):
        """update(self, AffineMap map)"""
        return _dolfin.Form_update(*args)

    def function(*args):
        """function(self, uint i) -> Function"""
        return _dolfin.Form_function(*args)

    def element(*args):
        """element(self, uint i) -> FiniteElement"""
        return _dolfin.Form_element(*args)

    __swig_setmethods__["num_functions"] = _dolfin.Form_num_functions_set
    __swig_getmethods__["num_functions"] = _dolfin.Form_num_functions_get
    if _newclass:num_functions = property(_dolfin.Form_num_functions_get, _dolfin.Form_num_functions_set)

class FormPtr(Form):
    def __init__(self, this):
        _swig_setattr(self, Form, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Form, 'thisown', 0)
        self.__class__ = Form
_dolfin.Form_swigregister(FormPtr)

class BilinearForm(Form):
    """Proxy of C++ BilinearForm class"""
    __swig_setmethods__ = {}
    for _s in [Form]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BilinearForm, name, value)
    __swig_getmethods__ = {}
    for _s in [Form]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BilinearForm, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::BilinearForm instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_BilinearForm):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def eval(*args):
        """
        eval(self, real block, AffineMap map)
        eval(self, real block, AffineMap map, uint segment)
        """
        return _dolfin.BilinearForm_eval(*args)

    def test(*args):
        """
        test(self) -> FiniteElement
        test(self) -> FiniteElement
        """
        return _dolfin.BilinearForm_test(*args)

    def trial(*args):
        """
        trial(self) -> FiniteElement
        trial(self) -> FiniteElement
        """
        return _dolfin.BilinearForm_trial(*args)


class BilinearFormPtr(BilinearForm):
    def __init__(self, this):
        _swig_setattr(self, BilinearForm, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BilinearForm, 'thisown', 0)
        self.__class__ = BilinearForm
_dolfin.BilinearForm_swigregister(BilinearFormPtr)

class LinearForm(Form):
    """Proxy of C++ LinearForm class"""
    __swig_setmethods__ = {}
    for _s in [Form]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, LinearForm, name, value)
    __swig_getmethods__ = {}
    for _s in [Form]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, LinearForm, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::LinearForm instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_LinearForm):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def eval(*args):
        """
        eval(self, real block, AffineMap map)
        eval(self, real block, AffineMap map, uint segment)
        """
        return _dolfin.LinearForm_eval(*args)

    def test(*args):
        """
        test(self) -> FiniteElement
        test(self) -> FiniteElement
        """
        return _dolfin.LinearForm_test(*args)


class LinearFormPtr(LinearForm):
    def __init__(self, this):
        _swig_setattr(self, LinearForm, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, LinearForm, 'thisown', 0)
        self.__class__ = LinearForm
_dolfin.LinearForm_swigregister(LinearFormPtr)

class Mesh(Variable):
    """Proxy of C++ Mesh class"""
    __swig_setmethods__ = {}
    for _s in [Variable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Mesh, name, value)
    __swig_getmethods__ = {}
    for _s in [Variable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Mesh, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Mesh instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    triangles = _dolfin.Mesh_triangles
    tetrahedra = _dolfin.Mesh_tetrahedra
    def __init__(self, *args):
        """
        __init__(self) -> Mesh
        __init__(self, char filename) -> Mesh
        __init__(self, Mesh mesh) -> Mesh
        """
        _swig_setattr(self, Mesh, 'this', _dolfin.new_Mesh(*args))
        _swig_setattr(self, Mesh, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Mesh):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def merge(*args):
        """merge(self, Mesh mesh2)"""
        return _dolfin.Mesh_merge(*args)

    def init(*args):
        """init(self)"""
        return _dolfin.Mesh_init(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.Mesh_clear(*args)

    def numSpaceDim(*args):
        """numSpaceDim(self) -> int"""
        return _dolfin.Mesh_numSpaceDim(*args)

    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.Mesh_numVertices(*args)

    def numCells(*args):
        """numCells(self) -> int"""
        return _dolfin.Mesh_numCells(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.Mesh_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.Mesh_numFaces(*args)

    def createVertex(*args):
        """
        createVertex(self, Point p) -> Vertex
        createVertex(self, real x, real y, real z) -> Vertex
        """
        return _dolfin.Mesh_createVertex(*args)

    def createCell(*args):
        """
        createCell(self, int n0, int n1, int n2) -> Cell
        createCell(self, int n0, int n1, int n2, int n3) -> Cell
        createCell(self, Vertex n0, Vertex n1, Vertex n2) -> Cell
        createCell(self, Vertex n0, Vertex n1, Vertex n2, Vertex n3) -> Cell
        """
        return _dolfin.Mesh_createCell(*args)

    def createEdge(*args):
        """
        createEdge(self, int n0, int n1) -> Edge
        createEdge(self, Vertex n0, Vertex n1) -> Edge
        """
        return _dolfin.Mesh_createEdge(*args)

    def createFace(*args):
        """
        createFace(self, int e0, int e1, int e2) -> Face
        createFace(self, Edge e0, Edge e1, Edge e2) -> Face
        """
        return _dolfin.Mesh_createFace(*args)

    def remove(*args):
        """
        remove(self, Vertex vertex)
        remove(self, Cell cell)
        remove(self, Edge edge)
        remove(self, Face face)
        """
        return _dolfin.Mesh_remove(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.Mesh_type(*args)

    def vertex(*args):
        """vertex(self, uint id) -> Vertex"""
        return _dolfin.Mesh_vertex(*args)

    def cell(*args):
        """cell(self, uint id) -> Cell"""
        return _dolfin.Mesh_cell(*args)

    def edge(*args):
        """edge(self, uint id) -> Edge"""
        return _dolfin.Mesh_edge(*args)

    def face(*args):
        """face(self, uint id) -> Face"""
        return _dolfin.Mesh_face(*args)

    def boundary(*args):
        """boundary(self) -> Boundary"""
        return _dolfin.Mesh_boundary(*args)

    def refine(*args):
        """refine(self)"""
        return _dolfin.Mesh_refine(*args)

    def refineUniformly(*args):
        """
        refineUniformly(self)
        refineUniformly(self, int i)
        """
        return _dolfin.Mesh_refineUniformly(*args)

    def parent(*args):
        """parent(self) -> Mesh"""
        return _dolfin.Mesh_parent(*args)

    def child(*args):
        """child(self) -> Mesh"""
        return _dolfin.Mesh_child(*args)

    def __eq__(*args):
        """__eq__(self, Mesh mesh) -> bool"""
        return _dolfin.Mesh___eq__(*args)

    def __ne__(*args):
        """__ne__(self, Mesh mesh) -> bool"""
        return _dolfin.Mesh___ne__(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.Mesh_disp(*args)


class MeshPtr(Mesh):
    def __init__(self, this):
        _swig_setattr(self, Mesh, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Mesh, 'thisown', 0)
        self.__class__ = Mesh
_dolfin.Mesh_swigregister(MeshPtr)

class Boundary(_object):
    """Proxy of C++ Boundary class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Boundary, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Boundary, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Boundary instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Boundary
        __init__(self, Mesh mesh) -> Boundary
        """
        _swig_setattr(self, Boundary, 'this', _dolfin.new_Boundary(*args))
        _swig_setattr(self, Boundary, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Boundary):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.Boundary_numVertices(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.Boundary_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.Boundary_numFaces(*args)

    def numFacets(*args):
        """numFacets(self) -> int"""
        return _dolfin.Boundary_numFacets(*args)


class BoundaryPtr(Boundary):
    def __init__(self, this):
        _swig_setattr(self, Boundary, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Boundary, 'thisown', 0)
        self.__class__ = Boundary
_dolfin.Boundary_swigregister(BoundaryPtr)

class Point(_object):
    """Proxy of C++ Point class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Point, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Point, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Point instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Point
        __init__(self, real x) -> Point
        __init__(self, real x, real y) -> Point
        __init__(self, real x, real y, real z) -> Point
        __init__(self, Point p) -> Point
        """
        _swig_setattr(self, Point, 'this', _dolfin.new_Point(*args))
        _swig_setattr(self, Point, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Point):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def dist(*args):
        """
        dist(self, Point p) -> real
        dist(self, real x, real y=0.0, real z=0.0) -> real
        dist(self, real x, real y=0.0) -> real
        dist(self, real x) -> real
        """
        return _dolfin.Point_dist(*args)

    def norm(*args):
        """norm(self) -> real"""
        return _dolfin.Point_norm(*args)

    def midpoint(*args):
        """midpoint(self, Point p) -> Point"""
        return _dolfin.Point_midpoint(*args)

    def __add__(*args):
        """__add__(self, Point p) -> Point"""
        return _dolfin.Point___add__(*args)

    def __sub__(*args):
        """__sub__(self, Point p) -> Point"""
        return _dolfin.Point___sub__(*args)

    def __mul__(*args):
        """__mul__(self, Point p) -> real"""
        return _dolfin.Point___mul__(*args)

    def __iadd__(*args):
        """__iadd__(self, Point p) -> Point"""
        return _dolfin.Point___iadd__(*args)

    def __isub__(*args):
        """__isub__(self, Point p) -> Point"""
        return _dolfin.Point___isub__(*args)

    def __imul__(*args):
        """__imul__(self, real a) -> Point"""
        return _dolfin.Point___imul__(*args)

    def __idiv__(*args):
        """__idiv__(self, real a) -> Point"""
        return _dolfin.Point___idiv__(*args)

    def cross(*args):
        """cross(self, Point p) -> Point"""
        return _dolfin.Point_cross(*args)

    __swig_setmethods__["x"] = _dolfin.Point_x_set
    __swig_getmethods__["x"] = _dolfin.Point_x_get
    if _newclass:x = property(_dolfin.Point_x_get, _dolfin.Point_x_set)
    __swig_setmethods__["y"] = _dolfin.Point_y_set
    __swig_getmethods__["y"] = _dolfin.Point_y_get
    if _newclass:y = property(_dolfin.Point_y_get, _dolfin.Point_y_set)
    __swig_setmethods__["z"] = _dolfin.Point_z_set
    __swig_getmethods__["z"] = _dolfin.Point_z_get
    if _newclass:z = property(_dolfin.Point_z_get, _dolfin.Point_z_set)

class PointPtr(Point):
    def __init__(self, this):
        _swig_setattr(self, Point, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Point, 'thisown', 0)
        self.__class__ = Point
_dolfin.Point_swigregister(PointPtr)

class Vertex(_object):
    """Proxy of C++ Vertex class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vertex, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Vertex, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Vertex instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Vertex
        __init__(self, real x) -> Vertex
        __init__(self, real x, real y) -> Vertex
        __init__(self, real x, real y, real z) -> Vertex
        """
        _swig_setattr(self, Vertex, 'this', _dolfin.new_Vertex(*args))
        _swig_setattr(self, Vertex, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Vertex):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args):
        """clear(self)"""
        return _dolfin.Vertex_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.Vertex_id(*args)

    def numVertexNeighbors(*args):
        """numVertexNeighbors(self) -> int"""
        return _dolfin.Vertex_numVertexNeighbors(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> int"""
        return _dolfin.Vertex_numCellNeighbors(*args)

    def numEdgeNeighbors(*args):
        """numEdgeNeighbors(self) -> int"""
        return _dolfin.Vertex_numEdgeNeighbors(*args)

    def vertex(*args):
        """vertex(self, int i) -> Vertex"""
        return _dolfin.Vertex_vertex(*args)

    def cell(*args):
        """cell(self, int i) -> Cell"""
        return _dolfin.Vertex_cell(*args)

    def edge(*args):
        """edge(self, int i) -> Edge"""
        return _dolfin.Vertex_edge(*args)

    def parent(*args):
        """parent(self) -> Vertex"""
        return _dolfin.Vertex_parent(*args)

    def child(*args):
        """child(self) -> Vertex"""
        return _dolfin.Vertex_child(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.Vertex_mesh(*args)

    def coord(*args):
        """
        coord(self) -> Point
        coord(self) -> Point
        """
        return _dolfin.Vertex_coord(*args)

    def midpoint(*args):
        """midpoint(self, Vertex n) -> Point"""
        return _dolfin.Vertex_midpoint(*args)

    def dist(*args):
        """
        dist(self, Vertex n) -> real
        dist(self, Point p) -> real
        dist(self, real x, real y=0.0, real z=0.0) -> real
        dist(self, real x, real y=0.0) -> real
        dist(self, real x) -> real
        """
        return _dolfin.Vertex_dist(*args)

    def neighbor(*args):
        """neighbor(self, Vertex n) -> bool"""
        return _dolfin.Vertex_neighbor(*args)

    def __ne__(*args):
        """__ne__(self, Vertex vertex) -> bool"""
        return _dolfin.Vertex___ne__(*args)

    def __eq__(*args):
        """
        __eq__(self, Vertex vertex) -> bool
        __eq__(self, int id) -> bool
        """
        return _dolfin.Vertex___eq__(*args)

    def __lt__(*args):
        """__lt__(self, int id) -> bool"""
        return _dolfin.Vertex___lt__(*args)

    def __le__(*args):
        """__le__(self, int id) -> bool"""
        return _dolfin.Vertex___le__(*args)

    def __gt__(*args):
        """__gt__(self, int id) -> bool"""
        return _dolfin.Vertex___gt__(*args)

    def __ge__(*args):
        """__ge__(self, int id) -> bool"""
        return _dolfin.Vertex___ge__(*args)

    __swig_setmethods__["nbids"] = _dolfin.Vertex_nbids_set
    __swig_getmethods__["nbids"] = _dolfin.Vertex_nbids_get
    if _newclass:nbids = property(_dolfin.Vertex_nbids_get, _dolfin.Vertex_nbids_set)

class VertexPtr(Vertex):
    def __init__(self, this):
        _swig_setattr(self, Vertex, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Vertex, 'thisown', 0)
        self.__class__ = Vertex
_dolfin.Vertex_swigregister(VertexPtr)

class Edge(_object):
    """Proxy of C++ Edge class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Edge, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Edge, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Edge instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> Edge
        __init__(self, Vertex n0, Vertex n1) -> Edge
        """
        _swig_setattr(self, Edge, 'this', _dolfin.new_Edge(*args))
        _swig_setattr(self, Edge, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Edge):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args):
        """clear(self)"""
        return _dolfin.Edge_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.Edge_id(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> unsigned int"""
        return _dolfin.Edge_numCellNeighbors(*args)

    def vertex(*args):
        """vertex(self, int i) -> Vertex"""
        return _dolfin.Edge_vertex(*args)

    def cell(*args):
        """cell(self, int i) -> Cell"""
        return _dolfin.Edge_cell(*args)

    def localID(*args):
        """localID(self, int i) -> int"""
        return _dolfin.Edge_localID(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.Edge_mesh(*args)

    def coord(*args):
        """coord(self, int i) -> Point"""
        return _dolfin.Edge_coord(*args)

    def length(*args):
        """length(self) -> real"""
        return _dolfin.Edge_length(*args)

    def midpoint(*args):
        """midpoint(self) -> Point"""
        return _dolfin.Edge_midpoint(*args)

    def equals(*args):
        """equals(self, Vertex n0, Vertex n1) -> bool"""
        return _dolfin.Edge_equals(*args)

    def contains(*args):
        """
        contains(self, Vertex n) -> bool
        contains(self, Point point) -> bool
        """
        return _dolfin.Edge_contains(*args)

    __swig_setmethods__["ebids"] = _dolfin.Edge_ebids_set
    __swig_getmethods__["ebids"] = _dolfin.Edge_ebids_get
    if _newclass:ebids = property(_dolfin.Edge_ebids_get, _dolfin.Edge_ebids_set)

class EdgePtr(Edge):
    def __init__(self, this):
        _swig_setattr(self, Edge, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Edge, 'thisown', 0)
        self.__class__ = Edge
_dolfin.Edge_swigregister(EdgePtr)

class Triangle(_object):
    """Proxy of C++ Triangle class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Triangle, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Triangle, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Triangle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, Vertex n0, Vertex n1, Vertex n2) -> Triangle"""
        _swig_setattr(self, Triangle, 'this', _dolfin.new_Triangle(*args))
        _swig_setattr(self, Triangle, 'thisown', 1)
    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.Triangle_numVertices(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.Triangle_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.Triangle_numFaces(*args)

    def numBoundaries(*args):
        """numBoundaries(self) -> int"""
        return _dolfin.Triangle_numBoundaries(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.Triangle_type(*args)

    def orientation(*args):
        """orientation(self) -> int"""
        return _dolfin.Triangle_orientation(*args)

    def volume(*args):
        """volume(self) -> real"""
        return _dolfin.Triangle_volume(*args)

    def diameter(*args):
        """diameter(self) -> real"""
        return _dolfin.Triangle_diameter(*args)

    def edgeAlignment(*args):
        """edgeAlignment(self, uint i) -> uint"""
        return _dolfin.Triangle_edgeAlignment(*args)

    def faceAlignment(*args):
        """faceAlignment(self, uint i) -> uint"""
        return _dolfin.Triangle_faceAlignment(*args)


class TrianglePtr(Triangle):
    def __init__(self, this):
        _swig_setattr(self, Triangle, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Triangle, 'thisown', 0)
        self.__class__ = Triangle
_dolfin.Triangle_swigregister(TrianglePtr)

class Tetrahedron(_object):
    """Proxy of C++ Tetrahedron class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Tetrahedron, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Tetrahedron, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Tetrahedron instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, Vertex n0, Vertex n1, Vertex n2, Vertex n3) -> Tetrahedron"""
        _swig_setattr(self, Tetrahedron, 'this', _dolfin.new_Tetrahedron(*args))
        _swig_setattr(self, Tetrahedron, 'thisown', 1)
    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.Tetrahedron_numVertices(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.Tetrahedron_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.Tetrahedron_numFaces(*args)

    def numBoundaries(*args):
        """numBoundaries(self) -> int"""
        return _dolfin.Tetrahedron_numBoundaries(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.Tetrahedron_type(*args)

    def orientation(*args):
        """orientation(self) -> int"""
        return _dolfin.Tetrahedron_orientation(*args)

    def volume(*args):
        """volume(self) -> real"""
        return _dolfin.Tetrahedron_volume(*args)

    def diameter(*args):
        """diameter(self) -> real"""
        return _dolfin.Tetrahedron_diameter(*args)

    def edgeAlignment(*args):
        """edgeAlignment(self, uint i) -> uint"""
        return _dolfin.Tetrahedron_edgeAlignment(*args)

    def faceAlignment(*args):
        """faceAlignment(self, uint i) -> uint"""
        return _dolfin.Tetrahedron_faceAlignment(*args)


class TetrahedronPtr(Tetrahedron):
    def __init__(self, this):
        _swig_setattr(self, Tetrahedron, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Tetrahedron, 'thisown', 0)
        self.__class__ = Tetrahedron
_dolfin.Tetrahedron_swigregister(TetrahedronPtr)

class Cell(_object):
    """Proxy of C++ Cell class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Cell, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Cell, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Cell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    triangle = _dolfin.Cell_triangle
    tetrahedron = _dolfin.Cell_tetrahedron
    none = _dolfin.Cell_none
    left = _dolfin.Cell_left
    right = _dolfin.Cell_right
    def __init__(self, *args):
        """
        __init__(self) -> Cell
        __init__(self, Vertex n0, Vertex n1, Vertex n2) -> Cell
        __init__(self, Vertex n0, Vertex n1, Vertex n2, Vertex n3) -> Cell
        """
        _swig_setattr(self, Cell, 'this', _dolfin.new_Cell(*args))
        _swig_setattr(self, Cell, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Cell):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args):
        """clear(self)"""
        return _dolfin.Cell_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.Cell_id(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.Cell_type(*args)

    def orientation(*args):
        """orientation(self) -> int"""
        return _dolfin.Cell_orientation(*args)

    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.Cell_numVertices(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.Cell_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.Cell_numFaces(*args)

    def numBoundaries(*args):
        """numBoundaries(self) -> int"""
        return _dolfin.Cell_numBoundaries(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> int"""
        return _dolfin.Cell_numCellNeighbors(*args)

    def numVertexNeighbors(*args):
        """numVertexNeighbors(self) -> int"""
        return _dolfin.Cell_numVertexNeighbors(*args)

    def numChildren(*args):
        """numChildren(self) -> int"""
        return _dolfin.Cell_numChildren(*args)

    def vertex(*args):
        """vertex(self, int i) -> Vertex"""
        return _dolfin.Cell_vertex(*args)

    def edge(*args):
        """edge(self, int i) -> Edge"""
        return _dolfin.Cell_edge(*args)

    def face(*args):
        """face(self, int i) -> Face"""
        return _dolfin.Cell_face(*args)

    def neighbor(*args):
        """neighbor(self, int i) -> Cell"""
        return _dolfin.Cell_neighbor(*args)

    def parent(*args):
        """parent(self) -> Cell"""
        return _dolfin.Cell_parent(*args)

    def child(*args):
        """child(self, int i) -> Cell"""
        return _dolfin.Cell_child(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.Cell_mesh(*args)

    def coord(*args):
        """coord(self, int i) -> Point"""
        return _dolfin.Cell_coord(*args)

    def midpoint(*args):
        """midpoint(self) -> Point"""
        return _dolfin.Cell_midpoint(*args)

    def vertexID(*args):
        """vertexID(self, int i) -> int"""
        return _dolfin.Cell_vertexID(*args)

    def edgeID(*args):
        """edgeID(self, int i) -> int"""
        return _dolfin.Cell_edgeID(*args)

    def faceID(*args):
        """faceID(self, int i) -> int"""
        return _dolfin.Cell_faceID(*args)

    def volume(*args):
        """volume(self) -> real"""
        return _dolfin.Cell_volume(*args)

    def diameter(*args):
        """diameter(self) -> real"""
        return _dolfin.Cell_diameter(*args)

    def edgeAlignment(*args):
        """edgeAlignment(self, uint i) -> uint"""
        return _dolfin.Cell_edgeAlignment(*args)

    def faceAlignment(*args):
        """faceAlignment(self, uint i) -> uint"""
        return _dolfin.Cell_faceAlignment(*args)

    def __eq__(*args):
        """__eq__(self, Cell cell) -> bool"""
        return _dolfin.Cell___eq__(*args)

    def __ne__(*args):
        """__ne__(self, Cell cell) -> bool"""
        return _dolfin.Cell___ne__(*args)

    def mark(*args):
        """
        mark(self, bool refine=True)
        mark(self)
        """
        return _dolfin.Cell_mark(*args)


class CellPtr(Cell):
    def __init__(self, this):
        _swig_setattr(self, Cell, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Cell, 'thisown', 0)
        self.__class__ = Cell
_dolfin.Cell_swigregister(CellPtr)

class Face(_object):
    """Proxy of C++ Face class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Face, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Face, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Face instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> Face"""
        _swig_setattr(self, Face, 'this', _dolfin.new_Face(*args))
        _swig_setattr(self, Face, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Face):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args):
        """clear(self)"""
        return _dolfin.Face_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.Face_id(*args)

    def numEdges(*args):
        """numEdges(self) -> unsigned int"""
        return _dolfin.Face_numEdges(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> unsigned int"""
        return _dolfin.Face_numCellNeighbors(*args)

    def edge(*args):
        """edge(self, int i) -> Edge"""
        return _dolfin.Face_edge(*args)

    def cell(*args):
        """cell(self, int i) -> Cell"""
        return _dolfin.Face_cell(*args)

    def localID(*args):
        """localID(self, int i) -> int"""
        return _dolfin.Face_localID(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.Face_mesh(*args)

    def area(*args):
        """area(self) -> real"""
        return _dolfin.Face_area(*args)

    def equals(*args):
        """
        equals(self, Edge e0, Edge e1, Edge e2) -> bool
        equals(self, Edge e0, Edge e1) -> bool
        """
        return _dolfin.Face_equals(*args)

    def contains(*args):
        """
        contains(self, Vertex n) -> bool
        contains(self, Point point) -> bool
        """
        return _dolfin.Face_contains(*args)

    __swig_setmethods__["fbids"] = _dolfin.Face_fbids_set
    __swig_getmethods__["fbids"] = _dolfin.Face_fbids_get
    if _newclass:fbids = property(_dolfin.Face_fbids_get, _dolfin.Face_fbids_set)

class FacePtr(Face):
    def __init__(self, this):
        _swig_setattr(self, Face, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Face, 'thisown', 0)
        self.__class__ = Face
_dolfin.Face_swigregister(FacePtr)

class VertexIterator(_object):
    """Proxy of C++ VertexIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, VertexIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, VertexIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::VertexIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, Mesh mesh) -> VertexIterator
        __init__(self, Mesh mesh) -> VertexIterator
        __init__(self, Boundary boundary) -> VertexIterator
        __init__(self, Vertex vertex) -> VertexIterator
        __init__(self, VertexIterator vertexIterator) -> VertexIterator
        __init__(self, Cell cell) -> VertexIterator
        __init__(self, CellIterator cellIterator) -> VertexIterator
        """
        _swig_setattr(self, VertexIterator, 'this', _dolfin.new_VertexIterator(*args))
        _swig_setattr(self, VertexIterator, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_VertexIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def increment(*args):
        """increment(self) -> VertexIterator"""
        return _dolfin.VertexIterator_increment(*args)

    def end(*args):
        """end(self) -> bool"""
        return _dolfin.VertexIterator_end(*args)

    def last(*args):
        """last(self) -> bool"""
        return _dolfin.VertexIterator_last(*args)

    def index(*args):
        """index(self) -> int"""
        return _dolfin.VertexIterator_index(*args)

    def __ref__(*args):
        """__ref__(self) -> Vertex"""
        return _dolfin.VertexIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> Vertex"""
        return _dolfin.VertexIterator___deref__(*args)

    def __eq__(*args):
        """
        __eq__(self, VertexIterator n) -> bool
        __eq__(self, Vertex n) -> bool
        """
        return _dolfin.VertexIterator___eq__(*args)

    def __ne__(*args):
        """
        __ne__(self, VertexIterator n) -> bool
        __ne__(self, Vertex n) -> bool
        """
        return _dolfin.VertexIterator___ne__(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.VertexIterator_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.VertexIterator_id(*args)

    def numVertexNeighbors(*args):
        """numVertexNeighbors(self) -> int"""
        return _dolfin.VertexIterator_numVertexNeighbors(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> int"""
        return _dolfin.VertexIterator_numCellNeighbors(*args)

    def numEdgeNeighbors(*args):
        """numEdgeNeighbors(self) -> int"""
        return _dolfin.VertexIterator_numEdgeNeighbors(*args)

    def vertex(*args):
        """vertex(self, int i) -> Vertex"""
        return _dolfin.VertexIterator_vertex(*args)

    def cell(*args):
        """cell(self, int i) -> Cell"""
        return _dolfin.VertexIterator_cell(*args)

    def edge(*args):
        """edge(self, int i) -> Edge"""
        return _dolfin.VertexIterator_edge(*args)

    def parent(*args):
        """parent(self) -> Vertex"""
        return _dolfin.VertexIterator_parent(*args)

    def child(*args):
        """child(self) -> Vertex"""
        return _dolfin.VertexIterator_child(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.VertexIterator_mesh(*args)

    def coord(*args):
        """
        coord(self) -> Point
        coord(self) -> Point
        """
        return _dolfin.VertexIterator_coord(*args)

    def midpoint(*args):
        """midpoint(self, Vertex n) -> Point"""
        return _dolfin.VertexIterator_midpoint(*args)

    def dist(*args):
        """
        dist(self, Vertex n) -> real
        dist(self, Point p) -> real
        dist(self, real x, real y=0.0, real z=0.0) -> real
        dist(self, real x, real y=0.0) -> real
        dist(self, real x) -> real
        """
        return _dolfin.VertexIterator_dist(*args)

    def neighbor(*args):
        """neighbor(self, Vertex n) -> bool"""
        return _dolfin.VertexIterator_neighbor(*args)

    def __lt__(*args):
        """__lt__(self, int id) -> bool"""
        return _dolfin.VertexIterator___lt__(*args)

    def __le__(*args):
        """__le__(self, int id) -> bool"""
        return _dolfin.VertexIterator___le__(*args)

    def __gt__(*args):
        """__gt__(self, int id) -> bool"""
        return _dolfin.VertexIterator___gt__(*args)

    def __ge__(*args):
        """__ge__(self, int id) -> bool"""
        return _dolfin.VertexIterator___ge__(*args)

    __swig_setmethods__["nbids"] = _dolfin.VertexIterator_nbids_set
    __swig_getmethods__["nbids"] = _dolfin.VertexIterator_nbids_get
    if _newclass:nbids = property(_dolfin.VertexIterator_nbids_get, _dolfin.VertexIterator_nbids_set)

class VertexIteratorPtr(VertexIterator):
    def __init__(self, this):
        _swig_setattr(self, VertexIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, VertexIterator, 'thisown', 0)
        self.__class__ = VertexIterator
_dolfin.VertexIterator_swigregister(VertexIteratorPtr)

class CellIterator(_object):
    """Proxy of C++ CellIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, CellIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, CellIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::CellIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, Mesh mesh) -> CellIterator
        __init__(self, Mesh mesh) -> CellIterator
        __init__(self, Vertex vertex) -> CellIterator
        __init__(self, VertexIterator vertexIterator) -> CellIterator
        __init__(self, Cell cell) -> CellIterator
        __init__(self, CellIterator cellIterator) -> CellIterator
        __init__(self, Edge edge) -> CellIterator
        __init__(self, EdgeIterator edgeIterator) -> CellIterator
        __init__(self, Face face) -> CellIterator
        __init__(self, FaceIterator faceIterator) -> CellIterator
        """
        _swig_setattr(self, CellIterator, 'this', _dolfin.new_CellIterator(*args))
        _swig_setattr(self, CellIterator, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_CellIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def increment(*args):
        """increment(self) -> CellIterator"""
        return _dolfin.CellIterator_increment(*args)

    def end(*args):
        """end(self) -> bool"""
        return _dolfin.CellIterator_end(*args)

    def last(*args):
        """last(self) -> bool"""
        return _dolfin.CellIterator_last(*args)

    def index(*args):
        """index(self) -> int"""
        return _dolfin.CellIterator_index(*args)

    def __ref__(*args):
        """__ref__(self) -> Cell"""
        return _dolfin.CellIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> Cell"""
        return _dolfin.CellIterator___deref__(*args)

    def __eq__(*args):
        """
        __eq__(self, CellIterator c) -> bool
        __eq__(self, Cell c) -> bool
        """
        return _dolfin.CellIterator___eq__(*args)

    def __ne__(*args):
        """
        __ne__(self, CellIterator c) -> bool
        __ne__(self, Cell c) -> bool
        """
        return _dolfin.CellIterator___ne__(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.CellIterator_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.CellIterator_id(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.CellIterator_type(*args)

    def orientation(*args):
        """orientation(self) -> int"""
        return _dolfin.CellIterator_orientation(*args)

    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.CellIterator_numVertices(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.CellIterator_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.CellIterator_numFaces(*args)

    def numBoundaries(*args):
        """numBoundaries(self) -> int"""
        return _dolfin.CellIterator_numBoundaries(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> int"""
        return _dolfin.CellIterator_numCellNeighbors(*args)

    def numVertexNeighbors(*args):
        """numVertexNeighbors(self) -> int"""
        return _dolfin.CellIterator_numVertexNeighbors(*args)

    def numChildren(*args):
        """numChildren(self) -> int"""
        return _dolfin.CellIterator_numChildren(*args)

    def vertex(*args):
        """vertex(self, int i) -> Vertex"""
        return _dolfin.CellIterator_vertex(*args)

    def edge(*args):
        """edge(self, int i) -> Edge"""
        return _dolfin.CellIterator_edge(*args)

    def face(*args):
        """face(self, int i) -> Face"""
        return _dolfin.CellIterator_face(*args)

    def neighbor(*args):
        """neighbor(self, int i) -> Cell"""
        return _dolfin.CellIterator_neighbor(*args)

    def parent(*args):
        """parent(self) -> Cell"""
        return _dolfin.CellIterator_parent(*args)

    def child(*args):
        """child(self, int i) -> Cell"""
        return _dolfin.CellIterator_child(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.CellIterator_mesh(*args)

    def coord(*args):
        """coord(self, int i) -> Point"""
        return _dolfin.CellIterator_coord(*args)

    def midpoint(*args):
        """midpoint(self) -> Point"""
        return _dolfin.CellIterator_midpoint(*args)

    def vertexID(*args):
        """vertexID(self, int i) -> int"""
        return _dolfin.CellIterator_vertexID(*args)

    def edgeID(*args):
        """edgeID(self, int i) -> int"""
        return _dolfin.CellIterator_edgeID(*args)

    def faceID(*args):
        """faceID(self, int i) -> int"""
        return _dolfin.CellIterator_faceID(*args)

    def volume(*args):
        """volume(self) -> real"""
        return _dolfin.CellIterator_volume(*args)

    def diameter(*args):
        """diameter(self) -> real"""
        return _dolfin.CellIterator_diameter(*args)

    def edgeAlignment(*args):
        """edgeAlignment(self, uint i) -> uint"""
        return _dolfin.CellIterator_edgeAlignment(*args)

    def faceAlignment(*args):
        """faceAlignment(self, uint i) -> uint"""
        return _dolfin.CellIterator_faceAlignment(*args)

    def mark(*args):
        """
        mark(self, bool refine=True)
        mark(self)
        """
        return _dolfin.CellIterator_mark(*args)


class CellIteratorPtr(CellIterator):
    def __init__(self, this):
        _swig_setattr(self, CellIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, CellIterator, 'thisown', 0)
        self.__class__ = CellIterator
_dolfin.CellIterator_swigregister(CellIteratorPtr)

class EdgeIterator(_object):
    """Proxy of C++ EdgeIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, EdgeIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, EdgeIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::EdgeIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, Mesh mesh) -> EdgeIterator
        __init__(self, Mesh mesh) -> EdgeIterator
        __init__(self, Boundary boundary) -> EdgeIterator
        __init__(self, Boundary boundary) -> EdgeIterator
        __init__(self, Vertex vertex) -> EdgeIterator
        __init__(self, VertexIterator vertexIterator) -> EdgeIterator
        __init__(self, Cell cell) -> EdgeIterator
        __init__(self, CellIterator cellIterator) -> EdgeIterator
        __init__(self, Face face) -> EdgeIterator
        __init__(self, FaceIterator faceIterator) -> EdgeIterator
        """
        _swig_setattr(self, EdgeIterator, 'this', _dolfin.new_EdgeIterator(*args))
        _swig_setattr(self, EdgeIterator, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_EdgeIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def increment(*args):
        """increment(self) -> EdgeIterator"""
        return _dolfin.EdgeIterator_increment(*args)

    def end(*args):
        """end(self) -> bool"""
        return _dolfin.EdgeIterator_end(*args)

    def last(*args):
        """last(self) -> bool"""
        return _dolfin.EdgeIterator_last(*args)

    def index(*args):
        """index(self) -> int"""
        return _dolfin.EdgeIterator_index(*args)

    def __ref__(*args):
        """__ref__(self) -> Edge"""
        return _dolfin.EdgeIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> Edge"""
        return _dolfin.EdgeIterator___deref__(*args)

    def __eq__(*args):
        """__eq__(self, EdgeIterator n) -> bool"""
        return _dolfin.EdgeIterator___eq__(*args)

    def __ne__(*args):
        """__ne__(self, EdgeIterator n) -> bool"""
        return _dolfin.EdgeIterator___ne__(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.EdgeIterator_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.EdgeIterator_id(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> unsigned int"""
        return _dolfin.EdgeIterator_numCellNeighbors(*args)

    def vertex(*args):
        """vertex(self, int i) -> Vertex"""
        return _dolfin.EdgeIterator_vertex(*args)

    def cell(*args):
        """cell(self, int i) -> Cell"""
        return _dolfin.EdgeIterator_cell(*args)

    def localID(*args):
        """localID(self, int i) -> int"""
        return _dolfin.EdgeIterator_localID(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.EdgeIterator_mesh(*args)

    def coord(*args):
        """coord(self, int i) -> Point"""
        return _dolfin.EdgeIterator_coord(*args)

    def length(*args):
        """length(self) -> real"""
        return _dolfin.EdgeIterator_length(*args)

    def midpoint(*args):
        """midpoint(self) -> Point"""
        return _dolfin.EdgeIterator_midpoint(*args)

    def equals(*args):
        """equals(self, Vertex n0, Vertex n1) -> bool"""
        return _dolfin.EdgeIterator_equals(*args)

    def contains(*args):
        """
        contains(self, Vertex n) -> bool
        contains(self, Point point) -> bool
        """
        return _dolfin.EdgeIterator_contains(*args)

    __swig_setmethods__["ebids"] = _dolfin.EdgeIterator_ebids_set
    __swig_getmethods__["ebids"] = _dolfin.EdgeIterator_ebids_get
    if _newclass:ebids = property(_dolfin.EdgeIterator_ebids_get, _dolfin.EdgeIterator_ebids_set)

class EdgeIteratorPtr(EdgeIterator):
    def __init__(self, this):
        _swig_setattr(self, EdgeIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, EdgeIterator, 'thisown', 0)
        self.__class__ = EdgeIterator
_dolfin.EdgeIterator_swigregister(EdgeIteratorPtr)

class FaceIterator(_object):
    """Proxy of C++ FaceIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FaceIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FaceIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::FaceIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, Mesh mesh) -> FaceIterator
        __init__(self, Mesh mesh) -> FaceIterator
        __init__(self, Boundary boundary) -> FaceIterator
        __init__(self, Boundary boundary) -> FaceIterator
        __init__(self, Cell cell) -> FaceIterator
        __init__(self, CellIterator cellIterator) -> FaceIterator
        """
        _swig_setattr(self, FaceIterator, 'this', _dolfin.new_FaceIterator(*args))
        _swig_setattr(self, FaceIterator, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_FaceIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def end(*args):
        """end(self) -> bool"""
        return _dolfin.FaceIterator_end(*args)

    def last(*args):
        """last(self) -> bool"""
        return _dolfin.FaceIterator_last(*args)

    def index(*args):
        """index(self) -> int"""
        return _dolfin.FaceIterator_index(*args)

    def __ref__(*args):
        """__ref__(self) -> Face"""
        return _dolfin.FaceIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> Face"""
        return _dolfin.FaceIterator___deref__(*args)

    def __eq__(*args):
        """
        __eq__(self, FaceIterator f) -> bool
        __eq__(self, Face f) -> bool
        """
        return _dolfin.FaceIterator___eq__(*args)

    def __ne__(*args):
        """
        __ne__(self, FaceIterator f) -> bool
        __ne__(self, Face f) -> bool
        """
        return _dolfin.FaceIterator___ne__(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.FaceIterator_clear(*args)

    def id(*args):
        """id(self) -> int"""
        return _dolfin.FaceIterator_id(*args)

    def numEdges(*args):
        """numEdges(self) -> unsigned int"""
        return _dolfin.FaceIterator_numEdges(*args)

    def numCellNeighbors(*args):
        """numCellNeighbors(self) -> unsigned int"""
        return _dolfin.FaceIterator_numCellNeighbors(*args)

    def edge(*args):
        """edge(self, int i) -> Edge"""
        return _dolfin.FaceIterator_edge(*args)

    def cell(*args):
        """cell(self, int i) -> Cell"""
        return _dolfin.FaceIterator_cell(*args)

    def localID(*args):
        """localID(self, int i) -> int"""
        return _dolfin.FaceIterator_localID(*args)

    def mesh(*args):
        """
        mesh(self) -> Mesh
        mesh(self) -> Mesh
        """
        return _dolfin.FaceIterator_mesh(*args)

    def area(*args):
        """area(self) -> real"""
        return _dolfin.FaceIterator_area(*args)

    def equals(*args):
        """
        equals(self, Edge e0, Edge e1, Edge e2) -> bool
        equals(self, Edge e0, Edge e1) -> bool
        """
        return _dolfin.FaceIterator_equals(*args)

    def contains(*args):
        """
        contains(self, Vertex n) -> bool
        contains(self, Point point) -> bool
        """
        return _dolfin.FaceIterator_contains(*args)

    __swig_setmethods__["fbids"] = _dolfin.FaceIterator_fbids_set
    __swig_getmethods__["fbids"] = _dolfin.FaceIterator_fbids_get
    if _newclass:fbids = property(_dolfin.FaceIterator_fbids_get, _dolfin.FaceIterator_fbids_set)

class FaceIteratorPtr(FaceIterator):
    def __init__(self, this):
        _swig_setattr(self, FaceIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FaceIterator, 'thisown', 0)
        self.__class__ = FaceIterator
_dolfin.FaceIterator_swigregister(FaceIteratorPtr)

class MeshIterator(_object):
    """Proxy of C++ MeshIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, MeshHierarchy meshs) -> MeshIterator
        __init__(self, MeshHierarchy meshs, Index index) -> MeshIterator
        """
        _swig_setattr(self, MeshIterator, 'this', _dolfin.new_MeshIterator(*args))
        _swig_setattr(self, MeshIterator, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def end(*args):
        """end(self) -> bool"""
        return _dolfin.MeshIterator_end(*args)

    def index(*args):
        """index(self) -> int"""
        return _dolfin.MeshIterator_index(*args)

    def __ref__(*args):
        """__ref__(self) -> Mesh"""
        return _dolfin.MeshIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> Mesh"""
        return _dolfin.MeshIterator___deref__(*args)

    def merge(*args):
        """merge(self, Mesh mesh2)"""
        return _dolfin.MeshIterator_merge(*args)

    def init(*args):
        """init(self)"""
        return _dolfin.MeshIterator_init(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.MeshIterator_clear(*args)

    def numSpaceDim(*args):
        """numSpaceDim(self) -> int"""
        return _dolfin.MeshIterator_numSpaceDim(*args)

    def numVertices(*args):
        """numVertices(self) -> int"""
        return _dolfin.MeshIterator_numVertices(*args)

    def numCells(*args):
        """numCells(self) -> int"""
        return _dolfin.MeshIterator_numCells(*args)

    def numEdges(*args):
        """numEdges(self) -> int"""
        return _dolfin.MeshIterator_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> int"""
        return _dolfin.MeshIterator_numFaces(*args)

    def createVertex(*args):
        """
        createVertex(self, Point p) -> Vertex
        createVertex(self, real x, real y, real z) -> Vertex
        """
        return _dolfin.MeshIterator_createVertex(*args)

    def createCell(*args):
        """
        createCell(self, int n0, int n1, int n2) -> Cell
        createCell(self, int n0, int n1, int n2, int n3) -> Cell
        createCell(self, Vertex n0, Vertex n1, Vertex n2) -> Cell
        createCell(self, Vertex n0, Vertex n1, Vertex n2, Vertex n3) -> Cell
        """
        return _dolfin.MeshIterator_createCell(*args)

    def createEdge(*args):
        """
        createEdge(self, int n0, int n1) -> Edge
        createEdge(self, Vertex n0, Vertex n1) -> Edge
        """
        return _dolfin.MeshIterator_createEdge(*args)

    def createFace(*args):
        """
        createFace(self, int e0, int e1, int e2) -> Face
        createFace(self, Edge e0, Edge e1, Edge e2) -> Face
        """
        return _dolfin.MeshIterator_createFace(*args)

    def remove(*args):
        """
        remove(self, Vertex vertex)
        remove(self, Cell cell)
        remove(self, Edge edge)
        remove(self, Face face)
        """
        return _dolfin.MeshIterator_remove(*args)

    def type(*args):
        """type(self) -> int"""
        return _dolfin.MeshIterator_type(*args)

    def vertex(*args):
        """vertex(self, uint id) -> Vertex"""
        return _dolfin.MeshIterator_vertex(*args)

    def cell(*args):
        """cell(self, uint id) -> Cell"""
        return _dolfin.MeshIterator_cell(*args)

    def edge(*args):
        """edge(self, uint id) -> Edge"""
        return _dolfin.MeshIterator_edge(*args)

    def face(*args):
        """face(self, uint id) -> Face"""
        return _dolfin.MeshIterator_face(*args)

    def boundary(*args):
        """boundary(self) -> Boundary"""
        return _dolfin.MeshIterator_boundary(*args)

    def refine(*args):
        """refine(self)"""
        return _dolfin.MeshIterator_refine(*args)

    def refineUniformly(*args):
        """
        refineUniformly(self)
        refineUniformly(self, int i)
        """
        return _dolfin.MeshIterator_refineUniformly(*args)

    def parent(*args):
        """parent(self) -> Mesh"""
        return _dolfin.MeshIterator_parent(*args)

    def child(*args):
        """child(self) -> Mesh"""
        return _dolfin.MeshIterator_child(*args)

    def __eq__(*args):
        """__eq__(self, Mesh mesh) -> bool"""
        return _dolfin.MeshIterator___eq__(*args)

    def __ne__(*args):
        """__ne__(self, Mesh mesh) -> bool"""
        return _dolfin.MeshIterator___ne__(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.MeshIterator_disp(*args)

    def rename(*args):
        """rename(self, string name, string label)"""
        return _dolfin.MeshIterator_rename(*args)

    def name(*args):
        """name(self) -> string"""
        return _dolfin.MeshIterator_name(*args)

    def label(*args):
        """label(self) -> string"""
        return _dolfin.MeshIterator_label(*args)

    def number(*args):
        """number(self) -> int"""
        return _dolfin.MeshIterator_number(*args)


class MeshIteratorPtr(MeshIterator):
    def __init__(self, this):
        _swig_setattr(self, MeshIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshIterator, 'thisown', 0)
        self.__class__ = MeshIterator
_dolfin.MeshIterator_swigregister(MeshIteratorPtr)

class UnitSquare(Mesh):
    """Proxy of C++ UnitSquare class"""
    __swig_setmethods__ = {}
    for _s in [Mesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, UnitSquare, name, value)
    __swig_getmethods__ = {}
    for _s in [Mesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, UnitSquare, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::UnitSquare instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint nx, uint ny) -> UnitSquare"""
        _swig_setattr(self, UnitSquare, 'this', _dolfin.new_UnitSquare(*args))
        _swig_setattr(self, UnitSquare, 'thisown', 1)

class UnitSquarePtr(UnitSquare):
    def __init__(self, this):
        _swig_setattr(self, UnitSquare, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, UnitSquare, 'thisown', 0)
        self.__class__ = UnitSquare
_dolfin.UnitSquare_swigregister(UnitSquarePtr)

class UnitCube(Mesh):
    """Proxy of C++ UnitCube class"""
    __swig_setmethods__ = {}
    for _s in [Mesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, UnitCube, name, value)
    __swig_getmethods__ = {}
    for _s in [Mesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, UnitCube, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::UnitCube instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint nx, uint ny, uint nz) -> UnitCube"""
        _swig_setattr(self, UnitCube, 'this', _dolfin.new_UnitCube(*args))
        _swig_setattr(self, UnitCube, 'thisown', 1)

class UnitCubePtr(UnitCube):
    def __init__(self, this):
        _swig_setattr(self, UnitCube, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, UnitCube, 'thisown', 0)
        self.__class__ = UnitCube
_dolfin.UnitCube_swigregister(UnitCubePtr)

class MeshConnectivity(_object):
    """Proxy of C++ MeshConnectivity class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshConnectivity, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshConnectivity, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshConnectivity instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> MeshConnectivity
        __init__(self, MeshConnectivity connectivity) -> MeshConnectivity
        """
        _swig_setattr(self, MeshConnectivity, 'this', _dolfin.new_MeshConnectivity(*args))
        _swig_setattr(self, MeshConnectivity, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshConnectivity):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """
        size(self) -> uint
        size(self, uint entity) -> uint
        """
        return _dolfin.MeshConnectivity_size(*args)

    def __call__(*args):
        """__call__(self, uint entity) -> uint"""
        return _dolfin.MeshConnectivity___call__(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.MeshConnectivity_clear(*args)

    def init(*args):
        """
        init(self, uint num_entities, uint num_connections)
        init(self, dolfin::Array<(dolfin::uint)> num_connections)
        """
        return _dolfin.MeshConnectivity_init(*args)

    def set(*args):
        """
        set(self, uint entity, uint connection, uint pos)
        set(self, uint entity, dolfin::Array<(dolfin::uint)> connections)
        set(self, uint entity, uint connections)
        set(self, dolfin::Array<(dolfin::Array<(dolfin::uint)>)> connectivity)
        """
        return _dolfin.MeshConnectivity_set(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.MeshConnectivity_disp(*args)


class MeshConnectivityPtr(MeshConnectivity):
    def __init__(self, this):
        _swig_setattr(self, MeshConnectivity, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshConnectivity, 'thisown', 0)
        self.__class__ = MeshConnectivity
_dolfin.MeshConnectivity_swigregister(MeshConnectivityPtr)

class MeshEditor(_object):
    """Proxy of C++ MeshEditor class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshEditor, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshEditor, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshEditor instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> MeshEditor"""
        _swig_setattr(self, MeshEditor, 'this', _dolfin.new_MeshEditor(*args))
        _swig_setattr(self, MeshEditor, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshEditor):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def open(*args):
        """open(self, NewMesh mesh, CellType::Type type, uint tdim, uint gdim)"""
        return _dolfin.MeshEditor_open(*args)

    def initVertices(*args):
        """initVertices(self, uint num_vertices)"""
        return _dolfin.MeshEditor_initVertices(*args)

    def initCells(*args):
        """initCells(self, uint num_cells)"""
        return _dolfin.MeshEditor_initCells(*args)

    def addVertex(*args):
        """
        addVertex(self, uint v, NewPoint p)
        addVertex(self, uint v, real x)
        addVertex(self, uint v, real x, real y)
        addVertex(self, uint v, real x, real y, real z)
        """
        return _dolfin.MeshEditor_addVertex(*args)

    def addCell(*args):
        """
        addCell(self, uint c, dolfin::Array<(dolfin::uint)> v)
        addCell(self, uint c, uint v0, uint v1)
        addCell(self, uint c, uint v0, uint v1, uint v2)
        addCell(self, uint c, uint v0, uint v1, uint v2, uint v3)
        """
        return _dolfin.MeshEditor_addCell(*args)

    def close(*args):
        """close(self)"""
        return _dolfin.MeshEditor_close(*args)


class MeshEditorPtr(MeshEditor):
    def __init__(self, this):
        _swig_setattr(self, MeshEditor, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshEditor, 'thisown', 0)
        self.__class__ = MeshEditor
_dolfin.MeshEditor_swigregister(MeshEditorPtr)

class MeshEntity(_object):
    """Proxy of C++ MeshEntity class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshEntity, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshEntity, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshEntity instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, NewMesh mesh, uint dim, uint index) -> MeshEntity"""
        _swig_setattr(self, MeshEntity, 'this', _dolfin.new_MeshEntity(*args))
        _swig_setattr(self, MeshEntity, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshEntity):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.MeshEntity_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.MeshEntity_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.MeshEntity_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.MeshEntity_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.MeshEntity_connections(*args)


class MeshEntityPtr(MeshEntity):
    def __init__(self, this):
        _swig_setattr(self, MeshEntity, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshEntity, 'thisown', 0)
        self.__class__ = MeshEntity
_dolfin.MeshEntity_swigregister(MeshEntityPtr)

class MeshEntityIterator(_object):
    """Proxy of C++ MeshEntityIterator class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshEntityIterator, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshEntityIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshEntityIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh, uint dim) -> MeshEntityIterator
        __init__(self, MeshEntity entity, uint dim) -> MeshEntityIterator
        __init__(self, MeshEntityIterator it, uint dim) -> MeshEntityIterator
        """
        _swig_setattr(self, MeshEntityIterator, 'this', _dolfin.new_MeshEntityIterator(*args))
        _swig_setattr(self, MeshEntityIterator, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshEntityIterator):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def end(*args):
        """end(self) -> bool"""
        return _dolfin.MeshEntityIterator_end(*args)

    def __ref__(*args):
        """__ref__(self) -> MeshEntity"""
        return _dolfin.MeshEntityIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> MeshEntity"""
        return _dolfin.MeshEntityIterator___deref__(*args)

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.MeshEntityIterator_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.MeshEntityIterator_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.MeshEntityIterator_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.MeshEntityIterator_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.MeshEntityIterator_connections(*args)


class MeshEntityIteratorPtr(MeshEntityIterator):
    def __init__(self, this):
        _swig_setattr(self, MeshEntityIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshEntityIterator, 'thisown', 0)
        self.__class__ = MeshEntityIterator
_dolfin.MeshEntityIterator_swigregister(MeshEntityIteratorPtr)

class MeshGeometry(_object):
    """Proxy of C++ MeshGeometry class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshGeometry, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshGeometry, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshGeometry instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> MeshGeometry
        __init__(self, MeshGeometry geometry) -> MeshGeometry
        """
        _swig_setattr(self, MeshGeometry, 'this', _dolfin.new_MeshGeometry(*args))
        _swig_setattr(self, MeshGeometry, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshGeometry):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.MeshGeometry_dim(*args)

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.MeshGeometry_size(*args)

    def x(*args):
        """
        x(self, uint n, uint i) -> real
        x(self, uint n, uint i) -> real
        """
        return _dolfin.MeshGeometry_x(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.MeshGeometry_clear(*args)

    def init(*args):
        """init(self, uint dim, uint size)"""
        return _dolfin.MeshGeometry_init(*args)

    def set(*args):
        """set(self, uint n, uint i, real x)"""
        return _dolfin.MeshGeometry_set(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.MeshGeometry_disp(*args)


class MeshGeometryPtr(MeshGeometry):
    def __init__(self, this):
        _swig_setattr(self, MeshGeometry, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshGeometry, 'thisown', 0)
        self.__class__ = MeshGeometry
_dolfin.MeshGeometry_swigregister(MeshGeometryPtr)

class MeshTopology(_object):
    """Proxy of C++ MeshTopology class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MeshTopology, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MeshTopology, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MeshTopology instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> MeshTopology
        __init__(self, MeshTopology topology) -> MeshTopology
        """
        _swig_setattr(self, MeshTopology, 'this', _dolfin.new_MeshTopology(*args))
        _swig_setattr(self, MeshTopology, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MeshTopology):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.MeshTopology_dim(*args)

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.MeshTopology_size(*args)

    def clear(*args):
        """clear(self)"""
        return _dolfin.MeshTopology_clear(*args)

    def init(*args):
        """
        init(self, uint dim)
        init(self, uint dim, uint size)
        """
        return _dolfin.MeshTopology_init(*args)

    def __call__(*args):
        """
        __call__(self, uint d0, uint d1) -> MeshConnectivity
        __call__(self, uint d0, uint d1) -> MeshConnectivity
        """
        return _dolfin.MeshTopology___call__(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.MeshTopology_disp(*args)


class MeshTopologyPtr(MeshTopology):
    def __init__(self, this):
        _swig_setattr(self, MeshTopology, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MeshTopology, 'thisown', 0)
        self.__class__ = MeshTopology
_dolfin.MeshTopology_swigregister(MeshTopologyPtr)

class NewMesh(Variable):
    """Proxy of C++ NewMesh class"""
    __swig_setmethods__ = {}
    for _s in [Variable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewMesh, name, value)
    __swig_getmethods__ = {}
    for _s in [Variable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewMesh, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewMesh instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> NewMesh
        __init__(self, NewMesh mesh) -> NewMesh
        __init__(self, string filename) -> NewMesh
        """
        _swig_setattr(self, NewMesh, 'this', _dolfin.new_NewMesh(*args))
        _swig_setattr(self, NewMesh, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewMesh):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.NewMesh_dim(*args)

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.NewMesh_size(*args)

    def numVertices(*args):
        """numVertices(self) -> uint"""
        return _dolfin.NewMesh_numVertices(*args)

    def numEdges(*args):
        """numEdges(self) -> uint"""
        return _dolfin.NewMesh_numEdges(*args)

    def numFaces(*args):
        """numFaces(self) -> uint"""
        return _dolfin.NewMesh_numFaces(*args)

    def numFacets(*args):
        """numFacets(self) -> uint"""
        return _dolfin.NewMesh_numFacets(*args)

    def numCells(*args):
        """numCells(self) -> uint"""
        return _dolfin.NewMesh_numCells(*args)

    def topology(*args):
        """
        topology(self) -> MeshTopology
        topology(self) -> MeshTopology
        """
        return _dolfin.NewMesh_topology(*args)

    def geometry(*args):
        """
        geometry(self) -> MeshGeometry
        geometry(self) -> MeshGeometry
        """
        return _dolfin.NewMesh_geometry(*args)

    def type(*args):
        """
        type(self) -> CellType
        type(self) -> CellType
        """
        return _dolfin.NewMesh_type(*args)

    def init(*args):
        """
        init(self, uint dim) -> uint
        init(self, uint d0, uint d1)
        init(self)
        """
        return _dolfin.NewMesh_init(*args)

    def refine(*args):
        """refine(self)"""
        return _dolfin.NewMesh_refine(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.NewMesh_disp(*args)


class NewMeshPtr(NewMesh):
    def __init__(self, this):
        _swig_setattr(self, NewMesh, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewMesh, 'thisown', 0)
        self.__class__ = NewMesh
_dolfin.NewMesh_swigregister(NewMeshPtr)

class NewMeshData(_object):
    """Proxy of C++ NewMeshData class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewMeshData, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NewMeshData, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewMeshData instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> NewMeshData
        __init__(self, NewMeshData data) -> NewMeshData
        """
        _swig_setattr(self, NewMeshData, 'this', _dolfin.new_NewMeshData(*args))
        _swig_setattr(self, NewMeshData, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewMeshData):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def clear(*args):
        """clear(self)"""
        return _dolfin.NewMeshData_clear(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.NewMeshData_disp(*args)

    __swig_setmethods__["topology"] = _dolfin.NewMeshData_topology_set
    __swig_getmethods__["topology"] = _dolfin.NewMeshData_topology_get
    if _newclass:topology = property(_dolfin.NewMeshData_topology_get, _dolfin.NewMeshData_topology_set)
    __swig_setmethods__["geometry"] = _dolfin.NewMeshData_geometry_set
    __swig_getmethods__["geometry"] = _dolfin.NewMeshData_geometry_get
    if _newclass:geometry = property(_dolfin.NewMeshData_geometry_get, _dolfin.NewMeshData_geometry_set)
    __swig_setmethods__["cell_type"] = _dolfin.NewMeshData_cell_type_set
    __swig_getmethods__["cell_type"] = _dolfin.NewMeshData_cell_type_get
    if _newclass:cell_type = property(_dolfin.NewMeshData_cell_type_get, _dolfin.NewMeshData_cell_type_set)

class NewMeshDataPtr(NewMeshData):
    def __init__(self, this):
        _swig_setattr(self, NewMeshData, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewMeshData, 'thisown', 0)
        self.__class__ = NewMeshData
_dolfin.NewMeshData_swigregister(NewMeshDataPtr)

class NewVertex(MeshEntity):
    """Proxy of C++ NewVertex class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntity]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewVertex, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntity]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewVertex, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewVertex instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh, uint index) -> NewVertex
        __init__(self, MeshEntity entity) -> NewVertex
        """
        _swig_setattr(self, NewVertex, 'this', _dolfin.new_NewVertex(*args))
        _swig_setattr(self, NewVertex, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewVertex):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def y(*args):
        """y(self) -> real"""
        return _dolfin.NewVertex_y(*args)

    def z(*args):
        """z(self) -> real"""
        return _dolfin.NewVertex_z(*args)

    def x(*args):
        """
        x(self) -> real
        x(self, uint i) -> real
        """
        return _dolfin.NewVertex_x(*args)

    def point(*args):
        """point(self) -> NewPoint"""
        return _dolfin.NewVertex_point(*args)


class NewVertexPtr(NewVertex):
    def __init__(self, this):
        _swig_setattr(self, NewVertex, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewVertex, 'thisown', 0)
        self.__class__ = NewVertex
_dolfin.NewVertex_swigregister(NewVertexPtr)

class NewVertexIterator(MeshEntityIterator):
    """Proxy of C++ NewVertexIterator class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewVertexIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewVertexIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewVertexIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh) -> NewVertexIterator
        __init__(self, MeshEntity entity) -> NewVertexIterator
        __init__(self, MeshEntityIterator it) -> NewVertexIterator
        """
        _swig_setattr(self, NewVertexIterator, 'this', _dolfin.new_NewVertexIterator(*args))
        _swig_setattr(self, NewVertexIterator, 'thisown', 1)
    def __ref__(*args):
        """__ref__(self) -> NewVertex"""
        return _dolfin.NewVertexIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> NewVertex"""
        return _dolfin.NewVertexIterator___deref__(*args)

    def x(*args):
        """
        x(self) -> real
        x(self, uint i) -> real
        """
        return _dolfin.NewVertexIterator_x(*args)

    def y(*args):
        """y(self) -> real"""
        return _dolfin.NewVertexIterator_y(*args)

    def z(*args):
        """z(self) -> real"""
        return _dolfin.NewVertexIterator_z(*args)

    def point(*args):
        """point(self) -> NewPoint"""
        return _dolfin.NewVertexIterator_point(*args)

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.NewVertexIterator_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.NewVertexIterator_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.NewVertexIterator_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.NewVertexIterator_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.NewVertexIterator_connections(*args)


class NewVertexIteratorPtr(NewVertexIterator):
    def __init__(self, this):
        _swig_setattr(self, NewVertexIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewVertexIterator, 'thisown', 0)
        self.__class__ = NewVertexIterator
_dolfin.NewVertexIterator_swigregister(NewVertexIteratorPtr)

class NewEdge(MeshEntity):
    """Proxy of C++ NewEdge class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntity]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewEdge, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntity]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewEdge, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewEdge instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh, uint index) -> NewEdge
        __init__(self, MeshEntity entity) -> NewEdge
        """
        _swig_setattr(self, NewEdge, 'this', _dolfin.new_NewEdge(*args))
        _swig_setattr(self, NewEdge, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewEdge):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def midpoint(*args):
        """midpoint(self) -> NewPoint"""
        return _dolfin.NewEdge_midpoint(*args)


class NewEdgePtr(NewEdge):
    def __init__(self, this):
        _swig_setattr(self, NewEdge, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewEdge, 'thisown', 0)
        self.__class__ = NewEdge
_dolfin.NewEdge_swigregister(NewEdgePtr)

class NewEdgeIterator(MeshEntityIterator):
    """Proxy of C++ NewEdgeIterator class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewEdgeIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewEdgeIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewEdgeIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh) -> NewEdgeIterator
        __init__(self, MeshEntity entity) -> NewEdgeIterator
        __init__(self, MeshEntityIterator it) -> NewEdgeIterator
        """
        _swig_setattr(self, NewEdgeIterator, 'this', _dolfin.new_NewEdgeIterator(*args))
        _swig_setattr(self, NewEdgeIterator, 'thisown', 1)
    def __ref__(*args):
        """__ref__(self) -> NewEdge"""
        return _dolfin.NewEdgeIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> NewEdge"""
        return _dolfin.NewEdgeIterator___deref__(*args)

    def midpoint(*args):
        """midpoint(self) -> NewPoint"""
        return _dolfin.NewEdgeIterator_midpoint(*args)

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.NewEdgeIterator_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.NewEdgeIterator_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.NewEdgeIterator_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.NewEdgeIterator_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.NewEdgeIterator_connections(*args)


class NewEdgeIteratorPtr(NewEdgeIterator):
    def __init__(self, this):
        _swig_setattr(self, NewEdgeIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewEdgeIterator, 'thisown', 0)
        self.__class__ = NewEdgeIterator
_dolfin.NewEdgeIterator_swigregister(NewEdgeIteratorPtr)

class NewFace(MeshEntity):
    """Proxy of C++ NewFace class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntity]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewFace, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntity]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewFace, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewFace instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, NewMesh mesh, uint index) -> NewFace"""
        _swig_setattr(self, NewFace, 'this', _dolfin.new_NewFace(*args))
        _swig_setattr(self, NewFace, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewFace):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class NewFacePtr(NewFace):
    def __init__(self, this):
        _swig_setattr(self, NewFace, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewFace, 'thisown', 0)
        self.__class__ = NewFace
_dolfin.NewFace_swigregister(NewFacePtr)

class NewFaceIterator(MeshEntityIterator):
    """Proxy of C++ NewFaceIterator class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewFaceIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewFaceIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewFaceIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh) -> NewFaceIterator
        __init__(self, MeshEntity entity) -> NewFaceIterator
        __init__(self, MeshEntityIterator it) -> NewFaceIterator
        """
        _swig_setattr(self, NewFaceIterator, 'this', _dolfin.new_NewFaceIterator(*args))
        _swig_setattr(self, NewFaceIterator, 'thisown', 1)
    def __ref__(*args):
        """__ref__(self) -> NewFace"""
        return _dolfin.NewFaceIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> NewFace"""
        return _dolfin.NewFaceIterator___deref__(*args)

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.NewFaceIterator_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.NewFaceIterator_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.NewFaceIterator_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.NewFaceIterator_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.NewFaceIterator_connections(*args)


class NewFaceIteratorPtr(NewFaceIterator):
    def __init__(self, this):
        _swig_setattr(self, NewFaceIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewFaceIterator, 'thisown', 0)
        self.__class__ = NewFaceIterator
_dolfin.NewFaceIterator_swigregister(NewFaceIteratorPtr)

class NewFacet(MeshEntity):
    """Proxy of C++ NewFacet class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntity]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewFacet, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntity]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewFacet, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewFacet instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, NewMesh mesh, uint index) -> NewFacet"""
        _swig_setattr(self, NewFacet, 'this', _dolfin.new_NewFacet(*args))
        _swig_setattr(self, NewFacet, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewFacet):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class NewFacetPtr(NewFacet):
    def __init__(self, this):
        _swig_setattr(self, NewFacet, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewFacet, 'thisown', 0)
        self.__class__ = NewFacet
_dolfin.NewFacet_swigregister(NewFacetPtr)

class NewFacetIterator(MeshEntityIterator):
    """Proxy of C++ NewFacetIterator class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewFacetIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewFacetIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewFacetIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh) -> NewFacetIterator
        __init__(self, MeshEntity entity) -> NewFacetIterator
        __init__(self, MeshEntityIterator it) -> NewFacetIterator
        """
        _swig_setattr(self, NewFacetIterator, 'this', _dolfin.new_NewFacetIterator(*args))
        _swig_setattr(self, NewFacetIterator, 'thisown', 1)
    def __ref__(*args):
        """__ref__(self) -> NewFacet"""
        return _dolfin.NewFacetIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> NewFacet"""
        return _dolfin.NewFacetIterator___deref__(*args)

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.NewFacetIterator_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.NewFacetIterator_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.NewFacetIterator_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.NewFacetIterator_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.NewFacetIterator_connections(*args)


class NewFacetIteratorPtr(NewFacetIterator):
    def __init__(self, this):
        _swig_setattr(self, NewFacetIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewFacetIterator, 'thisown', 0)
        self.__class__ = NewFacetIterator
_dolfin.NewFacetIterator_swigregister(NewFacetIteratorPtr)

class NewCell(MeshEntity):
    """Proxy of C++ NewCell class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntity]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewCell, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntity]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewCell, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewCell instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, NewMesh mesh, uint index) -> NewCell"""
        _swig_setattr(self, NewCell, 'this', _dolfin.new_NewCell(*args))
        _swig_setattr(self, NewCell, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewCell):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class NewCellPtr(NewCell):
    def __init__(self, this):
        _swig_setattr(self, NewCell, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewCell, 'thisown', 0)
        self.__class__ = NewCell
_dolfin.NewCell_swigregister(NewCellPtr)

class NewCellIterator(MeshEntityIterator):
    """Proxy of C++ NewCellIterator class"""
    __swig_setmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewCellIterator, name, value)
    __swig_getmethods__ = {}
    for _s in [MeshEntityIterator]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewCellIterator, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewCellIterator instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, NewMesh mesh) -> NewCellIterator
        __init__(self, MeshEntity entity) -> NewCellIterator
        __init__(self, MeshEntityIterator it) -> NewCellIterator
        """
        _swig_setattr(self, NewCellIterator, 'this', _dolfin.new_NewCellIterator(*args))
        _swig_setattr(self, NewCellIterator, 'thisown', 1)
    def __ref__(*args):
        """__ref__(self) -> NewCell"""
        return _dolfin.NewCellIterator___ref__(*args)

    def __deref__(*args):
        """__deref__(self) -> NewCell"""
        return _dolfin.NewCellIterator___deref__(*args)

    def mesh(*args):
        """
        mesh(self) -> NewMesh
        mesh(self) -> NewMesh
        """
        return _dolfin.NewCellIterator_mesh(*args)

    def dim(*args):
        """dim(self) -> uint"""
        return _dolfin.NewCellIterator_dim(*args)

    def index(*args):
        """index(self) -> uint"""
        return _dolfin.NewCellIterator_index(*args)

    def numConnections(*args):
        """numConnections(self, uint dim) -> uint"""
        return _dolfin.NewCellIterator_numConnections(*args)

    def connections(*args):
        """
        connections(self, uint dim) -> uint
        connections(self, uint dim) -> uint
        """
        return _dolfin.NewCellIterator_connections(*args)


class NewCellIteratorPtr(NewCellIterator):
    def __init__(self, this):
        _swig_setattr(self, NewCellIterator, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewCellIterator, 'thisown', 0)
        self.__class__ = NewCellIterator
_dolfin.NewCellIterator_swigregister(NewCellIteratorPtr)

class TopologyComputation(_object):
    """Proxy of C++ TopologyComputation class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TopologyComputation, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TopologyComputation, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TopologyComputation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def computeEntities(*args):
        """computeEntities(NewMesh mesh, uint dim) -> uint"""
        return _dolfin.TopologyComputation_computeEntities(*args)

    if _newclass:computeEntities = staticmethod(computeEntities)
    __swig_getmethods__["computeEntities"] = lambda x: computeEntities
    def computeConnectivity(*args):
        """computeConnectivity(NewMesh mesh, uint d0, uint d1)"""
        return _dolfin.TopologyComputation_computeConnectivity(*args)

    if _newclass:computeConnectivity = staticmethod(computeConnectivity)
    __swig_getmethods__["computeConnectivity"] = lambda x: computeConnectivity

class TopologyComputationPtr(TopologyComputation):
    def __init__(self, this):
        _swig_setattr(self, TopologyComputation, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TopologyComputation, 'thisown', 0)
        self.__class__ = TopologyComputation
_dolfin.TopologyComputation_swigregister(TopologyComputationPtr)

def TopologyComputation_computeEntities(*args):
    """TopologyComputation_computeEntities(NewMesh mesh, uint dim) -> uint"""
    return _dolfin.TopologyComputation_computeEntities(*args)

def TopologyComputation_computeConnectivity(*args):
    """TopologyComputation_computeConnectivity(NewMesh mesh, uint d0, uint d1)"""
    return _dolfin.TopologyComputation_computeConnectivity(*args)

class CellType(_object):
    """Proxy of C++ CellType class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, CellType, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, CellType, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::CellType instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    point = _dolfin.CellType_point
    interval = _dolfin.CellType_interval
    triangle = _dolfin.CellType_triangle
    tetrahedron = _dolfin.CellType_tetrahedron
    def __del__(self, destroy=_dolfin.delete_CellType):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def create(*args):
        """create(Type type) -> CellType"""
        return _dolfin.CellType_create(*args)

    if _newclass:create = staticmethod(create)
    __swig_getmethods__["create"] = lambda x: create
    def type(*args):
        """type(string type) -> int"""
        return _dolfin.CellType_type(*args)

    if _newclass:type = staticmethod(type)
    __swig_getmethods__["type"] = lambda x: type
    def cellType(*args):
        """cellType(self) -> int"""
        return _dolfin.CellType_cellType(*args)

    def facetType(*args):
        """facetType(self) -> int"""
        return _dolfin.CellType_facetType(*args)

    def numEntities(*args):
        """numEntities(self, uint dim) -> uint"""
        return _dolfin.CellType_numEntities(*args)

    def numVertices(*args):
        """numVertices(self, uint dim) -> uint"""
        return _dolfin.CellType_numVertices(*args)

    def createEntities(*args):
        """createEntities(self, uint e, uint dim, uint v)"""
        return _dolfin.CellType_createEntities(*args)

    def refineCell(*args):
        """refineCell(self, NewCell cell, MeshEditor editor, uint current_cell)"""
        return _dolfin.CellType_refineCell(*args)

    def description(*args):
        """description(self) -> string"""
        return _dolfin.CellType_description(*args)


class CellTypePtr(CellType):
    def __init__(self, this):
        _swig_setattr(self, CellType, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, CellType, 'thisown', 0)
        self.__class__ = CellType
_dolfin.CellType_swigregister(CellTypePtr)

def CellType_create(*args):
    """CellType_create(Type type) -> CellType"""
    return _dolfin.CellType_create(*args)

def CellType_type(*args):
    """CellType_type(string type) -> int"""
    return _dolfin.CellType_type(*args)

class Interval(CellType):
    """Proxy of C++ Interval class"""
    __swig_setmethods__ = {}
    for _s in [CellType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Interval, name, value)
    __swig_getmethods__ = {}
    for _s in [CellType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Interval, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Interval instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> Interval"""
        _swig_setattr(self, Interval, 'this', _dolfin.new_Interval(*args))
        _swig_setattr(self, Interval, 'thisown', 1)
    def numEntities(*args):
        """numEntities(self, uint dim) -> uint"""
        return _dolfin.Interval_numEntities(*args)

    def numVertices(*args):
        """numVertices(self, uint dim) -> uint"""
        return _dolfin.Interval_numVertices(*args)

    def createEntities(*args):
        """createEntities(self, uint e, uint dim, uint v)"""
        return _dolfin.Interval_createEntities(*args)

    def refineCell(*args):
        """refineCell(self, NewCell cell, MeshEditor editor, uint current_cell)"""
        return _dolfin.Interval_refineCell(*args)

    def description(*args):
        """description(self) -> string"""
        return _dolfin.Interval_description(*args)


class IntervalPtr(Interval):
    def __init__(self, this):
        _swig_setattr(self, Interval, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Interval, 'thisown', 0)
        self.__class__ = Interval
_dolfin.Interval_swigregister(IntervalPtr)

class NewTriangle(CellType):
    """Proxy of C++ NewTriangle class"""
    __swig_setmethods__ = {}
    for _s in [CellType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewTriangle, name, value)
    __swig_getmethods__ = {}
    for _s in [CellType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewTriangle, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewTriangle instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> NewTriangle"""
        _swig_setattr(self, NewTriangle, 'this', _dolfin.new_NewTriangle(*args))
        _swig_setattr(self, NewTriangle, 'thisown', 1)
    def numEntities(*args):
        """numEntities(self, uint dim) -> uint"""
        return _dolfin.NewTriangle_numEntities(*args)

    def numVertices(*args):
        """numVertices(self, uint dim) -> uint"""
        return _dolfin.NewTriangle_numVertices(*args)

    def createEntities(*args):
        """createEntities(self, uint e, uint dim, uint v)"""
        return _dolfin.NewTriangle_createEntities(*args)

    def refineCell(*args):
        """refineCell(self, NewCell cell, MeshEditor editor, uint current_cell)"""
        return _dolfin.NewTriangle_refineCell(*args)

    def description(*args):
        """description(self) -> string"""
        return _dolfin.NewTriangle_description(*args)


class NewTrianglePtr(NewTriangle):
    def __init__(self, this):
        _swig_setattr(self, NewTriangle, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewTriangle, 'thisown', 0)
        self.__class__ = NewTriangle
_dolfin.NewTriangle_swigregister(NewTrianglePtr)

class NewTetrahedron(CellType):
    """Proxy of C++ NewTetrahedron class"""
    __swig_setmethods__ = {}
    for _s in [CellType]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewTetrahedron, name, value)
    __swig_getmethods__ = {}
    for _s in [CellType]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewTetrahedron, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewTetrahedron instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> NewTetrahedron"""
        _swig_setattr(self, NewTetrahedron, 'this', _dolfin.new_NewTetrahedron(*args))
        _swig_setattr(self, NewTetrahedron, 'thisown', 1)
    def numEntities(*args):
        """numEntities(self, uint dim) -> uint"""
        return _dolfin.NewTetrahedron_numEntities(*args)

    def numVertices(*args):
        """numVertices(self, uint dim) -> uint"""
        return _dolfin.NewTetrahedron_numVertices(*args)

    def createEntities(*args):
        """createEntities(self, uint e, uint dim, uint v)"""
        return _dolfin.NewTetrahedron_createEntities(*args)

    def refineCell(*args):
        """refineCell(self, NewCell cell, MeshEditor editor, uint current_cell)"""
        return _dolfin.NewTetrahedron_refineCell(*args)

    def description(*args):
        """description(self) -> string"""
        return _dolfin.NewTetrahedron_description(*args)


class NewTetrahedronPtr(NewTetrahedron):
    def __init__(self, this):
        _swig_setattr(self, NewTetrahedron, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewTetrahedron, 'thisown', 0)
        self.__class__ = NewTetrahedron
_dolfin.NewTetrahedron_swigregister(NewTetrahedronPtr)

class UniformMeshRefinement(_object):
    """Proxy of C++ UniformMeshRefinement class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, UniformMeshRefinement, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, UniformMeshRefinement, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::UniformMeshRefinement instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def refine(*args):
        """refine(NewMesh mesh)"""
        return _dolfin.UniformMeshRefinement_refine(*args)

    if _newclass:refine = staticmethod(refine)
    __swig_getmethods__["refine"] = lambda x: refine
    def refineSimplex(*args):
        """refineSimplex(NewMesh mesh)"""
        return _dolfin.UniformMeshRefinement_refineSimplex(*args)

    if _newclass:refineSimplex = staticmethod(refineSimplex)
    __swig_getmethods__["refineSimplex"] = lambda x: refineSimplex

class UniformMeshRefinementPtr(UniformMeshRefinement):
    def __init__(self, this):
        _swig_setattr(self, UniformMeshRefinement, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, UniformMeshRefinement, 'thisown', 0)
        self.__class__ = UniformMeshRefinement
_dolfin.UniformMeshRefinement_swigregister(UniformMeshRefinementPtr)

def UniformMeshRefinement_refine(*args):
    """UniformMeshRefinement_refine(NewMesh mesh)"""
    return _dolfin.UniformMeshRefinement_refine(*args)

def UniformMeshRefinement_refineSimplex(*args):
    """UniformMeshRefinement_refineSimplex(NewMesh mesh)"""
    return _dolfin.UniformMeshRefinement_refineSimplex(*args)

class NewPoint(_object):
    """Proxy of C++ NewPoint class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewPoint, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, NewPoint, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewPoint instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> NewPoint
        __init__(self, real x) -> NewPoint
        __init__(self, real x, real y) -> NewPoint
        __init__(self, real x, real y, real z) -> NewPoint
        """
        _swig_setattr(self, NewPoint, 'this', _dolfin.new_NewPoint(*args))
        _swig_setattr(self, NewPoint, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_NewPoint):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def x(*args):
        """x(self) -> real"""
        return _dolfin.NewPoint_x(*args)

    def y(*args):
        """y(self) -> real"""
        return _dolfin.NewPoint_y(*args)

    def z(*args):
        """z(self) -> real"""
        return _dolfin.NewPoint_z(*args)


class NewPointPtr(NewPoint):
    def __init__(self, this):
        _swig_setattr(self, NewPoint, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewPoint, 'thisown', 0)
        self.__class__ = NewPoint
_dolfin.NewPoint_swigregister(NewPointPtr)

class BoundaryComputation(_object):
    """Proxy of C++ BoundaryComputation class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundaryComputation, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BoundaryComputation, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::BoundaryComputation instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def computeBoundary(*args):
        """computeBoundary(NewMesh mesh, BoundaryMesh boundary)"""
        return _dolfin.BoundaryComputation_computeBoundary(*args)

    if _newclass:computeBoundary = staticmethod(computeBoundary)
    __swig_getmethods__["computeBoundary"] = lambda x: computeBoundary

class BoundaryComputationPtr(BoundaryComputation):
    def __init__(self, this):
        _swig_setattr(self, BoundaryComputation, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BoundaryComputation, 'thisown', 0)
        self.__class__ = BoundaryComputation
_dolfin.BoundaryComputation_swigregister(BoundaryComputationPtr)

def BoundaryComputation_computeBoundary(*args):
    """BoundaryComputation_computeBoundary(NewMesh mesh, BoundaryMesh boundary)"""
    return _dolfin.BoundaryComputation_computeBoundary(*args)

class BoundaryMesh(NewMesh):
    """Proxy of C++ BoundaryMesh class"""
    __swig_setmethods__ = {}
    for _s in [NewMesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundaryMesh, name, value)
    __swig_getmethods__ = {}
    for _s in [NewMesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BoundaryMesh, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::BoundaryMesh instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self) -> BoundaryMesh
        __init__(self, NewMesh mesh) -> BoundaryMesh
        """
        _swig_setattr(self, BoundaryMesh, 'this', _dolfin.new_BoundaryMesh(*args))
        _swig_setattr(self, BoundaryMesh, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_BoundaryMesh):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def init(*args):
        """init(self, NewMesh mesh)"""
        return _dolfin.BoundaryMesh_init(*args)


class BoundaryMeshPtr(BoundaryMesh):
    def __init__(self, this):
        _swig_setattr(self, BoundaryMesh, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BoundaryMesh, 'thisown', 0)
        self.__class__ = BoundaryMesh
_dolfin.BoundaryMesh_swigregister(BoundaryMeshPtr)

class NewUnitCube(NewMesh):
    """Proxy of C++ NewUnitCube class"""
    __swig_setmethods__ = {}
    for _s in [NewMesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewUnitCube, name, value)
    __swig_getmethods__ = {}
    for _s in [NewMesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewUnitCube, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewUnitCube instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint nx, uint ny, uint nz) -> NewUnitCube"""
        _swig_setattr(self, NewUnitCube, 'this', _dolfin.new_NewUnitCube(*args))
        _swig_setattr(self, NewUnitCube, 'thisown', 1)

class NewUnitCubePtr(NewUnitCube):
    def __init__(self, this):
        _swig_setattr(self, NewUnitCube, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewUnitCube, 'thisown', 0)
        self.__class__ = NewUnitCube
_dolfin.NewUnitCube_swigregister(NewUnitCubePtr)

class NewUnitSquare(NewMesh):
    """Proxy of C++ NewUnitSquare class"""
    __swig_setmethods__ = {}
    for _s in [NewMesh]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, NewUnitSquare, name, value)
    __swig_getmethods__ = {}
    for _s in [NewMesh]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, NewUnitSquare, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::NewUnitSquare instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint nx, uint ny) -> NewUnitSquare"""
        _swig_setattr(self, NewUnitSquare, 'this', _dolfin.new_NewUnitSquare(*args))
        _swig_setattr(self, NewUnitSquare, 'thisown', 1)

class NewUnitSquarePtr(NewUnitSquare):
    def __init__(self, this):
        _swig_setattr(self, NewUnitSquare, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, NewUnitSquare, 'thisown', 0)
        self.__class__ = NewUnitSquare
_dolfin.NewUnitSquare_swigregister(NewUnitSquarePtr)

class Dependencies(_object):
    """Proxy of C++ Dependencies class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Dependencies, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Dependencies, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Dependencies instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint N) -> Dependencies"""
        _swig_setattr(self, Dependencies, 'this', _dolfin.new_Dependencies(*args))
        _swig_setattr(self, Dependencies, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Dependencies):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def setsize(*args):
        """setsize(self, uint i, uint size)"""
        return _dolfin.Dependencies_setsize(*args)

    def set(*args):
        """
        set(self, uint i, uint j, bool checknew=False)
        set(self, uint i, uint j)
        set(self, uBlasSparseMatrix A)
        """
        return _dolfin.Dependencies_set(*args)

    def transp(*args):
        """transp(self, Dependencies dependencies)"""
        return _dolfin.Dependencies_transp(*args)

    def detect(*args):
        """detect(self, ODE ode)"""
        return _dolfin.Dependencies_detect(*args)

    def sparse(*args):
        """sparse(self) -> bool"""
        return _dolfin.Dependencies_sparse(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.Dependencies_disp(*args)


class DependenciesPtr(Dependencies):
    def __init__(self, this):
        _swig_setattr(self, Dependencies, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Dependencies, 'thisown', 0)
        self.__class__ = Dependencies
_dolfin.Dependencies_swigregister(DependenciesPtr)

class Homotopy(_object):
    """Proxy of C++ Homotopy class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Homotopy, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Homotopy, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Homotopy instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_Homotopy):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(self)"""
        return _dolfin.Homotopy_solve(*args)

    def solutions(*args):
        """solutions(self) -> dolfin::Array<(p.dolfin::complex)>"""
        return _dolfin.Homotopy_solutions(*args)

    def z0(*args):
        """z0(self, complex z)"""
        return _dolfin.Homotopy_z0(*args)

    def F(*args):
        """F(self, complex z, complex y)"""
        return _dolfin.Homotopy_F(*args)

    def JF(*args):
        """JF(self, complex z, complex x, complex y)"""
        return _dolfin.Homotopy_JF(*args)

    def G(*args):
        """G(self, complex z, complex y)"""
        return _dolfin.Homotopy_G(*args)

    def JG(*args):
        """JG(self, complex z, complex x, complex y)"""
        return _dolfin.Homotopy_JG(*args)

    def modify(*args):
        """modify(self, complex z)"""
        return _dolfin.Homotopy_modify(*args)

    def verify(*args):
        """verify(self, complex z) -> bool"""
        return _dolfin.Homotopy_verify(*args)

    def degree(*args):
        """degree(self, uint i) -> uint"""
        return _dolfin.Homotopy_degree(*args)


class HomotopyPtr(Homotopy):
    def __init__(self, this):
        _swig_setattr(self, Homotopy, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Homotopy, 'thisown', 0)
        self.__class__ = Homotopy
_dolfin.Homotopy_swigregister(HomotopyPtr)

class HomotopyJacobian(uBlasKrylovMatrix):
    """Proxy of C++ HomotopyJacobian class"""
    __swig_setmethods__ = {}
    for _s in [uBlasKrylovMatrix]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, HomotopyJacobian, name, value)
    __swig_getmethods__ = {}
    for _s in [uBlasKrylovMatrix]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, HomotopyJacobian, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::HomotopyJacobian instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_HomotopyJacobian):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.HomotopyJacobian_size(*args)

    def mult(*args):
        """mult(self, uBlasVector x, uBlasVector y)"""
        return _dolfin.HomotopyJacobian_mult(*args)


class HomotopyJacobianPtr(HomotopyJacobian):
    def __init__(self, this):
        _swig_setattr(self, HomotopyJacobian, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, HomotopyJacobian, 'thisown', 0)
        self.__class__ = HomotopyJacobian
_dolfin.HomotopyJacobian_swigregister(HomotopyJacobianPtr)

class HomotopyODE(_object):
    """Proxy of C++ HomotopyODE class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, HomotopyODE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, HomotopyODE, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::HomotopyODE instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    ode = _dolfin.HomotopyODE_ode
    endgame = _dolfin.HomotopyODE_endgame
    def __init__(self, *args):
        """__init__(self, Homotopy homotopy, uint n, real T) -> HomotopyODE"""
        _swig_setattr(self, HomotopyODE, 'this', _dolfin.new_HomotopyODE(*args))
        _swig_setattr(self, HomotopyODE, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_HomotopyODE):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def z0(*args):
        """z0(self, complex z)"""
        return _dolfin.HomotopyODE_z0(*args)

    def f(*args):
        """f(self, complex z, real t, complex y)"""
        return _dolfin.HomotopyODE_f(*args)

    def M(*args):
        """M(self, complex x, complex y, complex z, real t)"""
        return _dolfin.HomotopyODE_M(*args)

    def J(*args):
        """J(self, complex x, complex y, complex u, real t)"""
        return _dolfin.HomotopyODE_J(*args)

    def update(*args):
        """update(self, complex z, real t, bool end) -> bool"""
        return _dolfin.HomotopyODE_update(*args)

    def state(*args):
        """state(self) -> int"""
        return _dolfin.HomotopyODE_state(*args)


class HomotopyODEPtr(HomotopyODE):
    def __init__(self, this):
        _swig_setattr(self, HomotopyODE, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, HomotopyODE, 'thisown', 0)
        self.__class__ = HomotopyODE
_dolfin.HomotopyODE_swigregister(HomotopyODEPtr)

class Method(_object):
    """Proxy of C++ Method class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Method, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Method, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Method instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    cG = _dolfin.Method_cG
    dG = _dolfin.Method_dG
    none = _dolfin.Method_none
    def __del__(self, destroy=_dolfin.delete_Method):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def type(*args):
        """type(self) -> int"""
        return _dolfin.Method_type(*args)

    def degree(*args):
        """degree(self) -> unsigned int"""
        return _dolfin.Method_degree(*args)

    def order(*args):
        """order(self) -> unsigned int"""
        return _dolfin.Method_order(*args)

    def nsize(*args):
        """nsize(self) -> unsigned int"""
        return _dolfin.Method_nsize(*args)

    def qsize(*args):
        """qsize(self) -> unsigned int"""
        return _dolfin.Method_qsize(*args)

    def npoint(*args):
        """npoint(self, unsigned int i) -> real"""
        return _dolfin.Method_npoint(*args)

    def qpoint(*args):
        """qpoint(self, unsigned int i) -> real"""
        return _dolfin.Method_qpoint(*args)

    def nweight(*args):
        """nweight(self, unsigned int i, unsigned int j) -> real"""
        return _dolfin.Method_nweight(*args)

    def qweight(*args):
        """qweight(self, unsigned int i) -> real"""
        return _dolfin.Method_qweight(*args)

    def eval(*args):
        """eval(self, unsigned int i, real tau) -> real"""
        return _dolfin.Method_eval(*args)

    def derivative(*args):
        """derivative(self, unsigned int i) -> real"""
        return _dolfin.Method_derivative(*args)

    def update(*args):
        """
        update(self, real x0, real f, real k, real values)
        update(self, real x0, real f, real k, real values, real alpha)
        """
        return _dolfin.Method_update(*args)

    def ueval(*args):
        """
        ueval(self, real x0, real values, real tau) -> real
        ueval(self, real x0, uBlasVector values, uint offset, real tau) -> real
        ueval(self, real x0, real values, uint i) -> real
        """
        return _dolfin.Method_ueval(*args)

    def residual(*args):
        """
        residual(self, real x0, real values, real f, real k) -> real
        residual(self, real x0, uBlasVector values, uint offset, real f, real k) -> real
        """
        return _dolfin.Method_residual(*args)

    def timestep(*args):
        """timestep(self, real r, real tol, real k0, real kmax) -> real"""
        return _dolfin.Method_timestep(*args)

    def error(*args):
        """error(self, real k, real r) -> real"""
        return _dolfin.Method_error(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.Method_disp(*args)


class MethodPtr(Method):
    def __init__(self, this):
        _swig_setattr(self, Method, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Method, 'thisown', 0)
        self.__class__ = Method
_dolfin.Method_swigregister(MethodPtr)

class MonoAdaptiveFixedPointSolver(_object):
    """Proxy of C++ MonoAdaptiveFixedPointSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MonoAdaptiveFixedPointSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MonoAdaptiveFixedPointSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MonoAdaptiveFixedPointSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, MonoAdaptiveTimeSlab timeslab) -> MonoAdaptiveFixedPointSolver"""
        _swig_setattr(self, MonoAdaptiveFixedPointSolver, 'this', _dolfin.new_MonoAdaptiveFixedPointSolver(*args))
        _swig_setattr(self, MonoAdaptiveFixedPointSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MonoAdaptiveFixedPointSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class MonoAdaptiveFixedPointSolverPtr(MonoAdaptiveFixedPointSolver):
    def __init__(self, this):
        _swig_setattr(self, MonoAdaptiveFixedPointSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MonoAdaptiveFixedPointSolver, 'thisown', 0)
        self.__class__ = MonoAdaptiveFixedPointSolver
_dolfin.MonoAdaptiveFixedPointSolver_swigregister(MonoAdaptiveFixedPointSolverPtr)

class MonoAdaptiveJacobian(_object):
    """Proxy of C++ MonoAdaptiveJacobian class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MonoAdaptiveJacobian, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MonoAdaptiveJacobian, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MonoAdaptiveJacobian instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, MonoAdaptiveTimeSlab timeslab, bool implicit, bool piecewise) -> MonoAdaptiveJacobian"""
        _swig_setattr(self, MonoAdaptiveJacobian, 'this', _dolfin.new_MonoAdaptiveJacobian(*args))
        _swig_setattr(self, MonoAdaptiveJacobian, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MonoAdaptiveJacobian):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.MonoAdaptiveJacobian_size(*args)

    def mult(*args):
        """mult(self, uBlasVector x, uBlasVector y)"""
        return _dolfin.MonoAdaptiveJacobian_mult(*args)


class MonoAdaptiveJacobianPtr(MonoAdaptiveJacobian):
    def __init__(self, this):
        _swig_setattr(self, MonoAdaptiveJacobian, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MonoAdaptiveJacobian, 'thisown', 0)
        self.__class__ = MonoAdaptiveJacobian
_dolfin.MonoAdaptiveJacobian_swigregister(MonoAdaptiveJacobianPtr)

class MonoAdaptiveNewtonSolver(_object):
    """Proxy of C++ MonoAdaptiveNewtonSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MonoAdaptiveNewtonSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MonoAdaptiveNewtonSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MonoAdaptiveNewtonSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, MonoAdaptiveTimeSlab timeslab, bool implicit=False) -> MonoAdaptiveNewtonSolver
        __init__(self, MonoAdaptiveTimeSlab timeslab) -> MonoAdaptiveNewtonSolver
        """
        _swig_setattr(self, MonoAdaptiveNewtonSolver, 'this', _dolfin.new_MonoAdaptiveNewtonSolver(*args))
        _swig_setattr(self, MonoAdaptiveNewtonSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MonoAdaptiveNewtonSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class MonoAdaptiveNewtonSolverPtr(MonoAdaptiveNewtonSolver):
    def __init__(self, this):
        _swig_setattr(self, MonoAdaptiveNewtonSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MonoAdaptiveNewtonSolver, 'thisown', 0)
        self.__class__ = MonoAdaptiveNewtonSolver
_dolfin.MonoAdaptiveNewtonSolver_swigregister(MonoAdaptiveNewtonSolverPtr)

class MonoAdaptiveTimeSlab(_object):
    """Proxy of C++ MonoAdaptiveTimeSlab class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MonoAdaptiveTimeSlab, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MonoAdaptiveTimeSlab, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MonoAdaptiveTimeSlab instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, ODE ode) -> MonoAdaptiveTimeSlab"""
        _swig_setattr(self, MonoAdaptiveTimeSlab, 'this', _dolfin.new_MonoAdaptiveTimeSlab(*args))
        _swig_setattr(self, MonoAdaptiveTimeSlab, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MonoAdaptiveTimeSlab):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def build(*args):
        """build(self, real a, real b) -> real"""
        return _dolfin.MonoAdaptiveTimeSlab_build(*args)

    def solve(*args):
        """solve(self) -> bool"""
        return _dolfin.MonoAdaptiveTimeSlab_solve(*args)

    def check(*args):
        """check(self, bool first) -> bool"""
        return _dolfin.MonoAdaptiveTimeSlab_check(*args)

    def shift(*args):
        """shift(self) -> bool"""
        return _dolfin.MonoAdaptiveTimeSlab_shift(*args)

    def sample(*args):
        """sample(self, real t)"""
        return _dolfin.MonoAdaptiveTimeSlab_sample(*args)

    def usample(*args):
        """usample(self, uint i, real t) -> real"""
        return _dolfin.MonoAdaptiveTimeSlab_usample(*args)

    def ksample(*args):
        """ksample(self, uint i, real t) -> real"""
        return _dolfin.MonoAdaptiveTimeSlab_ksample(*args)

    def rsample(*args):
        """rsample(self, uint i, real t) -> real"""
        return _dolfin.MonoAdaptiveTimeSlab_rsample(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.MonoAdaptiveTimeSlab_disp(*args)


class MonoAdaptiveTimeSlabPtr(MonoAdaptiveTimeSlab):
    def __init__(self, this):
        _swig_setattr(self, MonoAdaptiveTimeSlab, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MonoAdaptiveTimeSlab, 'thisown', 0)
        self.__class__ = MonoAdaptiveTimeSlab
_dolfin.MonoAdaptiveTimeSlab_swigregister(MonoAdaptiveTimeSlabPtr)

class MonoAdaptivity(_object):
    """Proxy of C++ MonoAdaptivity class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MonoAdaptivity, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MonoAdaptivity, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MonoAdaptivity instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, ODE ode, Method method) -> MonoAdaptivity"""
        _swig_setattr(self, MonoAdaptivity, 'this', _dolfin.new_MonoAdaptivity(*args))
        _swig_setattr(self, MonoAdaptivity, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MonoAdaptivity):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def timestep(*args):
        """timestep(self) -> real"""
        return _dolfin.MonoAdaptivity_timestep(*args)

    def update(*args):
        """update(self, real k0, real r, Method method, real t, bool first)"""
        return _dolfin.MonoAdaptivity_update(*args)


class MonoAdaptivityPtr(MonoAdaptivity):
    def __init__(self, this):
        _swig_setattr(self, MonoAdaptivity, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MonoAdaptivity, 'thisown', 0)
        self.__class__ = MonoAdaptivity
_dolfin.MonoAdaptivity_swigregister(MonoAdaptivityPtr)

class MultiAdaptiveFixedPointSolver(_object):
    """Proxy of C++ MultiAdaptiveFixedPointSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MultiAdaptiveFixedPointSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MultiAdaptiveFixedPointSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MultiAdaptiveFixedPointSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, MultiAdaptiveTimeSlab timeslab) -> MultiAdaptiveFixedPointSolver"""
        _swig_setattr(self, MultiAdaptiveFixedPointSolver, 'this', _dolfin.new_MultiAdaptiveFixedPointSolver(*args))
        _swig_setattr(self, MultiAdaptiveFixedPointSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MultiAdaptiveFixedPointSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class MultiAdaptiveFixedPointSolverPtr(MultiAdaptiveFixedPointSolver):
    def __init__(self, this):
        _swig_setattr(self, MultiAdaptiveFixedPointSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MultiAdaptiveFixedPointSolver, 'thisown', 0)
        self.__class__ = MultiAdaptiveFixedPointSolver
_dolfin.MultiAdaptiveFixedPointSolver_swigregister(MultiAdaptiveFixedPointSolverPtr)

class MultiAdaptivePreconditioner(uBlasPreconditioner):
    """Proxy of C++ MultiAdaptivePreconditioner class"""
    __swig_setmethods__ = {}
    for _s in [uBlasPreconditioner]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, MultiAdaptivePreconditioner, name, value)
    __swig_getmethods__ = {}
    for _s in [uBlasPreconditioner]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, MultiAdaptivePreconditioner, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MultiAdaptivePreconditioner instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, MultiAdaptiveTimeSlab timeslab, Method method) -> MultiAdaptivePreconditioner"""
        _swig_setattr(self, MultiAdaptivePreconditioner, 'this', _dolfin.new_MultiAdaptivePreconditioner(*args))
        _swig_setattr(self, MultiAdaptivePreconditioner, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MultiAdaptivePreconditioner):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(self, uBlasVector x, uBlasVector b)"""
        return _dolfin.MultiAdaptivePreconditioner_solve(*args)


class MultiAdaptivePreconditionerPtr(MultiAdaptivePreconditioner):
    def __init__(self, this):
        _swig_setattr(self, MultiAdaptivePreconditioner, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MultiAdaptivePreconditioner, 'thisown', 0)
        self.__class__ = MultiAdaptivePreconditioner
_dolfin.MultiAdaptivePreconditioner_swigregister(MultiAdaptivePreconditionerPtr)

class MultiAdaptiveNewtonSolver(_object):
    """Proxy of C++ MultiAdaptiveNewtonSolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MultiAdaptiveNewtonSolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MultiAdaptiveNewtonSolver, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MultiAdaptiveNewtonSolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, MultiAdaptiveTimeSlab timeslab) -> MultiAdaptiveNewtonSolver"""
        _swig_setattr(self, MultiAdaptiveNewtonSolver, 'this', _dolfin.new_MultiAdaptiveNewtonSolver(*args))
        _swig_setattr(self, MultiAdaptiveNewtonSolver, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MultiAdaptiveNewtonSolver):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass


class MultiAdaptiveNewtonSolverPtr(MultiAdaptiveNewtonSolver):
    def __init__(self, this):
        _swig_setattr(self, MultiAdaptiveNewtonSolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MultiAdaptiveNewtonSolver, 'thisown', 0)
        self.__class__ = MultiAdaptiveNewtonSolver
_dolfin.MultiAdaptiveNewtonSolver_swigregister(MultiAdaptiveNewtonSolverPtr)

class MultiAdaptiveTimeSlab(_object):
    """Proxy of C++ MultiAdaptiveTimeSlab class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MultiAdaptiveTimeSlab, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MultiAdaptiveTimeSlab, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MultiAdaptiveTimeSlab instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, ODE ode) -> MultiAdaptiveTimeSlab"""
        _swig_setattr(self, MultiAdaptiveTimeSlab, 'this', _dolfin.new_MultiAdaptiveTimeSlab(*args))
        _swig_setattr(self, MultiAdaptiveTimeSlab, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MultiAdaptiveTimeSlab):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def build(*args):
        """build(self, real a, real b) -> real"""
        return _dolfin.MultiAdaptiveTimeSlab_build(*args)

    def solve(*args):
        """solve(self) -> bool"""
        return _dolfin.MultiAdaptiveTimeSlab_solve(*args)

    def check(*args):
        """check(self, bool first) -> bool"""
        return _dolfin.MultiAdaptiveTimeSlab_check(*args)

    def shift(*args):
        """shift(self) -> bool"""
        return _dolfin.MultiAdaptiveTimeSlab_shift(*args)

    def reset(*args):
        """reset(self)"""
        return _dolfin.MultiAdaptiveTimeSlab_reset(*args)

    def sample(*args):
        """sample(self, real t)"""
        return _dolfin.MultiAdaptiveTimeSlab_sample(*args)

    def usample(*args):
        """usample(self, uint i, real t) -> real"""
        return _dolfin.MultiAdaptiveTimeSlab_usample(*args)

    def ksample(*args):
        """ksample(self, uint i, real t) -> real"""
        return _dolfin.MultiAdaptiveTimeSlab_ksample(*args)

    def rsample(*args):
        """rsample(self, uint i, real t) -> real"""
        return _dolfin.MultiAdaptiveTimeSlab_rsample(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.MultiAdaptiveTimeSlab_disp(*args)


class MultiAdaptiveTimeSlabPtr(MultiAdaptiveTimeSlab):
    def __init__(self, this):
        _swig_setattr(self, MultiAdaptiveTimeSlab, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MultiAdaptiveTimeSlab, 'thisown', 0)
        self.__class__ = MultiAdaptiveTimeSlab
_dolfin.MultiAdaptiveTimeSlab_swigregister(MultiAdaptiveTimeSlabPtr)

class MultiAdaptivity(_object):
    """Proxy of C++ MultiAdaptivity class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, MultiAdaptivity, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, MultiAdaptivity, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::MultiAdaptivity instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, ODE ode, Method method) -> MultiAdaptivity"""
        _swig_setattr(self, MultiAdaptivity, 'this', _dolfin.new_MultiAdaptivity(*args))
        _swig_setattr(self, MultiAdaptivity, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_MultiAdaptivity):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def timestep(*args):
        """timestep(self, uint i) -> real"""
        return _dolfin.MultiAdaptivity_timestep(*args)

    def residual(*args):
        """residual(self, uint i) -> real"""
        return _dolfin.MultiAdaptivity_residual(*args)

    def update(*args):
        """update(self, MultiAdaptiveTimeSlab ts, real t, bool first)"""
        return _dolfin.MultiAdaptivity_update(*args)


class MultiAdaptivityPtr(MultiAdaptivity):
    def __init__(self, this):
        _swig_setattr(self, MultiAdaptivity, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, MultiAdaptivity, 'thisown', 0)
        self.__class__ = MultiAdaptivity
_dolfin.MultiAdaptivity_swigregister(MultiAdaptivityPtr)

class ODE(_object):
    """Proxy of C++ ODE class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ODE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ODE, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::ODE instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint N, real T) -> ODE"""
        if self.__class__ == ODE:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, ODE, 'this', _dolfin.new_ODE(*args))
        _swig_setattr(self, ODE, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_ODE):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def u0(*args):
        """u0(self, uBlasVector u)"""
        return _dolfin.ODE_u0(*args)

    def f(*args):
        """
        f(self, uBlasVector u, real t, uBlasVector y)
        f(self, uBlasVector u, real t, uint i) -> real
        """
        return _dolfin.ODE_f(*args)

    def M(*args):
        """M(self, uBlasVector x, uBlasVector y, uBlasVector u, real t)"""
        return _dolfin.ODE_M(*args)

    def J(*args):
        """J(self, uBlasVector x, uBlasVector y, uBlasVector u, real t)"""
        return _dolfin.ODE_J(*args)

    def dfdu(*args):
        """dfdu(self, uBlasVector u, real t, uint i, uint j) -> real"""
        return _dolfin.ODE_dfdu(*args)

    def timestep(*args):
        """
        timestep(self, real t, real k0) -> real
        timestep(self, real t, uint i, real k0) -> real
        """
        return _dolfin.ODE_timestep(*args)

    def update(*args):
        """update(self, uBlasVector u, real t, bool end) -> bool"""
        return _dolfin.ODE_update(*args)

    def save(*args):
        """save(self, Sample sample)"""
        return _dolfin.ODE_save(*args)

    def sparse(*args):
        """
        sparse(self)
        sparse(self, uBlasSparseMatrix A)
        """
        return _dolfin.ODE_sparse(*args)

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.ODE_size(*args)

    def endtime(*args):
        """endtime(self) -> real"""
        return _dolfin.ODE_endtime(*args)

    def solve(*args):
        """solve(self)"""
        return _dolfin.ODE_solve(*args)

    def __disown__(self):
        self.thisown = 0
        _dolfin.disown_ODE(self)
        return weakref_proxy(self)

class ODEPtr(ODE):
    def __init__(self, this):
        _swig_setattr(self, ODE, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ODE, 'thisown', 0)
        self.__class__ = ODE
_dolfin.ODE_swigregister(ODEPtr)

class ODESolver(_object):
    """Proxy of C++ ODESolver class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, ODESolver, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, ODESolver, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::ODESolver instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def solve(*args):
        """solve(ODE ode)"""
        return _dolfin.ODESolver_solve(*args)

    if _newclass:solve = staticmethod(solve)
    __swig_getmethods__["solve"] = lambda x: solve

class ODESolverPtr(ODESolver):
    def __init__(self, this):
        _swig_setattr(self, ODESolver, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, ODESolver, 'thisown', 0)
        self.__class__ = ODESolver
_dolfin.ODESolver_swigregister(ODESolverPtr)

def ODESolver_solve(*args):
    """ODESolver_solve(ODE ode)"""
    return _dolfin.ODESolver_solve(*args)

class Partition(_object):
    """Proxy of C++ Partition class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Partition, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Partition, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Partition instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, uint N) -> Partition"""
        _swig_setattr(self, Partition, 'this', _dolfin.new_Partition(*args))
        _swig_setattr(self, Partition, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Partition):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.Partition_size(*args)

    def index(*args):
        """index(self, uint pos) -> uint"""
        return _dolfin.Partition_index(*args)

    def update(*args):
        """
        update(self, uint offset, uint end, MultiAdaptivity adaptivity, 
            real K) -> real
        """
        return _dolfin.Partition_update(*args)

    def debug(*args):
        """debug(self, uint offset, uint end)"""
        return _dolfin.Partition_debug(*args)


class PartitionPtr(Partition):
    def __init__(self, this):
        _swig_setattr(self, Partition, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Partition, 'thisown', 0)
        self.__class__ = Partition
_dolfin.Partition_swigregister(PartitionPtr)

class Sample(Variable):
    """Proxy of C++ Sample class"""
    __swig_setmethods__ = {}
    for _s in [Variable]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, Sample, name, value)
    __swig_getmethods__ = {}
    for _s in [Variable]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, Sample, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::Sample instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, TimeSlab timeslab, real t, string name, string label) -> Sample"""
        _swig_setattr(self, Sample, 'this', _dolfin.new_Sample(*args))
        _swig_setattr(self, Sample, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_Sample):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.Sample_size(*args)

    def t(*args):
        """t(self) -> real"""
        return _dolfin.Sample_t(*args)

    def u(*args):
        """u(self, uint index) -> real"""
        return _dolfin.Sample_u(*args)

    def k(*args):
        """k(self, uint index) -> real"""
        return _dolfin.Sample_k(*args)

    def r(*args):
        """r(self, uint index) -> real"""
        return _dolfin.Sample_r(*args)


class SamplePtr(Sample):
    def __init__(self, this):
        _swig_setattr(self, Sample, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Sample, 'thisown', 0)
        self.__class__ = Sample
_dolfin.Sample_swigregister(SamplePtr)

class TimeSlab(_object):
    """Proxy of C++ TimeSlab class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TimeSlab, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TimeSlab, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TimeSlab instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_TimeSlab):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def build(*args):
        """build(self, real a, real b) -> real"""
        return _dolfin.TimeSlab_build(*args)

    def solve(*args):
        """solve(self) -> bool"""
        return _dolfin.TimeSlab_solve(*args)

    def check(*args):
        """check(self, bool first) -> bool"""
        return _dolfin.TimeSlab_check(*args)

    def shift(*args):
        """shift(self) -> bool"""
        return _dolfin.TimeSlab_shift(*args)

    def sample(*args):
        """sample(self, real t)"""
        return _dolfin.TimeSlab_sample(*args)

    def size(*args):
        """size(self) -> uint"""
        return _dolfin.TimeSlab_size(*args)

    def starttime(*args):
        """starttime(self) -> real"""
        return _dolfin.TimeSlab_starttime(*args)

    def endtime(*args):
        """endtime(self) -> real"""
        return _dolfin.TimeSlab_endtime(*args)

    def length(*args):
        """length(self) -> real"""
        return _dolfin.TimeSlab_length(*args)

    def usample(*args):
        """usample(self, uint i, real t) -> real"""
        return _dolfin.TimeSlab_usample(*args)

    def ksample(*args):
        """ksample(self, uint i, real t) -> real"""
        return _dolfin.TimeSlab_ksample(*args)

    def rsample(*args):
        """rsample(self, uint i, real t) -> real"""
        return _dolfin.TimeSlab_rsample(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.TimeSlab_disp(*args)


class TimeSlabPtr(TimeSlab):
    def __init__(self, this):
        _swig_setattr(self, TimeSlab, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TimeSlab, 'thisown', 0)
        self.__class__ = TimeSlab
_dolfin.TimeSlab_swigregister(TimeSlabPtr)

class TimeSlabJacobian(uBlasKrylovMatrix):
    """Proxy of C++ TimeSlabJacobian class"""
    __swig_setmethods__ = {}
    for _s in [uBlasKrylovMatrix]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, TimeSlabJacobian, name, value)
    __swig_getmethods__ = {}
    for _s in [uBlasKrylovMatrix]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, TimeSlabJacobian, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TimeSlabJacobian instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_TimeSlabJacobian):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def size(*args):
        """size(self, uint dim) -> uint"""
        return _dolfin.TimeSlabJacobian_size(*args)

    def mult(*args):
        """mult(self, uBlasVector x, uBlasVector y)"""
        return _dolfin.TimeSlabJacobian_mult(*args)

    def init(*args):
        """init(self)"""
        return _dolfin.TimeSlabJacobian_init(*args)

    def update(*args):
        """update(self)"""
        return _dolfin.TimeSlabJacobian_update(*args)

    def matrix(*args):
        """matrix(self) -> uBlasDenseMatrix"""
        return _dolfin.TimeSlabJacobian_matrix(*args)


class TimeSlabJacobianPtr(TimeSlabJacobian):
    def __init__(self, this):
        _swig_setattr(self, TimeSlabJacobian, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TimeSlabJacobian, 'thisown', 0)
        self.__class__ = TimeSlabJacobian
_dolfin.TimeSlabJacobian_swigregister(TimeSlabJacobianPtr)

class TimeStepper(_object):
    """Proxy of C++ TimeStepper class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TimeStepper, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TimeStepper, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TimeStepper instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, ODE ode) -> TimeStepper"""
        _swig_setattr(self, TimeStepper, 'this', _dolfin.new_TimeStepper(*args))
        _swig_setattr(self, TimeStepper, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_TimeStepper):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(ODE ode)"""
        return _dolfin.TimeStepper_solve(*args)

    if _newclass:solve = staticmethod(solve)
    __swig_getmethods__["solve"] = lambda x: solve
    def step(*args):
        """step(self) -> real"""
        return _dolfin.TimeStepper_step(*args)

    def finished(*args):
        """finished(self) -> bool"""
        return _dolfin.TimeStepper_finished(*args)


class TimeStepperPtr(TimeStepper):
    def __init__(self, this):
        _swig_setattr(self, TimeStepper, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TimeStepper, 'thisown', 0)
        self.__class__ = TimeStepper
_dolfin.TimeStepper_swigregister(TimeStepperPtr)

def TimeStepper_solve(*args):
    """TimeStepper_solve(ODE ode)"""
    return _dolfin.TimeStepper_solve(*args)

class cGqMethod(Method):
    """Proxy of C++ cGqMethod class"""
    __swig_setmethods__ = {}
    for _s in [Method]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, cGqMethod, name, value)
    __swig_getmethods__ = {}
    for _s in [Method]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, cGqMethod, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::cGqMethod instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, unsigned int q) -> cGqMethod"""
        _swig_setattr(self, cGqMethod, 'this', _dolfin.new_cGqMethod(*args))
        _swig_setattr(self, cGqMethod, 'thisown', 1)
    def ueval(*args):
        """
        ueval(self, real x0, real values, real tau) -> real
        ueval(self, real x0, uBlasVector values, uint offset, real tau) -> real
        ueval(self, real x0, real values, uint i) -> real
        """
        return _dolfin.cGqMethod_ueval(*args)

    def residual(*args):
        """
        residual(self, real x0, real values, real f, real k) -> real
        residual(self, real x0, uBlasVector values, uint offset, real f, real k) -> real
        """
        return _dolfin.cGqMethod_residual(*args)

    def timestep(*args):
        """timestep(self, real r, real tol, real k0, real kmax) -> real"""
        return _dolfin.cGqMethod_timestep(*args)

    def error(*args):
        """error(self, real k, real r) -> real"""
        return _dolfin.cGqMethod_error(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.cGqMethod_disp(*args)


class cGqMethodPtr(cGqMethod):
    def __init__(self, this):
        _swig_setattr(self, cGqMethod, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, cGqMethod, 'thisown', 0)
        self.__class__ = cGqMethod
_dolfin.cGqMethod_swigregister(cGqMethodPtr)

class dGqMethod(Method):
    """Proxy of C++ dGqMethod class"""
    __swig_setmethods__ = {}
    for _s in [Method]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, dGqMethod, name, value)
    __swig_getmethods__ = {}
    for _s in [Method]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, dGqMethod, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::dGqMethod instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, unsigned int q) -> dGqMethod"""
        _swig_setattr(self, dGqMethod, 'this', _dolfin.new_dGqMethod(*args))
        _swig_setattr(self, dGqMethod, 'thisown', 1)
    def ueval(*args):
        """
        ueval(self, real x0, real values, real tau) -> real
        ueval(self, real x0, uBlasVector values, uint offset, real tau) -> real
        ueval(self, real x0, real values, uint i) -> real
        """
        return _dolfin.dGqMethod_ueval(*args)

    def residual(*args):
        """
        residual(self, real x0, real values, real f, real k) -> real
        residual(self, real x0, uBlasVector values, uint offset, real f, real k) -> real
        """
        return _dolfin.dGqMethod_residual(*args)

    def timestep(*args):
        """timestep(self, real r, real tol, real k0, real kmax) -> real"""
        return _dolfin.dGqMethod_timestep(*args)

    def error(*args):
        """error(self, real k, real r) -> real"""
        return _dolfin.dGqMethod_error(*args)

    def disp(*args):
        """disp(self)"""
        return _dolfin.dGqMethod_disp(*args)


class dGqMethodPtr(dGqMethod):
    def __init__(self, this):
        _swig_setattr(self, dGqMethod, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, dGqMethod, 'thisown', 0)
        self.__class__ = dGqMethod
_dolfin.dGqMethod_swigregister(dGqMethodPtr)

class TimeDependentPDE(_object):
    """Proxy of C++ TimeDependentPDE class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, TimeDependentPDE, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, TimeDependentPDE, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TimeDependentPDE instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """
        __init__(self, BilinearForm a, LinearForm L, Mesh mesh, BoundaryCondition bc, 
            int N, real k, real T) -> TimeDependentPDE
        """
        if self.__class__ == TimeDependentPDE:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, TimeDependentPDE, 'this', _dolfin.new_TimeDependentPDE(*args))
        _swig_setattr(self, TimeDependentPDE, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_TimeDependentPDE):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def solve(*args):
        """solve(self, Function u) -> uint"""
        return _dolfin.TimeDependentPDE_solve(*args)

    def fu(*args):
        """fu(self, Vector x, Vector dotx, real t)"""
        return _dolfin.TimeDependentPDE_fu(*args)

    def init(*args):
        """init(self, Function U)"""
        return _dolfin.TimeDependentPDE_init(*args)

    def save(*args):
        """save(self, Function U, real t)"""
        return _dolfin.TimeDependentPDE_save(*args)

    def preparestep(*args):
        """preparestep(self)"""
        return _dolfin.TimeDependentPDE_preparestep(*args)

    def prepareiteration(*args):
        """prepareiteration(self)"""
        return _dolfin.TimeDependentPDE_prepareiteration(*args)

    def elementdim(*args):
        """elementdim(self) -> uint"""
        return _dolfin.TimeDependentPDE_elementdim(*args)

    def a(*args):
        """a(self) -> BilinearForm"""
        return _dolfin.TimeDependentPDE_a(*args)

    def L(*args):
        """L(self) -> LinearForm"""
        return _dolfin.TimeDependentPDE_L(*args)

    def mesh(*args):
        """mesh(self) -> Mesh"""
        return _dolfin.TimeDependentPDE_mesh(*args)

    def bc(*args):
        """bc(self) -> BoundaryCondition"""
        return _dolfin.TimeDependentPDE_bc(*args)

    __swig_setmethods__["x"] = _dolfin.TimeDependentPDE_x_set
    __swig_getmethods__["x"] = _dolfin.TimeDependentPDE_x_get
    if _newclass:x = property(_dolfin.TimeDependentPDE_x_get, _dolfin.TimeDependentPDE_x_set)
    __swig_setmethods__["dotx"] = _dolfin.TimeDependentPDE_dotx_set
    __swig_getmethods__["dotx"] = _dolfin.TimeDependentPDE_dotx_get
    if _newclass:dotx = property(_dolfin.TimeDependentPDE_dotx_get, _dolfin.TimeDependentPDE_dotx_set)
    __swig_setmethods__["k"] = _dolfin.TimeDependentPDE_k_set
    __swig_getmethods__["k"] = _dolfin.TimeDependentPDE_k_get
    if _newclass:k = property(_dolfin.TimeDependentPDE_k_get, _dolfin.TimeDependentPDE_k_set)
    def __disown__(self):
        self.thisown = 0
        _dolfin.disown_TimeDependentPDE(self)
        return weakref_proxy(self)

class TimeDependentPDEPtr(TimeDependentPDE):
    def __init__(self, this):
        _swig_setattr(self, TimeDependentPDE, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TimeDependentPDE, 'thisown', 0)
        self.__class__ = TimeDependentPDE
_dolfin.TimeDependentPDE_swigregister(TimeDependentPDEPtr)

class TimeDependentODE(ODE):
    """Proxy of C++ TimeDependentODE class"""
    __swig_setmethods__ = {}
    for _s in [ODE]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, TimeDependentODE, name, value)
    __swig_getmethods__ = {}
    for _s in [ODE]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, TimeDependentODE, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::TimeDependentODE instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self, TimeDependentPDE pde, int N, real T) -> TimeDependentODE"""
        _swig_setattr(self, TimeDependentODE, 'this', _dolfin.new_TimeDependentODE(*args))
        _swig_setattr(self, TimeDependentODE, 'thisown', 1)
    def u0(*args):
        """u0(self, uBlasVector u)"""
        return _dolfin.TimeDependentODE_u0(*args)

    def timestep(*args):
        """timestep(self, real t, real k0) -> real"""
        return _dolfin.TimeDependentODE_timestep(*args)

    def f(*args):
        """
        f(self, uBlasVector u, real t, uBlasVector y)
        f(self, uBlasVector u, real t, uint i) -> real
        """
        return _dolfin.TimeDependentODE_f(*args)

    def fmono(*args):
        """fmono(self, real u, real t, real y)"""
        return _dolfin.TimeDependentODE_fmono(*args)

    def update(*args):
        """update(self, real u, real t, bool end) -> bool"""
        return _dolfin.TimeDependentODE_update(*args)


class TimeDependentODEPtr(TimeDependentODE):
    def __init__(self, this):
        _swig_setattr(self, TimeDependentODE, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, TimeDependentODE, 'thisown', 0)
        self.__class__ = TimeDependentODE
_dolfin.TimeDependentODE_swigregister(TimeDependentODEPtr)

class FEM(_object):
    """Proxy of C++ FEM class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FEM, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FEM, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::FEM instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def assemble(*args):
        """
        assemble(BilinearForm a, LinearForm L, GenericMatrix A, GenericVector b, 
            Mesh mesh)
        assemble(BilinearForm a, LinearForm L, GenericMatrix A, GenericVector b, 
            Mesh mesh, BoundaryCondition bc)
        assemble(BilinearForm a, GenericMatrix A, Mesh mesh)
        assemble(LinearForm L, GenericVector b, Mesh mesh)
        """
        return _dolfin.FEM_assemble(*args)

    if _newclass:assemble = staticmethod(assemble)
    __swig_getmethods__["assemble"] = lambda x: assemble
    def applyBC(*args):
        """
        applyBC(GenericMatrix A, GenericVector b, Mesh mesh, FiniteElement element, 
            BoundaryCondition bc)
        applyBC(GenericMatrix A, Mesh mesh, FiniteElement element, 
            BoundaryCondition bc)
        applyBC(GenericVector b, Mesh mesh, FiniteElement element, 
            BoundaryCondition bc)
        """
        return _dolfin.FEM_applyBC(*args)

    if _newclass:applyBC = staticmethod(applyBC)
    __swig_getmethods__["applyBC"] = lambda x: applyBC
    def assembleResidualBC(*args):
        """
        assembleResidualBC(GenericMatrix A, GenericVector b, GenericVector x, 
            Mesh mesh, FiniteElement element, BoundaryCondition bc)
        assembleResidualBC(GenericVector b, GenericVector x, Mesh mesh, FiniteElement element, 
            BoundaryCondition bc)
        """
        return _dolfin.FEM_assembleResidualBC(*args)

    if _newclass:assembleResidualBC = staticmethod(assembleResidualBC)
    __swig_getmethods__["assembleResidualBC"] = lambda x: assembleResidualBC
    def size(*args):
        """size(Mesh mesh, FiniteElement element) -> uint"""
        return _dolfin.FEM_size(*args)

    if _newclass:size = staticmethod(size)
    __swig_getmethods__["size"] = lambda x: size
    def disp(*args):
        """disp(Mesh mesh, FiniteElement element)"""
        return _dolfin.FEM_disp(*args)

    if _newclass:disp = staticmethod(disp)
    __swig_getmethods__["disp"] = lambda x: disp

class FEMPtr(FEM):
    def __init__(self, this):
        _swig_setattr(self, FEM, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FEM, 'thisown', 0)
        self.__class__ = FEM
_dolfin.FEM_swigregister(FEMPtr)

def FEM_assemble(*args):
    """
    assemble(BilinearForm a, LinearForm L, GenericMatrix A, GenericVector b, 
        Mesh mesh)
    assemble(BilinearForm a, LinearForm L, GenericMatrix A, GenericVector b, 
        Mesh mesh, BoundaryCondition bc)
    assemble(BilinearForm a, GenericMatrix A, Mesh mesh)
    FEM_assemble(LinearForm L, GenericVector b, Mesh mesh)
    """
    return _dolfin.FEM_assemble(*args)

def FEM_applyBC(*args):
    """
    applyBC(GenericMatrix A, GenericVector b, Mesh mesh, FiniteElement element, 
        BoundaryCondition bc)
    applyBC(GenericMatrix A, Mesh mesh, FiniteElement element, 
        BoundaryCondition bc)
    FEM_applyBC(GenericVector b, Mesh mesh, FiniteElement element, 
        BoundaryCondition bc)
    """
    return _dolfin.FEM_applyBC(*args)

def FEM_assembleResidualBC(*args):
    """
    assembleResidualBC(GenericMatrix A, GenericVector b, GenericVector x, 
        Mesh mesh, FiniteElement element, BoundaryCondition bc)
    FEM_assembleResidualBC(GenericVector b, GenericVector x, Mesh mesh, FiniteElement element, 
        BoundaryCondition bc)
    """
    return _dolfin.FEM_assembleResidualBC(*args)

def FEM_size(*args):
    """FEM_size(Mesh mesh, FiniteElement element) -> uint"""
    return _dolfin.FEM_size(*args)

def FEM_disp(*args):
    """FEM_disp(Mesh mesh, FiniteElement element)"""
    return _dolfin.FEM_disp(*args)

class FiniteElement(_object):
    """Proxy of C++ FiniteElement class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, FiniteElement, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, FiniteElement, name)
    def __init__(self): raise RuntimeError, "No constructor defined"
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::FiniteElement instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __del__(self, destroy=_dolfin.delete_FiniteElement):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def spacedim(*args):
        """spacedim(self) -> unsigned int"""
        return _dolfin.FiniteElement_spacedim(*args)

    def shapedim(*args):
        """shapedim(self) -> unsigned int"""
        return _dolfin.FiniteElement_shapedim(*args)

    def tensordim(*args):
        """tensordim(self, unsigned int i) -> unsigned int"""
        return _dolfin.FiniteElement_tensordim(*args)

    def elementdim(*args):
        """elementdim(self) -> unsigned int"""
        return _dolfin.FiniteElement_elementdim(*args)

    def rank(*args):
        """rank(self) -> unsigned int"""
        return _dolfin.FiniteElement_rank(*args)

    def nodemap(*args):
        """nodemap(self, int nodes, Cell cell, Mesh mesh)"""
        return _dolfin.FiniteElement_nodemap(*args)

    def pointmap(*args):
        """pointmap(self, Point points, unsigned int components, AffineMap map)"""
        return _dolfin.FiniteElement_pointmap(*args)

    def vertexeval(*args):
        """vertexeval(self, uint vertex_nodes, unsigned int vertex, Mesh mesh)"""
        return _dolfin.FiniteElement_vertexeval(*args)

    def spec(*args):
        """spec(self) -> FiniteElementSpec"""
        return _dolfin.FiniteElement_spec(*args)

    def makeElement(*args):
        """
        makeElement(FiniteElementSpec spec) -> FiniteElement
        makeElement(string type, string shape, uint degree, uint vectordim=0) -> FiniteElement
        makeElement(string type, string shape, uint degree) -> FiniteElement
        """
        return _dolfin.FiniteElement_makeElement(*args)

    if _newclass:makeElement = staticmethod(makeElement)
    __swig_getmethods__["makeElement"] = lambda x: makeElement
    def disp(*args):
        """disp(self)"""
        return _dolfin.FiniteElement_disp(*args)


class FiniteElementPtr(FiniteElement):
    def __init__(self, this):
        _swig_setattr(self, FiniteElement, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, FiniteElement, 'thisown', 0)
        self.__class__ = FiniteElement
_dolfin.FiniteElement_swigregister(FiniteElementPtr)

def FiniteElement_makeElement(*args):
    """
    makeElement(FiniteElementSpec spec) -> FiniteElement
    makeElement(string type, string shape, uint degree, uint vectordim=0) -> FiniteElement
    FiniteElement_makeElement(string type, string shape, uint degree) -> FiniteElement
    """
    return _dolfin.FiniteElement_makeElement(*args)

class AffineMap(_object):
    """Proxy of C++ AffineMap class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, AffineMap, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, AffineMap, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::AffineMap instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> AffineMap"""
        _swig_setattr(self, AffineMap, 'this', _dolfin.new_AffineMap(*args))
        _swig_setattr(self, AffineMap, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_AffineMap):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def update(*args):
        """
        update(self, Cell cell)
        update(self, Cell cell, uint facet)
        """
        return _dolfin.AffineMap_update(*args)

    def __call__(*args):
        """
        __call__(self, real X, real Y) -> Point
        __call__(self, real X, real Y, real Z) -> Point
        """
        return _dolfin.AffineMap___call__(*args)

    def cell(*args):
        """cell(self) -> Cell"""
        return _dolfin.AffineMap_cell(*args)

    __swig_setmethods__["det"] = _dolfin.AffineMap_det_set
    __swig_getmethods__["det"] = _dolfin.AffineMap_det_get
    if _newclass:det = property(_dolfin.AffineMap_det_get, _dolfin.AffineMap_det_set)
    __swig_setmethods__["scaling"] = _dolfin.AffineMap_scaling_set
    __swig_getmethods__["scaling"] = _dolfin.AffineMap_scaling_get
    if _newclass:scaling = property(_dolfin.AffineMap_scaling_get, _dolfin.AffineMap_scaling_set)
    __swig_setmethods__["f00"] = _dolfin.AffineMap_f00_set
    __swig_getmethods__["f00"] = _dolfin.AffineMap_f00_get
    if _newclass:f00 = property(_dolfin.AffineMap_f00_get, _dolfin.AffineMap_f00_set)
    __swig_setmethods__["f01"] = _dolfin.AffineMap_f01_set
    __swig_getmethods__["f01"] = _dolfin.AffineMap_f01_get
    if _newclass:f01 = property(_dolfin.AffineMap_f01_get, _dolfin.AffineMap_f01_set)
    __swig_setmethods__["f02"] = _dolfin.AffineMap_f02_set
    __swig_getmethods__["f02"] = _dolfin.AffineMap_f02_get
    if _newclass:f02 = property(_dolfin.AffineMap_f02_get, _dolfin.AffineMap_f02_set)
    __swig_setmethods__["f10"] = _dolfin.AffineMap_f10_set
    __swig_getmethods__["f10"] = _dolfin.AffineMap_f10_get
    if _newclass:f10 = property(_dolfin.AffineMap_f10_get, _dolfin.AffineMap_f10_set)
    __swig_setmethods__["f11"] = _dolfin.AffineMap_f11_set
    __swig_getmethods__["f11"] = _dolfin.AffineMap_f11_get
    if _newclass:f11 = property(_dolfin.AffineMap_f11_get, _dolfin.AffineMap_f11_set)
    __swig_setmethods__["f12"] = _dolfin.AffineMap_f12_set
    __swig_getmethods__["f12"] = _dolfin.AffineMap_f12_get
    if _newclass:f12 = property(_dolfin.AffineMap_f12_get, _dolfin.AffineMap_f12_set)
    __swig_setmethods__["f20"] = _dolfin.AffineMap_f20_set
    __swig_getmethods__["f20"] = _dolfin.AffineMap_f20_get
    if _newclass:f20 = property(_dolfin.AffineMap_f20_get, _dolfin.AffineMap_f20_set)
    __swig_setmethods__["f21"] = _dolfin.AffineMap_f21_set
    __swig_getmethods__["f21"] = _dolfin.AffineMap_f21_get
    if _newclass:f21 = property(_dolfin.AffineMap_f21_get, _dolfin.AffineMap_f21_set)
    __swig_setmethods__["f22"] = _dolfin.AffineMap_f22_set
    __swig_getmethods__["f22"] = _dolfin.AffineMap_f22_get
    if _newclass:f22 = property(_dolfin.AffineMap_f22_get, _dolfin.AffineMap_f22_set)
    __swig_setmethods__["g00"] = _dolfin.AffineMap_g00_set
    __swig_getmethods__["g00"] = _dolfin.AffineMap_g00_get
    if _newclass:g00 = property(_dolfin.AffineMap_g00_get, _dolfin.AffineMap_g00_set)
    __swig_setmethods__["g01"] = _dolfin.AffineMap_g01_set
    __swig_getmethods__["g01"] = _dolfin.AffineMap_g01_get
    if _newclass:g01 = property(_dolfin.AffineMap_g01_get, _dolfin.AffineMap_g01_set)
    __swig_setmethods__["g02"] = _dolfin.AffineMap_g02_set
    __swig_getmethods__["g02"] = _dolfin.AffineMap_g02_get
    if _newclass:g02 = property(_dolfin.AffineMap_g02_get, _dolfin.AffineMap_g02_set)
    __swig_setmethods__["g10"] = _dolfin.AffineMap_g10_set
    __swig_getmethods__["g10"] = _dolfin.AffineMap_g10_get
    if _newclass:g10 = property(_dolfin.AffineMap_g10_get, _dolfin.AffineMap_g10_set)
    __swig_setmethods__["g11"] = _dolfin.AffineMap_g11_set
    __swig_getmethods__["g11"] = _dolfin.AffineMap_g11_get
    if _newclass:g11 = property(_dolfin.AffineMap_g11_get, _dolfin.AffineMap_g11_set)
    __swig_setmethods__["g12"] = _dolfin.AffineMap_g12_set
    __swig_getmethods__["g12"] = _dolfin.AffineMap_g12_get
    if _newclass:g12 = property(_dolfin.AffineMap_g12_get, _dolfin.AffineMap_g12_set)
    __swig_setmethods__["g20"] = _dolfin.AffineMap_g20_set
    __swig_getmethods__["g20"] = _dolfin.AffineMap_g20_get
    if _newclass:g20 = property(_dolfin.AffineMap_g20_get, _dolfin.AffineMap_g20_set)
    __swig_setmethods__["g21"] = _dolfin.AffineMap_g21_set
    __swig_getmethods__["g21"] = _dolfin.AffineMap_g21_get
    if _newclass:g21 = property(_dolfin.AffineMap_g21_get, _dolfin.AffineMap_g21_set)
    __swig_setmethods__["g22"] = _dolfin.AffineMap_g22_set
    __swig_getmethods__["g22"] = _dolfin.AffineMap_g22_get
    if _newclass:g22 = property(_dolfin.AffineMap_g22_get, _dolfin.AffineMap_g22_set)

class AffineMapPtr(AffineMap):
    def __init__(self, this):
        _swig_setattr(self, AffineMap, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, AffineMap, 'thisown', 0)
        self.__class__ = AffineMap
_dolfin.AffineMap_swigregister(AffineMapPtr)

class BoundaryValue(_object):
    """Proxy of C++ BoundaryValue class"""
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundaryValue, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, BoundaryValue, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::BoundaryValue instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> BoundaryValue"""
        _swig_setattr(self, BoundaryValue, 'this', _dolfin.new_BoundaryValue(*args))
        _swig_setattr(self, BoundaryValue, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_BoundaryValue):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def set(*args):
        """set(self, real value)"""
        return _dolfin.BoundaryValue_set(*args)

    def reset(*args):
        """reset(self)"""
        return _dolfin.BoundaryValue_reset(*args)


class BoundaryValuePtr(BoundaryValue):
    def __init__(self, this):
        _swig_setattr(self, BoundaryValue, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BoundaryValue, 'thisown', 0)
        self.__class__ = BoundaryValue
_dolfin.BoundaryValue_swigregister(BoundaryValuePtr)

class BoundaryCondition(TimeDependent):
    """Proxy of C++ BoundaryCondition class"""
    __swig_setmethods__ = {}
    for _s in [TimeDependent]: __swig_setmethods__.update(_s.__swig_setmethods__)
    __setattr__ = lambda self, name, value: _swig_setattr(self, BoundaryCondition, name, value)
    __swig_getmethods__ = {}
    for _s in [TimeDependent]: __swig_getmethods__.update(_s.__swig_getmethods__)
    __getattr__ = lambda self, name: _swig_getattr(self, BoundaryCondition, name)
    def __repr__(self):
        return "<%s.%s; proxy of C++ dolfin::BoundaryCondition instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        """__init__(self) -> BoundaryCondition"""
        if self.__class__ == BoundaryCondition:
            args = (None,) + args
        else:
            args = (self,) + args
        _swig_setattr(self, BoundaryCondition, 'this', _dolfin.new_BoundaryCondition(*args))
        _swig_setattr(self, BoundaryCondition, 'thisown', 1)
    def __del__(self, destroy=_dolfin.delete_BoundaryCondition):
        """__del__(self)"""
        try:
            if self.thisown: destroy(self)
        except: pass

    def eval(*args):
        """eval(self, BoundaryValue value, Point p, uint i)"""
        return _dolfin.BoundaryCondition_eval(*args)

    def __disown__(self):
        self.thisown = 0
        _dolfin.disown_BoundaryCondition(self)
        return weakref_proxy(self)

class BoundaryConditionPtr(BoundaryCondition):
    def __init__(self, this):
        _swig_setattr(self, BoundaryCondition, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, BoundaryCondition, 'thisown', 0)
        self.__class__ = BoundaryCondition
_dolfin.BoundaryCondition_swigregister(BoundaryConditionPtr)


def get(*args):
    """get(string name) -> Parameter"""
    return _dolfin.get(*args)

def load_parameters(*args):
    """load_parameters(string filename)"""
    return _dolfin.load_parameters(*args)


def set(*args):
    """
    set(string name, real val)
    set(string name, int val)
    set(string name, bool val)
    set(string name, string val)
    """
    return _dolfin.set(*args)

