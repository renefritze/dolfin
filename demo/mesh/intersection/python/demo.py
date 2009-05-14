"""This demo shows the intersection of the boundary of a unit square
(omega1) with a unit circle (omega2) rotating around the center of the
square."""

__author__ = "Kristoffer Selim (selim@simula.no)"
__date__ = "2008-10-14 -- 2008-10-14"
__copyright__ = "Copyright (C) 2008 Kristoffer Selim"
__license__  = "GNU LGPL Version 2.1"

from dolfin import *
from numpy import *

# Create meshes (omega0 overlapped by omega1)
omega0 = UnitCircle(20)
omega1 = UnitSquare(20, 20)

# Access mesh geometry
x = omega0.coordinates()

# Move and scale circle
x *= 0.5
x += 1.0

# Iterate over angle
theta = 0.0
dtheta = 0.01*DOLFIN_PI
intersection = MeshFunction("uint", omega0, omega0.topology().dim())
_first = True
while theta < 2*DOLFIN_PI:

    # Compute intersection with boundary of square
    boundary = BoundaryMesh(omega1)
    cells = STLVectorUInt()
    omega0.intersection(boundary, cells, False)

    # Copy values to mesh function for plotting
    for j in range(intersection.size()):
        intersection.set(j, 0)
    for j in range(cells.size()):
        intersection.set(cells[j], 1)

    # Plot intersection
    if _first:
        p = plot(intersection, rescale=False)
        p.add_polygon([[0, 0, -0.01], [1, 0, -0.01], [1, 1, -0.01], [0, 1, -0.01], [0, 0, -0.01]])
        p.ren.ResetCamera()
        _first = False
    else:
        plot(intersection)

    # Rotate circle around (0.5, 0.5)
    xr = x[:, 0] - 0.5
    yr = x[:, 1] - 0.5
    x[:,0] = 0.5 + (cos(dtheta)*xr - sin(dtheta)*yr)
    x[:,1] = 0.5 + (sin(dtheta)*xr + cos(dtheta)*yr)

    theta += dtheta

# Hold plot
interactive()
