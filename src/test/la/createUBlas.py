"""Unit tests for creating uBlas matrices and vectors"""

__author__ = "Anders Logg (logg@simula.no)"
__date__ = "2006-08-09 -- 2006-08-09"
__copyright__ = "Copyright (C) 2006 Anders Logg"
__license__  = "GNU GPL Version 2"

import unittest
from dolfin import *

class CreateVectors(unittest.TestCase):

    def testUBlasVector(self):
        """Create uBlas vector"""
        x = uBlasVector(10)
        self.assertEqual(x.size(), 10)

class CreateMatrices(unittest.TestCase):

    def testUBlasDenseMatrix(self):
        """Create uBlas dense matrix"""
        A = uBlasSparseMatrix(10,20)
        self.assertEqual(A.size(0), 10)
        self.assertEqual(A.size(1), 20)

    def testUBlasSparseMatrix(self):
        """Create uBlas sparse matrix"""
        A = uBlasSparseMatrix(10,20)
        self.assertEqual(A.size(0), 10)
        self.assertEqual(A.size(1), 20)

class InitialiseMatrices(unittest.TestCase):

    def testUBlasDenseMatrix(self):
        """Create and initialise uBlas dense matrix"""
        A = uBlasDenseMatrix()
        A.init(10,20)
        self.assertEqual(A.size(0), 10)
        self.assertEqual(A.size(1), 20)

    def testUBlasSparseMatrix(self):
        """Create and initialise uBlas sparse matrix"""
        A = uBlasSparseMatrix()
        A.init(10,20)
        self.assertEqual(A.size(0), 10)
        self.assertEqual(A.size(1), 20)

    def testUBlasDenseMatrixNonzeros(self):
        """Create and initialise uBlas dense matrix with prescribed maximum non-zeros"""
        A = uBlasDenseMatrix()
        A.init(10,20,5)
        self.assertEqual(A.size(0), 10)
        self.assertEqual(A.size(1), 20)

    def testUBlasSparseMatrixNonZeros(self):
        """Create and initialise uBlas sparse matrix with prescribed maximum non-zeros"""
        A = uBlasSparseMatrix()
        A.init(10,20,5)
        self.assertEqual(A.size(0), 10)
        self.assertEqual(A.size(1), 20)

def suite():
    """Returns a suite containing all the test cases in this module."""
    # Build a TestSuite containing all the possible test case instances
    # that can be made from the ListTestCase class using its 'test*'
    # functions.
    suite1 = unittest.makeSuite(CreateVectors)
    suite2 = unittest.makeSuite(CreateMatrices)
    suite3 = unittest.makeSuite(InitialiseMatrices)
    return unittest.TestSuite((suite1, suite2, suite3))

if __name__ == "__main__":
    unittest.main()
