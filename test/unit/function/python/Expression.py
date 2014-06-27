"""Unit tests for the function library"""

# Copyright (C) 2007 Anders Logg
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
# Modified by Benjamin Kehlet 2012
#
# First added:  2007-05-24
# Last changed: 2014-05-30

import unittest
from dolfin import *
from math   import sin, cos, exp, tan
from numpy  import array, zeros, float_

mesh = UnitCubeMesh(8, 8, 8)
V = FunctionSpace(mesh, 'CG', 1)
W = VectorFunctionSpace(mesh, 'CG', 1)

class Eval(unittest.TestCase):

     def test_arbitraryEval(self):
          class F0(Expression):
               def eval(self, values, x):
                    values[0] = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])

          f0 = F0(name="f0", label="My expression")
          f1 = Expression("a*sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])", \
                          degree=2, a=1., name="f1")
          x = array([0.31, 0.32, 0.33])
          u00 = zeros(1); u01 = zeros(1)
          u10 = zeros(1); u20 = zeros(1)

          # Check usergeneration of name and label
          self.assertEqual(f0.name(), "f0")
          self.assertEqual(str(f0), "f0")
          self.assertEqual(f0.label(), "My expression")
          self.assertEqual(f1.name(), "f1")
          self.assertEqual(str(f1), "f1")
          self.assertEqual(f1.label(), "User defined expression")

          # Check outgeneration of name
          count = int(F0().name()[2:])
          self.assertEqual(F0().count(), count+1)

          # Test original and vs short evaluation
          f0.eval(u00, x)
          f0(x, values = u01)
          self.assertAlmostEqual(u00[0], u01[0])

          # Evaluation with and without return value
          f1(x, values = u10);
          u11 = f1(x);
          self.assertAlmostEqual(u10[0], u11)

          # Test *args for coordinate argument
          f1(0.31, 0.32, 0.33, values = u20)
          u21 = f0(0.31, 0.32, 0.33)
          self.assertAlmostEqual(u20[0], u21)

          # Test Point evaluation
          p0 = Point(0.31, 0.32, 0.33)
          u21 = f1(p0)
          self.assertAlmostEqual(u20[0], u21)

          same_result = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])
          self.assertAlmostEqual(u00[0], same_result)
          self.assertAlmostEqual(u11, same_result)
          self.assertAlmostEqual(u21, same_result)

          # Projection requires CGAL
          if not has_cgal():
               return

          x = (mesh.coordinates()[0]+mesh.coordinates()[1])/2
          f2 = Expression("1.0 + 3.0*x[0] + 4.0*x[1] + 0.5*x[2]", degree=2)
          V2 = FunctionSpace(mesh, 'CG', 2)
          g0 = interpolate(f2, V=V2)
          g1 = project(f2, V=V2)

          u3 = f2(x)
          u4 = g0(x)
          u5 = g1(x)
          self.assertAlmostEqual(u3, u4)
          self.assertAlmostEqual(u3, u5, 4)

     def test_ufl_eval(self):
          class F0(Expression):
               def eval(self, values, x):
                    values[0] = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])

          class V0(Expression):
               def eval(self, values, x):
                    values[0] = x[0]**2
                    values[1] = x[1]**2
                    values[2] = x[2]**2
               def value_shape(self):
                    return (3,)

          f0 = F0()
          v0 = V0()

          x = (2.0, 1.0, 3.0)

          # Test ufl evaluation through mapping (overriding the Expression with N here):
          def N(x):
               return x[0]**2 + x[1] + 3*x[2]

          self.assertEqual(f0(x, { f0: N }), 14)

          a = f0**2
          b = a(x, { f0: N })
          self.assertEqual(b, 196)

          # Test ufl evaluation together with Expression evaluation by dolfin
          # scalar
          self.assertEqual(f0(x), f0(*x))
          self.assertEqual((f0**2)(x), f0(*x)**2)
          # vector
          self.assertTrue(all(a == b for a,b in zip(v0(x), v0(*x))))
          self.assertEqual(dot(v0,v0)(x), sum(v**2 for v in v0(*x)))
          self.assertEqual(dot(v0,v0)(x), 98)

     def test_overload_and_call_back(self):
          class F0(Expression):
               def eval(self, values, x):
                    values[0] = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])

          class F1(Expression):
               def __init__(self, mesh, *arg, **kwargs):
                    self.mesh = mesh
               def eval_cell(self, values, x, cell):
                    c = Cell(self.mesh, cell.index)
                    values[0] = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])

          e0 = F0(degree=2)
          e1 = F1(mesh, degree=2)
          e2 = Expression("sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])", degree=2)

          s0 = norm(interpolate(e0, V))
          s1 = norm(interpolate(e1, V))
          s2 = norm(interpolate(e2, V))

          ref = 0.36557637568519191
          self.assertAlmostEqual(s0, ref)
          self.assertAlmostEqual(s1, ref)
          self.assertAlmostEqual(s2, ref)

     def test_wrong_eval(self):
          # Test wrong evaluation
          class F0(Expression):
               def eval(self, values, x):
                    values[0] = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])

          f0 = F0()
          f1 = Expression("sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])", degree=2)

          for f in [f0, f1]:
               self.assertRaises(TypeError, f, "s")
               self.assertRaises(TypeError, f, [])
               self.assertRaises(TypeError, f, 0.5, 0.5, 0.5, values = zeros(3,'i'))
               self.assertRaises(TypeError, f, [0.3, 0.2, []])
               self.assertRaises(TypeError, f, 0.3, 0.2, {})
               self.assertRaises(TypeError, f, zeros(3), values = zeros(4))
               self.assertRaises(TypeError, f, zeros(4), values = zeros(3))

     def test_no_write_to_const_array(self):
          class F1(Expression):
               def eval(self, values, x):
                    x[0] = 1.0
                    values[0] = sin(3.0*x[0])*sin(3.0*x[1])*sin(3.0*x[2])

          mesh = UnitCubeMesh(3,3,3)
          f1 = F1()
          self.assertRaises(StandardError, lambda : assemble(f1*dx(mesh)))

