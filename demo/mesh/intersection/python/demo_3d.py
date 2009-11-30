"""This demo shows the intersection of the boundary of a unit square
(omega1) with a unit circle (omega2) rotating around the center of the
square.
@todo Change camera perspective/ viewpoint to improve intersection visibility.
"""

__author__ = "Andre Massing (massing@simula.no)"
__date__ = "2008-11-17"
__copyright__ = "Copyright (C) 2009 Andre Massing"
__license__  = "GNU LGPL Version 2.1"

from dolfin import *
from numpy import *

if not has_cgal():
    print "DOLFIN must be compiled with CGAL to run this demo."
    exit(0)

sphere = UnitSphere(20)
cube = UnitCube(20, 20, 20)

# Access mesh geometry
x = sphere.coordinates()

# Start center and propagtion speed for the sphere.
dt = 0.1
t = -0.71

# Scale and move the circle.
x *= 0.7
x[:] += t

intersection = MeshFunction("uint", cube, cube.topology().dim())
_first = True

counter = 0
while t < 1.7 :

    boundary = BoundaryMesh(sphere)
    # Compute intersection with boundary of square
    cells = cube.all_intersected_entities(boundary)

    # Mark intersected values
    intersection.values()[:] = 0
    intersection.values()[cells] = 1

    counter +=1

    # Plot intersection
    if _first:
        p = plot(intersection, rescale=False)
        p.ren.SetViewPoint(-1,-1,-1)
        p.ren.ResetCamera()
        _first = False
    if counter == 2:
      interactive()
#      p.movie("movie")

    else:
        plot(intersection)

    #Propagate sphere along the line t(1,1,1).  
    x[:,0] += dt 
    x[:,1] += dt
    x[:,2] += dt 

    t += dt

# Hold plot
interactive()
