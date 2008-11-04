"""This demo program solves Poisson's equation

    - div grad u(x, y) = f(x, y)

on the unit square with source f given by

    f(x, y) = 500*exp(-((x-0.5)^2 + (y-0.5)^2)/0.02)

and boundary conditions given by

    u(x, y)     = 0               for x = 0
    du/dn(x, y) = 25 sin(5 pi y)  for x = 1
    du/dn(x, y) = 0               otherwise
"""

__author__ = "Anders Logg (logg@simula.no)"
__date__ = "2007-08-16 -- 2008-04-03"
__copyright__ = "Copyright (C) 2007-2008 Anders Logg"
__license__  = "GNU LGPL Version 2.1"

from dolfin import *

# Create mesh and FunctionSpace
mesh = UnitSquare(32, 32)

V = FunctionSpace(mesh, "Lagrange", 1)

# Source term
class Source(Function):
    def eval(self, values, x):
        dx = x[0] - 0.5
        dy = x[1] - 0.5
        values[0] = 500.0*exp(-(dx*dx + dy*dy)/0.02)
    def dim(self,i):
        return 2
    def rank(self):
        return 0

# Neumann boundary condition
class Flux(Function):
    def eval(self, values, x):
        if x[0] > DOLFIN_EPS:
            values[0] = 25.0*sin(5.0*DOLFIN_PI*x[1])
        else:
            values[0] = 0.0
    def dim(self,i):
        return 2
    def rank(self):
        return 0

# Sub domain for Dirichlet boundary condition
#class DirichletBoundary(SubDomain):
#    def inside(self, x, on_boundary):
#        return bool(on_boundary and x[0] < DOLFIN_EPS)

# Define variational problem
v = TestFunction(V)
u = TrialFunction(V)
f = Source(V)
g = Flux(V)

a = dot(grad(v), grad(u))*dx
L = v*f*dx + v*g*ds

# Define boundary condition
#u0 = Function(mesh, 0.0)
#boundary = DirichletBoundary()
#bc = DirichletBC(u0, mesh, boundary)

# Solve PDE and plot solution
#pde = LinearPDE(a, L, mesh, bc, symmetric)
#u = pde.solve()
#plot(u, warpscalar=True, rescale=True)

# Save solution to file
#file = File("poisson.pvd")
#file << u

# Hold plot
#interactive()

#summary()