class MeshEvaluation(unittest.TestCase):

     def test_compute_vertex_values(self):
          from numpy import zeros, all, array

          e0 = Expression("1")
          e1 = Expression(("1", "2", "3"))

          e0_values = e0.compute_vertex_values(mesh)
          e1_values = e1.compute_vertex_values(mesh)

          self.assertTrue(all(e0_values==1))
          self.assertTrue(all(e1_values[:mesh.num_vertices()]==1))
          self.assertTrue(all(e1_values[mesh.num_vertices():mesh.num_vertices()*2]==2))
          self.assertTrue(all(e1_values[mesh.num_vertices()*2:mesh.num_vertices()*3]==3))

class Instantiation(unittest.TestCase):

     def test_wrong_sub_classing(self):

          def noAttributes():
               class NoAttributes(Expression):pass

          def wrongEvalAttribute():
               class WrongEvalAttribute(Expression):
                    def eval(values, x):
                         pass

          def wrongEvalDataAttribute():
               class WrongEvalDataAttribute(Expression):
                    def eval_cell(values, data):
                         pass

          def noEvalAttribute():
               class NoEvalAttribute(Expression):
                    def evaluate(self, values, data):
                         pass

          def wrongArgs():
               class WrongArgs(Expression):
                    def eval(self, values, x):
                         pass
               e = WrongArgs(V)

          def deprecationWarning():
               class Deprecated(Expression):
                    def eval(self, values, x):
                         pass
                    def dim(self):
                         return 2
               e = Deprecated()

          def noDefaultValues():
               Expression("a")

          def wrongDefaultType():
               Expression("a", a="1")

          def wrongParameterNames0():
               Expression("long", str=1.0)

          def wrongParameterNames1():
               Expression("user_parameters", user_parameters=1.0)

          self.assertRaises(TypeError, noAttributes)
          self.assertRaises(TypeError, noEvalAttribute)
          self.assertRaises(TypeError, wrongEvalAttribute)
          self.assertRaises(TypeError, wrongEvalDataAttribute)
          self.assertRaises(TypeError, wrongArgs)
          self.assertRaises(DeprecationWarning, deprecationWarning)
          self.assertRaises(RuntimeError, noDefaultValues)
          self.assertRaises(TypeError, wrongDefaultType)
          self.assertRaises(RuntimeError, wrongParameterNames0)
          self.assertRaises(RuntimeError, wrongParameterNames1)

     def test_element_instantiation(self):
          class F0(Expression):
               def eval(self, values, x):
                    values[0] = 1.0
          class F1(Expression):
               def eval(self, values, x):
                    values[0] = 1.0
                    values[1] = 1.0
               def value_shape(self):
                    return (2,)

          class F2(Expression):
               def eval(self, values, x):
                    values[0] = 1.0
                    values[1] = 1.0
                    values[2] = 1.0
                    values[3] = 1.0
               def value_shape(self):
                    return (2,2)

          e0 = Expression("1")
          self.assertTrue(e0.ufl_element().cell() is None)

          e1 = Expression("1", cell=triangle)
          self.assertFalse(e1.ufl_element().cell() is None)

          e2 = Expression("1", cell=triangle, degree=2)
          self.assertEqual(e2.ufl_element().degree(), 2)

          e3 = Expression(["1", "1"], cell=triangle)
          self.assertTrue(isinstance(e3.ufl_element(), VectorElement))

          e4 = Expression((("1", "1"), ("1", "1")), cell=triangle)
          self.assertTrue(isinstance(e4.ufl_element(), TensorElement))

          f0 = F0()
          self.assertTrue(f0.ufl_element().cell() is None)

          f1 = F0(cell=triangle)
          self.assertFalse(f1.ufl_element().cell() is None)

          f2 = F0(cell=triangle, degree=2)
          self.assertEqual(f2.ufl_element().degree(), 2)

          f3 = F1(cell=triangle)
          self.assertTrue(isinstance(f3.ufl_element(), VectorElement))

          f4 = F2(cell=triangle)
          self.assertTrue(isinstance(f4.ufl_element(), TensorElement))

     def test_exponent_init(self):
          e0 = Expression("1e10")
          self.assertEqual(e0(0,0,0), 1e10)

          e1 = Expression("1e-10")
          self.assertEqual(e1(0,0,0), 1e-10)

          e2 = Expression("1e+10")
          self.assertEqual(e2(0,0,0), 1e+10)

     def test_name_space_usage(self):
          e0 = Expression("std::sin(x[0])*cos(x[1])")
          e1 = Expression("sin(x[0])*std::cos(x[1])")
          self.assertAlmostEqual(assemble(e0*dx(mesh)), \
                                 assemble(e1*dx(mesh)))

     def test_generic_function_attributes(self):
          tc = Constant(2.0)
          te = Expression("value", value=tc)

          self.assertAlmostEqual(tc(0), te(0))
          tc.assign(1.0)
          self.assertAlmostEqual(tc(0), te(0))

          tf = Function(V)
          tf.vector()[:] = 1.0

          e0 = Expression(["2*t", "-t"], t=tc)
          e1 = Expression(["2*t", "-t"], t=1.0)
          e2 = Expression("t", t=te)
          e3 = Expression("t", t=tf)

          self.assertAlmostEqual(assemble(inner(e0,e0)*dx(mesh)), \
                                 assemble(inner(e1,e1)*dx(mesh)))

          self.assertAlmostEqual(assemble(inner(e2,e2)*dx(mesh)), \
                                 assemble(inner(e3,e3)*dx(mesh)))

          tc.assign(3.0)
          e1.t = float(tc)

          self.assertAlmostEqual(assemble(inner(e0,e0)*dx(mesh)), \
                                 assemble(inner(e1,e1)*dx(mesh)))
          tc.assign(5.0)

          self.assertNotEqual(assemble(inner(e2,e2)*dx(mesh)), \
                              assemble(inner(e3,e3)*dx(mesh)))

          self.assertAlmostEqual(assemble(e0[0]*dx(mesh)), \
                                 assemble(2*e2*dx(mesh)))

          e2.t = e3.t

          self.assertEqual(assemble(inner(e2,e2)*dx(mesh)), \
                           assemble(inner(e3,e3)*dx(mesh)))

          # Test wrong kwargs
          self.assertRaises(TypeError, lambda : Expression("t", t=Constant((1,0))))
          self.assertRaises(TypeError, lambda : Expression("t", t=Function(V*V)))

          # Test non-scalar GenericFunction
          f2 = Function(V*V)
          e2.t = f2

          self.assertRaises(RuntimeError, lambda : e2(0, 0))

          # Test self assignment
          e2.t = e2
          self.assertRaises(RuntimeError, lambda : e2(0, 0))

          # Test user_parameters assignment
          self.assertTrue("value" in te.user_parameters)
          te.user_parameters["value"] = Constant(5.0)
          self.assertEqual(te(0.0), 5.0)

          te.user_parameters.update(dict(value=Constant(3.0)))
          self.assertEqual(te(0.0), 3.0)

          te.user_parameters.update([("value", Constant(4.0))])
          self.assertEqual(te(0.0), 4.0)

          # Test wrong assignment
          self.assertRaises(TypeError, lambda : te.user_parameters.__setitem__("value", 1.0))
          self.assertRaises(KeyError, lambda : te.user_parameters.__setitem__("values", 1.0))

     def test_doc_string_eval(self):
          """
          This test tests all features documented in the doc string of
          Expression. If this test breaks and it is fixed the corresponding fixes
          need also be updated in the docstring.
          """

          square = UnitSquareMesh(10,10)
          V = VectorFunctionSpace(square, "CG", 1)

          f0 = Expression('sin(x[0]) + cos(x[1])')
          f1 = Expression(('cos(x[0])', 'sin(x[1])'), element = V.ufl_element())
          self.assertAlmostEqual(f0(0,0), sum(f1(0,0)))

          f2 = Expression((('exp(x[0])','sin(x[1])'),
                           ('sin(x[0])','tan(x[1])')))
          self.assertAlmostEqual(sum(f2(0,0)), 1.0)

          f = Expression('A*sin(x[0]) + B*cos(x[1])', A=2.0, B=Constant(4.0))
          self.assertAlmostEqual(f(pi/4, pi/4), 6./sqrt(2))

          f.A = 5.0
          f.B = Expression("value", value=6.0)
          self.assertAlmostEqual(f(pi/4, pi/4), 11./sqrt(2))

          f.user_parameters["A"] = 1.0
          f.user_parameters["B"] = Constant(5.0)
          self.assertAlmostEqual(f(pi/4, pi/4), 6./sqrt(2))

     def test_doc_string_complex_compiled_expression(self):
          """
          This test tests all features documented in the doc string of
          Expression. If this test breaks and it is fixed the corresponding fixes
          need also be updated in the docstring.
          """

          code = '''
          class MyFunc : public Expression
          {
          public:

            std::shared_ptr<MeshFunction<std::size_t> > cell_data;

            MyFunc() : Expression()
            {
            }

          void eval(Array<double>& values, const Array<double>& x,
                    const ufc::cell& c) const
            {
              assert(cell_data);
              const Cell cell(*cell_data->mesh(), c.index);
              switch ((*cell_data)[cell.index()])
              {
              case 0:
                values[0] = exp(-x[0]);
                break;
              case 1:
                values[0] = exp(-x[2]);
                break;
              default:
                values[0] = 0.0;
              }
            }
          };'''

          cell_data = CellFunction('uint', mesh)
          f = Expression(code)
          f.cell_data = cell_data

          CompiledSubDomain("x[0]<=0.25").mark(cell_data, 0)
          CompiledSubDomain("x[0]>0.25 && x[0]<0.75").mark(cell_data, 1)
          CompiledSubDomain("x[0]>=0.75").mark(cell_data, 2)

          bb = mesh.bounding_box_tree()
          p0 = Point(0.1, 1.0, 0)
          c0 = bb.compute_first_entity_collision(p0)

          # If run in paralell
          if c0 > mesh.num_cells():
              return

          p1 = Point(0.5, 1.0, 0)
          c1 = bb.compute_first_entity_collision(p1)

          # If run in paralell
          if c1 > mesh.num_cells():
              return

          p2 = Point(1.0, 1.0, 1.0)
          c2 = bb.compute_first_entity_collision(p2)

          # If run in paralell
          if c2 > mesh.num_cells():
              return

          # Create cells for evaluation
          c0 = Cell(mesh, c0)
          c1 = Cell(mesh, c1)
          c2 = Cell(mesh, c2)

          coords = array([p0.x(), p0.y(), p0.z()], dtype=float_)
          values = zeros(1, dtype=float_)

          f.eval_cell(values, coords, c0)
          self.assertEqual(values[0], exp(-p0.x()))

          coords = array([p1.x(), p1.y(), p1.z()], dtype=float_)

          f.eval_cell(values, coords, c1)
          self.assertEqual(values[0], exp(-p1.z()))

          f.eval_cell(values, coords, c2)
          self.assertEqual(values[0], 0.0)

     @unittest.skipIf(MPI.size(mpi_comm_world()) > 1, "Skipping unit test(s) not working in parallel")
     def test_doc_string_compiled_expression_with_system_headers(self):
          """
          This test tests all features documented in the doc string of
          Expression. If this test breaks and it is fixed the corresponding fixes
          need also be updated in the docstring.
          """

          # Add header and it should compile
          code_compile = '''
          #include "dolfin/fem/GenericDofMap.h"
          namespace dolfin
          {
            class Delta : public Expression
            {
            public:

              Delta() : Expression() {}

              void eval(Array<double>& values, const Array<double>& data,
                        const ufc::cell& cell) const
              { }

              void update(const std::shared_ptr<const Function> u,
                          double nu, double dt, double C1,
                          double U_infty, double chord)
              {
                const std::shared_ptr<const Mesh> mesh = u->function_space()->mesh();
                const std::shared_ptr<const GenericDofMap> dofmap = u->function_space()->dofmap();
                const uint ncells = mesh->num_cells();
                uint ndofs_per_cell;
                if (ncells > 0)
                {
                  CellIterator cell(*mesh);
                  ndofs_per_cell = dofmap->cell_dimension(cell->index());
                }
                else
                {
                   return;
                }
              }
            };
          }'''

          e = Expression(code_compile)
          self.assertTrue(hasattr(e, "update"))

          # Test not compile
          code_not_compile = '''
          namespace dolfin
          {
            class Delta : public Expression
            {
            public:

              Delta() : Expression() {}

              void eval(Array<double>& values, const Array<double>& data,
                        const ufc::cell& cell) const
              { }

              void update(const std::shared_ptr<const Function> u,
                          double nu, double dt, double C1,
                          double U_infty, double chord)
              {
                const std::shared_ptr<const Mesh> mesh = u->function_space()->mesh();
                const std::shared_ptr<const GenericDofMap> dofmap = u->function_space()->dofmap();
                const uint ncells = mesh->num_cells();
                uint ndofs_per_cell;
                if (ncells > 0)
                {
                  CellIterator cell(*mesh);
                  ndofs_per_cell = dofmap->cell_dimension(cell->index());
                }
                else
                {
                   return;
                }
              }
            };
          }'''

          self.assertRaises(RuntimeError, Expression, code_not_compile)

     def test_doc_string_python_expressions(self):
          """
          This test tests all features documented in the doc string of
          Expression. If this test breaks and it is fixed the corresponding fixes
          need also be updated in the docstring.
          """

          square = UnitSquareMesh(4,4)

          class MyExpression0(Expression):
              def eval(self, value, x):
                  dx = x[0] - 0.5
                  dy = x[1] - 0.5
                  value[0] = 500.0*exp(-(dx*dx + dy*dy)/0.02)
                  value[1] = 250.0*exp(-(dx*dx + dy*dy)/0.01)
              def value_shape(self):
                  return (2,)

          f0 = MyExpression0()
          values = f0(0.2,0.3)
          dx = 0.2-0.5
          dy = 0.3-0.5

          self.assertAlmostEqual(values[0], 500.0*exp(-(dx*dx + dy*dy)/0.02))
          self.assertAlmostEqual(values[1], 250.0*exp(-(dx*dx + dy*dy)/0.01))

          ufc_cell_attrs = ["cell_shape", "index", "topological_dimension",
                            "geometric_dimension", "local_facet", "mesh_identifier"]

          class MyExpression1(Expression):
              def eval_cell(self_expr, value, x, ufc_cell):
                  if ufc_cell.index > 10:
                      value[0] = 1.0
                  else:
                      value[0] = -1.0

               # Check attributes in ufc cell
                  for attr in ufc_cell_attrs:
                       self.assertTrue(hasattr(ufc_cell, attr))

          f1 = MyExpression1()
          assemble(f1*ds(square))

          class MyExpression2(Expression):
              def __init__(self, mesh, domain):
                  self._mesh = mesh
                  self._domain = domain
              def eval(self, values, x):
                  pass

          cell_data = CellFunction('uint', square)

          f3 = MyExpression2(square, cell_data)

          self.assertEqual(id(f3._mesh), id(square))
          self.assertEqual(id(f3._domain), id(cell_data))

if __name__ == "__main__":
    unittest.main()
