from ffc import *

# Reserved variables for forms
(a, L, M) = (None, None, None)

# Reserved variable for element
element = None

# Copyright (C) 2005 Garth N. Wells.
# Licensed under the GNU LGPL Version 2.1.
#
# The linearised bilinear form a(v, U) and linear form L(v) for
# the nonlinear equation - div (1+u^2) grad u = f
#
# Compile this form with FFC: ffc -l dolfin NonlinearPoisson.form.

element = FiniteElement("Lagrange", "triangle", 1)

v = TestFunction(element)
U = TrialFunction(element)
U0= Function(element)
f = Function(element)

a = v.dx(i)*(1+U0*U0)*U.dx(i)*dx + v.dx(i)*2*U0*U*U0.dx(i)*dx
L = v*f*dx - v.dx(i)*(1+U0*U0)*U0.dx(i)*dx

compile([a, L, M, element], "NonlinearPoisson", options={'language': 'dolfin', 'blas': False, 'form_postfix': True, 'precision': '15', 'cpp optimize': False, 'split_implementation': False, 'quadrature_points': False, 'output_dir': '.', 'representation': 'tensor', 'cache_dir': None, 'optimize': False}, global_variables=globals())
