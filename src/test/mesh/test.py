"""Unit test for the mesh library"""

__author__ = "Anders Logg (logg@simula.no)"
__date__ = "2006-08-08 -- 2006-09-20"
__copyright__ = "Copyright (C) 2006 Anders Logg"
__license__  = "GNU GPL Version 2"

import unittest
from dolfin import *

class SimpleShapes(unittest.TestCase):

    def testUnitSquare(self):
        """Create mesh of unit square"""
        mesh = UnitSquare(5, 7)
        self.assertEqual(mesh.numVertices(), 48)
        self.assertEqual(mesh.numCells(), 70)

    def testUnitCube(self):
        """Create mesh of unit cube"""
        mesh = UnitCube(5, 7, 9)
        self.assertEqual(mesh.numVertices(), 480)
        self.assertEqual(mesh.numCells(), 1890)

class MeshRefinement(unittest.TestCase):

    def testRefineUnitSquare(self):
        """Refine mesh of unit square"""
        mesh = UnitSquare(5, 7)
        mesh.refine()
        self.assertEqual(mesh.numVertices(), 165)
        self.assertEqual(mesh.numCells(), 280)

    def testRefineUnitCube(self):
        """Refine mesh of unit cube"""
        mesh = UnitCube(5, 7, 9)
        mesh.refine()
        self.assertEqual(mesh.numVertices(), 3135)
        self.assertEqual(mesh.numCells(), 15120)

class MeshIterators(unittest.TestCase):

    def testVertexIterators(self):
        """Iterate over vertices"""
        mesh = UnitCube(5, 5, 5)
        n = 0
        for v in vertices(mesh):
            n += 1
        self.assertEqual(n, mesh.numVertices())

    def testEdgeIterators(self):
        """Iterate over edges"""
        mesh = UnitCube(5, 5, 5)
        n = 0
        for e in edges(mesh):
            n += 1
        self.assertEqual(n, mesh.numEdges())

    def testFaceIterators(self):
        """Iterate over faces"""
        mesh = UnitCube(5, 5, 5)
        n = 0
        for f in faces(mesh):
            n += 1
        self.assertEqual(n, mesh.numFaces())

    def testFacetIterators(self):
        """Iterate over facets"""
        mesh = UnitCube(5, 5, 5)
        n = 0
        for f in facets(mesh):
            n += 1
        self.assertEqual(n, mesh.numFacets())

    def testCellIterators(self):
        """Iterate over cells"""
        mesh = UnitCube(1, 1, 1)
        n = 0
        for c in cells(mesh):
            n += 1
        print "should be: " + str(mesh.numCells())
        self.assertEqual(n, mesh.numCells())
        
    def testMixedIterators(self):
        """Iterate over vertices of cells"""
        mesh = UnitCube(5, 5, 5)
        n = 0
        for c in cells(mesh):
            for v in vertices(c):
                n += 1
        self.assertEqual(n, 4*mesh.numCells())


class PyCCInterface(unittest.TestCase):

    def testGetGeometricalDimension(self):
        """Get geometrical dimension of mesh"""
        mesh = UnitSquare(5, 5)
        self.assertEqual(mesh.geometry().dim(), 2)

    def testGetVertices(self):
        """Get vertices of mesh"""
        mesh = UnitSquare(5, 5)
        self.assertEqual(len(mesh.vertices()), 36)

    def testGetCells(self):
        """Get cells of mesh"""
        mesh = UnitSquare(5, 5)
        self.assertEqual(len(mesh.cells()), 50)

if __name__ == "__main__":
    unittest.main()
