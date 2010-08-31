# This demo solves the Stokes equations, using stabilized
# first order elements for the velocity and pressure. The
# sub domains for the different boundary conditions used
# in this simulation are computed by the demo program in
# src/demo/mesh/subdomains.
#
# Original implementation: ../cpp/main.cpp by Anders Logg

__author__ = "Kristian B. Oelgaard (k.b.oelgaard@tudelft.nl)"
__date__ = "2007-11-15 -- 2009-11-26"
__copyright__ = "Copyright (C) 2007 Kristian B. Oelgaard"
__license__  = "GNU LGPL Version 2.1"

# Modified by Anders Logg, 2009.

from dolfin import *

# Load mesh and subdomains
mesh = Mesh("../../../../../data/meshes/dolfin-2.xml.gz")
sub_domains = MeshFunction("uint", mesh, "../subdomains.xml.gz");

# Define function spaces
scalar = FunctionSpace(mesh, "CG", 1)
vector = VectorFunctionSpace(mesh, "CG", 1)
system = vector * scalar

# Create functions for boundary conditions
noslip = Constant((0, 0))
inflow = Expression(("-sin(x[1]*pi)", "0"))
zero   = Constant(0)

# No-slip boundary condition for velocity
bc0 = DirichletBC(system.sub(0), noslip, sub_domains, 0)

# Inflow boundary condition for velocity
bc1 = DirichletBC(system.sub(0), inflow, sub_domains, 1)

# Boundary condition for pressure at outflow
bc2 = DirichletBC(system.sub(1), zero, sub_domains, 2)

# Collect boundary conditions
bcs = [bc0, bc1, bc2]

# Define variational problem
(v, q) = TestFunctions(system)
(u, p) = TrialFunctions(system)

f = Constant((0, 0))
h = CellSize(mesh)

beta  = 0.2
delta = beta*h*h

a = (inner(grad(v), grad(u)) - div(v)*p + q*div(u) + delta*inner(grad(q), grad(p)))*dx
L = inner(v + delta*grad(q), f)*dx

# Set up PDE
problem = VariationalProblem(a, L, bcs)

# Solve PDE
(U, P) = problem.solve().split()

# Save solution in VTK format
ufile_pvd = File("velocity.pvd")
ufile_pvd << U
pfile_pvd = File("pressure.pvd")
pfile_pvd << P

# Plot solution
plot(U)
plot(P)
interactive()
