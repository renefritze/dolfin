from math import *
from dolfin import *
from heatpde import *

# Define right-hand side
class Source(Function):
    def eval(self, point, i):
        if(point.x < 0.3 and point.y < 0.3):
            if(self.time() < 2.0):
                return 5.0
            else:
                return 0.0
        else:
            return 0.0

# Define boundary condition
class SimpleBC(BoundaryCondition):
    def eval(self, value, point, i):
        #if point.x == 1.0:
        #    value.set(0.0)
        #value.set(0.0)
        return

f = Source()
bc = SimpleBC()

# Create a mesh of the unit square
mesh = UnitSquare(20, 20)


k = 1e-3
T = 10.0

t = doublep()
t.assign(0.0)
f.sync(t)


set("ODE method", "dg");
set("ODE order", 0);
set("ODE nonlinear solver", "newton");
set("ODE tolerance", 1e0);

set("ODE fixed time step", True);
set("ODE initial time step", 1e-2);
set("ODE maximum time step", 1e+0);

set("ODE save solution", False);
set("ODE solution file name", "primal.py");
set("ODE number of samples", 400);

pde = HeatPDE(mesh, f, bc, k, T, t)

pde.solve(pde.U)

# Plot with Mayavi

# Load mayavi
from mayavi import *

# Plot solution
#v = mayavi()
#d = v.open_vtk_xml("poisson000000.vtu")
#m = v.load_module("Axes")
#m = v.load_module("BandedSurfaceMap")
#f = v.load_filter('WarpScalar', config=0)

# Wait until window is closed
#v.master.wait_window()
