"""This demo program demonstrates how to mark sub domains of a mesh
and store the sub domain markers as a mesh function to a DOLFIN XML
file.

The sub domain markers produced by this demo program are the ones used
for the Stokes demo programs.
"""

__author__ = "Kristian B. Oelgaard (k.b.oelgaard@tudelft.nl)"
__date__ = "2007-11-15 -- 2008-12-07"
__copyright__ = "Copyright (C) 2007 Kristian B. Oelgaard"
__license__  = "GNU LGPL Version 2.1"

# Modified by Anders Logg, 2008.

from dolfin import *

dolfin_set("debug level", 1)

# Sub domain for no-slip (mark whole boundary, inflow and outflow will overwrite)
class Noslip(SubDomain):
    def inside(self, x, on_boundary):
        return on_boundary

# Sub domain for inflow (right)
class Inflow(SubDomain):
    def inside(self, x, on_boundary):
        return x[0] > 1.0 - DOLFIN_EPS and on_boundary

# Sub domain for outflow (left)
class Outflow(SubDomain):
    def inside(self, x, on_boundary):
        return x[0] < DOLFIN_EPS and on_boundary

# Read mesh
mesh = Mesh("../../../../data/meshes/dolfin-2.xml.gz")

# Create mesh function over the cell facets
sub_domains = MeshFunction("uint", mesh, mesh.topology().dim() - 1)

# Mark all facets as sub domain 3
for i in range(sub_domains.size()):
    sub_domains.set(i, 3)

# Mark no-slip facets as sub domain 0
noslip = Noslip()
noslip.mark(sub_domains, 0)

# Mark inflow as sub domain 1
inflow = Inflow()
inflow.mark(sub_domains, 1)

# Mark outflow as sub domain 2
outflow = Outflow()
outflow.mark(sub_domains, 2)

# Save sub domains to file
file = File("subdomains.xml")
file << sub_domains
