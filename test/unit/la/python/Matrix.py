"""Unit tests for the Matrix interface"""

# Copyright (C) 2011-2014 Garth N. Wells
#
# This file is part of DOLFIN.
#
# DOLFIN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DOLFIN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with DOLFIN. If not, see <http://www.gnu.org/licenses/>.
#
# Modified by Anders Logg 2011
# Modified by Mikael Mortensen 2011
# Modified by Jan Blechta 2013

from __future__ import print_function
import unittest
from dolfin import *
from six.moves import xrange as range

class AbstractBaseTest(object):
    count = 0
    def setUp(self):
        if self.backend != "default":
            parameters.linear_algebra_backend = self.backend
        type(self).count += 1
        if type(self).count == 1:
            # Only print this message once per class instance
            print("\nRunning:",type(self).__name__)


    def assemble_matrices(self, use_backend=False, keep_diagonal=False):
        " Assemble a pair of matrices, one (square) MxM and one MxN"
        mesh = UnitSquareMesh(21, 23)

        V = FunctionSpace(mesh, "Lagrange", 2)
        W = FunctionSpace(mesh, "Lagrange", 1)

        v = TestFunction(V)
        u = TrialFunction(V)
        s = TrialFunction(W)

        # Forms
        a = dot(grad(u), grad(v))*ds
        b = v*s*dx

        return self.assemble(a, use_backend=use_backend, keep_diagonal=keep_diagonal), \
               self.assemble(b, use_backend=use_backend, keep_diagonal=keep_diagonal)

    def assemble(self, form, use_backend=False, keep_diagonal=False):
        if use_backend:
            backend = globals()[self.backend + self.sub_backend + 'Factory'].instance()
            return assemble(form, backend=backend, keep_diagonal=keep_diagonal)
        else:
            return assemble(form, keep_diagonal=keep_diagonal)

    def test_basic_la_operations(self, use_backend=False):
        from numpy import ndarray, array, ones, sum

        # Tests bailout for this choice
        if self.backend == "uBLAS" and not use_backend:
            return

        A, B = self.assemble_matrices(use_backend)
        unit_norm = A.norm('frobenius')

        def wrong_getitem(type):
            if type == 0:
                A["0,1"]
            elif type == 1:
                A[0]
            elif type == 2:
                A[0, 0, 0]

        # Test wrong getitem
        self.assertRaises(TypeError, wrong_getitem, 0)
        self.assertRaises(TypeError, wrong_getitem, 1)
        self.assertRaises(TypeError, wrong_getitem, 2)

        # Test __imul__ operator
        A *= 2
        self.assertAlmostEqual(A.norm('frobenius'), 2*unit_norm)

        # Test __idiv__ operator
        A /= 2
        self.assertAlmostEqual(A.norm('frobenius'), unit_norm)

        # Test __mul__ operator
        C = 4*A
        self.assertAlmostEqual(C.norm('frobenius'), 4*unit_norm)

        # Test __iadd__ operator
        A += C
        self.assertAlmostEqual(A.norm('frobenius'), 5*unit_norm)

        # Test __isub__ operator
        A -= C
        self.assertAlmostEqual(A.norm('frobenius'), unit_norm)

        # Test __mul__ and __add__ operator
        D = (C+A)*0.2
        self.assertAlmostEqual(D.norm('frobenius'), unit_norm)

        # Test __div__ and __sub__ operator
        F = (C-A)/3
        self.assertAlmostEqual(F.norm('frobenius'), unit_norm)

        # Test axpy
        A.axpy(10,C,True)
        self.assertAlmostEqual(A.norm('frobenius'), 41*unit_norm)

        # Test expected size of rectangular array
        self.assertEqual(A.size(0), B.size(0))
        self.assertEqual(B.size(1), 528)

        # Test setitem/getitem
        #A[5,5] = 15
        #self.assertEqual(A[5,5],15)

    @unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
    def test_numpy_array(self, use_backend=False):
        from numpy import ndarray, array, ones, sum

        # Tests bailout for this choice
        if self.backend == "uBLAS" and not use_backend:
            return

        # Assemble matrices
        A, B = self.assemble_matrices(use_backend)

        # Test to NumPy array
        A2 = A.array()
        self.assertTrue(isinstance(A2,ndarray))
        self.assertEqual(A2.shape, (2021, 2021))
        self.assertAlmostEqual(sqrt(sum(A2**2)), A.norm('frobenius'))

        if self.backend == 'uBLAS' and self.sub_backend == 'Sparse':
            try:
                import scipy.sparse
                import numpy.linalg
                A3 = A.sparray()
                self.assertTrue(isinstance(A3, scipy.sparse.csr_matrix))
                self.assertAlmostEqual(numpy.linalg.norm(A3.todense() - A2), 0.0)
            except ImportError:
                pass

    def test_basic_la_operations_with_backend(self):
        self.test_basic_la_operations(True)

    #def create_sparsity_pattern(self):
    #    "Create a sparsity pattern"
    #    mesh = UnitSquareMesh(34, 33)
    #
    #    V = FunctionSpace(mesh, "Lagrange", 2)
    #    W = FunctionSpace(mesh, "Lagrange", 1)
    #
    #    v = TestFunction(V)
    #    u = TrialFunction(V)
    #    s = TrialFunction(W)
    #
    #    # Forms
    #    a = dot(grad(u), grad(v))*dx
    #    b = v*s*dx

    def test_create_empty_matrix(self):
        A = Matrix()
        self.assertEqual(A.size(0), 0)
        self.assertEqual(A.size(1), 0)
        info(A)
        info(A, True)

    def test_copy_empty_matrix(self):
        A = Matrix()
        B = Matrix(A)
        self.assertEqual(B.size(0), 0)
        self.assertEqual(B.size(1), 0)

    def test_copy_matrix(self):
        A0, B0 = self.assemble_matrices()

        A1 = Matrix(A0)
        self.assertEqual(A0.size(0), A1.size(0))
        self.assertEqual(A0.size(1), A1.size(1))
        self.assertEqual(A0.norm("frobenius"), A1.norm("frobenius"))

        B1 = Matrix(B0)
        self.assertEqual(B0.size(0), B1.size(0))
        self.assertEqual(B0.size(1), B1.size(1))
        self.assertAlmostEqual(B0.norm("frobenius"), B1.norm("frobenius"))

    def test_compress_matrix(self):

        A, B = self.assemble_matrices()
        A_norm = A.norm('frobenius')
        C = Matrix()
        A.compressed(C)
        C_norm = C.norm('frobenius')
        self.assertAlmostEqual(A_norm, C_norm)

    def test_ident_zeros(self, use_backend=False):

        # Check that PETScMatrix::ident_zeros() rethrows PETSc error
        if self.backend[0:5] == "PETSc":
            A, B = self.assemble_matrices(use_backend=use_backend)
            self.assertRaises(RuntimeError, A.ident_zeros)

        # Assemble matrix A with diagonal entries
        A, B = self.assemble_matrices(use_backend=use_backend, keep_diagonal=True)

        # Find zero rows
        zero_rows = []
        for i in range(A.local_range(0)[0], A.local_range(0)[1]):
            row = A.getrow(i)[1]
            if sum(abs(row)) < DOLFIN_EPS:
                zero_rows.append(i)

        # Set zero rows to (0,...,0, 1, 0,...,0)
        A.ident_zeros()

        # Check it
        for i in zero_rows:
            cols = A.getrow(i)[0]
            row  = A.getrow(i)[1]
            for j in range(cols.size + 1):
                if i == cols[j]:
                    self.assertAlmostEqual(row[j], 1.0)
                    break
            self.assertTrue(j < cols.size)
            self.assertAlmostEqual(sum(abs(row)), 1.0)

    def test_ident_zeros_with_backend(self):
        self.test_ident_zeros(use_backend=True)

    def test_setting_diagonal(self, use_backend=False):

        mesh = UnitSquareMesh(21, 23)

        V = FunctionSpace(mesh, "Lagrange", 2)
        W = FunctionSpace(mesh, "Lagrange", 1)

        v = TestFunction(V)
        u = TrialFunction(V)

        B = self.assemble(u*v*dx(), use_backend=use_backend, keep_diagonal=True)

        b = assemble(action(u*v*dx(), Constant(1)))
        A = B.copy()
        A.zero()
        A.set_diagonal(b)

        resultsA = Vector()
        resultsB = Vector()
        A.init_vector(resultsA, 1)
        B.init_vector(resultsB, 1)

        ones = b.copy()
        ones[:] = 1.0

        A.mult(ones, resultsA)
        B.mult(ones, resultsB)
        self.assertAlmostEqual(resultsA.norm("l2"), resultsB.norm("l2"))

    def test_setting_diagonal_with_backend(self):
        self.test_setting_diagonal(True)

    #def test_create_from_sparsity_pattern(self):

    #def test_size(self):

    #def test_local_range(self):

    #def test_zero(self):

    #def test_apply(self):

    #def test_str(self):

    #def test_resize(self):


