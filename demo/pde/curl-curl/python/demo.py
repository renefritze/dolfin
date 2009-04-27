""" Eddy currents phenomena in low conducting body can be 
described using electric vector potential and curl-curl operator:
   \nabla \times \nabla \times T = - \frac{\partial B}{\partial t}
Electric vector potential defined as:
   \nabla \times T = J

Boundary condition:
   J_n = 0, 
   T_t=T_w=0, \frac{\partial T_n}{\partial n} = 0
which is naturaly fulfilled for zero Dirichlet BC with Nedelec (edge) 
elements.
"""

__author__ = "Bartosz Sawicki (sawickib@iem.pw.edu.pl)"
__date__ = "2009-04-02"
__copyright__ = "Copyright (C) 2009 Bartosz Sawicki"
__license__  = "GNU LGPL Version 2.1"

from dolfin import *

# Create mesh
mesh = UnitSphere(8)

# Define function spaces
PN = FunctionSpace(mesh, "Nedelec 1st kind H(curl)", 0)
P1 = VectorFunctionSpace(mesh, "CG", 1)

# Define test and trial functions
v0 = TestFunction(PN)
u0 = TrialFunction(PN)
v1 = TestFunction(P1)
u1 = TrialFunction(P1)

# Define functions
dbdt = Function(P1, ("0.0", "0.0", "1.0"))
zero = Function(P1, ("0.0", "0.0", "0.0"))
T = Function(PN)
J = Function(P1)

# Dirichlet boundary
class DirichletBoundary(SubDomain):
    def inside(self, x, on_boundary):
        return on_boundary

# Boundary condition
bc = DirichletBC(PN, zero, DirichletBoundary())

# Eddy currents equation (using potential T) 
Teqn = inner(curl(v0), curl(u0))*dx, -dot(v0, dbdt)*dx
Tproblem = VariationalProblem(Teqn[0], Teqn[1], bc)
T = Tproblem.solve()

# Current density equation
Jeqn = inner(v1, u1)*dx, dot(v1, curl(T))*dx
Jproblem = VariationalProblem(Jeqn[0], Jeqn[1])
J = Jproblem.solve()

# Plot solution
plot(J)

# Hold plot
interactive()
