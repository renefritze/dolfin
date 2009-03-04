"This demo illustrates mesh refinement."

__author__ = "Anders Logg"
__date__ = "2007-06-01 -- 2009-03-04"
__copyright__ = "Copyright (C) 2007-2009 Anders Logg"
__license__  = "GNU LGPL Version 2.1"

from dolfin import *

# Create mesh of unit square
mesh = UnitSquare(5, 5)
plot(mesh, interactive=True)

# Uniform refinement
mesh.refine()
plot(mesh, interactive=True)

# Refine mesh close to x = (0.5, 0.5)
p = Point(0.5, 0.5)
for i in range(5):

    # Mark cells for refinement
    cell_markers = MeshFunction("bool", mesh, mesh.topology().dim())
    for c in cells(mesh):
        if c.midpoint().distance(p) < 0.1:
            cell_markers.set(c, True)
        else:
            cell_markers.set(c, False)

    # Refine mesh
    mesh.refine(cell_markers)
    
    # Plot mesh
    plot(mesh, interactive=True)