# A DataTester class that test the acces of the raw data through pointers
# This is only available for uBLAS and MTL4 backends
class DataTester:
    def test_matrix_data(self, use_backend=False):
        """ Test for ordinary Matrix"""
        # Tests bailout for this choice
        if self.backend == "uBLAS" and \
               (not use_backend or self.sub_backend =="Dense"):
            return

        A, B = self.assemble_matrices(use_backend)
        array = A.array()
        rows, cols, values = A.data()
        i = 0
        for row in range(A.size(0)):
            for col in range(rows[row], rows[row+1]):
                self.assertEqual(array[row, cols[col]],values[i])
                i += 1

        # Test none writeable of a shallow copy of the data
        rows, cols, values = A.data(False)
        def write_data(data):
            data[0] = 1
        self.assertRaises(Exception, write_data, rows)
        self.assertRaises(Exception, write_data, cols)
        self.assertRaises(Exception, write_data, values)

        # Test for as_backend_typeed Matrix
        A = as_backend_type(A)
        rows, cols, values = A.data()
        for row in range(A.size(0)):
            for k in range(rows[row], rows[row+1]):
                self.assertEqual(array[row,cols[k]], values[k])

    def test_matrix_data_use_backend(self):
        self.test_matrix_data(True)

class DataNotWorkingTester:
    def test_matrix_data(self):
        A, B = self.assemble_matrices()
        self.assertRaises(RuntimeError, A.data)

        A = as_backend_type(A)
        self.assertRaises(RuntimeError, A.data)

@unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
class uBLASSparseTester(DataTester, AbstractBaseTest, unittest.TestCase):
    backend     = "uBLAS"
    sub_backend = "Sparse"

@unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
class uBLASDenseTester(DataTester, AbstractBaseTest, unittest.TestCase):
    backend     = "uBLAS"
    sub_backend = "Dense"

if has_linear_algebra_backend("PETScCusp"):
    @unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
    class PETScCuspTester(DataNotWorkingTester, AbstractBaseTest, unittest.TestCase):
        backend    = "PETScCusp"
        sub_backend = ""

if has_linear_algebra_backend("PETSc"):
    class PETScTester(DataNotWorkingTester, AbstractBaseTest, unittest.TestCase):
        backend    = "PETSc"
        sub_backend = ""

#class STLTester(DataNotWorkingTester, AbstractBaseTest, unittest.TestCase):
#    backend    = "STL"

if __name__ == "__main__":

    # Turn off DOLFIN output
    set_log_active(False)

    print("")
    print("Testing DOLFIN Matrix classes")
    print("------------------------------------------------")
    unittest.main()
